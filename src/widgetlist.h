#ifndef __MATANZA_WIDGET_LIST_H__
#define __MATANZA_WIDGET_LIST_H__

#include <widget.h>

typedef int  (MatWidgetListText) ( void *data, char *buf, int size );
typedef void (MatWidgetListMove) ( void *data, int dir );

MatScm                   *mat_widget_list_new              ( int                 x,
                                                             int                 y,
                                                             int                 size,
                                                             void               *data,
                                                             MatWidgetListMove  *move,
                                                             MatWidgetListText  *text );

INLINE void               mat_widget_list_on_focus         ( MatScm             *list,
                                                             MatWidgetEvent      get,
                                                             MatWidgetEvent      lose,
                                                             void               *data );

#endif
