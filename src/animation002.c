#include <matanza.h>

typedef struct MatWidgetAnimation002 MatWidgetAnimation002;

struct MatWidgetAnimation002
{
  int delta;
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

static MatScm            *a_new                            ( void );

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

MatScmType mat_scm_animation002_type =
{
  "animation002",
  free,
  NULL,
  NULL,
  NULL,
  NULL
};

void
mat_animation_002_play ( MatScm *window )
{
  ASSERT(window);

  mat_window_reset(window);

  mat_window_randomval_set(window, 60);

  mat_widget_window_add(a_new(), window, NULL);
}

static MatScm *
a_new ( void )
{
  MatWidgetAnimation002 *data;

  data = malloc(sizeof(MatWidgetAnimation002));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->delta = 3;

  return mat_widget_new(&a_desc, mat_scm_obj_malloc(&mat_scm_animation002_type, data));
}

static void
a_redraw ( MatScm *window, MatScm *widget )
{
  MatWidgetAnimation002 *data;

  ASSERT(window);
  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_animation002_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  mat_window_randomval_set(window, mat_window_randomval_get(window) + data->delta);
  mat_asciirender_render_all(window, 1);
  mat_window_flush(window);
}

static int
a_update ( MatScm *window, MatScm *widget )
{
  MatWidgetAnimation002 *data;

  ASSERT(window);
  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_animation002_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  if (mat_window_randomval_get(window) > 120)
    data->delta = -1;
  else if (mat_window_randomval_get(window) < 20)
    {
      // free(data);  This will come at a latter time
      mat_animation_fire_play(window);
      return 0;
    }

  return 1;
}

static int
a_key ( MatScm *window, MatScm *widget, int key )
{
  MatWidgetAnimation002 *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_animation002_type);
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
