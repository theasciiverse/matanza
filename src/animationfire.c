/* There is a bug here somewhere that is causing the fire to move to the
 * left.  Probably an off-by-one error somewhere. */

#include <matanza.h>

#define MAXTABLE (256*5)

typedef struct MatWidgetAnimationFire MatWidgetAnimationFire;

struct MatWidgetAnimationFire
{
  unsigned int table[MAXTABLE];
  int height, loop;
  int done, curr, text_pos;
};

static int                a_focus                          ( MatScm             *window,
                                                             MatScm             *widget );

static int                a_key                            ( MatScm             *window,
                                                             MatScm             *w,
                                                             int                 key );

static int                a_update                         ( MatScm             *window,
                                                             MatScm             *widget );

static void               a_redraw                         ( MatScm             *window,
                                                             MatScm             *widget );

static MatScm            *a_new                            ( MatScm             *window );

static MatWidgetTypeDesc a_desc =
{
  a_focus,
  NULL,
  NULL,
  a_key,
  a_update,
  a_redraw,
  NULL,
  NULL
};

MatScmType mat_scm_animationfire_type =
{
  "animationfire",
  free,
  NULL,
  NULL,
  NULL,
  NULL
};

#define COLORRGB(r,g,b) (((r)*30+(g)*59+(b)*11)>>8)

#define FP(j) (fire_pal[i * 3 + j] * 4)

static int fire_pal[] =
{
  0, 0, 0, 0, 0, 6, 0, 0, 6, 0, 0, 7, 0, 0, 8, 0, 0, 8, 0, 0, 9, 0, 0, 10,
  2, 0, 10, 4, 0, 9, 6, 0, 9, 8, 0, 8, 10, 0, 7, 12, 0, 7, 14, 0, 6, 16, 0, 5,
  18, 0, 5, 20, 0, 4, 22, 0, 4, 24, 0, 3, 26, 0, 2, 28, 0, 2, 30, 0, 1, 32, 0, 0,
  32, 0, 0, 33, 0, 0, 34, 0, 0, 35, 0, 0, 36, 0, 0, 36, 0, 0, 37, 0, 0, 38, 0, 0,
  39, 0, 0, 40, 0, 0, 40, 0, 0, 41, 0, 0, 42, 0, 0, 43, 0, 0, 44, 0, 0, 45, 0, 0,
  46, 1, 0, 47, 1, 0, 48, 2, 0, 49, 2, 0, 50, 3, 0, 51, 3, 0, 52, 4, 0, 53, 4, 0,
  54, 5, 0, 55, 5, 0, 56, 6, 0, 57, 6, 0, 58, 7, 0, 59, 7, 0, 60, 8, 0, 61, 8, 0,
  63, 9, 0, 63, 9, 0, 63, 10, 0, 63, 10, 0, 63, 11, 0, 63, 11, 0, 63, 12, 0, 63, 12, 0,
  63, 13, 0, 63, 13, 0, 63, 14, 0, 63, 14, 0, 63, 15, 0, 63, 15, 0, 63, 16, 0, 63, 16, 0,
  63, 17, 0, 63, 17, 0, 63, 18, 0, 63, 18, 0, 63, 19, 0, 63, 19, 0, 63, 20, 0, 63, 20, 0,
  63, 21, 0, 63, 21, 0, 63, 22, 0, 63, 22, 0, 63, 23, 0, 63, 24, 0, 63, 24, 0, 63, 25, 0,
  63, 25, 0, 63, 26, 0, 63, 26, 0, 63, 27, 0, 63, 27, 0, 63, 28, 0, 63, 28, 0, 63, 29, 0,
  63, 29, 0, 63, 30, 0, 63, 30, 0, 63, 31, 0, 63, 31, 0, 63, 32, 0, 63, 32, 0, 63, 33, 0,
  63, 33, 0, 63, 34, 0, 63, 34, 0, 63, 35, 0, 63, 35, 0, 63, 36, 0, 63, 36, 0, 63, 37, 0,
  63, 38, 0, 63, 38, 0, 63, 39, 0, 63, 39, 0, 63, 40, 0, 63, 40, 0, 63, 41, 0, 63, 41, 0,
  63, 42, 0, 63, 42, 0, 63, 43, 0, 63, 43, 0, 63, 44, 0, 63, 44, 0, 63, 45, 0, 63, 45, 0,
  63, 46, 0, 63, 46, 0, 63, 47, 0, 63, 47, 0, 63, 48, 0, 63, 48, 0, 63, 49, 0, 63, 49, 0,
  63, 50, 0, 63, 50, 0, 63, 51, 0, 63, 52, 0, 63, 52, 0, 63, 52, 0, 63, 52, 0, 63, 52, 0,
  63, 53, 0, 63, 53, 0, 63, 53, 0, 63, 53, 0, 63, 54, 0, 63, 54, 0, 63, 54, 0, 63, 54, 0,
  63, 54, 0, 63, 55, 0, 63, 55, 0, 63, 55, 0, 63, 55, 0, 63, 56, 0, 63, 56, 0, 63, 56, 0,
  63, 56, 0, 63, 57, 0, 63, 57, 0, 63, 57, 0, 63, 57, 0, 63, 57, 0, 63, 58, 0, 63, 58, 0,
  63, 58, 0, 63, 58, 0, 63, 59, 0, 63, 59, 0, 63, 59, 0, 63, 59, 0, 63, 60, 0, 63, 60, 0,
  63, 60, 0, 63, 60, 0, 63, 60, 0, 63, 61, 0, 63, 61, 0, 63, 61, 0, 63, 61, 0, 63, 62, 0,
  63, 62, 0, 63, 62, 0, 63, 62, 0, 63, 63, 0, 63, 63, 1, 63, 63, 2, 63, 63, 3, 63, 63, 4,
  63, 63, 5, 63, 63, 6, 63, 63, 7, 63, 63, 8, 63, 63, 9, 63, 63, 10, 63, 63, 10, 63, 63, 11,
  63, 63, 12, 63, 63, 13, 63, 63, 14, 63, 63, 15, 63, 63, 16, 63, 63, 17, 63, 63, 18, 63, 63, 19,
  63, 63, 20, 63, 63, 21, 63, 63, 21, 63, 63, 22, 63, 63, 23, 63, 63, 24, 63, 63, 25, 63, 63, 26,
  63, 63, 27, 63, 63, 28, 63, 63, 29, 63, 63, 30, 63, 63, 31, 63, 63, 31, 63, 63, 32, 63, 63, 33,
  63, 63, 34, 63, 63, 35, 63, 63, 36, 63, 63, 37, 63, 63, 38, 63, 63, 39, 63, 63, 40, 63, 63, 41,
  63, 63, 42, 63, 63, 42, 63, 63, 43, 63, 63, 44, 63, 63, 45, 63, 63, 46, 63, 63, 47, 63, 63, 48,
  63, 63, 49, 63, 63, 50, 63, 63, 51, 63, 63, 52, 63, 63, 52, 63, 63, 53, 63, 63, 54, 63, 63, 55,
  63, 63, 56, 63, 63, 57, 63, 63, 58, 63, 63, 59, 63, 63, 60, 63, 63, 61, 63, 63, 62, 63, 63, 63
};

void
mat_animation_fire_play ( MatScm *window )
{
  ASSERT(window);

  mat_window_reset(window);
  mat_window_randomval_set(window, 0);
  mat_widget_window_add(a_new(window), window, NULL);
}

static MatScm *
a_new ( MatScm *window )
{
  MatWidgetAnimationFire *data;

  int minus;
  unsigned int i;
  unsigned int *pal;

  data = malloc(sizeof(MatWidgetAnimationFire));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  minus = 800 / (mat_window_img_size_y_get(window) - 4);

  if (minus == 0)
    minus = 1;

  for (i = 0; i < MAXTABLE; i++)
    {
      if (i > minus)
        data->table[i] = (i - minus) / 5;
      else
        data->table[i] = 0;
    }

  pal = mat_window_palette_get(window);

  ASSERT(pal);

  for (i = 0; i < 256; i++)
    pal[i] = COLORRGB(FP(0), FP(1), FP(2));

  data->loop = data->height = 0;

  data->text_pos = 0;
  data->done = mat_window_img_size_y_get(window) / 3;
  data->curr = 0;

  return mat_widget_new(&a_desc, mat_scm_obj_malloc(&mat_scm_animationfire_type, data));
}

#define min(x,y) ((x)<(y)?(x):(y))

#define CALC(m) \
  (mat_telnet_imgbuffer_get(mat_window_telnet_get(window)) \
   + mat_window_img_size_x_get(window) * (mat_window_img_size_y_get(window) - m))

#define CALC1(y,x) (*(c + y * mat_window_img_size_x_get(window) + x))

static void
a_redraw ( MatScm *window, MatScm *widget )
{
  ASSERT(window);
  mat_asciirender_render_all(window, 0);
  mat_window_flush(window);
} 

static int
a_update ( MatScm *window, MatScm *widget )
{
  MatWidgetAnimationFire *data;

  static char *text = "Welcome to Matanza, the multiplayer, action, "
  "network-based, real time, ASCII art based, spaceship, slogan lacking game."
  "                                                      "
  "Matanza is currently under development by Alejandro Forero Cuervo of the "
  "Freaks Unidos.  You can find more information on this organization on the "
  "web at <http://bachue.com>."
  "                                                      ";

  char buffer[2];
  unsigned char *c;
  unsigned int i, last1, i1, i2;

  ASSERT(window);
  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_animationfire_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  data->height ++;
  data->loop--;

  if (data->loop < 0)
    data->loop = rand() % 3;

  i1 = 1;
  i2 = 4 * mat_window_img_size_x_get(window) + 1;

  ASSERT(CALC(3) - CALC(4) == mat_window_img_size_x_get(window));

  for (c = CALC(4); c < CALC(3); c++, i1 += 4, i2 -= 4)
    {
      last1 = rand() % min(i1, min(i2, data->height));
      i = rand() % 6;
      while (c < CALC(3) && i != 0)
        {
	  *(c                          ) = last1;
          last1 += rand() % 6 - 2;
          *(c +     mat_window_img_size_x_get(window)) = last1;
          last1 += rand() % 6 - 2;
          *(c + 2 * mat_window_img_size_x_get(window)) = last1;
          last1 += rand() % 6 - 2;

          c++;
          i--;
          i1 += 4;
          i2 -= 4;
        }
    }


  c = mat_telnet_imgbuffer_get(mat_window_telnet_get(window));

  while (c <= CALC(4))
    *(c++) = data->table[CALC1(1,-1)+CALC1(1,1)+CALC1(1,0)+CALC1(2,-1)+CALC1(2,1)];

  data->text_pos++;

  if (text[MAX(data->text_pos - mat_window_img_size_x_get(window) + 1, 0)] == 0)
    data->text_pos = 0;

  if (!data->done --)
    {
      data->done = mat_window_img_size_y_get(window) / 3;

      if (data->curr >= 7)
        {
          if (data->curr ++ == 13)
            data->curr = 0;
        }
      else
        {
          buffer[0] = "Matanza"[data->curr++];
          buffer[1] = 0;

          mat_asciirender_print(window,
                                mat_window_img_size_x_get(window)  / 8,
                                mat_window_img_size_y_get(window) / 10,
                                6 * mat_window_img_size_x_get(window)  / 8,
                                8 * mat_window_img_size_y_get(window) / 10,
                                graph_font, 255, buffer);
        }
    }

  return 1;
}

static int
a_key ( MatScm *window, MatScm *widget, int key )
{
  MatWidgetAnimationFire *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_animationfire_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  ASSERT(mat_widget_focus_get(widget));
  ASSERT(data);

  switch (key)
    {
    case 13:
      free(data);
      mat_menu0_start(window);
      break;

    default:
      return 0;
    }

  return 1;
}

static int
a_focus ( MatScm *window, MatScm *widget )
{
  return 1;
}
