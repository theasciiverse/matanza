#include <matanza.h>

typedef struct MatWidgetList MatWidgetList;

struct MatWidgetList
{
  int   size;

  unsigned int   x, y;

  void *data;

  MatWidgetListMove *move;
  MatWidgetListText *text;

  void *focus_data;
  MatWidgetEvent focus_got_func;
  MatWidgetEvent focus_lost_func;
};

static int  l_focus_check                   ( MatScm *window, MatScm *w );
static void l_focus_got                     ( MatScm *window, MatScm *w );
static void l_focus_lost                    ( MatScm *window, MatScm *w );
static int  l_key                           ( MatScm *window, MatScm *w, int key );
static void l_redraw_text                   ( MatScm *window, MatScm *w );
static void l_redraw                        ( MatScm *window, MatScm *w );

static MatWidgetTypeDesc l_desc =
{
  l_focus_check,
  l_focus_got,
  l_focus_lost,
  l_key,
  NULL,
  l_redraw,
  NULL,
  NULL
};

MatScmType mat_scm_widgetlist_type =
{
  "widgetlist",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

static int
l_focus_check ( MatScm *window, MatScm *w )
{
  return 1;
}

static void
l_focus_got ( MatScm *window, MatScm *widget )
{
  MatWidgetList *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetlist_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  if (data->focus_got_func)
    data->focus_got_func(window, data->focus_data);

  l_redraw(window, widget);
}

static void
l_focus_lost ( MatScm *window, MatScm *widget )
{
  MatWidgetList *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetlist_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);


  if (data->focus_lost_func)
    data->focus_lost_func(window, data);

  l_redraw(window, widget);
}

static int
l_key ( MatScm *window, MatScm *widget, int key )
{
  MatWidgetList *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetlist_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);
  ASSERT(mat_widget_focus_get(widget));

  switch (key)
    {
    case KEY_ARROW_LEFT:
    case 127:
    case 8:
      data->move(data->data, 0);
      l_redraw_text(window, widget);
      break;
    case ' ':
    case KEY_ARROW_RIGHT:
      data->move(data->data, 1);
      l_redraw_text(window, widget);
      break;
    default:
      return 0;
    }

  return 1;
}

static void
l_redraw_text ( MatScm *window, MatScm *widget )
{
  char *buf;
  int i, x, len, color;
  MatWidgetList *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetlist_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  buf = malloc(data->size);
  if (!buf)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  len = data->text(data->data, buf, data->size);

  color = mat_widget_focus_get(widget) ? MAT_NORMAL: MAT_BLUE;

  x = data->x + 1;

  for (i = 0; i < data->size; i ++)
    mat_window_textbuffer_set(window, x + i, data->y, i < len ? buf[i] : ' ', color);

  free(buf);

  mat_window_flush(window);
}

MatScm *
mat_widget_list_new ( int x, int y, int size, void *userdata, MatWidgetListMove *move, MatWidgetListText *text )
{
  MatWidgetList *data;

  data = malloc(sizeof(MatWidgetList));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->x       = x;
  data->y       = y;
  data->size    = size;
  data->move    = move;
  data->text    = text;

  data->focus_got_func  = NULL;
  data->focus_lost_func = NULL;

  data->data    = userdata;

  return mat_widget_new(&l_desc, mat_scm_obj_malloc(&mat_scm_widgetlist_type, data));
}

static void
l_redraw ( MatScm *window, MatScm *widget )
{
  int color;
  MatWidgetList *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetlist_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  color = mat_widget_focus_get(widget) ? MAT_RED : MAT_BLUE;

  mat_window_textbuffer_set(window, data->x,                  data->y, '[', color);
  mat_window_textbuffer_set(window, data->x + data->size + 1, data->y, ']', color);

  l_redraw_text(window, widget);
}

INLINE void
mat_widget_list_on_focus ( MatScm *widget, MatWidgetEvent got, MatWidgetEvent lost, void *userdata )
{
  MatWidgetList *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgetlist_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  data->focus_data = userdata;
  data->focus_got_func  = got;
  data->focus_lost_func = lost;
}
