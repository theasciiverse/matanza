#include <matanza.h>

typedef struct MatWidgetText MatWidgetText;

struct MatWidgetText
{
  char *buf;
  unsigned int   buf_max;
  unsigned int   buf_len;

  unsigned int   x, y;

  unsigned int   passwd : 1;

  void *focus_data;

  MatWidgetEvent focus_got_func;
  MatWidgetEvent focus_lost_func;
};

static int  t_focus_check                   ( MatScm *window, MatScm *w );
static void t_focus_got                     ( MatScm *window, MatScm *w );
static void t_focus_lost                    ( MatScm *window, MatScm *w );
static int  t_key                           ( MatScm *window, MatScm *w, int key );
static void t_redraw_text                   ( MatScm *window, MatScm *w );
static void t_redraw                        ( MatScm *window, MatScm *w );

static MatWidgetTypeDesc t_desc =
{
  t_focus_check,
  t_focus_got,
  t_focus_lost,
  t_key,
  NULL,
  t_redraw,
  NULL,
  NULL
};

MatScmType mat_scm_widgettext_type =
{
  "widgettext",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

static int
t_focus_check ( MatScm *window, MatScm *w )
{
  return 1;
}

static void
t_focus_got ( MatScm *window, MatScm *widget )
{
  MatWidgetText *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgettext_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  if (data->focus_got_func)
    data->focus_got_func(window, data->focus_data);

  t_redraw(window, widget);
}

static void
t_focus_lost ( MatScm *window, MatScm *widget )
{
  MatWidgetText *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgettext_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  if (data->focus_lost_func)
    data->focus_lost_func(window, data);

  t_redraw(window, widget);
}

static int
t_key ( MatScm *window, MatScm *widget, int key )
{
  MatWidgetText *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgettext_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  ASSERT(mat_widget_focus_get(widget));

  switch (key)
    {
    case 21:
      data->buf[0] = 0;
      data->buf_len = 0;
      break;
#if 0
    case 27:
      menu->state = MENU_STATE_KEY;
      break;
#endif

#if 0
    case 3:
      mat_connection_free(con, MMP_SERVER_CLOSE_CLIENT, "User quit.");
      return;
#endif

    case 127:
    case 8:
      if (data->buf_len)
        data->buf[--data->buf_len] = 0;
      break;

    default:
      if (isalnum(key) && data->buf_len < data->buf_max)
        {
          data->buf[data->buf_len ++] = key;
          data->buf[data->buf_len   ] = 0;
        }
      else
        return 0;
    }

  t_redraw_text(window, widget);
  return 1;
}

static void
t_redraw_text ( MatScm *window, MatScm *widget )
{
  int i, x, color;
  MatWidgetText *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgettext_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  color = mat_widget_focus_get(widget) ? MAT_NORMAL: MAT_BLUE;

  x = data->x + 1;

  for (i = 0; i < data->buf_max; i ++)
    mat_window_textbuffer_set(window, x + i, data->y, i < data->buf_len ? (data->passwd ? '*' : data->buf[i]) : ' ', color);

  mat_window_flush(window);
}

MatScm *
mat_widget_text_new ( char *buf, int buf_max, int x, int y, int passwd )
{
  MatWidgetText *data;

  data = malloc(sizeof(MatWidgetText));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->buf     = buf;
  data->buf_max = buf_max;
  data->buf_len = strlen(buf);
  data->x       = x;
  data->y       = y;
  data->passwd  = passwd;

  data->focus_got_func = NULL;
  data->focus_lost_func = NULL;

  return mat_widget_new(&t_desc, mat_scm_obj_malloc(&mat_scm_widgettext_type, data));
}

void
mat_widget_text_set ( MatScm *widget, char *newtext, unsigned int len )
{
  MatWidgetText *data;

  ASSERT(newtext);

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgettext_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);
  ASSERT(len <= data->buf_max);

  strcpy(data->buf, newtext);
  data->buf_len = len;

  if (mat_widget_window_get(widget))
    mat_window_flush(mat_widget_window_get(widget));
}

static void
t_redraw ( MatScm *window, MatScm *widget )
{
  int color;
  MatWidgetText *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgettext_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  color = mat_widget_focus_get(widget) ? MAT_RED : MAT_BLUE;

  mat_window_textbuffer_set(window, data->x,                     data->y, '[', color);
  mat_window_textbuffer_set(window, data->x + data->buf_max + 1, data->y, ']', color);

  t_redraw_text(window, widget);
}

INLINE void
mat_widget_text_on_focus ( MatScm *widget, MatWidgetEvent got, MatWidgetEvent lost, void *userdata )
{
  MatWidgetText *data;

  ASSERT(widget);
  ASSERT(mat_widget_data_get(widget));
  ASSERT(mat_scm_obj_type_get(mat_widget_data_get(widget)) == &mat_scm_widgettext_type);
  data = mat_scm_obj_data_get(mat_widget_data_get(widget));
  ASSERT(data);

  data->focus_data = userdata;
  data->focus_got_func  = got;
  data->focus_lost_func = lost;
}
