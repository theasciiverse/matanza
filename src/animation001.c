#include <matanza.h>

typedef struct MatWidgetAnimation001 MatWidgetAnimation001;

struct MatWidgetAnimation001
{
  unsigned int width, height;
  unsigned int pos_x, pos_y;
  double head, delta, delay;
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

static void               a_resize                         ( MatScm             *window,
                                                             MatScm             *widget );

static MatScm            *a_new                            ( void );

static char *text[] = { "P", "r", "e", "s", "e", "n", "t", "s" };

static MatWidgetTypeDesc a_desc =
{
  a_focus,
  NULL,
  NULL,
  a_key,
  a_update,
  a_redraw,
  a_resize,
  NULL
};

MatScmType mat_scm_animation001_type =
{
  "animation001",
  free,
  NULL,
  NULL,
  NULL,
  NULL
};

void
mat_animation_001_play ( MatScm *window )
{
  ASSERT(window);

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> mat_animation_001_play\n");

  mat_window_reset(window);
  mat_window_randomval_set(window, 60);
  mat_widget_window_add(a_new(), window, NULL);

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- mat_animation_001_play\n");
}

static MatScm *
a_new ( void )
{
  MatWidgetAnimation001 *data;

  data = malloc(sizeof(MatWidgetAnimation001));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->delay = 0;
  data->delta = 0.3;
  data->head  = -7;

  return mat_widget_new(&a_desc, mat_scm_obj_malloc(&mat_scm_animation001_type, data));
}

static void
a_redraw ( MatScm *window, MatScm *widget )
{
  MatWidgetAnimation001 *data;
  
  int i;
  double c;

  ASSERT(window);
  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_animation001_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  for (i = 0; i < sizeof(text) / sizeof(char *); i++)
    {
      ASSERT(text[i]);

      c = 36.0 * ((double) i - (double)data->head);
      c = i >= data->head ? 255 - c : 255 + c;

      if (c < 0)
        c = 0;

      mat_asciirender_print(window, data->pos_x + data->width * i, data->pos_y,
                            data->width, data->height,
                            graph_font, c, text[i]);
    }

  mat_asciirender_render_all(window, 1);
  mat_window_flush(window);
}

static int
a_update ( MatScm *window, MatScm *widget )
{
  MatWidgetAnimation001 *data;

  ASSERT(window);
  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_animation001_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);


  if (data->head <= -4)
    data->delta = +0.5;
  else if (data->head >= 11)
    {
      if (data->delay < 1)
        data->delta = -0.5;
      else if (data->delay > 20)
        {
          free(data);
          mat_animation_002_play(window);
          return 0;
        }

      data->delay++;
    }

  data->head += data->delta;

  return 1;
}

static void
a_resize ( MatScm *window, MatScm *widget )
{
  MatWidgetAnimation001 *data;

  ASSERT(window);
  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_animation001_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  data->width  = mat_window_img_size_x_get (window) / (1 + sizeof(text) / sizeof(char *));
  data->height = mat_window_img_size_y_get(window) / 2;

  data->pos_x  = (mat_window_img_size_x_get (window) - data->width  * (sizeof(text) / sizeof(char *))) / 2;
  data->pos_y  = (mat_window_img_size_y_get(window) - data->height) / 2;
}

static int
a_key ( MatScm *window, MatScm *widget, int key )
{
  MatWidgetAnimation001 *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_animation001_type);
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
