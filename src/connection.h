#ifndef __MATANZA_CONNECTION_H__
#define __MATANZA_CONNECTION_H__

typedef struct MatConnection MatConnection;

typedef enum MatConnectionState MatConnectionState;
typedef enum MatConnectionType  MatConnectionType;
typedef MatScm *(*MatConnectionInit) ( MatScm * );

#include <network.h>

enum MatConnectionState
{
#if MATANZA_SERVER
  MAT_STATE_REMOVE,
  MAT_STATE_PREPARE,
  MAT_STATE_NORMAL,
  MAT_STATE_PLAYING,
  MAT_STATE_MACHINE,
  MAT_STATE_SHUTDOWN,
#else
  MAT_STATE_NORMAL
#endif
};

#if MATANZA_SERVER

/* Describes the protocol used by the connection. */
enum MatConnectionType
{
  /* The remote end is a Telnet client. */
  MAT_CONNECTION_TYPE_TELNET,

  /* The remote end is a MMP client. */
  MAT_CONNECTION_TYPE_MMP,
};

#endif

#define MAXCHATBUFFERLEN 60
#define MAXMESSAGES       5
#define MAXMESSAGELEN    80

void *mat_connection_data_get ( MatScm *c );
char *mat_connection_addr_get ( MatScm *c );

#if MATANZA_SERVER

MatConnectionState mat_connection_state_get ( MatScm *p );

INLINE void               mat_connection_state_set         ( MatScm             *p,
                                                             MatConnectionState  s );

#endif

INLINE MatScm            *mat_connection_next_get          ( MatScm             *c );
INLINE MatScm            *mat_connection_prev_get          ( MatScm             *c );

INLINE void               mat_connection_data_set          ( MatScm             *c,
                                                             void               *data );

void mat_connection_remove ( MatScm        *c );
void mat_connection_input (MatScm        *con);

MatScm                   *mat_connection_new               ( MatSocketT          fd,
							     char *addr );

#if MATANZA_SERVER
INLINE void               mat_connection_type_set          ( MatScm             *con,
							     MatConnectionType   type );
#endif

INLINE void               mat_connection_input_handler_set ( MatScm             *con,
                                                             void (*func) ( void *, unsigned char ),
                                                             void               *funcdata );

void mat_connection_free ( MatScm        *con, unsigned char reason, char *desc );

int mat_connection_printf ( MatScm        *con, char *format, ... );

/* mat_connection_flush calls send to send the contents of the buffer through
 * the socket.  It is harmless to call mat_connection_flush when the buffer is
 * empty: it checks for said condition and does nothing if it happens. */
int                mat_connection_flush             ( MatScm              *con );

void mat_connection_message_printf ( MatScm        *t, char *msg, int showip );

INLINE void               mat_connection_update            ( MatScm             *c );

INLINE void               mat_connection_out_gen_set       ( MatScm             *con,
                                                             void (*gen) ( MatScm        *, void *),
                                                             void               *data );

/* mat_connection_write works like a fwrite. */
int                mat_connection_write             ( MatScm              *con,
                                                      void                *buffer,
                                                      size_t               len );

MatScm                   *mat_connection_head;
MatScm                   *mat_connection_tail;

#endif
