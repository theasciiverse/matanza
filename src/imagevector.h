#ifndef __MATANZA_IMAGEVECTOR_H__
#define __MATANZA_IMAGEVECTOR_H__

//typedef struct MatImageVector MatImageVector;

#include <image.h>

/* Create a new vector with ang_total evenly distributed rotations of src. */

MatScm                   *mat_imagevector_new              ( MatScm             *src,
                                                             unsigned int        ang_total,
                                                             MatAngleCache      *ang_cache );

/* Return the pos rotation of the image. */

INLINE MatScm            *mat_imagevector_get              ( MatScm             *v,
                                                             unsigned int        pos );

/* Resize a whole image vector. */

INLINE MatScm            *mat_imagevector_resize           ( MatScm             *v,
                                                             double              scale_x,
                                                             double              scale_y );

/* Apply a palette to all the images in one vector. */

void                      mat_imagevector_palette_apply    ( MatScm             *v,
                                                             unsigned char      *pal );

/* Destroy the image vector. */

INLINE void               mat_imagevector_free             ( MatScm             *img );

#endif
