#include <matanza.h>

static INLINE void quit_init ( MatScm *data );
static INLINE void quit_now ( MatScm *data );
static int quit_key ( MatScm *data, int key );
static INLINE void play_help ( MatScm *data );
static INLINE void chat_start ( MatScm *data );
static void key_view ( MatScm *d, int prev );

int
mat_widget_play_key_normal ( MatScm *data, int key )
{
  ASSERT(data);

  switch (key)
    {
    case 'p':
      mat_ship_key_pause(mat_widget_play_self_get(data));
      break;

    case '[':
      key_view(data, 1);
      break;

    case ']':
      key_view(data, 0);
      break;

    case 'l':
    case KEY_ARROW_RIGHT:
      mat_ship_key_right(mat_widget_play_self_get(data));
      break;

    case KEY_ARROW_LEFT:
    case 'h':
      mat_ship_key_left(mat_widget_play_self_get(data));
      break;

    case 'k':
    case KEY_ARROW_UP:
      mat_ship_key_up(mat_widget_play_self_get(data));
      break;

    case 'j':
    case KEY_ARROW_DOWN:
      mat_ship_key_down(mat_widget_play_self_get(data));
      break;

    case 's':
      mat_widget_play_pos_swap(data);
      break;

    case 'c':
      mat_widget_play_msg_rm(data);
      break;

    case 'C':
      mat_widget_play_msg_clean(data);
      break;

    case ' ':
      mat_ship_shot(mat_widget_play_self_get(data), MAT_MISSILE_SIZE_TINY,   1);
      break;

    case 'b':
      mat_ship_shot(mat_widget_play_self_get(data), MAT_MISSILE_SIZE_TINY,  -1);
      break;

    case 'n':
      mat_ship_shot(mat_widget_play_self_get(data), MAT_MISSILE_SIZE_SMALL,  1);
      break;

    case 'm':
      mat_ship_shot(mat_widget_play_self_get(data), MAT_MISSILE_SIZE_BIG,    1);
      break;

    case '-':
    case '_':
      mat_widget_play_zoom(data, 1.1);
      break;

    case '=':
    case '+':
      mat_widget_play_zoom(data, 0.9);
      break;

    case '\r':
      chat_start(data);
      break;

    case '?':
      play_help(data);
      break;

    case 'Q':
      quit_now(data);
      break;

    case 'q':
      quit_init(data);
      break;

#if 0
    case 'i':
      ship_key_visibility(mat_widget_play_self_get(data));
      break;

    case 'z':
      ship_key_stats(mat_widget_play_self_get(data));
      break;

    case 'v':
      ship_key_messages_view(mat_widget_play_self_get(data));
      break;

#endif

    default:
      MAT_VERBOSE(MAT_VERBOSE_INTERFACE)("Ignored Input: [key:%d][state:KEY_STATE_DEFAULT]\n", key);
      return 0;
    }

  return 1;
}

static void
quit_init ( MatScm *data )
{
  ASSERT(data);
  ASSERT(mat_widget_play_self_get(data));
  ASSERT(mat_ship_name_get(mat_widget_play_self_get(data)));

  mat_widget_play_printf(data, "-=> Are you sure you want to quit, %s?",
                         mat_scm_string_data_get(mat_ship_name_get(mat_widget_play_self_get(data))));

  mat_widget_play_key_set(data, quit_key);
}

static INLINE void
quit_now ( MatScm *data )
{
  ASSERT(data);
  ASSERT(mat_widget_play_window_get(data));
  ASSERT(mat_window_telnet_get(mat_widget_play_window_get(data)));
  ASSERT(mat_telnet_connection_get(mat_window_telnet_get(mat_widget_play_window_get(data))));

  mat_connection_free(mat_telnet_connection_get(mat_window_telnet_get(mat_widget_play_window_get(data))),
                      MMP_SERVER_CLOSE_CLIENT, "User quit.");
}

static int
quit_key ( MatScm *data, int key )
{
  ASSERT(data);

  if (tolower(key) == 'q' || tolower(key) == 'y')
    {
      quit_now(data);
      return 1;
    }

  mat_widget_play_printf(data, "-=> Phew, You scared me...");

  mat_widget_play_key_set(data, mat_widget_play_key_normal);

  return 1;
}

static INLINE void
play_help ( MatScm *data )
{
  ASSERT(data);

  /* TODO: Print some help info. */
}

static INLINE void
chat_start ( MatScm *data )
{
  ASSERT(data);

  mat_widget_play_chat_len_set(data, 0);
  mat_widget_play_chat_buffer_get(data)[0] = 0;

  mat_widget_play_key_set(data, mat_widget_play_key_chat);
}

int
mat_widget_play_key_chat ( MatScm *d, int key )
{
  ASSERT(d);

  switch (key)
    {
    case '\r':

      if (mat_widget_play_chat_len_get(d))
        mat_universe_talk(mat_movingobj_universe_get(mat_widget_play_self_get(d)), mat_widget_play_self_get(d),
                          mat_scm_string_new(mat_widget_play_chat_buffer_get(d), mat_widget_play_chat_len_get(d)));

      mat_widget_play_key_set(d, mat_widget_play_key_normal);
      break;

    case 21:
      mat_widget_play_chat_len_set(d, 0);
      mat_widget_play_chat_buffer_get(d)[0] = 0;
      break;

    case 127:
    case 8:
      if (mat_widget_play_chat_len_get(d))
        {
          mat_widget_play_chat_len_set(d, mat_widget_play_chat_len_get(d) - 1);
          mat_widget_play_chat_buffer_get(d)[mat_widget_play_chat_len_get(d)] = 0;
        }
      break;

    default:
      if (isprint(key) && mat_widget_play_chat_len_get(d) < MATANZA_WIDGET_PLAY_CHAT_SIZE)
        {
          mat_widget_play_chat_buffer_get(d)[mat_widget_play_chat_len_get(d)] = (char) key;
          mat_widget_play_chat_len_set(d, mat_widget_play_chat_len_get(d) + 1);
          mat_widget_play_chat_buffer_get(d)[mat_widget_play_chat_len_get(d)] = 0;
        }
      else
        {
          MAT_VERBOSE(MAT_VERBOSE_INTERFACE)("Ignoring key: %d\n", key);
          return 0;
        }
    }

  return 1;
}

static void
key_view ( MatScm *d, int prev )
{
  MatScm *obj;

#if 0
  ASSERT(d);
  ASSERT(d->ship);

  obj = (prev ? mat_movingobj_prev_type_get : mat_movingobj_next_type_get)(mat_ship_movingobj_get(d->ship));

  if (!obj)
    obj = *(prev ? mat_universe_obj_tail_get : mat_universe_obj_head_get)(mat_ship_universe_get(d->ship), MAT_MOVINGOBJ_TYPE_SHIP);

  ASSERT(obj);
  ASSERT(mat_movingobj_data_get(obj));

  if (obj == mat_ship_movingobj_get(d->ship))
    return;

  mat_widget_play_view_set(d, mat_movingobj_data_get(obj));
#endif
}
