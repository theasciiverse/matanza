#include <matanza.h>

struct MatConnection
{
  MatSocketT           fd;
  char                *addr;

  char                 in_buf[MAT_CONNECTION_IN_SIZE];
  unsigned int         in_len;

  /* Buffer for output. */
  char                *out_buf;
  unsigned int         out_len;
  unsigned int         out_siz;

  void               (*out_gen) ( MatScm *c, void *data );
  void                *out_gen_data;

#if MATANZA_SERVER
  MatConnectionType    type;

  /* For machines, this is MAT_STATE_MACHINE. */
  int                  state;
#endif

  void               (*input_handler) ( void *data, unsigned char key );
  void                *input_handler_data;

#if 0
  union
  {
    /* Here we keep information when this connection is for a machine. */
    struct
    {
      int              number;
      int              number_byte;

      char            *string;
      int              string_len;
      int              string_siz;
      int              string_maxlength;

      void           (*handler) ( MatConnection * );

    } m;

  } info;
#endif

  /* Data points to the data for the specific type of connection. */
  void                *data;
};

INLINE static MatConnection *data_get                      ( MatScm             *obj );

INLINE static int         mat_connection_buffer_resize     ( MatScm             *con,
                                                             int                 nlen );

MatScm                   *mat_connection_head = NULL;
MatScm                   *mat_connection_tail = NULL;

INLINE static MatConnection *
data_get ( MatScm *obj )
{
  MatConnection  *data;

  ASSERT(obj);

  if (mat_scm_obj_type_get(obj) == &mat_scm_window_type)
    obj = mat_window_telnet_get(obj);

  if (mat_scm_obj_type_get(obj) == &mat_scm_telnet_type)
    obj = mat_telnet_connection_get(obj);

  ASSERT(obj);
  ASSERT(mat_scm_obj_type_get(obj) == &mat_scm_connection_type);

  data = mat_scm_obj_data_get(obj);

  ASSERT(data);

  return data;
}

INLINE char *
mat_connection_addr_get ( MatScm *c )
{
  return data_get(c)->addr;
}

INLINE void *
mat_connection_data_get ( MatScm *c )
{
  return data_get(c)->data;
}

#if MATANZA_SERVER

INLINE MatConnectionState
mat_connection_state_get ( MatScm *c )
{
  return data_get(c)->state;
}

INLINE void
mat_connection_state_set ( MatScm *c, MatConnectionState s )
{
  data_get(c)->state = s;
}

#endif

INLINE void
mat_connection_data_set ( MatScm *c, void *data )
{
  data_get(c)->data = data;
}

/* machine is be evaluated to know whether the peer is a player (telnet client)
 * or a machine (speaking MMP). */

INLINE MatScm *
mat_connection_new ( MatSocketT fd, char *addr )
{
  MatConnection *data;
  MatScm *c;

  data = malloc(sizeof(MatConnection));
  if (!data)
    {
      /* TODO: Kill this connection rather than the whole program. */
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  c = mat_scm_obj_malloc(&mat_scm_connection_type, data);

  data->fd = fd;

  data->out_buf = NULL;
  data->out_len = 0;
  data->out_siz = 0;

  data->out_gen = NULL;

  data->in_len = 0;

  data->addr = strdup(addr);
  if (!data->addr)
    {
      /* TODO: Kill this connection rather than the whole program. */
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  return mat_dlist_add_head(&mat_connection_head, &mat_connection_tail, c);
}

#if MATANZA_SERVER

INLINE void
mat_connection_type_set ( MatScm *c, MatConnectionType type )
{
  data_get(c)->type = type;
}

INLINE void
mat_connection_remove ( MatScm *c )
{
  mat_dlist_rm(&mat_connection_head, &mat_connection_tail, c);
  free(data_get(c));
}

#endif

INLINE void
mat_connection_input ( MatScm *c )
{
  MatConnection *data = data_get(c);
  int len;

  ASSERT(data);
  ASSERT(data->input_handler);

  while (!data->out_gen && data->out_len < MAT_CONNECTION_OUT_BLOCK)
    {
      int i = 0;

#if MATANZA_SERVER
      if (data->state == MAT_STATE_REMOVE)
        return;
#endif

      ASSERT(data->in_len <= MAT_CONNECTION_IN_SIZE);

      if (data->in_len < MAT_CONNECTION_IN_SIZE)
        {
          len = mat_network_read(data->fd, data->in_buf + data->in_len, MAT_CONNECTION_IN_SIZE - data->in_len);

          if (len == 0 && data->in_len == 0)
            return;

          if (len == -1)
            {
#if MATANZA_SERVER
              if (mat_connection_state_get(c) == MAT_STATE_PLAYING)
                mat_connection_message_printf(c, "has gone netdead.", 0);

              mat_connection_free(c, MMP_SERVER_CLOSE_SYSERROR, strerror(mat_network_errno()));
#else
              /* TODO: Handle error! */
#endif

              return;
            }

          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Read: %d\n", len);

          data->in_len += len;
        }

      ASSERT(data->in_len <= MAT_CONNECTION_IN_SIZE);

      ASSERT(data->input_handler);
      while (i < data->in_len && !data->out_gen && data->out_len < MAT_CONNECTION_OUT_BLOCK)
        {
#if MATANZA_SERVER
          if (data->state == MAT_STATE_REMOVE)
            break;
#endif
          data->input_handler(data->input_handler_data, data->in_buf[i ++]);
        }

      memmove(data->in_buf, data->in_buf + i, data->in_len - i);
      data->in_len -= i;
    }
}

INLINE void
mat_connection_input_handler_set ( MatScm *c, void (*func) (void *, unsigned char), void *funcdata )
{
  data_get(c)->input_handler = func;
  data_get(c)->input_handler_data = funcdata;
}

#if MATANZA_SERVER

/* Now we will add a function to remove a connection.
 *
 * It doesn't actually remove it from the list because that may mess up things
 * for the caller.  It sets its |state| to |MAT_STATE_REMOVE| so it gets
 * removed eventually. */

void
mat_connection_free ( MatScm *c, unsigned char reason, char *desc )
{
  MatConnection *data = data_get(c);

  ASSERT(mat_connection_state_get(c) != MAT_STATE_REMOVE);

  if (data->state == MAT_STATE_PLAYING || data->state == MAT_STATE_MACHINE)
    {
      /* TODO: Allow this! */
#if 0
      MatMovingObj *tmp;

      while (con->ship_list)
        {
          @<Send |MMP_SERVER_DEFAULT_SHIPRM| messages to machines in this universe@>;

          data->ship_list->info.player.conn = NULL;

          tmp = data->ship_list->info.player.conn_list_next;
          mat_moving_obj_unref(data->ship_list);
          data->ship_list = tmp;
        }
#endif
    }

  if (data->state != MAT_STATE_MACHINE)
    {
      mat_telnet_image_reset(data->data);
      mat_connection_printf(c,
                            "You have been disconnected from the server.\r\nReason: %s\r\n\r\n"
                            "Thanks for playing Matanza.\r\n"
                            "FU Matanza %s\r\n%s\r\n",
                            desc, VERSION, "Copyright 2000, 2001, 2002, 2003 Matanza Core");
      //free(con->buffer);
    }
#if MAT_BUILD_MMP
  else
    {
      mat_sub_machine_rm(data->data);
      mat_connection_printf(c, "%c%c", MMP_SERVER_CLOSE, reason);
      if (reason == MMP_SERVER_CLOSE_SYSERROR || reason == MMP_SERVER_CLOSE_MMPERROR)
        mat_connection_printf(c, "%s%c", desc, 0);
    }
#endif

  mat_connection_flush(c);

  data->state = MAT_STATE_REMOVE;

  free(data->addr);
  free(data->out_buf);
  mat_network_close(data->fd);
}

#endif /* MATANZA_SERVER */

int
mat_connection_printf ( MatScm *c, char *format, ... )
{
  MatConnection *data = data_get(c);
  int       slen[10];
  int       len;
  char     *write_buf;
  int       strcur = 0;
  va_list   arg;
  MatScm   *img;
  char     *tmp;
  unsigned int h, w;

  ASSERT(format);

  va_start(arg, format);

  len = 0;
  tmp = format;

  while (*tmp)
    if (*(tmp++) == '%')
      switch (*(tmp++))
        {
        case '%':
          len ++;
          break;

        case 'd':
          va_arg(arg, int);
          len += 10;
          break;

        case 'c':
          len ++;
          va_arg(arg, int);
          break;

        case 's':
          /* If you get an assert fail in next line, increase the length of slen. */
          ASSERT(strcur < sizeof(slen) / sizeof(int));
          slen[strcur] = strlen(va_arg(arg, char *));
          len += slen[strcur++];
          break;

        case 'i':
          img = va_arg(arg, MatScm *);

          ASSERT(img);
          ASSERT(img->type == &mat_scm_image_type);

          len += 2 * 2 + mat_image_size_x_get(img) * mat_image_size_y_get(img);

          break;

        default:
          ASSERT(0);
        }
    else
      len ++;

  va_end(arg);

  if (!mat_connection_buffer_resize(c, data->out_len + len))
    return 0;

  write_buf = data->out_buf + data->out_len;

  strcur = 0;

  va_start(arg, format);

  while (*format)
    if (*format == '%')
      {
        switch (*(++format))
          {
          case '%':
            *(write_buf++) = *format;
            break;
          case 'd':
            write_buf += sprintf(write_buf, "%d", va_arg(arg, int));
            break;
          case 'c':
            *(write_buf++) = (char) va_arg(arg, int);
            break;
          case 's':
            memcpy(write_buf, va_arg(arg, char *), slen[strcur]);
            write_buf += slen[strcur ++];
            break;
          case 'i':
            img = va_arg(arg, MatScm *);

            w = mat_image_size_x_get(img);
            h = mat_image_size_y_get(img);
            write_buf += sprintf(write_buf, "%c%c%c%c", w >> 8, w & 0x00ff, h >> 8, h & 0x00ff);

            memcpy(write_buf, mat_image_core_buffer_get(img), w * h);
            write_buf += w * h;
            break;
          default:
            ASSERT(0);
          }
        format++;
      }
    else
      *(write_buf++) = *(format++);

  va_end(arg);

  ASSERT(write_buf >= data->out_buf);

  data->out_len = write_buf - data->out_buf;

  ASSERT(data->out_len >= 0);

  if (data->out_len > 65000)
    mat_connection_flush(c);

  return 1;
}

int
mat_connection_flush ( MatScm *c )
{
  MatConnection *data = data_get(c);
  int retval;

#ifdef MSG_NOSIGNAL
#  define MAT_SEND_FLAGS MSG_NOSIGNAL
#else
#  define MAT_SEND_FLAGS 0
#endif

  while (data->out_len > 0 || data->out_gen)
    {
      ASSERT(data->out_buf);

#if MATANZA_SERVER
      if (data->state == MAT_STATE_REMOVE)
        return 0;
#endif

      if (data->out_len > 0)
        {
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> send (%d/%d)\n", data->fd, data->out_len);
          retval = send(data->fd, data->out_buf, data->out_len, MAT_SEND_FLAGS);
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- send (%d/%d)\n", retval, data->out_len);

          if (retval == SOCKET_ERROR)
            switch (mat_network_errno())
              {
              case SOCKET_CALL_INTR:
                /* I think SOCKET_CALL_INTR will never happened on non-blocking
                 * sockets, but lets be safe. */
                break;

              case EPIPE:

#ifdef ECONNRESET
              case ECONNRESET:
#endif

#ifdef WSAECONNRESET
#  if WSAECONNRESET != EPIPE
              case WSAECONNRESET:
#  endif
#endif

#ifdef WSAECONNABORTED
#  if WSAECONNABORTED != EPIPE
              case WSAECONNABORTED:
#  endif
#endif

#ifdef ECONNABORTED
              case ECONNABORTED:
#endif

                MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("send failed: connection closed\n");
                return 0;

              case EAGAIN:

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

#if MATANZA_SERVER
                /* When a connection for a player is slow, we discard his data.  
                 *
                 * When a machine connection gets slow, we leave the data in the
                 * buffer and return. */

                MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("send failed: connection lagging\n");

                if (mat_connection_state_get(c) != MAT_STATE_MACHINE)
                  {
                    mat_telnet_clean_set(data->data, 0);
                    data->out_len = 0;
                  }
                else
                  return 1;
#else
                /* TODO: Handle error in connection with server! */
#endif

                break;

              default:
                fprintf(stderr, "%s: send: Unknown error (code %d:%s)\n", program_name, mat_network_errno(), strerror(mat_network_errno()));
                exit(EXIT_FAILURE);
              }

          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("send successful\n");

          if (data->out_len == retval)
            data->out_len = 0;
          else
            {
              data->out_len -= retval;
              memmove(data->out_buf, data->out_buf + retval, data->out_len);
            }
        }

      while (data->out_gen && data->out_len < MAT_CONNECTION_OUT_BLOCK)
        {
#if MATANZA_SERVER
          if (data->state == MAT_STATE_REMOVE)
            return 0;
#endif
          data->out_gen(c, data->out_gen_data);
        }
    }

  return 1;
}

#if MATANZA_SERVER
void
mat_connection_message_printf ( MatScm *c, char *msg, int showip )
{
  MatConnection *data = data_get(c);
  switch (data->type)
    {
    case MAT_CONNECTION_TYPE_TELNET:
      mat_telnet_message_printf(data->data, msg, showip);
      break;
    case MAT_CONNECTION_TYPE_MMP:
      /* TODO: Handle this. */
      break;
    }
}
#endif

#if MATANZA_SERVER
INLINE void
mat_connection_update ( MatScm *c )
{
  MatConnection *data = data_get(c);
  if (data->out_gen || data->out_len || data->state == MAT_STATE_REMOVE)
    return;

  switch (data->type)
    {
    case MAT_CONNECTION_TYPE_TELNET:
      mat_telnet_update(data->data);
      break;
#if MAT_BUILD_MMP
    case MAT_CONNECTION_TYPE_MMP:
      mat_machine_update(data->data);
      break;
#endif
    }
}
#endif

INLINE int
mat_connection_write ( MatScm *c, void *buffer, size_t len )
{
  MatConnection *data = data_get(c);
  if (!mat_connection_buffer_resize(c, data->out_len + len))
    return 0;

  memcpy(data->out_buf + data->out_len, buffer, len);
  data->out_len += len;

  return 1;
}

INLINE static int
mat_connection_buffer_resize ( MatScm *c, int nlen )
{
  MatConnection *data = data_get(c);
  char *nbuf;
  int   nsiz;

  if (nlen < data->out_siz)
    return 1;

  if (MAT_CONNECTION_OUT_MAX < nlen)
    return 0;

  nsiz = 2048 + nlen;
  nbuf = realloc(data->out_buf, nsiz);
  if (!nbuf)
    return 0;

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("mat_connection_buffer_resize: %d\n", nsiz);

  data->out_buf = nbuf;
  data->out_siz = nsiz;

  return 1;
}

INLINE void
mat_connection_out_gen_set ( MatScm *c, void (*gen) ( MatScm *, void *), void *data )
{
  data_get(c)->out_gen      = gen;
  data_get(c)->out_gen_data = data;
}
