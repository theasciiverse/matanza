#ifndef __MATANZA_RENDER_IMAGE_H__
#define __MATANZA_RENDER_IMAGE_H__

MatScm                   *mat_render_image_new             ( MatScm             *img );

MatScm                   *mat_render_image_widget_new      ( MatScm             *image,
                                                             MatScm             *window,
                                                             int                 x,
                                                             int                 y );

#endif
