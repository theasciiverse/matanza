#include <matanza.h>

struct MatWidgetWindow
{
  MatScm              *head;
  MatScm              *tail;

  MatScm              *focus;

  unsigned int         graph_randomval;

  int                  graph_palette[256];

  /* The position of this view in the Telnet window.  The coordinates are
   * given as characters. */
  unsigned int         chars_pos_x;
  unsigned int         chars_pos_y;
  unsigned int         chars_size_x;
  unsigned int         chars_size_y;

  /* The Telnet window we are in. */
  MatScm              *telnet;
};

INLINE static MatWidgetWindow *data_get                    ( MatScm             *obj );

static INLINE void        resize                           ( void               *data,
                                                             MatScm             *obj );

static INLINE void        redraw                           ( void               *data,
                                                             MatScm             *obj );

static MatScm            *ww_alloc                         ( MatScm             *telnet );

static void               ww_key                           ( MatScm             *telnet,
                                                             void               *window,
                                                             int                 key );

static void               ww_focus_move                    ( MatScm             *window,
                                                             MatScm    *(*move) ( MatScm * ),
                                                             MatScm             *start );

INLINE static MatWidgetWindow *
data_get ( MatScm *obj )
{
  MatWidgetWindow *data;

  ASSERT(obj);
  ASSERT(mat_scm_obj_type_get(obj) == &mat_scm_window_type);

  data = mat_scm_obj_data_get(obj);

  ASSERT(data);
  ASSERT(data->telnet);
  ASSERT(mat_scm_obj_type_get(data->telnet) == &mat_scm_telnet_type);

  return data;
}

static MatScm *
ww_alloc ( MatScm *telnet )
{
  MatWidgetWindow *data;
  MatScm *window;
  unsigned int i;

  data = malloc(sizeof(MatWidgetWindow));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  window = mat_scm_obj_malloc(&mat_scm_window_type, data);

  data->head = data->tail = NULL;
  data->focus = NULL;
  data->graph_randomval = 0;

  for (i = 0; i < 256; i++)
    data->graph_palette[i] = i;

  data->telnet = telnet;

  mat_telnet_window_set(telnet, window);

  return window;
}

MatScm *
mat_window_new ( MatScm *telnet )
{
  MatScm *data;

  ASSERT(telnet);
  ASSERT(mat_scm_obj_type_get(telnet) == &mat_scm_telnet_type);

  data = ww_alloc(telnet);

  ASSERT(data);

  mat_telnet_window_set(telnet, data);
  mat_telnet_key_handler_set(telnet, ww_key, data);

  return data;
}

static void
ww_key ( MatScm *telnet, void *funcwindow, int key )
{
  MatScm *window = (MatScm *) funcwindow;
  MatWidgetWindow *data = data_get(window);

  ASSERT(telnet);
  ASSERT(mat_scm_obj_type_get(telnet) == &mat_scm_telnet_type);
  ASSERT(window);
  ASSERT(mat_scm_obj_type_get(window) == &mat_scm_window_type);

  if (!data->focus)
    return;

  if (!mat_widget_key_press(mat_dlist_obj_get(data->focus), key))
    switch (key)
      {
      case '\t':
      case '\n':
      case '\r':
      case KEY_ARROW_DOWN:
        ww_focus_move(window, mat_dlist_next_get, data->head);
        break;
      case KEY_ARROW_UP:
        ww_focus_move(window, mat_dlist_prev_get, data->tail);
        break;
      default:
        MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Ignoring key: %d\n", key);
      }
}

static void
ww_focus_move ( MatScm *window, MatScm *(*move) ( MatScm * ), MatScm *start )
{
  MatWidgetWindow *data = data_get(window);

  MatScm *widget;

  ASSERT(data->focus);
  ASSERT(start);

  widget = move(data->focus);
  while (widget != data->focus)
    {
      if (!widget)
        widget = start;

      ASSERT(widget);

      if (mat_widget_focus_check_get(mat_dlist_obj_get(widget)))
        mat_window_focus_set(window, widget);
      else
        widget = move(widget);
    }
}

#if 0
void
mat_window_redraw ( MatWidgetWindow *window )
{
  MatWidgetWindow *window = (MatWidgetWindow *) funcwindow;
  MatWidgetNode *nd;

  for (nd = window->head; nd; nd = nd->next)
    mat_widget_redraw(view, nd->widget);
}
#endif

MatScm *
mat_window_add ( MatScm *window, MatScm *widget, MatScm *prev )
{
  MatWidgetWindow *data = data_get(window);
  MatScm *node;

  ASSERT(window);
  ASSERT(widget);

  ASSERT(!prev || mat_scm_obj_type_get(prev) == &mat_scm_cons_type);

  if (prev)
    node = mat_dlist_add_after(&data->head, &data->tail, prev, widget);
  else
    node = mat_dlist_add_tail(&data->head, &data->tail, widget);

  if (!data->focus && mat_widget_focus_check_get(widget))
    mat_window_focus_set(window, node);

  mat_widget_resize(widget);

  return node;
}

void
mat_window_rm ( MatScm *window, MatScm *widget )
{
  MatWidgetWindow *data = data_get(window);

  ASSERT(widget);

  if (data->focus == widget)
    {
      ww_focus_move(window, mat_dlist_next_get, data->head);

      if (data->focus == widget)
        data->focus = NULL;
    }

  mat_dlist_rm(&data->head, &data->tail, mat_widget_window_list_get(widget));
}

void
mat_window_reset ( MatScm *window )
{
  MatWidgetWindow *data = data_get(window);

  data->focus = data->head = data->tail = NULL;

  mat_telnet_image_reset(data->telnet);
}

#if 0
INLINE void
mat_window_prev_set ( MatWidgetWindow *window, MatWidgetWindow *prev )
{
  ASSERT(window);
  window->prev = prev;
}

INLINE void
mat_window_next_set ( MatWidgetWindow *window, MatWidgetWindow *next )
{
  ASSERT(window);
  window->next = next;
}
#endif

void
mat_window_chars_set ( MatScm *window, unsigned int pos_x, unsigned int pos_y, unsigned int size_x, unsigned int size_y )
{
  MatWidgetWindow *data = data_get(window);

  data->chars_pos_x = pos_x;
  data->chars_pos_y = pos_y;
  data->chars_size_x = size_x;
  data->chars_size_y = size_y;

  mat_window_resize(window);
}

INLINE void
mat_window_resize ( MatScm *window )
{
  mat_dlist_forall(mat_window_head_get(window), resize, NULL);
}

static INLINE void
resize ( void *data, MatScm *obj )
{
  mat_widget_resize(obj);
}

INLINE int
mat_window_update ( MatScm *window )
{
  MatScm *widget;
  int update = 0;

  for (widget = mat_window_head_get(window); widget; widget = mat_dlist_next_get(widget))
    if (mat_widget_update(mat_dlist_obj_get(widget)))
      {
        update = 1;
        mat_widget_redraw(mat_dlist_obj_get(widget));
      }

  return update;
}

INLINE void
mat_window_redraw ( MatScm *window )
{
  mat_window_textbuffer_reset(window);
  mat_dlist_forall(mat_window_head_get(window), redraw, window);
}

static INLINE void
redraw ( void *data, MatScm *obj )
{
  ASSERT(mat_widget_window_get(obj) == data);
  mat_widget_redraw(obj);
}

#if 0
MatWidgetWindow *
mat_window_next_get ( MatWidgetWindow *window )
{
  ASSERT(window);
  return window->next;
}
#endif

#if 0
INLINE void
mat_window_anim_func_set ( MatWidgetWindow *v, MatWidgetWindowUpdate u, MatWidgetWindowRedraw d, MatWidgetWindowResize s, void *data )
{
  ASSERT(v);

  v->anim_update = u;
  v->anim_redraw = d;
  v->anim_resize = s;

  v->anim_data = data;

  mat_window_resize(v);
  mat_window_redraw(v);
}
#endif

INLINE void
mat_window_randomval_set ( MatScm *window, unsigned int val )
{
  MatWidgetWindow *data = data_get(window);
  data->graph_randomval = val;
}

INLINE void
mat_window_flush ( MatScm *window )
{
  mat_telnet_flush_schedule(window);
}

INLINE int *
mat_window_palette_get ( MatScm *window )
{
  MatWidgetWindow *data = data_get(window);
  return data->graph_palette;
}

INLINE MatScm *
mat_window_telnet_get ( MatScm *window )
{
  MatWidgetWindow *data = data_get(window);
  return data->telnet;
}

INLINE unsigned int
mat_window_chars_size_x_get ( MatScm *window )
{
  MatWidgetWindow *data = data_get(window);
  return data->chars_size_x;
}

INLINE unsigned int
mat_window_chars_size_y_get ( MatScm *window )
{
  MatWidgetWindow *data = data_get(window);
  return data->chars_size_y;
}

INLINE unsigned int
mat_window_randomval_get ( MatScm *window )
{
  MatWidgetWindow *data = data_get(window);
  return data->graph_randomval;
}

INLINE unsigned int
mat_window_chars_pos_x_get ( MatScm *window )
{
  MatWidgetWindow *data = data_get(window);
  return data->chars_pos_x;
}

INLINE unsigned int
mat_window_chars_pos_y_get ( MatScm *window )
{
  MatWidgetWindow *data = data_get(window);
  return data->chars_pos_y;
}

void
mat_window_free ( MatScm *window )
{
  MatWidgetWindow *data = data_get(window);
  free(data);
}

INLINE void
mat_window_focus_set ( MatScm *window, MatScm *entry )
{
  MatWidgetWindow *data = data_get(window);

  ASSERT(entry);
  ASSERT(mat_widget_focus_check_get(mat_dlist_obj_get(entry)));

  if (data->focus)
    mat_widget_focus_lost(mat_dlist_obj_get(data->focus));

  data->focus = entry;
  mat_widget_focus_got(mat_dlist_obj_get(entry));
}

INLINE MatScm *
mat_window_head_get ( MatScm *window )
{
  MatWidgetWindow *data = data_get(window);
  return data->head;
}

INLINE MatScm *
mat_window_tail_get ( MatScm *window )
{
  MatWidgetWindow *data = data_get(window);
  return data->tail;
}
