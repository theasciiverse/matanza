#ifndef __MATANZA_WIDGET_BUTTON_H__
#define __MATANZA_WIDGET_BUTTON_H__

#include <widget.h>

typedef void (*MatWidgetButtonClick) ( MatScm *, void * );

MatScm                   *mat_widget_button_new            ( MatScm             *text,
                                                             int                 x,
                                                             int                 y,
                                                             int                 width );

INLINE void               mat_widget_button_on_click       ( MatScm             *button,
                                                             MatWidgetEvent      event,
                                                             void               *data );

INLINE void               mat_widget_button_on_focus       ( MatScm             *text,
                                                             MatWidgetEvent      get,
                                                             MatWidgetEvent      lose,
                                                             void               *data );


#endif
