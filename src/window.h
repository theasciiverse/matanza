#ifndef __MATANZA_WINDOW_H__
#define __MATANZA_WINDOW_H__

typedef struct MatWidgetWindow MatWidgetWindow;

#if 0
typedef int  (MatWidgetWindowUpdate) ( MatScm *, void * );
typedef void (MatWidgetWindowRedraw) ( MatScm *, void * );
typedef void (MatWidgetWindowResize) ( MatScm *, void * );
#endif

#include <telnet.h>
#include <scm.h>

MatScm                   *mat_window_new                   ( MatScm             * );

MatScm                   *mat_window_add                   ( MatScm             *window,
                                                             MatScm             *widget,
                                                             MatScm             *prev );

void                      mat_window_rm                    ( MatScm             *window,
                                                             MatScm             *widget );

void                      mat_window_reset                 ( MatScm             *window );

INLINE void               mat_window_prev_set              ( MatScm             *window,
                                                             MatScm             *prev );

INLINE void               mat_window_next_set              ( MatScm             *window,
                                                             MatScm             *next );

INLINE void               mat_window_redraw                ( MatScm             *c );

INLINE void               mat_window_resize                ( MatScm             *window );

INLINE int                mat_window_update                ( MatScm             *window );

void                      mat_window_chars_set             ( MatScm             *c,
                                                             unsigned int        x,
                                                             unsigned int        y,
                                                             unsigned int        width,
                                                             unsigned int        height );

MatScm                   *mat_window_next_get              ( MatScm             *window );

INLINE void               mat_window_randomval_set         ( MatScm             *v,
                                                             unsigned int        val );

#if 0
INLINE void               mat_window_anim_func_set         ( MatScm             *v,
                                                             MatWidgetWindowUpdate u,
                                                             MatWidgetWindowRedraw d,
                                                             MatWidgetWindowResize s,
                                                             void               *data );
#endif

INLINE void               mat_window_flush                 ( MatScm             *window );

INLINE int               *mat_window_palette_get           ( MatScm             *v );

INLINE MatScm            *mat_window_telnet_get            ( MatScm             * );

INLINE void               mat_window_print                 ( MatScm             *window,
                                                             unsigned int        x,
                                                             unsigned int        y,
                                                             char               *str,
                                                             char                a );

INLINE unsigned int       mat_window_randomval_get         ( MatScm             *window );

INLINE unsigned int       mat_window_chars_size_y_get       ( MatScm             *window );

INLINE unsigned int       mat_window_chars_size_x_get       ( MatScm             *window );

INLINE unsigned int       mat_window_chars_pos_y_get       ( MatScm             *window );

INLINE unsigned int       mat_window_chars_pos_x_get       ( MatScm             *window );

INLINE unsigned int       mat_window_img_size_y_get         ( MatScm             *window );

INLINE unsigned int       mat_window_img_size_x_get         ( MatScm             *window );

void                      mat_window_free                  ( MatScm             *window );

INLINE void               mat_window_focus_set             ( MatScm             *window,
                                                             MatScm             *widget );

INLINE MatScm            *mat_window_head_get              ( MatScm             *window );

INLINE MatScm            *mat_window_tail_get              ( MatScm             *window );

#endif
