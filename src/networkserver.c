#include <matanza.h>

static void               mat_network_accept_real          ( MatSocketT          source,
                                                             MatConnectionType   machine );

static MatSocketT         mat_network_listen               ( int                 port );


static char              *mat_network_addr                 ( struct sockaddr_in  in_con );

/* mat_network_addr returns the address a connection comes from.
 *
 * We don't reverse lookup the address to the name because it may take long
 * amounts of time: The whole game would freeze during resolution (unless we do
 * some sort of dirty hack (such as keep a child process and use it to perform
 * the looks) that seems too much effort).
 *
 * If inet_ntop is available, we use it.  On those systems, inet_ntoa is
 * probably deprecated.  inet_ntop supports IPV6.  On older machines where it's
 * not available, we try with |inet_ntoa|.  To be safe, we make sure inet_ntoa
 * is actually available and, if it isn't, we report a bogus address. */

static char *
mat_network_addr ( struct sockaddr_in in_con )
{
#if HAVE_INET_NTOP
  static char buffer[256];

  if (!inet_ntop(in_con.sin_family, &in_con.sin_addr, buffer, sizeof(buffer)))
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  return buffer;
#elif HAVE_INET_NTOA
  return inet_ntoa(in_con.sin_addr);
#else
  return "Unknown";
#endif
}

void
mat_network_init ( )
{
  /* On Windows we need to call WSAStartup() before any sockets-related
   * functions.  Windows sucks. */

#if HAVE_WSASTARTUP
  WORD ver;
  WSADATA data;
  int err;

  ver = MAKEWORD(2, 2);

  err = WSAStartup(ver, &data);
  if (err != 0)
    {
      fprintf(stderr, "%s: WSAStartup: Initialization failed.\n", program_name);
      exit(EXIT_FAILURE);
    }
#endif

  mat_mainsock         = mat_network_listen(arg_port);
  mat_mainsock_machine = mat_network_listen(arg_port_machine);
}

/* Set a socket to listen on the specified port and return it. */
static MatSocketT
mat_network_listen ( int port )
{
  struct sockaddr_in in;
  MatSocketT fd;
  int on = 1;

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Listening on port %d...", port);
  fflush(stdout);

  memset(&in, 0, sizeof(struct sockaddr_in));
  in.sin_family = AF_INET;
  in.sin_addr.s_addr = INADDR_ANY;
  in.sin_port = htons(port);

#if HAVE_WSASOCKET
  fd = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
#else
  fd = socket(AF_INET, SOCK_STREAM, 0);
#endif

  if (fd == SOCKET_ERROR)
    {
      /* There seems to be no portable way of describing an error.
       *
       * On Windows it seems we have to check for every individual error.  We
       * only check for those we think might happen.
       *
       * On Unix, everything is easy.
       *
       * Windows sucks. */

#if WIN32
      switch (mat_network_errno())
        {
        case WSAEMFILE:
          fprintf(stderr, "%s: socket: No more socket descriptors are available.\n", program_name);
          break;
        default:
          fprintf(stderr, "%s: socket: Unknown error (code %d)\n", program_name, mat_network_errno());
        }
#else
        fprintf(stderr, "%s: socket: %s\n", program_name, strerror(mat_network_errno()));
#endif

      exit(EXIT_FAILURE);
    }

#if HAVE_SETSOCKOPT
#  ifdef SOL_SOCKET
#    ifdef SO_REUSEADDR
  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("setsockopt [fd:%d][option:SO_REUSEADDR]\n", fd);
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *) &on, sizeof(on));
#    endif
#  endif
#endif

  if (bind(fd, (struct sockaddr*)&in, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
    {
      fprintf(stderr, "%s: bind: %s\n", program_name, strerror(mat_network_errno()));
      exit(EXIT_FAILURE);
    }

  if (listen(fd, 10) == SOCKET_ERROR)
    {
      exit(EXIT_FAILURE);
      fprintf(stderr, "%s: listen: %s\n", program_name, strerror(mat_network_errno()));
    }

  /* Make socket nonblocking. */
  mat_network_nonblocking(fd);

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)(" Done\n");

  return fd;
}

void
mat_network_wait_accept ( )
{
  fd_set i;

  ASSERT(players_count == 0);

  FD_ZERO(&i);
  FD_SET(mat_mainsock,         &i);
  FD_SET(mat_mainsock_machine, &i);

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> select\n");
  select(MAX(mat_mainsock, mat_mainsock_machine) + 1, &i, NULL, NULL, NULL);
  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- select\n");
}

void
mat_network_accept ( )
{
  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> accept incomming connections\n");
  mat_network_accept_real(mat_mainsock,         MAT_CONNECTION_TYPE_TELNET);
  mat_network_accept_real(mat_mainsock_machine, MAT_CONNECTION_TYPE_MMP);
  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- accept incomming connections\n");
}

/* TODO: Make some checks against DoS attacks: we shouldn't allow more than X
 * connections from the same host.  I'm not sure this (rather than
 * connection.c, for example) is the place to do it. */

static void
mat_network_accept_real ( MatSocketT source, MatConnectionType type )
{
  MatSocketT          fd;
  socklen_t           in_con_size = sizeof(struct sockaddr_in);
  struct sockaddr_in  in_con;
  int                 on = 1;

  while (1)
    {
      do
        {
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> accept [fd:%d]\n", source);
          fd = accept(source, (struct sockaddr*)&in_con, &in_con_size);
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- accept [source:%d] [fd:%d]\n", source, fd);
        }
      while (fd == SOCKET_ERROR && fd == SOCKET_CALL_INTR);

      if (fd == SOCKET_ERROR)
        {
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("error in accept: %d\n", mat_network_errno());
          switch (mat_network_errno())
            {
#ifdef EAGAIN
            case EAGAIN:
#endif
#ifdef EWOULDBLOCK
#  if EWOULDBLOCK != EAGAIN
            case EWOULDBLOCK:
#  endif
#endif
#ifdef WSAEWOULDBLOCK
#  if WSAEWOULDBLOCK != EAGAIN
            case WSAEWOULDBLOCK:
#  endif
#endif

              return;

            default:
              fprintf(stderr, "%s: accept: code: %d\n", program_name, mat_network_errno());
              fprintf(stderr, "%s: accept: desc: %s\n", program_name, strerror(mat_network_errno()));
              exit(EXIT_FAILURE);
            }
        }
      else
        {
          MAT_VERBOSE(MAT_VERBOSE_CONNECTIONS)("Incomming connection: [fd:%d][type:%d]\n", fd, type);

#if HAVE_SETSOCKOPT
#  ifdef SO_KEEPALIVE
#    ifdef SOL_SOCKET
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("setsockopt [fd:%d][option:SO_KEEPALIVE]\n", fd);
          if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void *) &on, sizeof(on)) == -1)
            fprintf(stderr, "%s: Warning: setsockopt: SO_KEEPALIVE: %s\n", program_name, strerror(errno));
#    endif
#  endif
#endif

	  mat_network_nonblocking(fd);

	  ASSERT(arg_max_players == 0 || players_count <= arg_max_players);

	  if (arg_max_players != 0 && players_count == arg_max_players)
	    {
	      char *msg, buffer[] = { MMP_SERVER_INITIAL_FULL };
	      int len;

	      switch (type)
		{
		case MAT_CONNECTION_TYPE_TELNET:
		  msg = "Sorry, the server is full. Please try again later.\n";
		  len = strlen(msg);
		  break;
		case MAT_CONNECTION_TYPE_MMP:
		  msg = buffer;
		  len = 1;
		  break;
		default:
		  ASSERT(0);
		}

#ifdef MSG_NOSIGNAL
#  define MAT_SEND_FLAGS MSG_NOSIGNAL
#else
#  define MAT_SEND_FLAGS 0
#endif

              send(fd, msg, len, MAT_SEND_FLAGS);

              mat_network_close(fd);
	    }
	  else
	    {
              MatScm *con;
#if MAT_BUILD_MMP
	      static MatConnectionInit init[] = { mat_telnet_new, mat_machine_new };
	      ASSERT(type == MAT_CONNECTION_TYPE_TELNET || type == MAT_CONNECTION_TYPE_MMP);
#else
	      static MatConnectionInit init[] = { mat_telnet_new };
              ASSERT(type == MAT_CONNECTION_TYPE_TELNET);
#endif

              con = mat_dlist_obj_get(mat_connection_new(fd, mat_network_addr(in_con)));

              mat_connection_type_set(con, type);
              mat_connection_data_set(con, init[type](con));

              players_count++;
	    }
        }
    }
}
