#ifndef __MATANZA_WIDGET_H__
#define __MATANZA_WIDGET_H__

typedef struct MatWidget MatWidget;
typedef struct MatWidgetTypeDesc MatWidgetTypeDesc;
typedef void (*MatWidgetEvent) ( MatScm *, void * );

struct MatWidgetTypeDesc
{
  int   (*focus_check)   ( MatScm *, MatScm * );
  void  (*focus_got)     ( MatScm *, MatScm * );
  void  (*focus_lost)    ( MatScm *, MatScm * );

  int   (*key_press)     ( MatScm *, MatScm *, int );
  int   (*update)        ( MatScm *, MatScm * );
  void  (*redraw)        ( MatScm *, MatScm * );

  void  (*resize)        ( MatScm *, MatScm * );

  void  (*free)          ( MatScm * );
};

INLINE int mat_widget_focus_get ( MatScm *w );
INLINE int mat_widget_focus_check_get ( MatScm * );
void  mat_widget_focus_got       ( MatScm * );
void  mat_widget_focus_lost      ( MatScm * );
int   mat_widget_key_press       ( MatScm *, int key );
int   mat_widget_update          ( MatScm * );
void  mat_widget_redraw          ( MatScm * );
void  mat_widget_resize          ( MatScm * );

INLINE MatWidgetTypeDesc *mat_widget_type_get              ( MatScm             *widget );

INLINE MatScm            *mat_widget_data_get              ( MatScm             *widget );

INLINE MatScm            *mat_widget_prev_get              ( MatScm             *widget );

INLINE MatScm            *mat_widget_next_get              ( MatScm             *widget );

INLINE void               mat_widget_prev_set              ( MatScm             *widget,
                                                             MatScm             *prev );

INLINE void               mat_widget_next_set              ( MatScm             *widget,
                                                             MatScm             *next );

INLINE void               mat_widget_free                  ( MatScm             *widget );

MatScm                   *mat_widget_new                   ( MatWidgetTypeDesc  *type,
                                                             MatScm             *data );

INLINE void               mat_widget_window_add            ( MatScm             *w,
                                                             MatScm             *window,
                                                             MatScm             *prev );

INLINE MatScm            *mat_widget_window_list_get       ( MatScm             *widget );

INLINE MatScm            *mat_widget_window_get            ( MatScm             *w );

#endif
