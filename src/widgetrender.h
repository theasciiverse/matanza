#ifndef __MATANZA_WIDGET_RENDER_H__
#define __MATANZA_WIDGET_RENDER_H__

#include <widget.h>
#include <image.h>

MatScm             *mat_widget_render_new           ( MatScm              *window,
                                                      int                  x,
                                                      int                  y,
                                                      MatScm            *(*redraw) ( void *data ),
                                                      int                (*update) ( void *data ),
                                                      void                *userdata );

#endif
