#include <matanza.h>

struct MatWidget
{
  MatWidgetTypeDesc *type;
  MatScm            *data;
  int                focus;

  MatScm            *window;
  MatScm            *window_list;
};

static INLINE MatWidget *data_get ( MatScm *w );

static INLINE MatWidget *
data_get ( MatScm *w )
{
  MatWidget *data;

  ASSERT(w);
  ASSERT(mat_scm_obj_type_get(w) == &mat_scm_widget_type);

  data = mat_scm_obj_data_get(w);

  ASSERT(data);
  ASSERT(data->type);

  return data;
}

INLINE MatWidgetTypeDesc *
mat_widget_type_get ( MatScm *w )
{
  return data_get(w)->type;
}

INLINE int
mat_widget_focus_get ( MatScm *w )
{
  return data_get(w)->focus;
}

INLINE int
mat_widget_focus_check_get ( MatScm *w )
{
  MatWidget *data = data_get(w);

  if (!data->type->focus_check)
    return 0;

  return data->type->focus_check(data->window, w);
}

void
mat_widget_focus_got ( MatScm *w )
{
  MatWidget *data = data_get(w);

  ASSERT(mat_widget_focus_check_get(w));

  data->focus = 1;

  if (!data->type->focus_got)
    return;

  data->type->focus_got(data->window, w);
}

void
mat_widget_focus_lost ( MatScm *w )
{
  MatWidget *data = data_get(w);

  if (!data->type->focus_lost)
    return;

  data->focus = 0;
  data->type->focus_lost(data->window, w);
}

int
mat_widget_key_press ( MatScm *w, int key )
{
  MatWidget *data = data_get(w);

  if (!data->type->key_press)
    return 0;

  return data->type->key_press(data->window, w, key);
}

int
mat_widget_update ( MatScm *w )
{
  MatWidget *data = data_get(w);
  return data->type->update ? data->type->update(data->window, w) : 0;
}

MatScm *
mat_widget_new ( MatWidgetTypeDesc *type, MatScm *data )
{
  MatWidget *w;
  MatScm    *widget;

  w = malloc(sizeof(MatWidget));
  if (!w)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  widget = mat_scm_obj_malloc(&mat_scm_widget_type, w);

  w->type  = type;
  w->data  = data;
  w->focus = 0;

  w->window = NULL;
  w->window_list = NULL;

  return widget;
}

void
mat_widget_redraw ( MatScm *w )
{
  MatWidget *data = data_get(w);
  ASSERT(data->window);
  if (!data->type->redraw)
    return;
  data->type->redraw(data->window, w);
}

INLINE MatScm *
mat_widget_data_get ( MatScm *w )
{
  return data_get(w)->data;
}

void
mat_widget_resize ( MatScm *w )
{
  MatWidget *data = data_get(w);
  if (!data->type->resize)
    return;
  ASSERT(data->window);
  data->type->resize(data->window, w);
}

INLINE void
mat_widget_free ( MatScm *w )
{
  MatWidget *data = data_get(w);

  if (data->type->free)
    data->type->free(w);

  free(data);
}

INLINE void
mat_widget_window_add ( MatScm *w, MatScm *window, MatScm *prev )
{
  MatWidget *data = data_get(w);

  data->window = window;
  if (window)
    data->window_list = mat_window_add(window, w, prev ? mat_widget_window_list_get(prev) : NULL);
  else
    data->window_list = NULL;
}

INLINE MatScm *
mat_widget_window_list_get ( MatScm *widget )
{
  return data_get(widget)->window_list;
}

INLINE MatScm *
mat_widget_window_get ( MatScm *w )
{
  return data_get(w)->window;
}
