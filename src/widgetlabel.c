#include <matanza.h>

typedef struct MatWidgetLabel MatWidgetLabel;

struct MatWidgetLabel
{
  MatScm *text;
  char    attr;
  int   x, y;

  unsigned int            width;

  MatWidgetLabelJust      just;

  unsigned int            text_start;
  unsigned int            text_end;
};

static void           l_redraw                               ( MatScm          *window,
                                                               MatScm          *w );

static MatWidgetTypeDesc l_desc =
{
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  l_redraw,
  NULL,
  NULL
};

MatScmType mat_scm_widgetlabel_type =
{
  "widgetlabel",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

MatScm *
mat_widget_label_new ( MatScm *text, char attr, int x, int y, int width, MatWidgetLabelJust j )
{
  MatWidgetLabel *data;
  MatScm *label;

  data = malloc(sizeof(MatWidgetLabel));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->attr       = attr;
  data->x          = x;
  data->y          = y;

  data->width      = width;
  data->just       = j;

  label = mat_widget_new(&l_desc, mat_scm_obj_malloc(&mat_scm_widgetlabel_type, data));

  mat_widget_label_text_set(label, text);

  return label;
}

static void
l_redraw ( MatScm *window, MatScm *widget )
{
  MatWidgetLabel *data;
  unsigned int i, pos;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetlabel_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  if (!data->text)
    return;

  ASSERT(mat_scm_obj_type_get(data->text) == &mat_scm_string_type);

  for (i = pos = 0; i < data->width; i ++)
    if (i < data->text_start || i >= data->text_end)
      mat_window_textbuffer_set(window, data->x + i, data->y, ' ', data->attr);
    else
      mat_window_textbuffer_set(window, data->x + i, data->y, mat_scm_string_data_get(data->text)[pos ++], data->attr);

  mat_window_flush(window);
}

INLINE void
mat_widget_label_text_set ( MatScm *widget, MatScm *text )
{
  MatWidgetLabel *data;
  unsigned int len;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetlabel_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  data->text = text;

  if (!data->text)
    return;

  ASSERT(mat_scm_obj_type_get(data->text) == &mat_scm_string_type);

  switch (data->just)
    {
    case MAT_WIDGET_LABEL_JUST_LEFT:
      data->text_start = 0;
      break;
    case MAT_WIDGET_LABEL_JUST_CENTER:
      data->text_start = (data->width - mat_scm_string_length_get(text)) / 2;
      break;
    case MAT_WIDGET_LABEL_JUST_RIGHT:
      data->text_start = data->width - mat_scm_string_length_get(text);
      break;
    }

  data->text_end = data->text_start + mat_scm_string_length_get(text);
}
