#ifndef __MATANZA_TELNET_H__
#define __MATANZA_TELNET_H__

typedef struct MatTelnet MatTelnet;

#define MAT_NORMAL     0
#define MAT_DIM        1
#define MAT_BOLD       2
#define MAT_BOLDFONT   3
#define MAT_REVERSE    4
#define MAT_SPECIAL    5
#define MAT_RED        6
#define MAT_RED_BOLD   7
#define MAT_BLUE       8
#define MAT_GREEN      9
#define MAT_TERM_CODES 10

int mat_telnet_xwinsize_get ( MatScm    *data );
int mat_telnet_ywinsize_get ( MatScm    *data );
unsigned char *mat_telnet_imgbuffer_get ( MatScm    *data );
unsigned char *mat_telnet_textbuffer_get ( MatScm    *data );
INLINE int mat_telnet_window_count_get ( MatScm    *t );
INLINE int mat_telnet_imgheight_get ( MatScm    *t );
INLINE int mat_telnet_imgwidth_get  ( MatScm    *t );
INLINE int mat_telnet_clean_get ( MatScm    *t );

INLINE void mat_telnet_flush ( MatScm    *t );

INLINE void               mat_telnet_flush_schedule        ( MatScm             *t );

void mat_telnet_xwinsize_set ( MatScm    *data, int size );
void mat_telnet_ywinsize_set ( MatScm    *data, int size );
void mat_telnet_clean_set ( MatScm    *t, int value );
INLINE void mat_telnet_textbuffer_set ( MatScm    *t, int pos, char c, char a );
INLINE void mat_telnet_imgbuffer_set ( MatScm    *t, unsigned int pos, unsigned char value );
INLINE unsigned char mat_telnet_imgbuffer_pos_get ( MatScm    *t, unsigned int pos );

INLINE void mat_telnet_window_count_set ( MatScm    *t, int nv );
void mat_telnet_resize ( MatScm    *t, int width, int height );
INLINE void mat_telnet_key_handler_set ( MatScm    *, void (*func) (MatScm    *, void *, int), void * );

INLINE void               mat_telnet_input                 ( void               *t,
                                                             unsigned char       c );

INLINE MatScm            *mat_telnet_connection_get        ( MatScm             *t );

MatScm *mat_telnet_new ( MatScm        *con );
void mat_telnet_free ( MatScm    *t );
void mat_telnet_window_size_set ( MatScm    *t );
void mat_telnet_message_printf ( MatScm    *t, char *msg, int showip );

INLINE void               mat_telnet_update                ( MatScm             *t );

INLINE void               mat_telnet_close                 ( MatScm             *t,
                                                             char               *reason );

void mat_telnet_window_set ( MatScm    *t, MatScm *v );
INLINE MatScm *mat_telnet_window_get ( MatScm    *t );

INLINE void               mat_telnet_image_reset           ( MatScm             *p );

#endif
