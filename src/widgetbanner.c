#include <matanza.h>

typedef struct MatWidgetBanner MatWidgetBanner;

struct MatWidgetBanner
{
  int row;
  MatScm *text;
  MatScm *cmsg;
  int cpos;
};

static int  b_update ( MatScm *, MatScm * );
static void b_redraw ( MatScm *, MatScm * );

static MatWidgetTypeDesc b_desc =
{
  NULL,
  NULL,
  NULL,
  NULL,
  b_update,
  b_redraw,
  NULL,
  NULL
};

MatScmType mat_scm_widgetbanner_type =
{
  "widgetbanner",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

MatScm *
mat_widget_banner_new ( MatScm *window, MatScm *text, int row )
{
  MatWidgetBanner *data;

  ASSERT(text);
  ASSERT(mat_scm_obj_type_get(text) == &mat_scm_cons_type);

  data = malloc(sizeof(MatWidgetBanner));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->row  = row;
  data->text = text;
  data->cmsg = text;

  data->cpos = 0;

  return mat_widget_new(&b_desc, mat_scm_obj_malloc(&mat_scm_widgetbanner_type, data));
}

static int
b_update ( MatScm *window, MatScm *widget )
{
  MatWidgetBanner *data;
  unsigned int size_x;

  ASSERT(window);
  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetbanner_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  size_x = mat_window_chars_size_x_get(window);

  ASSERT(data->cmsg);
  ASSERT(mat_scm_obj_type_get(data->cmsg) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(data->cmsg));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(data->cmsg)) == &mat_scm_string_type);

  if (data->cpos >= size_x && data->cpos - size_x >= mat_scm_string_length_get(mat_scm_cons_car_get(data->cmsg)))
    {
      data->cpos = 0;
      data->cmsg = mat_scm_cons_cdr_get(data->cmsg);

      if (!data->cmsg)
        data->cmsg = data->text;
    }

  data->cpos ++;

  return 1;
}

static void
b_redraw ( MatScm *window, MatScm *widget )
{
  MatWidgetBanner *data;
  unsigned int i, size_x;
  int fzero = 0;
  char *text;

  ASSERT(window);
  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetbanner_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  ASSERT(data->cmsg);
  ASSERT(mat_scm_obj_type_get(data->cmsg) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(data->cmsg));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(data->cmsg)) == &mat_scm_string_type);

  text  = mat_scm_string_data_get(mat_scm_cons_car_get(data->cmsg));
  size_x = mat_window_chars_size_x_get(window);

  for (i = 0; i < size_x; i ++)
    if (data->cpos + i < size_x|| fzero)
      {
        mat_window_textbuffer_set(window, i, data->row, ' ', MAT_NORMAL);
      }
    else if (text[data->cpos + i - size_x] == 0)
      {
        mat_window_textbuffer_set(window, i, data->row, ' ', MAT_NORMAL);
        fzero = 1;
      }
    else
      mat_window_textbuffer_set(window, i, data->row, text[data->cpos + i - size_x], MAT_NORMAL);
}
