#ifndef __MATANZA_WIDGET_PLAY_H__
#define __MATANZA_WIDGET_PLAY_H__

#define MATANZA_WIDGET_PLAY_CHAT_SIZE 256
#define MATANZA_WIDGET_PLAY_MSG_MAX     6

//#define MAT_MIST_COMPLETE 400

#include <universe.h>
#include <team.h>

typedef struct MatWidgetPlay MatWidgetPlay;
typedef struct MatWidgetPlayData MatWidgetPlayData;

struct MatWidgetPlayData
{
  MatScm          *data;
  MatScm          *window;
  MatScm          *un;

  /* The positions in the universe from where the view begins */
  unsigned startx;
  unsigned starty;
};

void                      mat_widget_play_start            ( MatScm             *window,
                                                             MatScm             *name,
                                                             MatScm             *un,
                                                             MatScm             *team,
                                                             MatScm             *type );

INLINE void               mat_widget_play_zoom             ( MatScm             *data,
                                                             double              rate );

void                      mat_widget_play_printf           ( MatScm             *widget,
                                                             char               *fmt,
                                                             ... );

void                      mat_widget_play_vprintf          ( MatScm             *widget,
                                                             char               *fmt,
                                                             va_list             args );

INLINE void               mat_widget_play_view_reset       ( MatScm             *data );

INLINE MatScm            *mat_widget_play_view_prev_get    ( MatScm             *data );

INLINE MatScm            *mat_widget_play_view_next_get    ( MatScm             *data );

INLINE void               mat_widget_play_view_prev_set    ( MatScm             *data,
                                                             MatScm             *prev );

INLINE void               mat_widget_play_view_next_set    ( MatScm             *data,
                                                             MatScm             *next );

INLINE MatScm            *mat_widget_play_self_get         ( MatScm             *widget );

INLINE MatScm            *mat_widget_play_view_get         ( MatScm             *widget );

/* Return the size of the widget (in pixels) */

INLINE unsigned int       mat_widget_play_size_x_get       ( MatScm             *widget );
INLINE unsigned int       mat_widget_play_size_y_get       ( MatScm             *widget );

/* Return the amount of information that should be rendered into the widget */

INLINE unsigned int       mat_widget_play_view_x_get       ( MatScm             *widget );
INLINE unsigned int       mat_widget_play_view_y_get       ( MatScm             *widget );

INLINE MatScm            *mat_widget_play_window_get       ( MatScm             *widget );

INLINE void               mat_widget_play_key_set          ( MatScm             *widget,
                                                             int (*key) ( MatScm *, int ) );

INLINE void               mat_widget_play_chat_len_set     ( MatScm             *widget,
                                                             unsigned int        len );

INLINE char              *mat_widget_play_chat_buffer_get  ( MatScm             *widget );

INLINE void               mat_widget_play_pos_swap         ( MatScm             *widget );

INLINE void               mat_widget_play_msg_rm           ( MatScm             *data );

INLINE void               mat_widget_play_msg_clean        ( MatScm             *data );

#endif
