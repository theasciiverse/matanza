#include <matanza.h>

typedef struct MatMenu1        MatMenu1;
typedef enum   MatMenu1TeamMode  MatMenu1TeamMode;
typedef struct MatMenu1Focus   MatMenu1Focus;

struct MatMenu1Focus
{
  MatScm          *text;
  MatScm          *label;
};

enum MatMenu1TeamMode
{
  MAT_MENU1_TEAM_NONE,
  MAT_MENU1_TEAM_JOIN,
  MAT_MENU1_TEAM_NEW
};

struct MatMenu1
{
  /* Parent window */
  MatScm                *window;

  MatScm                *nickname;
  MatScm                *un;

  char                   team_name_buffer[MAT_TEAM_NAME_MAX + 1];

  MatMenu1Focus          focus_play;
  MatMenu1Focus          focus_quit;
  MatMenu1Focus          focus_team_mode;
  MatMenu1Focus          focus_team_name;
  MatMenu1Focus          focus_team_list;
  MatMenu1Focus          focus_ship;

  MatScm                *tip_label;

  MatScm                *ship_img;
  MatScm                *ship;
  MatScm                *ship_list;

  MatScm                *rotate;

  MatMenu1TeamMode       team_mode;

  MatScm                *team_mode_list;
  MatScm                *team;

  MatScm                *team_list;
  MatScm                *team_name;
  MatScm                *team_label;
};

static MatScm *ship_img ( void *funcdata );
static void ship_set ( MatMenu1 *data, MatScm *st ) ;
static void ship_move ( void *funcdata, int dir );
static int ship_text ( void *funcdata, char *buf, int size );
static void tip_set ( MatScm *view, void *funcdata );
static void quit ( MatScm *window, void *funcdata );
static void play ( MatScm *window, void *funcdata );
static void team_mode_move ( void *funcdata, int dir );
static void team_mode_set ( MatMenu1 *data, int newmode, int dir );
static int team_mode_text ( void *funcdata, char *buf, int size );
static void team_move ( void *funcdata, int dir );
static int team_text ( void *funcdata, char *buf, int size );
static int list_text ( char *src, char *dst, int n );

static char *team_mode_msgs[] = { "None", "Join a Team", "Create a Team" };
static char *team_mode_msgs_label[] = { "", "Team:", "Team name:" };

void
mat_menu1_start ( MatScm *window, MatScm *nickname, MatScm *un )
{
  MatMenu1       *data;
  MatScm         *widget;

  unsigned int    width;
  unsigned int    height;

  unsigned int    pos;

  ASSERT(window);
  ASSERT(nickname);
  ASSERT(un);

  mat_window_randomval_set(window, 0);
  mat_window_reset(window);

  data = malloc(sizeof(MatMenu1));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->nickname = nickname;
  data->un = un;

  data->team_name_buffer[0] = 0;

  data->window = window;

  width  = mat_window_chars_size_x_get(data->window);
  height = mat_window_chars_size_y_get(data->window);

  data->tip_label = mat_widget_label_new(NULL, MAT_NORMAL, 0, height - 2, width, MAT_WIDGET_LABEL_JUST_CENTER);

  mat_widget_window_add(data->tip_label, data->window, NULL);

  data->focus_play     .text = mat_scm_string_data_new("Hit return to start the game...");
  data->focus_quit     .text = mat_scm_string_data_new("Hit enter to quit.");
  data->focus_team_mode.text = mat_scm_string_data_new("Select the team mode you want.");
  data->focus_team_name.text = mat_scm_string_data_new("Enter your team's name.");
  data->focus_team_list.text = mat_scm_string_data_new("Select the team you want to join.");
  data->focus_ship     .text = NULL;

  data->focus_play     .label = data->tip_label;
  data->focus_quit     .label = data->tip_label;
  data->focus_team_mode.label = data->tip_label;
  data->focus_team_name.label = data->tip_label;
  data->focus_team_list.label = data->tip_label;
  data->focus_ship     .label = data->tip_label;

  data->team_mode = MAT_MENU1_TEAM_NONE;

  ASSERT(mat_universe_shiptype_head_get(data->un));

  //image = mat_universe_image_get(data->universe, 0);
  //image = mat_imagevector_resize(image, 2.0 * 3.0 / 3.0, 3.0);
  //mat_imagevector_palette_apply(image, mat_ship_palette[0]);

  if (mat_universe_banner_get(data->un))
    mat_widget_window_add(mat_widget_banner_new(data->window, mat_universe_banner_get(data->un), 0), data->window, NULL);

  widget = mat_widget_label_new(mat_universe_name_get(un), MAT_BOLD, 3, 3, MAT_UNIVERSE_NAME_MAX, MAT_WIDGET_LABEL_JUST_LEFT);
  mat_widget_window_add(widget, data->window, NULL);

  pos = 5;

  ASSERT(mat_universe_shiptype_head_get(data->un));
  ASSERT(mat_scm_obj_type_get(mat_universe_shiptype_head_get(data->un)) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(mat_universe_shiptype_head_get(data->un)));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(mat_universe_shiptype_head_get(data->un))) == &mat_scm_shiptype_type);

  data->ship_img = NULL;
  ship_set(data, mat_universe_shiptype_head_get(data->un));

  widget = mat_widget_label_new(mat_scm_string_data_new("Ship:"), MAT_NORMAL, 1, pos, 9, MAT_WIDGET_LABEL_JUST_LEFT);
  mat_widget_window_add(widget, data->window, NULL);

  data->ship_list = mat_widget_list_new(11, pos, 20, data, ship_move, ship_text);

  mat_widget_list_on_focus(data->ship_list, tip_set, NULL, &data->focus_ship);
  mat_widget_window_add(data->ship_list, data->window, NULL);

  pos += 2;

  widget = mat_widget_label_new(mat_scm_string_data_new("Team mode:"), MAT_NORMAL, 1, pos, 9, MAT_WIDGET_LABEL_JUST_LEFT);
  mat_widget_window_add(widget, data->window, NULL);

  data->team_mode = MAT_MENU1_TEAM_NONE;

  data->team_mode_list = mat_widget_list_new(11, pos, 20, data, team_mode_move, team_mode_text);
  mat_widget_list_on_focus(data->team_mode_list, tip_set, NULL, &data->focus_team_mode);
  mat_widget_window_add(data->team_mode_list, data->window, NULL);

  pos += 2;

  data->team_label = mat_widget_label_new(NULL, MAT_NORMAL, 1, pos, 9, MAT_WIDGET_LABEL_JUST_LEFT);
  mat_widget_window_add(data->team_label, data->window, NULL);

  data->team_name = mat_widget_text_new(data->team_name_buffer, MAT_TEAM_NAME_MAX, 11, pos, 0);
  mat_widget_text_on_focus(data->team_name, tip_set, NULL, &data->focus_team_name);

  data->team_list = mat_widget_list_new(11, pos, 20, data, team_move, team_text);
  mat_widget_list_on_focus(data->team_list, tip_set, NULL, &data->focus_team_list);

  widget = mat_widget_button_new(mat_scm_string_data_new("Play"), 0, height - 1, width / 2);
  mat_widget_button_on_click(widget, play, data);
  mat_widget_button_on_focus(widget, tip_set, NULL, &data->focus_play);
  mat_widget_window_add(widget, data->window, NULL);

  widget = mat_widget_button_new(mat_scm_string_data_new("Quit"), width / 2, height - 1, width - width / 2);
  mat_widget_button_on_click(widget, quit, data);
  mat_widget_button_on_focus(widget, tip_set, NULL, &data->focus_quit);
  mat_widget_window_add(widget, data->window, NULL);

  data->rotate = mat_render_rotate_new(ship_img, data, mat_universe_angles_get(data->un));
  mat_widget_window_add(mat_render_rotate_widget_new(data->rotate, data->window, 50, 1), data->window, NULL);

  mat_window_redraw(data->window);
  mat_window_flush(data->window);
}

static MatScm *
ship_img ( void *funcdata )
{
  MatMenu1 *data = (MatMenu1 *) funcdata;

  ASSERT(data);
  ASSERT(data->ship);
  ASSERT(data->ship_img);

  return data->ship_img;
}

static void
ship_set ( MatMenu1 *data, MatScm *st ) 
{
  MatScm *img;

  ASSERT(data);

  ASSERT(st);
  ASSERT(mat_scm_obj_type_get(st) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(st));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(st)) == &mat_scm_shiptype_type);

  data->ship = st;

  img = mat_universe_image_get(data->un, mat_shiptype_image_get(mat_scm_cons_car_get(data->ship)));

  ASSERT(img);
  ASSERT(img->type == &mat_scm_imagevector_type);

  data->ship_img = mat_imagevector_resize(img, 2.0 * 3.0 / 3.0, 3.0);

  ASSERT(data->ship_img);

  data->focus_ship.text = mat_shiptype_desc_get(mat_scm_cons_car_get(st));
  mat_widget_label_text_set(data->tip_label, data->focus_ship.text);
  mat_widget_redraw(data->tip_label);
}

static void
ship_move ( void *funcdata, int dir )
{
  MatMenu1 *data = (MatMenu1 *) funcdata;
  MatScm *st;

  ASSERT(data);
  ASSERT(data->ship);
  ASSERT(mat_scm_obj_type_get(data->ship) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(data->ship));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(data->ship)) == &mat_scm_shiptype_type);
  ASSERT(mat_scm_cons_cdr_get(data->ship));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_cdr_get(data->ship)) == &mat_scm_cons_type);

  st = (! dir ? mat_scm_cons_car_get : mat_scm_cons_cdr_get)(mat_scm_cons_cdr_get(data->ship));

  if (!st) 
    st = (! dir ? mat_universe_shiptype_tail_get : mat_universe_shiptype_head_get)(data->un);

  ASSERT(st);

  ship_set(data, st);
}

static int
ship_text ( void *funcdata, char *buf, int size )
{
  MatMenu1 *data = (MatMenu1 *) funcdata;

  ASSERT(data);
  ASSERT(data->ship);
  ASSERT(mat_scm_obj_type_get(data->ship) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(data->ship));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(data->ship)) == &mat_scm_shiptype_type);
  ASSERT(mat_scm_cons_cdr_get(data->ship));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_cdr_get(data->ship)) == &mat_scm_cons_type);

  return list_text(mat_scm_string_data_get(mat_shiptype_name_get(mat_scm_cons_car_get(data->ship))), buf, size);
}

static void
tip_set ( MatScm *view, void *funcdata )
{
  MatMenu1Focus *data = (MatMenu1Focus *) funcdata;

  ASSERT(data);

  mat_widget_label_text_set(data->label, data->text);
  mat_widget_redraw(data->label);
}

static void
quit ( MatScm *window, void *funcdata )
{
  mat_telnet_close(window, "User quit.");
}

static void
play ( MatScm *window, void *funcdata )
{
  MatMenu1 *data = (MatMenu1 *) funcdata;
  MatScm *team, *name;

  ASSERT(data);

  switch (data->team_mode)
    {
    case MAT_MENU1_TEAM_NONE:
      team = NULL;
      break;

    case MAT_MENU1_TEAM_JOIN:
      team = mat_dlist_obj_get(data->team);
      break;

    case MAT_MENU1_TEAM_NEW:
      name = mat_scm_string_data_new(data->team_name_buffer);
      if (!data->team_name_buffer[0])
        {
          int i = 0;
          do
            sprintf(data->team_name_buffer, "Team %d", i ++);
          while (mat_universe_team_get(data->un, name));
        }
      else
        {
          if (mat_universe_team_get(data->un, name))
            {
              mat_widget_label_text_set(data->tip_label, mat_scm_string_data_new("That team alredy exists!"));
              mat_widget_redraw(data->tip_label);
              mat_window_focus_set(data->window, data->team_name);
              return;
            }
        }

      ASSERT(name);
      ASSERT(mat_scm_obj_type_get(name) == &mat_scm_string_type);

      team = mat_team_new(name, data->un);
      break;
    }

  mat_widget_play_start(window, data->nickname, data->un, team, mat_scm_cons_car_get(data->ship));
}

static void
team_mode_move ( void *funcdata, int dir )
{
  MatMenu1 *data = (MatMenu1 *) funcdata;
  int newmode;

  newmode = data->team_mode + (!dir ? -1 : 1);

  if (newmode == -1)
    newmode = sizeof(team_mode_msgs) / sizeof(team_mode_msgs[0]) - 1;
  else if (newmode == sizeof(team_mode_msgs) / sizeof(team_mode_msgs[0]))
    newmode = 0;

  team_mode_set(data, newmode, dir);
}

static void
team_mode_set ( MatMenu1 *data, int newmode, int dir )
{
  ASSERT(data);
  ASSERT(data->window);

  switch (data->team_mode)
    {
    case MAT_MENU1_TEAM_NONE:
      break;
    case MAT_MENU1_TEAM_JOIN:
      mat_window_rm(data->window, data->team_list);
      break;
    case MAT_MENU1_TEAM_NEW:
      mat_window_rm(data->window, data->team_name);
      break;
    }

  data->team_mode = newmode;

  if (data->team_mode == 1)
    {
      data->team = mat_universe_team_head_get(data->un);
      if (!data->team)
        data->team_mode += !dir ? -1 : 1;
    }

  switch (data->team_mode)
    {
    case MAT_MENU1_TEAM_NONE:
      break;
    case MAT_MENU1_TEAM_JOIN:
      mat_widget_window_add(data->team_list, data->window, data->team_mode_list);
      break;
    case MAT_MENU1_TEAM_NEW:
      mat_widget_window_add(data->team_name, data->window, data->team_mode_list);
      break;
    }

  mat_widget_label_text_set(data->team_label, mat_scm_string_data_new(team_mode_msgs_label[data->team_mode]));
  mat_window_redraw(data->window);
}

static int
team_mode_text ( void *funcdata, char *buf, int size )
{
  MatMenu1 *data = (MatMenu1 *) funcdata;

  ASSERT(data);

  return list_text(team_mode_msgs[data->team_mode], buf, size);
}

static void
team_move ( void *funcdata, int dir )
{
  MatMenu1 *data = (MatMenu1 *) funcdata;

  ASSERT(data);
  ASSERT(data->team);
  ASSERT(data->un);

  data->team = (!dir ? mat_dlist_prev_get : mat_dlist_next_get)(data->team);

  if (!data->team)
    data->team = (!dir ? mat_universe_team_tail_get : mat_universe_team_head_get )(data->un);

  ASSERT(data->team);
}

static int
team_text ( void *funcdata, char *buf, int size )
{
  MatMenu1 *data = (MatMenu1 *) funcdata;

  ASSERT(data);
  ASSERT(data->team);

  return list_text(mat_scm_string_data_get(mat_team_name_get(mat_dlist_obj_get(data->team))), buf, size);
}

static int
list_text ( char *src, char *dst, int n )
{
  int i;

  for (i = 0; src[i] && i < n - 1; i ++)
    dst[i] = src[i];

  return i;
}
