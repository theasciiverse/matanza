#include <matanza.h>

typedef struct MatWidgetAnimation000 MatWidgetAnimation000;

struct MatWidgetAnimation000
{
  unsigned int width, height;
  unsigned int pos_x, pos_y;

  int mode;
  int colors[12];
  int delay;
  int next;
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

static char *text0[] = { "F", "r", "e", "a", "k", "s" };
static char *text1[] = { "U", "n", "i", "d", "o", "s" };

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

MatScmType mat_scm_animation000_type =
{
  "animation000",
  free,
  NULL,
  NULL,
  NULL,
  NULL
};

void
mat_animation_000_play ( MatScm *window )
{
  ASSERT(window);
  ASSERT(mat_scm_obj_type_get(window) == &mat_scm_window_type);

  mat_window_reset(window);

  mat_window_randomval_set(window, 60);

  mat_widget_window_add(a_new(), window, NULL);
}

static MatScm *
a_new ( void )
{
  MatWidgetAnimation000 *data;
  int i;

  data = malloc(sizeof(MatWidgetAnimation000));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  for (i = 0; i < 12; i ++)
    data->colors[i] = 0;

  data->next  = 0;
  data->delay = 30;

  return mat_widget_new(&a_desc, mat_scm_obj_malloc(&mat_scm_animation000_type, data));
}

static int
a_update ( MatScm *window, MatScm *widget )
{
  MatWidgetAnimation000 *data;

  ASSERT(window);
  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_animation000_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  if (data->delay == 0)
    {
      if (data->next != 6)
        {
          data->colors[data->next] = 255;
          data->colors[11 - data->next] = 255;
          data->next++;
          data->delay = 12;
        }
      else
        data->delay--;
    }
  else
    {
      int i;

      if (data->delay == -1)
        {
          if (data->colors[5] == 0)
            {
              free(data);
              mat_animation_001_play(window);
              return 0;
            }
        }
      else
        data->delay --;

      for (i = 0; i < 12; i ++)
        if (data->colors[i])
          data->colors[i] -= 3;
    }

  return 1;
}

static void
a_redraw ( MatScm *window, MatScm *widget )
{
  MatWidgetAnimation000 *data;
  int i;

  ASSERT(window);
  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_animation000_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  for (i = 0; i < 6; i ++)
    if (data->colors[i])
      mat_asciirender_print(window, data->pos_x + data->width * i, 0,
                            data->width, data->height,
                            graph_font, data->colors[i], text0[i]);

  for (i = 6; i < 12; i ++)
    if (data->colors[i])
      mat_asciirender_print(window, data->pos_x + data->width * (i - 6), data->pos_y,
                            data->width, data->height,
                            graph_font, data->colors[i], text1[i - 6]);

  mat_asciirender_render_all(window, 1);
  mat_window_flush(window);
}

static void
a_resize ( MatScm *window, MatScm *widget )
{
  MatWidgetAnimation000 *data;

  ASSERT(window);
  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_animation000_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  data->width  = mat_window_img_size_x_get (window) / (1 + sizeof(text0) / sizeof(char *));
  data->height = mat_window_img_size_y_get(window) / 2;

  data->pos_x  = (mat_window_img_size_x_get (window) - data->width * (sizeof(text0) / sizeof(char *))) / 2;
  data->pos_y  = mat_window_img_size_y_get(window) / 2;
}

static int
a_key ( MatScm *window, MatScm *widget, int key )
{
  MatWidgetAnimation000 *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_animation000_type);
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
