#include <matanza.h>

/* Here we check to see how to make a socket nonblocking. */

#if HAVE_SETSOCKOPT
#  ifdef SOL_SOCKET
#    ifdef SO_NONBLOCK
#      define HAVE_SETSOCKOPT_NONBLOCK 1
#    else
#      define HAVE_SETSOCKOPT_NONBLOCK 0
#    endif
#  else
#    define HAVE_SETSOCKOPT_NONBLOCK 0
#  endif
#else
#    define HAVE_SETSOCKOPT_NONBLOCK 0
#endif

#if HAVE_FCNTL
#  ifdef F_GETFL
#    ifdef F_SETFL
#      ifdef O_NONBLOCK
#        define HAVE_FCNTL_NONBLOCK 1
#      else
#        define HAVE_FCNTL_NONBLOCK 0
#      endif
#    else
#      define HAVE_FCNTL_NONBLOCK 0
#    endif
#  else
#    define HAVE_FCNTL_NONBLOCK 0
#  endif
#else
#  define HAVE_FCNTL_NONBLOCK 0
#endif

/* According to the results of the above checks, we define
 * mat_network_nonblocking in a way that we think will work. */

void
mat_network_nonblocking ( int fd )
{
#if HAVE_SETSOCKOPT_NONBLOCK
  char on = 1;

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("setsockopt [fd:%d][option:SO_NONBLOCK]\n", fd);

  if (setsockopt(fd, SOL_SOCKET, SO_NONBLOCK, (void *) &on, sizeof(char)) == -1)
    {
      fprintf(stderr, "%s: setsockopt: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }
#elif HAVE_IOCTLSOCKET
  int dummy = 1;

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("ioctlsocket [fd:%d][option:FIONBIO]\n", fd);
  if (ioctlsocket(fd, FIONBIO, &dummy) == SOCKET_ERROR)
    {
      /* TODO: Make sure it's ok to call strerror like this. */
      fprintf(stderr, "%s: ioctlsocket: %s\n", strerror(mat_network_errno()));
      exit(EXIT_FAILURE);
    }
#elif HAVE_FCNTL_NONBLOCK
  int opts;

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("fcntl [fd:%d][option:O_NONBLOCK]\n", fd);
  opts = fcntl(fd, F_GETFL);
  if (opts < 0)
    {
      fprintf(stderr, "%s: fcntl: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  opts = (opts | O_NONBLOCK);
  if (fcntl(fd, F_SETFL, opts) < 0)
    {
      fprintf(stderr, "%s: fcntl: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }
#else
#  error Could not find a way to implement NONBLOCKING_GENERIC.
#endif
}

/* For the moment, we are using recv everywhere, as read seems to be
 * unavailable on Windows.  Windows forces us to use recv instead of read.
 * Windows sucks.
 *
 * A return value higher than 0 indicates that there might be more input left
 * to be read.  A value of 0 indicates no more input is left to be read.  A
 * value of -1 indicates the connection was closed. */

int
mat_network_read ( int fd, char *buffer, int len )
{
  int read_len;

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> recv [fd:%d][len:%d]\n", fd, len);
  read_len = recv(fd, buffer, len, 0);
  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- recv [len:%d]\n", read_len);

  switch (read_len)
    {
    case SOCKET_ERROR:
      switch (mat_network_errno())
        {
        case SOCKET_CALL_INTR:
          return 1;

#ifdef WSAECONNRESET
        case WSAECONNRESET:
#endif
#ifdef WSAESHUTDOWN
        case WSAESHUTDOWN:
#endif
#ifdef WSAENETRESET
        case WSAENETRESET:
#endif
#ifdef ECONNRESET
        case ECONNRESET:
#endif
#ifdef EPIPE
        case EPIPE:
#endif

          return -1;

          /* TODO: There might be trouble if EAGAIN is not defined but
           * EWOULDBLOCK or WSAEWOULDBLOCK are. */

#ifdef EAGAIN
        case EAGAIN:
#endif
#ifdef WSAEWOULDBLOCK
#  if WSAEWOULDBLOCK != EAGAIN
        case WSAEWOULDBLOCK:
#  endif
#endif
#ifdef EWOULDBLOCK
#  if EWOULDBLOCK != EAGAIN
        case EWOULDBLOCK:
#  endif
#endif
          return 0;

        default:
          fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
          exit(EXIT_FAILURE);
        }
      break;
    case 0:

      return 0;
      break;

    default:

      ASSERT(read_len > 0);
      return read_len;
    }
}

void
mat_network_close ( MatSocketT fd )
{
#if HAVE_CLOSESOCKET
  closesocket(fd);
#else
  close(fd);
#endif
}

INLINE int
mat_network_errno ( void )
{
#ifdef WIN32
  return WSAGetLastError();
#else
  return errno;
#endif
}
