#ifndef __MATANZA_IMAGE_H__
#define __MATANZA_IMAGE_H__

//typedef struct MatImage            MatImage;

#include <anglecache.h>
#include <scm.h>

/* Extract the color in the position (x, y) in the matrix associated with the
 * image.  (x, y) must be a valid coordinate in the matrix. */

INLINE unsigned char      mat_image_core_get               ( MatScm             *img,
                                                             unsigned int       x,
                                                             unsigned int       y );

/* Return the X size of the matrix associated with the image. */

INLINE unsigned int       mat_image_size_x_get             ( MatScm             *img );

/* Return the Y size of the matrix associated with the image. */

INLINE unsigned int       mat_image_size_y_get             ( MatScm             *img );

/* Load one PNG image from the path specified. */

MatScm                   *mat_image_load                   ( const char         *path );

/* Resize and image. scale_x and scale_y are the new sizes. */

MatScm                   *mat_image_resize                 ( MatScm             *img,
                                                             double              scale_x,
                                                             double              scale_y );

/* Apply a palette to an image.  Basically, the following transformation is
 * applied to every pixel in the image:
 *
 *   pixel = pal[pixel]
 *
 * pal must hold 255 characters. */

void                      mat_image_palette_apply          ( MatScm             *img,
                                                             unsigned char      *pal );

/* Returns a pointer to the buffer holding the matrix associated with the
 * image.  This function is deprecated. */

INLINE unsigned char     *mat_image_core_buffer_get        ( MatScm             *img );

/* Set the value of the point at the (x, y) coordinate in the matrix associated
 * with img to the specified value. */

INLINE void               mat_image_core_set               ( MatScm             *img,
                                                             unsigned int        x,
                                                             unsigned int        y,
                                                             unsigned char       val );

/* Create a new (random) image of the specified size. */

MatScm                   *mat_image_new                    ( unsigned int        width,
                                                             unsigned int        height );

/* Free the memory allocation used to hold the image given. */

//void                      mat_image_free                   ( MatScm             *img );

/* Set all the points in img to a given value. */

INLINE void               mat_image_core_set_all           ( MatScm             *img,
                                                             unsigned char       val );

/* Set the point in img to val.  The actual point is specified by pos, which is
 * the position in the buffer holding the image's matrix. */

INLINE void               mat_image_core_set_pos           ( MatScm             *img,
                                                             unsigned int        pos,
                                                             unsigned char       val );

/* Remove the borders of an image so that (xbeg, ybeg) is sent to the upper
 * left and (xend, yend) to the lower right corners of the new image.  I think
 * this process is called to `crop' the image. */

INLINE void               mat_image_sub                    ( MatScm             *img,
                                                             unsigned int        xbeg,
                                                             unsigned int        ybeg,
                                                             unsigned int        xend,
                                                             unsigned int        yend );

#endif
