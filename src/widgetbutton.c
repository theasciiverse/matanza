#include <matanza.h>

typedef struct MatWidgetButton MatWidgetButton;

struct MatWidgetButton
{
  MatScm *text;

  int   x, y, width;

  int   color;

  void *focus_data;
  MatWidgetEvent focus_got_func;
  MatWidgetEvent focus_lost_func;

  void *click_data;
  MatWidgetEvent click;
};

static int            b_focus_check                          ( MatScm          *window,
                                                               MatScm    *w );

static void           b_focus_got                            ( MatScm          *window,
                                                               MatScm    *w );

static void           b_focus_lost                           ( MatScm          *window,
                                                               MatScm    *w );

static int            b_key                                  ( MatScm          *window,
                                                               MatScm    *w,
                                                               int key );

static void           b_redraw                               ( MatScm          *window,
                                                               MatScm    *w );

static int            b_update                               ( MatScm          *window,
                                                               MatScm    *w );

/* When the button doesn't have the focus what color should be used to draw it? */
#define COLOR_FOCUS_NO MAT_BLUE

static int color_perm[] = { MAT_RED, MAT_GREEN };

static MatWidgetTypeDesc b_desc =
{
  b_focus_check,
  b_focus_got,
  b_focus_lost,
  b_key,
  b_update,
  b_redraw,
  NULL,
  NULL
};

MatScmType mat_scm_widgetbutton_type =
{
  "widgetbutton",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

static int
b_focus_check ( MatScm *window, MatScm *w )
{
  return 1;
}

static void
b_focus_got ( MatScm *window, MatScm *widget )
{
  MatWidgetButton *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetbutton_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  if (data->focus_got_func)
    data->focus_got_func(window, data->focus_data);

  b_redraw(window, widget);
}

static void
b_focus_lost ( MatScm *window, MatScm *widget )
{
  MatWidgetButton *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetbutton_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  if (data->focus_lost_func)
    data->focus_lost_func(window, data);

  b_redraw(window, widget);
}

static int
b_key ( MatScm *window, MatScm *widget, int key )
{
  MatWidgetButton *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetbutton_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  ASSERT(mat_widget_focus_get(widget));

  switch (key)
    {
    case 13:
      if (data->click)
        data->click(window, data->click_data);
      break;

    default:
      return 0;
    }

  return 1;
}

MatScm *
mat_widget_button_new ( MatScm *text, int x, int y, int width )
{
  MatWidgetButton *data;
  MatScm *widget;

  data = malloc(sizeof(MatWidgetButton));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  widget = mat_widget_new(&b_desc, mat_scm_obj_malloc(&mat_scm_widgetbutton_type, data));

  data->text       = text;
  data->x          = x;
  data->y          = y;
  data->width      = width;
  data->color      = 0;

  mat_widget_button_on_focus(widget, NULL, NULL, NULL);
  mat_widget_button_on_click(widget, NULL, NULL);

  return widget;
}

static void
b_redraw ( MatScm *window, MatScm *widget )
{
  MatWidgetButton *data;
  int i, x, color;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetbutton_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  if (mat_widget_focus_get(widget))
    color = color_perm[data->color];
  else
    color = COLOR_FOCUS_NO;

  mat_window_textbuffer_set(window, data->x,                   data->y, '[', color);
  mat_window_textbuffer_set(window, data->x + data->width - 1, data->y, ']', color);

  ASSERT(mat_scm_string_length_get(data->text) < data->width);

  x = (data->width - mat_scm_string_length_get(data->text)) / 2;

  ASSERT(0 <= x);
  ASSERT(x + mat_scm_string_length_get(data->text) <= data->width);

  for (i = 0; i < data->width - 2; i ++)
    {
      ASSERT(i < data->x + data->width + 1);
      if (x <= i && i < x + mat_scm_string_length_get(data->text))
        mat_window_textbuffer_set(window, data->x + i + 1, data->y, mat_scm_string_data_get(data->text)[i - x], color);
      else
        mat_window_textbuffer_set(window, data->x + i + 1, data->y, ' ', color);
    }

  mat_window_flush(window);
}

static int
b_update ( MatScm *window, MatScm *widget )
{
  MatWidgetButton *data;

  if (!mat_widget_focus_get(widget))
    return 0;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetbutton_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  data->color = (data->color + 1) % (sizeof(color_perm) / sizeof(color_perm[0]));

  return 1;
}

INLINE void
mat_widget_button_on_click ( MatScm *widget, MatWidgetEvent click, void *clickdata )
{
  MatWidgetButton *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetbutton_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  data->click = click;
  data->click_data = clickdata;
}

INLINE void
mat_widget_button_on_focus ( MatScm *widget, MatWidgetEvent got, MatWidgetEvent lost, void *userdata )
{
  MatWidgetButton *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetbutton_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  data->focus_data = userdata;
  data->focus_got_func  = got;
  data->focus_lost_func = lost;
}
