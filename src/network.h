#ifndef __MATANZA_NETWORK_H__
#define __MATANZA_NETWORK_H__

#include <config.h>

/* In Windows, the type SOCKET is used for sockets.  int is used everywhere
 * else.  Windows forces us to define a MatSocketT appropriately.  Windows
 * sucks. */

#ifdef WIN32
typedef SOCKET MatSocketT;
#else /* ! WIN32 */
typedef int MatSocketT;
#endif /* ! WIN32 */

/* On UNIX, when a call is interrupted by a signal, SOCKET_ERRNO() returns
 * EINTR.  On Windows, SOCKET_ERRNO() returns WSAEINTR.  We define the
 * constant SOCKET_CALL_INTR to the appropriate value. */

#ifdef WIN32
#  define SOCKET_CALL_INTR WSAEINTR
#else
#  define SOCKET_CALL_INTR EINTR
#endif

/* Windows returns SOCKET_ERROR when an error with a socket operation takes
 * place.  UNIX returns -1.  Windows forces us to use SOCKET_ERROR
 * everywhere.  Windows sucks. */

#ifndef WIN32
#define SOCKET_ERROR -1
#endif

void mat_network_nonblocking ( int fd );

/* In Windows you don't close a socket: you closesocket a socket.  Socket's are
 * not real file descriptors in Windows.  Windows is a joke. */

void mat_network_close ( MatSocketT fd );

int  mat_network_read ( int fd, char *buffer, int len );

/* To get the reason why a socket call failed we simply look errno in UNIX.  On
 * Windows, we are forced to call WSAGetLastError.  We define mat_network_errno
 * appropriately.  Windows sucks. */

INLINE int                mat_network_errno                ( void );

#endif
