#ifndef __MATANZA_WIDGET_TEXT_H__
#define __MATANZA_WIDGET_TEXT_H__

#include <widget.h>

MatScm                   *mat_widget_text_new              ( char               *buf,
                                                             int                 buf_max,
                                                             int                 x,
                                                             int                 y,
                                                             int                 passwd );

INLINE void               mat_widget_text_on_focus         ( MatScm             *text,
                                                             MatWidgetEvent      get,
                                                             MatWidgetEvent      lose,
                                                             void               *data );

void                      mat_widget_text_set              ( MatScm             *text,
                                                             char               *newtext,
                                                             unsigned int        len );

#endif
