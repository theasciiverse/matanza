#ifndef __MATANZA_RENDER_ROTATE_H__
#define __MATANZA_RENDER_ROTATE_H__

typedef struct MatRenderRotate MatRenderRotate;

#include <imagevector.h>
#include <scm.h>

MatScm                   *mat_render_rotate_new            ( MatScm           *(*func) (void *),
                                                             void               *userdata,
                                                             unsigned int        ang );

MatScm                   *mat_render_rotate_widget_new     ( MatScm             *rotate,
                                                             MatScm             *window,
                                                             int                 x,
                                                             int                 y );

#endif
