#include <matanza.h>

typedef struct MatWidgetRender MatWidgetRender;

struct MatWidgetRender
{
  int         x;
  int         y;
  MatScm   *(*redraw) ( void *data );
  int       (*update) ( void *data );
  void       *userdata;
};

static int  mat_widget_render_update ( MatScm *window, MatScm *data );
static void mat_widget_render_redraw ( MatScm *window, MatScm *data );

static MatWidgetTypeDesc mat_widget_render_type_desc =
{
  NULL,
  NULL,
  NULL,
  NULL,
  mat_widget_render_update,
  mat_widget_render_redraw,
  NULL,
  NULL
};

MatScmType mat_scm_widgetrender_type =
{
  "widgetrender",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

MatScm *
mat_widget_render_new ( MatScm *window, int x, int y, MatScm *(*redraw) (void *), int (*update) (void *), void *userdata )
{
  MatWidgetRender *data;

  ASSERT(window);

  data = malloc(sizeof(MatWidgetRender));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->x = x;
  data->y = y;

  data->redraw   = redraw;
  data->update   = update;
  data->userdata = userdata;

  return mat_widget_new(&mat_widget_render_type_desc, mat_scm_obj_malloc(&mat_scm_widgetrender_type, data));
}

static int
mat_widget_render_update ( MatScm *window, MatScm *widget )
{
  MatWidgetRender *data;

  ASSERT(window);
  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetrender_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  return data->update && data->update(data->userdata);
}

static void
mat_widget_render_redraw ( MatScm *window, MatScm *widget )
{
  MatWidgetRender *data;
  MatScm *img;
  unsigned int x, y, h, w;

  ASSERT(window);
  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetrender_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  img = data->redraw(data->userdata);

  if (!img)
    return;

  ASSERT(img);

  w = mat_image_size_x_get(img);
  h = mat_image_size_y_get(img);

  for (y = 0; y < h; y ++)
    for (x = 0; x < w; x ++)
      mat_window_imgbuffer_set(window, data->x * 2 + x, data->y * 2 + y, mat_image_core_get(img, x, y), 1.0);

  mat_asciirender_render(window, data->x, data->y, data->x + w / 2, data->y + h / 2, 0);

  //mat_asciirender_render(window, 0, 0, mat_widget_window_chars_x_get(window), mat_widget_window_chars_y_get(window), wipe);
  //mat_asciirender_render_all(window, 1);

  mat_window_flush(window);
}
