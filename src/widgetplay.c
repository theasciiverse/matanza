#include <matanza.h>

struct MatWidgetPlay
{
  /* The ship controled by this widget */
  MatScm          *self;

  /* The ship entry in the list of objects in the universe.
   * The widget shows the world around this ship. */
  MatScm          *ship;

  /* The entry in the list of views for ship */
  MatScm          *ship_list;

  /* The window this widget is in */
  MatScm          *window;

  /* The entry for this widget in the list of widgets in the window */
  MatScm          *window_list;

  /* How much of the world should he see? */
  unsigned int view_x, view_y;

  unsigned int pos_x, pos_y;

  /* What's the size (in pixels) of this widget? */
  unsigned int size_x, size_y;

  /* Memory for allowing the player to chat with others */
  char         chat_buf[MATANZA_WIDGET_PLAY_CHAT_SIZE];

  /* The length of the buffer or -1 if the player is not chatting */
  int          chat_len;

  /* Function to handle input */
  int        (*key) ( MatScm *, int );

#if 0
  /* The pointers in the list of widgets viewing the world around the same ship */
  MatWidgetPlay *ship_next;
  MatWidgetPlay *ship_prev;
#endif

  /* List of messages */
  unsigned int msg_pos   : 1;
  unsigned int msg_count : 3;
  MatMessage  *msg_head;
  MatMessage  *msg_tail;
};

static INLINE MatScm     *wp_new                           ( MatScm             *window,
                                                             MatScm             *name,
                                                             MatScm             *un,
                                                             MatScm             *team,
                                                             MatScm             *type );

static int                wp_true                          ( MatScm             *window,
                                                             MatScm             *widget );

static INLINE MatWidgetPlay *data_get                      ( MatScm             *widget );

static void               wp_resize                        ( MatScm             *window,
                                                             MatScm             *widget );

static INLINE unsigned    wp_calc_view                     ( MatScm             *data,
                                                             double              view_x );

static void               view_set                         ( MatScm             *data,
                                                             MatScm             *ship );

static INLINE unsigned    wp_view_pos_one                  ( unsigned            pos,
                                                             unsigned            mapsize,
                                                             unsigned            imgsize );

static INLINE void        wp_view_pos                      ( MatWidgetPlayData  *d );

static INLINE void        mat_widget_play_bg               ( MatWidgetPlayData  *data );

#if 0
static INLINE double      mat_widget_play_mist_calc        ( MatWidgetPlayData  *data,
                                                             unsigned int        x,
                                                             unsigned int        y );

static void               mat_widget_play_mist             ( MatWidgetPlayData  *data );
#endif

static void               wp_redraw                        ( MatScm             *window,
                                                             MatScm             *widget );

#if 0
static int                mat_widget_play_dist             ( unsigned int        shx,
                                                             unsigned int        objx,
                                                             unsigned int        unx );

static int                mat_widget_play_dist_calc        ( MatWidgetPlayData  *data,
                                                             MatMovingObj       *obj,
                                                             int                *distx,
                                                             int                *disty );
#endif

static void               wp_dir                           ( void               *funcdata,
                                                             MatScm             *obj );

static void               wp_border                        ( MatWidgetPlayData  *data,
                                                             double              distx,
                                                             double              disty,
                                                             char                c );

static INLINE void        wp_msg_draw                      ( MatScm             *window,
                                                             MatScm             *data );

#if 0
static INLINE int         mat_widget_play_ship_in          ( MatWidgetPlay      *a,
                                                             MatMovingObj       *b );
#endif

static INLINE int         mat_widget_play_ship_coord       ( double              diff,
                                                             double              map,
                                                             double              view );

static void               wp_free                          ( MatScm             *widget );

static int                wp_key                           ( MatScm             *window,
                                                             MatScm             *widget,
                                                             int                 key );

static MatWidgetTypeDesc wp_desc =
{
  wp_true,
  NULL,
  NULL,
  wp_key,
  wp_true,
  wp_redraw,
  wp_resize,
  wp_free,
};

void
mat_widget_play_start ( MatScm *window, MatScm *name, MatScm *un, MatScm *team, MatScm *type )
{
  MatScm *widget;

  ASSERT(window);

  mat_window_reset(window);
  mat_window_randomval_set(window, 0);

  widget = wp_new(window, name, un, team, type);

  mat_widget_window_add(widget, window, NULL);

  mat_window_redraw(window);
}

static INLINE MatScm *
wp_new ( MatScm *window, MatScm *name, MatScm *un, MatScm *team, MatScm *type )
{
  MatWidgetPlay *data;
  MatScm *widget;

  ASSERT(window);
  ASSERT(name);
  ASSERT(un);

  data = malloc(sizeof(MatWidgetPlay));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  widget = mat_scm_obj_malloc(&mat_scm_widgetplay_type, data);

  data->window = window;

  data->pos_x = 0;
  data->pos_y = 0;

  data->view_x = arg_imgwidth;

  data->key = mat_widget_play_key_normal;

  data->msg_pos = 0;
  data->msg_count = 0;
  data->msg_head = data->msg_tail = NULL;

  data->ship = data->self = NULL;
  data->ship_list = NULL;

  data->ship = data->self = mat_ship_new(un, name, team, NULL, widget, type);
  if (!data->self)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->ship_list = mat_movingobj_view_add(data->ship, widget);

  return mat_widget_new(&wp_desc, mat_scm_obj_malloc(&mat_scm_widgetplay_type, data));
}

static int
wp_true ( MatScm *window, MatScm *widget )
{
  return 1;
}

static INLINE MatWidgetPlay *
data_get ( MatScm *widget )
{
  MatWidgetPlay *data;

  ASSERT(widget);

  if (mat_scm_obj_type_get(widget) == &mat_scm_widget_type)
    widget = mat_widget_data_get(widget);

  ASSERT(widget);
  ASSERT(mat_scm_obj_type_get(widget) == &mat_scm_widgetplay_type);

  data = mat_scm_obj_data_get(widget);

  ASSERT(data);
  ASSERT(!data->ship || mat_scm_obj_type_get(data->ship) == &mat_scm_ship_type);
  ASSERT(!data->ship || mat_movingobj_universe_get(data->ship));
  ASSERT(!data->ship || mat_scm_obj_type_get(mat_movingobj_universe_get(data->ship)) == &mat_scm_universe_type);
  ASSERT(data->window);
  ASSERT(data->key);

  return data;
}

static void
wp_resize ( MatScm *window, MatScm *widget )
{
  MatWidgetPlay *data = data_get(widget);

  data->pos_x  = 0;
  data->pos_y  = 0;

  data->size_x = mat_window_img_size_x_get(window);
  data->size_y = mat_window_img_size_y_get(window);

  data->view_y = wp_calc_view(widget, data->view_x);

  if (data->view_y < mat_image_size_y_get(mat_movingobj_image_get(mat_widget_play_view_get(widget))))
    {
      data->view_y = mat_image_size_y_get(mat_movingobj_image_get(mat_widget_play_view_get(widget)));
      data->view_x = (data->view_y * data->size_x) / (1.5 * data->size_y);
    }

  if (mat_universe_mapsize_y_get(mat_movingobj_universe_get(mat_widget_play_view_get(widget))) < data->view_y)
    {
      data->view_y = mat_universe_mapsize_y_get(mat_movingobj_universe_get(mat_widget_play_view_get(widget)));
      data->view_x = (data->view_y * data->size_x) / (1.5 * data->size_y);
    }

  ASSERT(data->view_x <= mat_universe_mapsize_x_get(mat_movingobj_universe_get(mat_widget_play_view_get(widget))));
  ASSERT(data->view_y <= mat_universe_mapsize_y_get(mat_movingobj_universe_get(mat_widget_play_view_get(widget))));

  ASSERT(mat_image_size_x_get(mat_movingobj_image_get(mat_widget_play_view_get(widget))) <= data->view_x);
  ASSERT(mat_image_size_y_get(mat_movingobj_image_get(mat_widget_play_view_get(widget))) <= data->view_y);
}

static INLINE unsigned
wp_calc_view ( MatScm *widget, double view_x )
{
  MatWidgetPlay *data = data_get(widget);
  return view_x * 1.5 * (double) data->size_y / (double) data->size_x;
}

INLINE void
mat_widget_play_zoom ( MatScm *widget, double rate )
{
  MatWidgetPlay *data = data_get(widget);
  MatScm *un;
  MatScm *img;
  double nx, ny;

  un  = mat_movingobj_universe_get(mat_widget_play_self_get(widget));
  img = mat_movingobj_image_get(mat_widget_play_view_get(widget));

  ASSERT(un);

  nx = data->view_x * rate;

  if (mat_universe_mapsize_x_get(un) < (int) nx)
    return;

  if ((int) nx <= mat_image_size_x_get(img))
    return;

  ny = wp_calc_view(widget, nx);

  if (mat_universe_mapsize_y_get(un) < (int) ny)
    return;

  if ((int) ny <= mat_image_size_y_get(img))
    return;

  data->view_x = nx;
  data->view_y = ny;
}

static INLINE unsigned int
wp_view_pos_one ( unsigned int pos, unsigned int mapsize, unsigned int viewsize )
{
  ASSERT(pos < mapsize);
  ASSERT(viewsize < mapsize);

  if (pos < viewsize / 2)
    pos += mapsize;

  pos -= viewsize / 2;

  ASSERT(pos < mapsize);

  return pos;
}

static INLINE void
wp_view_pos ( MatWidgetPlayData *d )
{
  MatScm *o;
  MatScm *un;

  ASSERT(d);
  ASSERT(d->data);

  o   = mat_widget_play_view_get(d->data);
  ASSERT(o);
  ASSERT(mat_scm_obj_type_get(o) == &mat_scm_ship_type);

  un  = mat_movingobj_universe_get(o);
  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universe_type);

  d->startx = wp_view_pos_one(mat_movingobj_pos_x_center_get(o), mat_universe_mapsize_x_get(un), mat_widget_play_view_x_get(d->data));
  d->starty = wp_view_pos_one(mat_movingobj_pos_y_center_get(o), mat_universe_mapsize_y_get(un), mat_widget_play_view_y_get(d->data));
}

static INLINE void
mat_widget_play_bg ( MatWidgetPlayData *wpdata )
{
  MatWidgetPlay *data = data_get(wpdata->data);

  if (arg_bg_dots)
    {
      int i, j;

      for (i = 19 - (wpdata->starty * data->size_y / data->view_y) % 20; i < data->size_y; i += 20)
        for (j = 39 - (wpdata->startx * data->size_x / data->view_x) % 40; j < data->size_x; j += 40)
          mat_window_imgbuffer_set(data->window, j, data->size_y - i - 1, 100, 1.0);
    }
  else
    {
      static unsigned char pal[255];
      static int initpal = 0;

      if (!initpal)
        {
          int i;

          initpal = 1;
          for (i = 0; i < 255; i ++)
            pal[i] = i;
        }

      //mat_image_draw(self, pl, arg_background_img, 0, 0, startx, starty, pal);
    }
}

#if 0
static double
mat_widget_play_mist_calc ( MatWidgetPlayData *data, unsigned int x, unsigned int y )
{
  double dist_x, dist_y, total;

  ASSERT(x < data->data->size_x);
  ASSERT(y < data->data->size_y);

  if (x * 2 < data->data->size_x)
    dist_x = data->data->size_x / 2 - x;
  else
    dist_x = x - data->data->size_x / 2;

  if (y * 2 < data->data->size_y)
    dist_y = data->data->size_y / 2 - y;
  else
    dist_y = y - data->data->size_y / 2;

  ASSERT(dist_x >= 0.0);
  ASSERT(dist_y >= 0.0);

  dist_x *= data->data->view_x / data->data->size_x;
  dist_y *= data->data->view_y / data->data->size_y;

  ASSERT(dist_x >= 0.0);
  ASSERT(dist_y >= 0.0);

  total = (dist_x * dist_x + dist_y * dist_y) / (double) (MAT_MIST_COMPLETE * MAT_MIST_COMPLETE);

  ASSERT(total >= 0.0);

  return total >= 1.0 ? 1.0 : total;
}

static void
mat_widget_play_mist ( MatWidgetPlayData *data )
{
  unsigned int x, y;

  printf("mist: %d\n", data->data->size_y * data->data->size_x);

  for (y = 0; y < data->data->size_y; y ++)
    for (x = 0; x < data->data->size_x; x ++)
      mat_window_imgbuffer_set(data->data->window, x, y, 0, mat_widget_play_mist_calc(data, x, y));
}
#endif

static void
wp_redraw ( MatScm *window, MatScm *widget )
{
  MatWidgetPlayData data;

  ASSERT(window);
  ASSERT(widget);

  data.data = mat_widget_data_get(widget);
  data.window = window;
  data.un = mat_movingobj_universe_get(mat_widget_play_view_get(data.data));

  ASSERT(data.data);
  ASSERT(data.un);

  wp_view_pos(&data);

  mat_widget_play_bg(&data);

  mat_universe_movingobj_forall(data.un, mat_widget_play_draw_obj, &data);

#if 0
  if (mat_arg_mist)
    mat_widget_play_mist(&data);
#endif

  mat_asciirender_render_all(window, 1);

  wp_msg_draw(window, data.data);

  if (mat_arg_radar)
    {
      MatAngleCache *c;
      int a;

      mat_universe_movingobj_forall(data.un, wp_dir, &data);

      a = mat_movingobj_ang_get(mat_widget_play_view_get(data.data));
      c = mat_universe_angles_cache_get(data.un);

      wp_border(&data, mat_anglecache_cos(c, a), mat_anglecache_sin(c, a), '+');
    }

  mat_window_flush(window);
}

#if 0
static int
mat_widget_play_dist ( unsigned int shx, unsigned int objx, unsigned int unx )
{
  unsigned int m0, m1;
  int dir;

  if (shx <= objx)
    dir = 1, m0 = shx, m1 = objx;
  else
    dir = -1, m0 = objx, m1 = shx;

  if (m1 - m0 <= unx + m0 - m1)
    return dir * (m1 - m0);

  return -1 * dir * (unx + m0 - m1);
}
#endif

static void
wp_dir ( void *funcdata, MatScm *obj )
{
  MatWidgetPlayData *data = (MatWidgetPlayData *) funcdata;
  double distx, disty;
  char c;

  if (!mat_movingobj_image_get(obj))
    return;

  mat_distance_get(mat_widget_play_view_get(data->data), obj, &distx, &disty);

  if (distx == 0 && disty == 0)
    return;

  ASSERT(mat_movingobj_alive_get(obj));

  if (mat_movingobj_ship_get(obj))
    c = mat_scm_string_data_get(mat_ship_name_get(obj))[0];
  else
    c = '*';

  wp_border(data, distx, disty, c);
}

#if 0
static int
mat_widget_play_dist_calc ( MatWidgetPlayData *data, MatMovingObj *obj, int *distx, int *disty )
{
  MatMovingObj *sh;
  unsigned int objx, shx, objy, shy, unx, uny;
  MatImage *objimg, *shimg;

  sh = mat_movingobj_movingobj_get(data->data->ship);
  if (sh == obj)
    return 0;

  shimg = mat_movingobj_image_get(sh);
  objimg = mat_movingobj_image_get(obj);

  objx = mat_movingobj_pos_x_center_get(obj);
  shx  = mat_movingobj_pos_x_center_get(sh);
  unx = mat_universe_mapsize_x_get(data->un);

  if (unx <= objx)
    objx -= unx;

  if (unx <= shx)
    shx -= unx;

  ASSERT(objx < unx);
  ASSERT(shx  < unx);

  *distx = mat_widget_play_dist(shx, objx, unx);

  ASSERT( *distx <= mat_universe_mapsize_x_get(data->un) / 2);
  ASSERT(-*distx <= mat_universe_mapsize_x_get(data->un) / 2);

  objy = mat_movingobj_pos_y_get(obj) + mat_image_size_y_get(objimg) / 2;
  shy  = mat_movingobj_pos_y_get(sh)  + mat_image_size_y_get(shimg)  / 2;
  uny  = mat_universe_mapsize_y_get(data->un);

  if (uny <= objy)
    objy -= uny;

  if (uny <= shy)
    shy -= uny;

  ASSERT(objy < uny);
  ASSERT(shy  < uny);

  *disty = mat_widget_play_dist(shy, objy, uny);

  ASSERT( *disty <= mat_universe_mapsize_y_get(data->un) / 2);
  ASSERT(-*disty <= mat_universe_mapsize_y_get(data->un) / 2);

  if (   - (int) data->data->view_x / 2 < *distx && *distx <  (int) data->data->view_x / 2
      && - (int) data->data->view_y / 2 < *disty && *disty <  (int) data->data->view_y / 2)
    return 0;

  return *distx * *distx + *disty * *disty;
}
#endif

static void
wp_border ( MatWidgetPlayData *wpdata, double distx, double disty, char c )
{
  MatWidgetPlay *data = data_get(wpdata->data);
  double x, y;
  char buffer[] = { c, 0 };

  disty *= -1;

  if (ABS(disty) * (double) data->view_x < ABS(distx) * (double) data->view_y)
    {
      x = distx < 0 ? 0 : data->view_x - 1;
      y = ((double) data->view_y + (double) data->view_x * disty / ABS(distx)) / 2;
    }
  else if (ABS(disty) * (double) data->view_x > ABS(distx) * (double) data->view_y)
    {
      x = ((double) data->view_x + (double) data->view_y * distx / ABS(disty)) / 2;
      y = disty < 0 ? 0 : data->view_y - 1;
    }
  else
    {
      x = distx < 0 ? 0 : data->view_x - 1;
      y = disty < 0 ? 0 : data->view_y - 1;
    }

  ASSERT(0 <= x);
  ASSERT(0 <= y);
  ASSERT(x < data->view_x);
  ASSERT(y < data->view_y);
  ASSERT(x == 0 || x == data->view_x - 1 || y == 0 || y == data->view_y - 1);

  x *= (double) data->size_x / (double) (2 * data->view_x);
  y *= (double) data->size_y / (double) (2 * data->view_y);

  ASSERT(0 <= (int) x);
  ASSERT(0 <= (int) y);
  ASSERT((int) x < data->size_x / 2);
  ASSERT((int) y < data->size_y / 2);
  ASSERT((int) x == 0 || (int) x == data->size_x / 2 - 1 || (int) y == 0 || (int) y == data->size_y / 2 - 1);

  mat_window_print(wpdata->window, data->pos_x + x, data->pos_y + y, buffer, 0);
}

static INLINE void
wp_msg_draw ( MatScm *window, MatScm *widget )
{
  MatWidgetPlay *data = data_get(widget);
  char buffer[128];
  int pos;
  MatMessage *tmp;

  ASSERT(data);

  pos = data->msg_pos ? data->pos_y + data->size_y / 2 - 1 - data->msg_count - 1: data->pos_y + 1;

  if (mat_widget_play_self_get(widget) != mat_widget_play_view_get(widget))
    sprintf(buffer, "[name: %s][view: %s][health: %d]",
            mat_scm_string_data_get(mat_ship_name_get(mat_widget_play_self_get(widget))),
            mat_scm_string_data_get(mat_ship_name_get(mat_widget_play_view_get(widget))),
            mat_movingobj_health_get(mat_widget_play_self_get(widget)));
  else
    sprintf(buffer, "[name: %s][health: %d]",
            mat_scm_string_data_get(mat_ship_name_get(mat_widget_play_self_get(widget))),
            mat_movingobj_health_get(mat_widget_play_self_get(widget)));

  mat_window_print(window, 1, pos ++, buffer, 0);

  for (tmp = data->msg_head; tmp; tmp = mat_message_next_get(tmp), pos ++)
    {
      if (mat_message_repeat_get(tmp))
        {
          char b[64];
          mat_window_print(window, 1 + sprintf(b, "%d: ", mat_message_repeat_get(tmp) + 1), pos, mat_message_text_get(tmp), 0);
          mat_window_print(window, 1, pos, b, 0);
        }
      else
        mat_window_print(window, 1, pos, mat_message_text_get(tmp), 0);
    }

  if (data->key == mat_widget_play_key_chat)
    { 
      pos = !data->msg_pos ? data->pos_y + data->size_y / 2 - 2 : data->pos_y + 1;
      mat_window_print(window, 1, pos, "> ", 0);
      mat_window_print(window, 3, pos, mat_widget_play_chat_buffer_get(widget), 0);
    }
}
 
void
mat_widget_play_printf ( MatScm *widget, char *fmt, ... )
{
  va_list args;
  va_start(args, fmt);
  mat_widget_play_vprintf(widget, fmt, args);
  va_end(args);
}

void
mat_widget_play_vprintf ( MatScm *widget, char *fmt, va_list args )
{
  MatWidgetPlay *data = data_get(widget);
  char buffer[2048];
  MatMessage *msg;

  ASSERT(widget);
  ASSERT(fmt);

  /* TODO: Arg, find out some way to make sure they never vfprintf something
   * longer than our buffer.  Or use some other function. */

  vsprintf(buffer, fmt, args);

  if (data->msg_tail && !strcmp(buffer, mat_message_text_get(data->msg_tail)))
    {
      mat_message_repeat_set(data->msg_tail, 1 + mat_message_repeat_get(data->msg_tail));
      return;
    }

  msg = mat_message_new(buffer);

  mat_message_prev_set(msg, data->msg_tail);

  if (data->msg_tail)
    mat_message_next_set(data->msg_tail, msg);
  else
    data->msg_head = msg;

  data->msg_tail = msg;

  if (++ data->msg_count > MATANZA_WIDGET_PLAY_MSG_MAX)
    mat_widget_play_msg_rm(widget);
}

#if 0
static INLINE int
mat_widget_play_ship_in ( MatWidgetPlay *data, MatMovingObj *b )
{
  MatMovingObj *a;
  MatUniverse *un;
  MatImage *img;
  double ax, ay, bx, by;

  ASSERT(data);
  ASSERT(data->ship);

  a = mat_ship_movingobj_get(data->ship);

  ASSERT(a);
  ASSERT(b);

  if (a == b)
    return 1;

  ax = mat_movingobj_pos_x_get(a);
  ay = mat_movingobj_pos_y_get(a);

  bx = mat_movingobj_pos_x_get(b);
  by = mat_movingobj_pos_y_get(b);

  un  = mat_movingobj_universe_get(a);
  img = mat_movingobj_image_get(b);

  if (!mat_widget_play_ship_coord(ax - bx, mat_universe_mapsize_x_get(un), data->view_x + mat_image_size_x_get(img)))
    return 0;

  if (!mat_widget_play_ship_coord(ay - by, mat_universe_mapsize_y_get(un), data->view_y + mat_image_size_y_get(img)))
    return 0;

  return 1;
}
#endif

static INLINE int
mat_widget_play_ship_coord ( double diff, double map, double view )
{
  if (diff < 0)
    diff = -diff;

  return 2 * diff < view || 2 * (map - diff) < view;
}

static void
wp_free ( MatScm *widget )
{
#if 0
  MatWidgetPlay *data;

  ASSERT(widget);

  data = (MatWidgetPlay *) mat_widget_data_get(widget);

  ASSERT(data);
  ASSERT(data->ship);
  ASSERT(data->self);
  ASSERT(mat_ship_widget_play_get(data->self) == data);

  while (data->msg_count)
    mat_widget_play_msg_rmv(data);

  mat_movingobj_view_rm(data->ship, data);

  if (mat_ship_alive_get(data->self))
    mat_ship_remove(data->self);

  mat_ship_widget_play_set(data->self, NULL);

  mat_movingobj_refs_dec(mat_ship_movingobj_get(data->self));

  free(data);
#endif
  free(mat_scm_obj_data_get(widget));
}

INLINE void
mat_widget_play_view_reset ( MatScm *data )
{
  view_set(data, mat_widget_play_self_get(data));
}

INLINE MatScm *
mat_widget_play_self_get ( MatScm *widget )
{
  return data_get(widget)->self;
}

INLINE MatScm *
mat_widget_play_view_get ( MatScm *widget )
{
  return data_get(widget)->ship;
}

static void
view_set ( MatScm *widget, MatScm *ship )
{
  MatWidgetPlay *data = data_get(widget);

  if (mat_widget_play_view_get(widget))
    {
      if (mat_widget_play_view_get(widget) == ship)
        return;

      if (ship != mat_widget_play_self_get(widget))
        mat_widget_play_printf(widget, "-=> Viewing the world as [%s] . . .", mat_ship_name_get(ship));
      else
        mat_widget_play_printf(widget, "-=> Back to your view . . .");

        mat_movingobj_view_rm(mat_widget_play_view_get(widget), data->ship_list);
    }

  data->ship = ship;
  data->ship_list = mat_movingobj_view_add(ship, widget);

  /* Just in case we land on a different image. */
  mat_widget_play_zoom(widget, 1.0);
}

INLINE unsigned int
mat_widget_play_size_x_get ( MatScm *widget )
{
  return data_get(widget)->size_x;
}

INLINE unsigned int
mat_widget_play_size_y_get ( MatScm *widget )
{
  return data_get(widget)->size_y;
}

INLINE unsigned int
mat_widget_play_view_x_get ( MatScm *widget )
{
  return data_get(widget)->view_x;
}

INLINE unsigned int
mat_widget_play_view_y_get ( MatScm *widget )
{
  return data_get(widget)->view_y;
}

INLINE MatScm *
mat_widget_play_window_get ( MatScm *widget )
{
  return data_get(widget)->window;
}

INLINE void
mat_widget_play_key_set ( MatScm *widget, int (*key) ( MatScm *, int ) )
{
  data_get(widget)->key = key;
}

INLINE void
mat_widget_play_chat_len_set ( MatScm *widget, unsigned int len )
{
  data_get(widget)->chat_len = len;
}

INLINE char *
mat_widget_play_chat_buffer_get ( MatScm *widget )
{
  return data_get(widget)->chat_buf;
}

INLINE int
mat_widget_play_chat_len_get ( MatScm *widget )
{
  return data_get(widget)->chat_len;
}

#if 0
INLINE MatWidgetPlay *
mat_widget_play_view_prev_get ( MatWidgetPlay *data )
{
  ASSERT(data);
  return data->ship_prev;
}

INLINE MatWidgetPlay *
mat_widget_play_view_next_get ( MatWidgetPlay *data )
{
  ASSERT(data);
  return data->ship_next;
}

INLINE void
mat_widget_play_view_prev_set ( MatWidgetPlay *data, MatWidgetPlay *prev )
{
  ASSERT(data);
  data->ship_prev = prev;
}

INLINE void
mat_widget_play_view_next_set ( MatWidgetPlay *data, MatWidgetPlay *next )
{
  ASSERT(data);
  data->ship_next = next;
}
#endif

INLINE void
mat_widget_play_pos_swap ( MatScm *widget )
{
  MatWidgetPlay *data = data_get(widget);
  data->msg_pos = !data->msg_pos;
}

INLINE void
mat_widget_play_msg_rm ( MatScm *widget )
{
  MatWidgetPlay *data = data_get(widget);
  MatMessage *old;

  if (!data->msg_count)
    return;

  ASSERT(data->msg_count);
  ASSERT(data->msg_head);
  ASSERT(data->msg_tail);

  data->msg_count --;

  old = data->msg_head;

  data->msg_head = mat_message_next_get(data->msg_head);

  if (!data->msg_head)
    data->msg_tail = NULL;
  else
    mat_message_prev_set(data->msg_head, NULL);

  mat_message_free(old);
}

INLINE void
mat_widget_play_msg_clean ( MatScm *widget)
{
  MatWidgetPlay *data = data_get(widget);
  while (data->msg_count)
    mat_widget_play_msg_rm(widget);
}

static int
wp_key ( MatScm *window, MatScm *widget, int key )
{
  return data_get(mat_widget_data_get(widget))->key(widget, key);
}
