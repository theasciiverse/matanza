#ifndef __MATANZA_WINDOW_BUFFER_H__
#define __MATANZA_WINDOW_BUFFER_H__

#include <scm.h>

INLINE void               mat_window_imgbuffer_reset       ( MatScm             *window,
                                                             unsigned char       value );

INLINE void               mat_window_textbuffer_reset       (MatScm             *window );

INLINE unsigned int       mat_window_img_size_y_get        ( MatScm             *window );

INLINE unsigned int       mat_window_img_size_x_get        ( MatScm             *window );

INLINE unsigned int       mat_window_img_pos_y_get         ( MatScm             *window );

INLINE unsigned int       mat_window_img_pos_x_get         ( MatScm             *window );

INLINE void               mat_window_imgbuffer_set         ( MatScm             *window,
                                                             unsigned int        x,
                                                             unsigned int        y,
                                                             unsigned char       value,
                                                             double              alpha );

INLINE void               mat_window_textbuffer_set        ( MatScm             *window,
                                                             unsigned int        x,
                                                             unsigned int        y,
                                                             char                c,
                                                             char                a );

#endif
