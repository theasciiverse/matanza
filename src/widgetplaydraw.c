#include <matanza.h>

INLINE static void        mat_image_draw_size_calc         ( unsigned int        size,
                                                             unsigned int        view,
                                                             unsigned int        img,
                                                             double             *rate,
                                                             double             *draw );

void                      mat_image_draw                   ( MatWidgetPlayData  *data,
                                                             MatScm             *img,
                                                             int                 x,
                                                             int                 y,
                                                             double              sp_x,
                                                             double              sp_y,
                                                             unsigned char      *pal,
                                                             int                 adj,
                                                             double              alpha );

INLINE static void        draw_pos                         ( MatScm             *d,
                                                             MatScm             *i,
                                                             double              px,
                                                             double              py,
                                                             double              rx,
                                                             double              ry,
                                                             double              sx,
                                                             double              sy,
                                                             double              vx,
                                                             double              vy,
                                                             unsigned char      *pal,
                                                             double              alpha );

void                      draw_pos_one                     ( MatScm             *data,
                                                             MatScm             *i,
                                                             double              pos_x,
                                                             double              pos_y,
                                                             double              rate_x,
                                                             double              rate_y,
                                                             double              size_x,
                                                             double              size_y,
                                                             unsigned char      *pal,
                                                             double              alpha );

INLINE void
mat_widget_play_draw_obj ( void *funcdata, MatScm *obj )
{
  MatWidgetPlayData *data = (MatWidgetPlayData *) funcdata;
  unsigned int x, y;

  ASSERT(obj);
  ASSERT(data);
  ASSERT(data->un);
  ASSERT(mat_scm_obj_type_get(data->un) == &mat_scm_universe_type);
  ASSERT(data->data);
  ASSERT(mat_scm_obj_type_get(data->data) == &mat_scm_widgetplay_type);
  ASSERT(mat_movingobj_universe_get(obj) == data->un);

  x = (int) mat_movingobj_pos_x_ul_get(obj);
  y = (int) mat_movingobj_pos_y_ul_get(obj);

  ASSERT(x < mat_universe_mapsize_x_get(data->un));
  ASSERT(y < mat_universe_mapsize_y_get(data->un));

  if (!mat_movingobj_image_get(obj))
    {
      if (x < data->startx)
        x += mat_universe_mapsize_x_get(data->un);
      if (y < data->starty)
        y += mat_universe_mapsize_y_get(data->un);

      ASSERT(data->startx <= x);
      ASSERT(data->starty <= y);

      x -= data->startx;
      y -= data->starty;

      ASSERT(x < mat_universe_mapsize_x_get(data->un));
      ASSERT(y < mat_universe_mapsize_y_get(data->un));

      if (mat_widget_play_view_x_get(data->data) <= x || mat_widget_play_view_y_get(data->data) <= y)
        return;

      if (mat_widget_play_size_x_get(data->data) != mat_widget_play_view_x_get(data->data))
        x = (double) x * (double) mat_widget_play_size_x_get(data->data) / (double) mat_widget_play_view_x_get(data->data);

      if (mat_widget_play_size_y_get(data->data) != mat_widget_play_view_y_get(data->data))
        y = (double) y * (double) mat_widget_play_size_y_get(data->data) / (double) mat_widget_play_view_y_get(data->data);

      ASSERT(x < mat_widget_play_size_x_get(data->data));
      ASSERT(y < mat_widget_play_size_y_get(data->data));

      y = mat_widget_play_size_y_get(data->data) - y - 1;

      mat_window_imgbuffer_set(mat_widget_play_window_get(data->data), x, y, 255, 1.0);
    }
  else
    {
      double sp_x, sp_y;
      double alpha;

      ASSERT(mat_movingobj_image_get(obj));

      sp_x = mat_movingobj_sp_x_get(obj);
      sp_y = mat_movingobj_sp_y_get(obj);

      sp_x -= mat_movingobj_sp_x_get(mat_widget_play_self_get(data->data));
      sp_y -= mat_movingobj_sp_y_get(mat_widget_play_self_get(data->data));

      alpha = mat_movingobj_alpha_get(obj);

      mat_image_draw(data, mat_movingobj_image_get(obj), x, y, sp_x, sp_y, mat_movingobj_pal_get(obj), 0, alpha);
    }
}

INLINE static void
size_calc ( unsigned int size, unsigned int view, unsigned int img, double *rate, double *draw )
{
  ASSERT(size > 0);
  ASSERT(view > 0);

  if (size != view)
    {
      *rate = (double) view / (double) size;
      *draw = (double) img / *rate;
    }
  else
    {
      *rate = 1;
      *draw = img;
    }

  ASSERT(*rate > 0);
}

void
mat_image_draw ( MatWidgetPlayData *d, MatScm *img, int x, int y, double sp_x, double sp_y, unsigned char *pal, int adj, double alpha )
{
  double view_x, view_y, sp;
  double rate_x, rate_y, pos_x, pos_y;
  int size_x, size_y;

  ASSERT(d);
  ASSERT(d->un);
  ASSERT(d->data);

  ASSERT(img);

  ASSERT(0 < mat_image_size_x_get(img));
  ASSERT(0 < mat_image_size_y_get(img));

  ASSERT(x < mat_universe_mapsize_x_get(d->un));
  ASSERT(y < mat_universe_mapsize_y_get(d->un));

  ASSERT(0.0 <= alpha);
  ASSERT(alpha <= 1.0);

  if (alpha == 0.0)
    return;

  size_calc(mat_widget_play_size_x_get(d->data), mat_widget_play_view_x_get(d->data), mat_image_size_x_get(img), &rate_x, &view_x);
  size_calc(mat_widget_play_size_y_get(d->data), mat_widget_play_view_y_get(d->data), mat_image_size_y_get(img), &rate_y, &view_y);

  ASSERT(mat_image_size_x_get(img) < mat_universe_mapsize_x_get(d->un));
  ASSERT(mat_image_size_y_get(img) < mat_universe_mapsize_y_get(d->un));

  if (adj)
    {
      size_x = (int) mat_image_size_x_get(img);
      size_y = (int) mat_image_size_y_get(img);
    }
  else
    {
      size_x = (int) mat_universe_mapsize_x_get(d->un);
      size_y = (int) mat_universe_mapsize_y_get(d->un);
    }

  pos_x = (double) x - (double) d->startx;
  pos_y = (double) y - (double) d->starty;

  if (adj)
    {
      pos_x *= (double) size_x / (double) mat_universe_mapsize_x_get(d->un);
      pos_y *= (double) size_y / (double) mat_universe_mapsize_y_get(d->un);
    }

  if (0 < (int) pos_x)
    pos_x -= (double) size_x;

  if (0 < (int) pos_y)
    pos_y -= (double) size_y;

  ASSERT((int) pos_x <= 0);
  ASSERT((int) -pos_x < size_x);

  ASSERT((int) pos_y <= 0);
  ASSERT((int) -pos_y < size_y);

  sp = sp_x * sp_x + sp_y * sp_y;

  if (!mat_arg_motion_blur || sp < 1.0 / (double) arg_universe_update_steps)
    draw_pos(d->data, img, pos_x, pos_y, rate_x, rate_y, size_x, size_y, view_x, view_y, pal, alpha);
  else
    {
      int steps;
      int i;

      sp = sqrt(sp);

      steps = sp * arg_universe_update_steps;
      alpha /= (double) steps;

      for (i = 0; i < steps; i ++)
        {
          draw_pos(d->data, img, pos_x, pos_y, rate_x, rate_y, size_x, size_y, view_x, view_y, pal, alpha);

          ASSERT(- size_x < (int) pos_x);
          ASSERT((int) pos_x <= 0);

          ASSERT(ABS(sp_x) <= size_x);

          pos_x += sp_x;

          ASSERT(2 * - size_x < (int) pos_x);
          ASSERT((int) pos_x <= size_x);

          if ((int) pos_x <= - size_x)
            pos_x += size_x;
          else if (0 < (int) pos_x)
            pos_x -= size_x;

          ASSERT(- size_x < (int) pos_x);
          ASSERT((int) pos_x <= 0);

          ASSERT(- size_y < (int) pos_y);
          ASSERT((int) pos_y <= 0);

          ASSERT(ABS(sp_y) <= size_y);

          pos_y += sp_y;

          ASSERT(2 * - size_y < (int) pos_y);
          ASSERT((int) pos_y <= size_y);

          if ((int) pos_y <= - size_y)
            pos_y += size_y;
          else if (0 < (int) pos_y)
            pos_y -= size_y;

          ASSERT(- size_y < (int) pos_y);
          ASSERT((int) pos_y <= 0);
        }
    }
}

INLINE static void
draw_pos ( MatScm *d, MatScm *i, double px, double py, double rx, double ry, double sx, double sy, double vx, double vy, unsigned char *pal, double alpha )
{
  ASSERT((int) px <= 0);
  ASSERT((int) py <= 0);
  ASSERT(0 <= (int) (px + sx));
  ASSERT(0 <= (int) (py + sy));

  if ((int) -px < mat_image_size_x_get(i))
    {
      if ((int) -py < mat_image_size_y_get(i))
        draw_pos_one(d, i, px     , py     , rx, ry, vx, vy, pal, alpha);

      if ((int) py + (int) sy < mat_widget_play_view_y_get(d))
        draw_pos_one(d, i, px     , py + sy, rx, ry, vx, vy, pal, alpha);
    }

  if ((int) px + (int) sx < mat_widget_play_view_x_get(d))
    {
      if ((int) -py < mat_image_size_y_get(i))
        draw_pos_one(d, i, px + sx, py     , rx, ry, vx, vy, pal, alpha);

      if ((int) py + (int) sy < mat_widget_play_view_y_get(d))
        draw_pos_one(d, i, px + sx, py + sy, rx, ry, vx, vy, pal, alpha);
    }
}

#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) > (y) ? (x) : (y))

void
draw_pos_one ( MatScm *widget, MatScm *img, double pos_x, double pos_y, double rate_x, double rate_y, double view_x, double view_y, unsigned char *pal, double alpha )
{
  int i, j;
  double x1 = 0, y1 = 0;

  ASSERT(widget);

  ASSERT(0 < view_x);
  ASSERT(0 < view_y);

  ASSERT(0 < rate_x);
  ASSERT(0 < rate_y);

  pos_x /= rate_x;

  /* Assert that it is vissible */
  ASSERT(0 <= (int) pos_x + (int) view_x);
  ASSERT((int) pos_x <= (int) mat_widget_play_size_x_get(widget));

  x1 = max(0, -pos_x);

  /* Assert that we start inside the image */
  ASSERT(0 <= (int) x1);
  ASSERT((int) x1 <= (int) view_x);

  view_x = min(view_x, (int) mat_widget_play_size_x_get(widget) - (int) pos_x);

  ASSERT(0 <= (int) pos_x + (int) view_x);
  ASSERT((int) pos_x + (int) view_x <= (int) mat_widget_play_size_x_get(widget));

  pos_y /= rate_y;

  /* Assert that it is vissible */
  ASSERT(0 <= (int) pos_y + (int) view_y);
  ASSERT((int) pos_y <= (int) mat_widget_play_size_y_get(widget));

  y1 = max(0, (int) pos_y + (int) view_y - (int) mat_widget_play_size_y_get(widget));

  /* Assert that we start inside the image */
  ASSERT(0 <= (int) y1);
  ASSERT((int) y1 <= (int) view_y);

  view_y = min(view_y, view_y + pos_y);
  pos_y = (int) mat_widget_play_size_y_get(widget) - (int) view_y - max(0, (int) pos_y);

  ASSERT(0 <= (int) pos_y + (int) view_y);
  ASSERT((int) pos_y + (int) view_y <= (int) mat_widget_play_size_y_get(widget));

  if ((int) pos_x + (int) view_x == 0 || (int) pos_y + (int) view_y == 0)
    return;

  if ((int) pos_x == (int) mat_widget_play_size_x_get(widget) || (int) pos_y == (int) mat_widget_play_size_y_get(widget))
    return;

  ASSERT(0 <= (int) view_x);
  ASSERT(0 <= (int) view_y);

  if (!arg_image_scale_good && ((int) (view_y - y1) < 3 || (int) (view_x - x1) < 3))
    {
      mat_window_imgbuffer_set(mat_widget_play_window_get(widget), pos_x + x1, pos_y + y1, 255 / 2, alpha);
      return;
    }

  for (i = (int) y1; i < (int) view_y; i ++)
    for (j = (int) x1; j < (int) view_x; j ++)
      {
        unsigned int col;

        if (arg_image_scale_good)
          {
            unsigned int col_x, col_y;

            col = 0;

            for (col_y = 0; col_y < rate_y && (int) (i * rate_y) + col_y < mat_image_size_y_get(img); col_y ++)
              for (col_x = 0; col_x < rate_x && (int) (j * rate_x) + col_x < mat_image_size_x_get(img); col_x ++)
                col += pal[(int) mat_image_core_get(img, j * rate_x + col_x, i * rate_y + col_y)];

            ASSERT(0 < col_x);
            ASSERT(0 < col_y);

            ASSERT(col >= 0);

            col /= col_y * col_x;
          }
        else
          col = pal[(int) mat_image_core_get(img, j * rate_x, i * rate_y)];

        if (col > 0)
          mat_window_imgbuffer_set(mat_widget_play_window_get(widget), pos_x + j, pos_y + i, col, alpha);
      }
}
