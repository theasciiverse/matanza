#include <matanza.h>

typedef struct MatTerm MatTerm;

struct MatTelnet
{
  /* The descriptor of this connection. */
  MatScm              *con;

  /* Describes the terminal. */
  MatTerm             *term;

  /* The name of the terminal. */
  char                 term_name[MAT_TELNET_TERM_NAME_MAX];

  /* The state (attribute) of the terminal. */
  int                  term_state;

  /* Buffer containing the telnet arguments. */
  char                 telnet_args[64];
  unsigned int         telnet_args_pos;

  /* The number of columns (characters) in the telnet window. */
  int                  xwinsize;

  /* The number of rows (characters) in the telnet window. */
  int                  ywinsize;

  void               (*key_handler) (MatScm *, void *data, int);
  void                *key_data;

  /* The window in this Telnet connection. */
  MatScm              *window;

  /* The number of views in this Telnet connection. */
  int                  window_count;

  /* TODO: Add a description here. */
  int                  clean;

  /* TODO: Remove this. */
  //struct aa_hardware_params params;

  int                  input_state;

  /* Do we need to reflush the buffers as the result of user input? */
  int                  flush;

  unsigned char       *graph_imagebuffer;
  unsigned char       *graph_textbuffer;
  unsigned char       *graph_textbufferold;
  unsigned char       *graph_attrbuffer;
  unsigned char       *graph_attrbufferold;
};

struct MatTerm
{
  char *code    [MAT_TERM_CODES];
  int   code_len[MAT_TERM_CODES];

  char *cls;

  char *cursor_hide;
  char *cursor_show;
  char *cursor_pos;
  char *cursor_adv;
};

INLINE static void check_neg ( MatScm *t );
INLINE static void flush_un ( MatScm *t );
INLINE static void flush ( MatScm *t );
INLINE static void input_got ( MatScm *t, int c );
INLINE static void t_clear_screen ( MatScm *t );
INLINE static void t_attr_set ( MatScm *t, int term_type );
INLINE static void t_move ( MatScm *t, unsigned int y, unsigned int x );
INLINE static void t_advance ( MatScm *t, unsigned int len );

MatTerm term_vt100 = {

  { "\033[0;39m", "", "\033[1;39m", "\033[1;39m", "\033[0;7m",
    "\033[0;7m", "\033[0;31m", "\033[1;31m", "\033[0;34m", "\033[0;32m" },
  {           7,   0,           7,            7,           6,
              6,            7,            7,            7,            7 },
  "\033[2J",
  "\033[?25l", "\033[?25h", "\033[%d;%dH", "\033[%dC"
};

INLINE static MatTelnet *
data_get ( MatScm *obj )
{
  MatTelnet *data;

  ASSERT(obj);

  if (mat_scm_obj_type_get(obj) == &mat_scm_widgetplay_type)
    obj = mat_widget_play_window_get(obj);

  if (mat_scm_obj_type_get(obj) == &mat_scm_window_type)
    obj = mat_window_telnet_get(obj);

  ASSERT(obj);
  ASSERT(mat_scm_obj_type_get(obj) == &mat_scm_telnet_type);

  data = mat_scm_obj_data_get(obj);

  ASSERT(data);
  ASSERT(data->con);

  return data;
}
void
mat_telnet_free ( MatScm *t )
{
  MatTelnet *data = data_get(t);

  free(data->graph_imagebuffer);
  free(data->graph_textbuffer);
  free(data->graph_textbufferold);
  free(data->graph_attrbuffer);
  free(data->graph_attrbufferold);

  free(data);
}

#define MAT_KEY_DATA  0
#define MAT_KEY_IAC   1
#define MAT_KEY_WILL  2
#define MAT_KEY_WONT  3
#define MAT_KEY_DO    4
#define MAT_KEY_DONT  5
#define MAT_KEY_SB    6
#define MAT_KEY_SE    7
#define MAT_KEY_27    8
#define MAT_KEY_79    9
#define MAT_KEY_91   10

INLINE static void
input_got ( MatScm *t, int c )
{
  MatTelnet *data = data_get(t);
  if (data->key_handler)
    data->key_handler(t, data->key_data, c);
}

/* TODO: There are problems when a packet gets cut in the middle of a telnet
 * command.  They should be fixed. */

static void
input ( void *funcdata, unsigned char c )
{
  MatScm *t = (MatScm *) funcdata;
  MatTelnet *data = data_get(t);

  //c = *(buffer + i) & 0377;

  switch (data->input_state)
    {
    case MAT_KEY_DATA:
      switch (c)
        {
        case IAC:
          data->input_state = MAT_KEY_IAC;
          break;
        case 27:
          data->input_state = MAT_KEY_27;
          break;
        case 0:
          break;
        default:
          input_got(t, (int) c);
        }
      break;

    case MAT_KEY_IAC:
      switch(c)
        {
        case WILL:
          data->input_state = MAT_KEY_WILL;
          break;
        case WONT:
          data->input_state = MAT_KEY_WONT;
          break;
        case DO:
          data->input_state = MAT_KEY_DO;
          break;
        case DONT:
          data->input_state = MAT_KEY_DONT;
          break;
        case IAC:
          break;
        case IP:
          if (mat_connection_state_get(t) == MAT_STATE_PLAYING)
            mat_connection_message_printf(t, "has left for the real world.", 0);
          mat_connection_free(t, MMP_SERVER_CLOSE_CLIENT, "User interrupt.");
          break;
        case BREAK:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Got IAC BREAK\n");
          break;
        case AYT:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Got IAC AYT\n");
          break;
        case AO:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Got IAC AO\n");
          break;
        case SB:
          data->input_state = MAT_KEY_SB;
          data->telnet_args_pos = 0;
          break;
        default:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Ignored Input: [key:%d][state:%d]\n", c, data->input_state);
          data->input_state = MAT_KEY_DATA;
        }
      break;

    case MAT_KEY_27:
      switch (c)
        {
        case 79:
          data->input_state = MAT_KEY_79;
          break;

        case 91:
          data->input_state = MAT_KEY_91;
          break;

        default:
          data->input_state = MAT_KEY_DATA;
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Ignored Input: [key:%d][state:%d]\n", c, data->input_state);
        }
      break;

    case MAT_KEY_79:
    case MAT_KEY_91:
      switch(c)
        {
        case 65:
          input_got(t, KEY_ARROW_UP);
          break;
        case 66:
          input_got(t, KEY_ARROW_DOWN);
          break;
        case 67:
          input_got(t, KEY_ARROW_RIGHT);
          break;
        case 68:
          input_got(t, KEY_ARROW_LEFT);
          break;
        default:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Ignored Input: [key:%d][state:%d]\n", c, data->input_state);
        }
      data->input_state = MAT_KEY_DATA;
      break;

    case MAT_KEY_SB:
      if (c == IAC)
        data->input_state = MAT_KEY_SE;
      else if (data->telnet_args_pos < sizeof(data->telnet_args))
        data->telnet_args[data->telnet_args_pos ++] = c;
      break;

    case MAT_KEY_SE:
      if (c != SE)
        {
          ASSERT(c == IAC);
          data->input_state = MAT_KEY_IAC;
        }
      else
        {
          int j, width, height;

          switch (data->telnet_args[0])
            {
            case TELOPT_TTYPE:

              /* TODO: Just kill this player. */
              if ((data->telnet_args[1] & 0xff) != TELQUAL_IS)
                ASSERT(0);

              for (j = 0; j < 14 && j + 3 < data->telnet_args_pos; j ++)
                data->term_name[j] = tolower(data->telnet_args[j + 3]);

              data->term_name[j] = 0;

              MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Got terminal name: %s\n", data->term_name);
              break;

            case TELOPT_NAWS:

              if (data->telnet_args_pos == 5)
                {
                  width  = (data->telnet_args[1] & 0xff) << 8;
                  width |= (data->telnet_args[2] & 0xff);
                  height = (data->telnet_args[3] & 0xff) << 8;
                  height|= (data->telnet_args[4] & 0xff);

                  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Got window size: %d x %d\n", width, height);

                  if (width > 0 && height > 0)
                    mat_telnet_resize(t, width, height);
                }

              break;

            case TELOPT_LINEMODE:

              if (data->telnet_args_pos < 3)
                {
                  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Telnet: Bad telnet args for TELOPT_LINEMODE: %s:%d\n", __FILE__, __LINE__);
                }
              else if ((data->telnet_args[2] & 0xff) == LM_SLC)
                {
                  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Disabling TELOPT_LINEMODE for player.\n");
                  mat_connection_printf(t, "%c%c%c%c%c%c", IAC, SB, TELOPT_LINEMODE, LM_SLC, IAC, SE);
                }
              else if ((data->telnet_args[2] & 0xff) == LM_MODE)
                {
                  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Use edit mode: %d\n", data->telnet_args[3]);
                }
              else
                {
                  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Telnet: Bad char received in TELOPT_LINEMODE: %s:%d\n", __FILE__, __LINE__);
                }

              break;

            default:
              MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Unknown telnet parameter: %d\n", data->telnet_args[0]);
              break;
            }
          data->input_state = MAT_KEY_DATA;
          check_neg(t);
        }
      break;

    case MAT_KEY_WILL:
      switch (c)
        {
        case TELOPT_TTYPE:
        case TELOPT_NAWS:
          mat_connection_printf(t, "%c%c%c%c%c%c", IAC, SB, c, TELQUAL_SEND, IAC, SE);
          mat_connection_flush(t);
          break;

        case TELOPT_LINEMODE:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Telnet: Player TELOPT_LINEMODE\n");
          break;

        default:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Telnet: Got WILL %d\n", c);

          mat_connection_printf(t, "%c%c%c", IAC, DONT, c);
          mat_connection_flush(t);
        }

      data->input_state = MAT_KEY_DATA;
      break;

    case MAT_KEY_WONT:
      switch (c)
        {
        case TELOPT_TTYPE:
          strcpy(data->term_name, "dumb");
          check_neg(t);
          break;

        case TELOPT_NAWS:
          mat_telnet_resize(t, 80, 24);
          check_neg(t);
          break;

        case TELOPT_ECHO:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Telnet: Client won't echo\n");
          mat_connection_printf(t, "%c%c%c", IAC, WONT, c);
          mat_connection_flush(t);
          break;

        case TELOPT_LINEMODE:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Telnet: Client won't do linemode (may do buffering)\n");
          break;

        default:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Telnet: Got WONT %d\n", c);

          mat_connection_printf(t, "%c%c%c", IAC, DONT, c);
          mat_connection_flush(t);
        }

      data->input_state = MAT_KEY_DATA;
      break;

    case MAT_KEY_DO:
      switch (c)
        {
        case TELOPT_LOGOUT:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Telnet: Got LOGOUT\n");

          mat_connection_printf(t, "%c%c%c", IAC, WILL, c);
          mat_connection_flush(t);

          if (mat_connection_state_get(t) == MAT_STATE_PLAYING)
            mat_connection_message_printf(t, "has left for the real world.", 0);
          mat_connection_free(t, MMP_SERVER_CLOSE_CLIENT, "User logout.");

          break;

        case TELOPT_ECHO:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Telnet: Got DO ECHO\n");
          break;

        case TELOPT_SGA:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Telnet: Got DO SGA\n");

          mat_connection_printf(t, "%c%c%c", IAC, WILL, c);
          mat_connection_flush(t);

          break;

        default:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Telnet: Got DO %d\n", c);

          mat_connection_printf(t, "%c%c%c", IAC, WONT, c);
          mat_connection_flush(t);
        }
      data->input_state = MAT_KEY_DATA;
      break;

    case MAT_KEY_DONT:
      MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Telnet: Got DONT %d\n", c);

      mat_connection_printf(t, "%c%c%c", IAC, WONT, c);
      mat_connection_flush(t);

      data->input_state = MAT_KEY_DATA;
      break;

    default:
      ASSERT(0);
    }
}

INLINE MatScm *
mat_telnet_connection_get ( MatScm *t )
{
  return data_get(t)->con;
}

MatScm *
mat_telnet_new ( MatScm *con )
{
  MatTelnet *data;
  MatScm *telnet;

  data = malloc(sizeof(MatTelnet));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  telnet = mat_scm_obj_malloc(&mat_scm_telnet_type, data);

  data->con = con;

  data->clean = 0;

  data->input_state = MAT_KEY_DATA;

  data->term_name[0] = 0;
  data->term = &term_vt100;
  data->term_state = MAT_NORMAL;

  data->xwinsize = -1;
  data->ywinsize = -1;

  data->window = NULL;
  data->window_count = 0;

  data->key_handler = NULL;

  mat_connection_printf(con, "%s" "%c%c%c" "%c%c%c" "%c%c%c" "%c%c%c" "%c%c%c",
                        "-=> Please stand by . . .\r\n"
                        "-=> Matanza is preparing your machine to maximize your experience . . . ",
                        IAC, DO,   TELOPT_TTYPE,
                        IAC, DO,   TELOPT_NAWS,
                        IAC, DONT, TELOPT_LINEMODE,
                        IAC, WILL, TELOPT_SGA,
                        IAC, WILL, TELOPT_ECHO);

  mat_connection_flush(con);

  mat_connection_state_set(con, MAT_STATE_PREPARE);

  data->flush               = 0;
  data->graph_textbuffer    = NULL;
  data->graph_textbufferold = NULL;
  data->graph_attrbuffer    = NULL;
  data->graph_attrbufferold = NULL;
  data->graph_imagebuffer   = NULL;

  mat_connection_input_handler_set(con, input, telnet);

  return telnet;
}

INLINE int
mat_telnet_xwinsize_get ( MatScm *t )
{
  return data_get(t)->xwinsize;
}

INLINE int
mat_telnet_ywinsize_get ( MatScm *t )
{
  return data_get(t)->ywinsize;
}

INLINE void
mat_telnet_xwinsize_set ( MatScm *t, int size )
{
  data_get(t)->xwinsize = size;
}

INLINE void
mat_telnet_ywinsize_set ( MatScm *t, int size )
{
  data_get(t)->ywinsize = size;
}

INLINE static void
check_neg ( MatScm *t )
{
  if (mat_connection_state_get(t) == MAT_STATE_PREPARE)
    {
      MatTelnet *data = data_get(t);

      ASSERT(!data->window);

      if (data->term_name[0] == 0 || data->xwinsize == -1 || data->ywinsize == -1)
        return;

      /* Create a new view. */
      mat_window_new(t);
      mat_telnet_window_size_set(t);

      /* For the moment we only support vt100.  In the future other terminal
       * types might be added.  We should check data->term_name. */
      data->term = &term_vt100;

      t_clear_screen(t);
      mat_connection_printf(t, "%s", data_get(t)->term->cursor_hide);

      mat_connection_state_set(t, MAT_STATE_NORMAL);

      mat_animation_000_play(mat_telnet_window_get(t));
    }
}

INLINE unsigned char *
mat_telnet_imgbuffer_get ( MatScm *t )
{
  ASSERT(t);
  return data_get(t)->graph_imagebuffer;
}

unsigned char *
mat_telnet_textbuffer_get ( MatScm *t )
{
  ASSERT(t);
  return data_get(t)->graph_textbuffer;
}

INLINE void
mat_telnet_textbuffer_set ( MatScm *t, int pos, char c, char a )
{
  ASSERT(pos >= 0);
  ASSERT(pos < mat_telnet_xwinsize_get(t) * mat_telnet_ywinsize_get(t));

  data_get(t)->graph_textbuffer[pos] = c;
  data_get(t)->graph_attrbuffer[pos] = a;
}

INLINE int
mat_telnet_clean_get ( MatScm *t )
{
  return data_get(t)->clean;
}

INLINE void
mat_telnet_clean_set ( MatScm *t, int value )
{
  data_get(t)->clean = value;
}

void
mat_telnet_resize ( MatScm *t, int width, int height )
{
  MatTelnet *data = data_get(t);
  int c;

  if (width <= 0)
    width = 80;
  if (height <= 0)
    height = 24;

  ASSERT(width  > 0);
  ASSERT(height > 0);

  if (mat_telnet_xwinsize_get(t) == width && mat_telnet_ywinsize_get(t) == height)
    return;

  if (data->graph_imagebuffer)
    free(data->graph_imagebuffer);
  if (data->graph_textbuffer)
    free(data->graph_textbuffer);
  if (data->graph_textbufferold)
    free(data->graph_textbufferold);
  if (data->graph_attrbuffer)
    free(data->graph_attrbuffer);
  if (data->graph_attrbufferold)
    free(data->graph_attrbufferold);

  mat_telnet_xwinsize_set(t, width);
  mat_telnet_ywinsize_set(t, height);

#if 0
  con->graph_imgwidth  = mat_connection_xwinsize_get(con) * 2;
  con->graph_imgheight = mat_connection_ywinsize_get(con) * 2;
#endif

  c = width * height;

  data->graph_imagebuffer = malloc(c * 4);
  if (!data->graph_imagebuffer)
    {
      /* TODO: Don't exit, just kill the player. */
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->graph_textbuffer = malloc(c);
  if (!data->graph_textbuffer)
    {
      /* TODO: Don't exit, just kill the player. */
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->graph_attrbuffer = malloc(c);
  if (!data->graph_attrbuffer)
    {
      /* TODO: Don't exit, just kill the player. */
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  memset(data->graph_textbuffer, ' ',        c);
  memset(data->graph_attrbuffer, MAT_NORMAL, c);

  memset(data->graph_imagebuffer, 0, 4 * mat_telnet_xwinsize_get(t) * mat_telnet_ywinsize_get(t));

  data->graph_textbufferold = NULL;
  data->graph_attrbufferold = NULL;
  mat_telnet_clean_set(t, 0);
  mat_telnet_window_size_set(t);
}

INLINE int
mat_telnet_window_count_get ( MatScm *t )
{
  return data_get(t)->window_count;
}

INLINE MatScm *
mat_telnet_window_get ( MatScm *t )
{
  return data_get(t)->window;
}

void
mat_telnet_window_size_set ( MatScm *t )
{
  if (!mat_telnet_window_get(t))
    return;

  mat_window_chars_set(mat_telnet_window_get(t), 0, 0, mat_telnet_xwinsize_get(t), mat_telnet_ywinsize_get(t));
}

INLINE void
mat_telnet_window_count_set ( MatScm *t, int nv )
{
  data_get(t)->window_count = nv;
}

void
mat_telnet_message_printf ( MatScm *t, char *msg, int showip )
{
#if 0
  MatView *v;

  for (v = mat_telnet_window_get(t); v; v = mat_view_next_get(v))
    {
      MatShip     *sh;
      MatUniverse *un;
      char        *name;

      sh = mat_view_ship_get(v);
      un = mat_movingobj_universe_get(mat_ship_movingobj_get(sh));
      name = mat_movingobj_name_get(sh);

      if (showip)
        mat_universe_message_printf(un, "-=> %s <%s> %s", name, mat_connection_addr_get(t->con), msg);
      else
        mat_universe_message_printf(un, "-=> %s %s", name, msg);
    }
#endif
}

INLINE void
mat_telnet_update ( MatScm *t )
{
  if (mat_telnet_window_get(t) && mat_window_update(mat_telnet_window_get(t)))
    {
      mat_telnet_flush_schedule(t);
      mat_telnet_flush(t);
    }
}

INLINE int
mat_telnet_imgwidth_get ( MatScm *t )
{
  return data_get(t)->xwinsize * 2;
}

INLINE int
mat_telnet_imgheight_get ( MatScm *t )
{
  return data_get(t)->ywinsize * 2;
}

INLINE void
mat_telnet_flush ( MatScm *t )
{
  if (!data_get(t)->flush)
    return;

  data_get(t)->flush = 0;

  if (arg_compress)
    flush(t);
  else
    flush_un(t);

  t_attr_set(t, MAT_NORMAL);

  if (!mat_connection_flush(t))
    {
      if (mat_connection_state_get(t) == MAT_STATE_PLAYING)
        mat_connection_message_printf(t, "has gone netdead.", 0);
      mat_connection_free(t, MMP_SERVER_CLOSE_SYSERROR, strerror(errno));
    }
}

INLINE static void
flush_un ( MatScm *t )
{
  MatTelnet *data = data_get(t);
  unsigned char *buf, *att;
  int len;
  int x, y, w, h;

  buf = data->graph_textbuffer;
  att = data->graph_attrbuffer;

  ASSERT(buf);
  ASSERT(att);

  w = mat_telnet_xwinsize_get(t);
  h = mat_telnet_ywinsize_get(t);

  t_move(t, 1, 1);

  for (y = 0; y < h; y ++)
    {
      for (x = 0; x < w; x += len)
        {
          for (len = 1; len + x < w && att[x + len] == att[x]; len ++);
          t_attr_set(t, att[x]);
          mat_connection_write(t, buf + x, len);
        }

      if (y < h - 1)
        {
          mat_connection_printf(t, "\r\n");
          buf += w;
          att += w;
        }
    }
}

INLINE static void
flush ( MatScm *t )
{
  MatTelnet *data = data_get(t);
  int x, y, w, h, j;
  int   advl;
  char *bufold, *attold, *buf, *att;
  int   w_x = -1, w_y = -1;

  if (!data->graph_textbufferold)
    {
      unsigned int size;

      size = mat_telnet_xwinsize_get(t) * mat_telnet_ywinsize_get(t);

      data->graph_textbufferold = malloc(size);
      if (!data->graph_textbufferold)
        {
          fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
          exit(EXIT_FAILURE);
        }

      data->graph_attrbufferold = malloc(size);
      if (!data->graph_attrbufferold)
        {
          fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
          exit(EXIT_FAILURE);
        }

      ASSERT(data->graph_textbufferold);
      ASSERT(data->graph_attrbufferold);

      ASSERT(!mat_telnet_clean_get(t));
    }

  ASSERT(data->graph_textbufferold);
  ASSERT(data->graph_attrbufferold);

  if (!mat_telnet_clean_get(t))
    {
      unsigned int size;

      MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Dirty (%p)\n", t);

      t_clear_screen(t);

      size = mat_telnet_xwinsize_get(t) * mat_telnet_ywinsize_get(t);

      ASSERT(data->graph_textbufferold);
      ASSERT(data->graph_attrbufferold);

      memset(data->graph_textbufferold, (char) ' '       , size);
      memset(data->graph_attrbufferold, (char) MAT_NORMAL, size);

      mat_telnet_clean_set(t, 1);
    }

  buf    = data->graph_textbuffer;
  att    = data->graph_attrbuffer;
  bufold = data->graph_textbufferold;
  attold = data->graph_attrbufferold;

  ASSERT(buf);
  ASSERT(att);
  ASSERT(bufold);
  ASSERT(attold);

  w = mat_telnet_xwinsize_get(t);
  h = mat_telnet_ywinsize_get(t);

  advl = strlen(data->term->cursor_adv);

  for (y = 0; y < h; y ++)
    {
      for (x = 0; x < w;)
        {
          for (j = x; j < w && bufold[j] == buf[j] && (attold[j] == att[j] || buf[j] == ' '); j++);

          if (j < w)
            {
              t_attr_set(t, att[j]);

              ASSERT(x <= j);

              if (w_x == -1)
                {
                  t_move(t, y + 1, j + 1);
                }
              else if (y == w_y && j == w_x)
                {
                  /* Noop. */
                }
              else if (y != w_y)
                {
                  if (j == 0 && y == w_y + 1)
                    mat_connection_printf(t, "\r\n");
                  else
                    t_move(t, y + 1, j + 1);
                }
              else
                t_advance(t, j - w_x);

              w_x = j;
              w_y = y;

              mat_connection_write(t, buf + j, 1);
              w_x ++;

              attold[j] = att[j];
              bufold[j] = buf[j];

              x = j + 1;
            }
          else
            x = j;
        }

      buf += w;
      att += w;
      bufold += w;
      attold += w;
    }
}

INLINE void
mat_telnet_key_handler_set ( MatScm *t, void (*func) (MatScm *, void *, int), void *data )
{
  data_get(t)->key_handler = func;
  data_get(t)->key_data = data;
}

INLINE void
mat_telnet_imgbuffer_set ( MatScm *t, unsigned int pos, unsigned char value )
{
  ASSERT(t);
  ASSERT(pos <= mat_telnet_imgwidth_get(t) * mat_telnet_imgheight_get(t));

  data_get(t)->graph_imagebuffer[pos] = value;
}

INLINE unsigned char
mat_telnet_imgbuffer_pos_get ( MatScm *t, unsigned int pos )
{
  ASSERT(pos <= mat_telnet_imgwidth_get(t) * mat_telnet_imgheight_get(t));
  return data_get(t)->graph_imagebuffer[pos];
}

INLINE void
mat_telnet_flush_schedule ( MatScm *t )
{
  data_get(t)->flush = 1;
}

INLINE void
mat_telnet_close ( MatScm *t, char *reason )
{
  t_clear_screen(t);
  mat_connection_printf(t, "%s", data_get(t)->term->cursor_show);
  t_move(t, 0, 0);

  mat_connection_free(t, MMP_SERVER_CLOSE_CLIENT, reason);
}

void
mat_telnet_window_set ( MatScm *telnet, MatScm *window )
{
  data_get(telnet)->window = window;
}

INLINE void
mat_telnet_image_reset ( MatScm *p )
{
  MatTelnet *data = data_get(p);

  memset(data->graph_imagebuffer, 0, 4 * mat_telnet_xwinsize_get(p) * mat_telnet_ywinsize_get(p));

  if (data->graph_textbufferold && mat_telnet_clean_get(p))
    {
      unsigned int size = mat_telnet_xwinsize_get(p) * mat_telnet_ywinsize_get(p);
      ASSERT(data->graph_attrbufferold);
      memset(data->graph_textbufferold, (char) ' '       , size);
      memset(data->graph_attrbufferold, (char) MAT_NORMAL, size);
    }

  mat_connection_printf(p, "%s", data->term->cls);
  t_move(p, 1, 1);
}

INLINE static void
t_clear_screen ( MatScm *t )
{
  mat_connection_printf(t, "%s", data_get(t)->term->cls);
}

INLINE static void
t_attr_set ( MatScm *t, int term_type )
{
  MatTelnet *data = data_get(t);
  if (term_type != data->term_state)
    {
      mat_connection_printf(t, data->term->code[(int)(term_type)]);
      data->term_state = term_type;
    }
}

INLINE static void
t_move ( MatScm *t, unsigned int y, unsigned int x )
{
  mat_connection_printf(t, data_get(t)->term->cursor_pos, y, x);
}

INLINE static void
t_advance ( MatScm *t, unsigned int len )
{
  mat_connection_printf(t, data_get(t)->term->cursor_adv, len);
}
