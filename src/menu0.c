#include <matanza.h>

typedef struct MatMenu0        MatMenu0;
typedef enum   MatMenu0UnMode  MatMenu0UnMode;
typedef struct MatMenu0Focus   MatMenu0Focus;

struct MatMenu0Focus
{
  MatScm          *text;
  MatScm          *label;
};

enum MatMenu0UnMode
{
  MAT_MENU0_UN_MODE_JOIN,
  MAT_MENU0_UN_MODE_NEW
};

struct MatMenu0
{
  /* Parent window */
  MatScm                  *window;

  char             nickname_buffer[MAT_USER_MAX + 1];
  char             password_buffer[MAT_PASSWORD_MAX + 1];
  char             un_name_buffer[MAT_UNIVERSE_NAME_MAX + 1];

  MatMenu0UnMode           un_mode;
  MatScm                  *un_type;
  MatScm                  *un;

  MatMenu0Focus            focus_nickname;
  MatMenu0Focus            focus_password;

  MatMenu0Focus            focus_un_mode;
  MatMenu0Focus            focus_un_list;
  MatMenu0Focus            focus_un_name;

  MatMenu0Focus            focus_play;
  MatMenu0Focus            focus_quit;

  MatScm                  *tip_label;

  MatScm                  *password;

  MatScm                  *w_un_mode_list;

  MatScm                  *w_un_name_label;
  MatScm                  *w_un_name;

  MatScm                  *w_un_list_label;
  MatScm                  *w_un_list;
};

static void tip_set ( MatScm *view, void *funcdata );
static int un_text ( void *funcdata, char *buf, int size );
static void un_move ( void *funcdata, int dir );
static int unmode_text ( void *funcdata, char *buf, int size );
static void unmode_move ( void *funcdata, int dir );
static INLINE void unmode_rm ( MatMenu0 *data );
static void unmode_set ( MatMenu0 *data );
static void quit ( MatScm *window, void *funcdata );
static void play ( MatScm *window, void *funcdata );
static int list_text ( char *src, char *dst, int n );

#define BMSG0 "Welcome to Matanza"
#define BMSG1 "Matanza is developed by Alejandro Forero Cuervo, Nelson Castillo and Sergio García..."
void
mat_menu0_start ( MatScm *window )
{
  MatMenu0       *data;
  MatScm         *widget;

  static int init = 0;

  static MatScm *text;

  /* Please send me more names. */
  static char *name[] =
  {
    "Joe", "CmdrTaco", "Rob", "Pong", "Hob", "John", "Pepe", "Moe", "Bart",
    "Cmdr", "Hemos", "Azul", "Bull", "Hal", "Fox", "Zeus", "Odin", "Thor",
    "Ra", "God", "Star", "Cow", "Jack", "Pat", "Dude", "Wyrm", "Poet", "Llama",
    "Argos", "Galactus", "Javo", "Vulpex", "MrPong", "JoeCool", "Morgon",
    "Magneto", "Polux", "Magician", "Vader", "Draco", "Dragon", "Demon", "DJ",
    "Mystery", "Killer", "Slayer", "Vampire", "Paco", "Gato", "Midnight",
    "Spock", "Kirk", "Bastard", "Reindeer", "Hitler", "Stalin", "Lenin",
    "Guevara", "Fidel", "Khan", "Napoleon", "Ulises", "Futura", "Tux",
    "Mozilla", "Duke", "Bowie", "SpaceBoy", "Gorilla", "Godzilla", "Chuk",
    "Mercury", "Silver", "Blake", "Stallman", "Raymond", "Torvalds", "Wall",
    "Gates", "Jobs", "Michael", "Abyss", "DotCom", "Dolly", "Sturm", "Tanis",
    "Eastwood", "Arnold", "Joey", "Joy", "Snake", "Anaconda", "Orb", "Erinyes",
    "Eyes", "Shadow", "Andy", "Pett", "Frog", "Drop", "Hell", "Mega", "Hard",
    "Dharma", "Punk", "Jesse", "Ugo", "Dancer", "Zuul", "Pike", "Duck",
    "Derek", "Eric", "Richard", "ESR", "RMS", "Master", "Bill", "Steve",
    "Hax0r", "Lamer", "Cracker", "Irving", "Kiddo", "Waine", "Kent", "Wright",
    "Stephen", "Fred", "Tintin", "Haddock", "Milu", "Shiva", "Apollo", "Mars",
    "Jupiter", "Butthead", "Beavis", "ElCHAVO", "Laid", "Rex", "Riff",
    "Hunter", "Elvis", "Hendrix", "MajorTom", "Moe", "Musician", "Cat",
    "Vladimir", "Tricky", "Hitman",
    "Tato", "Pigidik", "NOD", "Snoopy", "Homer",

    "Darwin", "Freud", "Einstein", "Newton",

    "Gandalf", "Sauron", "Nazgul",
    "Bilbo", "Sam", "Frodo", "Elrond", "Eowyn", "Saruman", "Radagast", "Pippin",
    "Merry", "Arwen", "Boromir", "Aragorn", "Isildur", "Faramir", "Legolas",
    "Celeborn",

    "Harry", "Hagrid", "Albus", "Scabbers", "Sirus"

  };

  unsigned int    width;
  unsigned int    height;

  unsigned int    pos;

  ASSERT(window);

  if (!init)
    {
      init = 1;
      text = mat_scm_cons_new(mat_scm_string_new(BMSG0, sizeof(BMSG0) - 1),
                              mat_scm_cons_new(mat_scm_string_new(BMSG1, sizeof(BMSG1) - 1), NULL));
      mat_scm_obj_protect(&text);
    }

  mat_window_randomval_set(window, 0);
  mat_window_reset(window);

  data = malloc(sizeof(MatMenu0));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  strcpy(data->nickname_buffer, name[(int) (((double) rand() / (double) RAND_MAX) * (sizeof(name) / sizeof(char *)))]);

  data->password_buffer[0] = 0;
  data->un_name_buffer[0] = 0;

  data->window = window;

  width  = mat_window_chars_size_x_get(data->window);
  height = mat_window_chars_size_y_get(data->window);

  data->tip_label = mat_widget_label_new(NULL, MAT_NORMAL, 0, height - 2, width, MAT_WIDGET_LABEL_JUST_CENTER);

  mat_widget_window_add(data->tip_label, data->window, NULL);

  data->focus_nickname .text = mat_scm_string_data_new("Enter your nickname and press Enter.");
  data->focus_password .text = mat_scm_string_data_new("Enter the password to play.");
  data->focus_play     .text = mat_scm_string_data_new("Hit return to start the game...");
  data->focus_quit     .text = mat_scm_string_data_new("Hit enter to quit.");
  data->focus_un_mode.text = mat_scm_string_data_new("Select the universe you want to create.");
  data->focus_un_list.text = mat_scm_string_data_new("Select the universe you want to play in.");
  data->focus_un_name.text = mat_scm_string_data_new("Enter the name of your universe.");

  data->focus_nickname .label = data->tip_label;
  data->focus_password .label = data->tip_label;
  data->focus_play     .label = data->tip_label;
  data->focus_quit     .label = data->tip_label;
  data->focus_un_mode.label = data->tip_label;
  data->focus_un_list.label = data->tip_label;
  data->focus_un_name.label = data->tip_label;

  /* Make sure there is at least one universe type */
  ASSERT(mat_universetype_head_get());

  data->un_mode = mat_universe_head_get() ? MAT_MENU0_UN_MODE_JOIN : MAT_MENU0_UN_MODE_NEW;
  data->un_type = mat_universetype_head_get();
  data->un      = mat_universe_head_get();

  mat_widget_window_add(mat_widget_banner_new(data->window, text, 0), data->window, NULL);

  pos = 5;

  widget = mat_widget_label_new(mat_scm_string_data_new("Nickname:"), MAT_NORMAL, 1, pos, 9, MAT_WIDGET_LABEL_JUST_LEFT);
  mat_widget_window_add(widget, data->window, NULL);

  widget = mat_widget_text_new(data->nickname_buffer, MAT_USER_MAX, 12, pos, 0);
  mat_widget_text_on_focus(widget, tip_set, NULL, &data->focus_nickname);

  mat_widget_window_add(widget, data->window, NULL);

  pos += 2;

  if (arg_password)
    {
      widget = mat_widget_label_new(mat_scm_string_data_new("Password:"), MAT_NORMAL, 1, pos, 9, MAT_WIDGET_LABEL_JUST_LEFT);
      mat_widget_window_add(widget, data->window, NULL);

      data->password = mat_widget_text_new(data->password_buffer, MAT_PASSWORD_MAX, 12, pos, 1);
      mat_widget_text_on_focus(data->password, tip_set, NULL, &data->focus_password);

      mat_widget_window_add(widget, data->window, NULL);

      pos += 2;
    }

  widget = mat_widget_label_new(mat_scm_string_data_new("Universe:"), MAT_NORMAL, 1, pos, 9, MAT_WIDGET_LABEL_JUST_LEFT);
  mat_widget_window_add(widget, data->window, NULL);

  data->w_un_mode_list = mat_widget_list_new(12, pos, 20, data, unmode_move, unmode_text);
  mat_widget_list_on_focus(data->w_un_mode_list, tip_set, NULL, &data->focus_un_mode);
  mat_widget_window_add(data->w_un_mode_list, data->window, NULL);

  pos += 2;

  data->w_un_name_label = mat_widget_label_new(mat_scm_string_data_new("Name:"), MAT_NORMAL, 1, pos, 9, MAT_WIDGET_LABEL_JUST_LEFT);
  data->w_un_list_label = mat_widget_label_new(mat_scm_string_data_new("Join:"), MAT_NORMAL, 1, pos, 9, MAT_WIDGET_LABEL_JUST_LEFT);

  data->w_un_name = mat_widget_text_new(data->un_name_buffer, MAT_UNIVERSE_NAME_MAX, 12, pos, 0);
  mat_widget_text_on_focus(data->w_un_name, tip_set, NULL, &data->focus_un_name);

  if (data->un_mode = MAT_MENU0_UN_MODE_NEW)
    {
      char *name;
      name = mat_scm_string_data_get(mat_universetype_name_get(mat_dlist_obj_get(data->un_type)));
      mat_widget_text_set(data->w_un_name, name, strlen(name));
    }

  data->w_un_list = mat_widget_list_new(12, pos, 20, data, un_move, un_text);

  unmode_set(data);
  
  widget = mat_widget_button_new(mat_scm_string_data_new("Play"),         0, height - 1,         width / 2);
  mat_widget_button_on_click(widget, play, data);
  mat_widget_button_on_focus(widget, tip_set, NULL, &data->focus_play);
  mat_widget_window_add(widget, data->window, NULL);

  widget = mat_widget_button_new(mat_scm_string_data_new("Quit"), width / 2, height - 1, width - width / 2);
  mat_widget_button_on_click(widget, quit, data);
  mat_widget_button_on_focus(widget, tip_set, NULL, &data->focus_quit);
  mat_widget_window_add(widget, data->window, NULL);

  mat_window_redraw(data->window);
}

static void
tip_set ( MatScm *view, void *funcdata )
{
  MatMenu0Focus *data = (MatMenu0Focus *) funcdata;

  ASSERT(data);

  mat_widget_label_text_set(data->label, data->text);
  mat_widget_redraw(data->label);
}

static int
un_text ( void *funcdata, char *buf, int size )
{
  MatMenu0 *data = (MatMenu0 *) funcdata;

  ASSERT(data);
  ASSERT(data->un_mode == MAT_MENU0_UN_MODE_JOIN);
  ASSERT(data->un);
  ASSERT(mat_scm_obj_type_get(data->un) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(data->un));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(data->un)) == &mat_scm_universe_type);
  ASSERT(mat_scm_obj_data_get(mat_scm_cons_car_get(data->un)));

  return list_text(mat_scm_string_data_get(mat_universe_name_get(mat_scm_cons_car_get(data->un))), buf, size);
}

static void
un_move ( void *funcdata, int dir )
{
  MatMenu0 *data = (MatMenu0 *) funcdata;

  ASSERT(data);
  ASSERT(data->un_mode == MAT_MENU0_UN_MODE_JOIN);
  ASSERT(data->un);
  ASSERT(mat_scm_obj_type_get(data->un) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(data->un));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(data->un)) == &mat_scm_universe_type);
  ASSERT(mat_scm_obj_data_get(mat_scm_cons_car_get(data->un)));

  data->un = (dir <= 0 ? mat_scm_cons_car_get : mat_scm_cons_cdr_get)(mat_scm_cons_cdr_get(data->un));

  if (!data->un)
    data->un = (dir <= 0 ? mat_universe_tail_get : mat_universe_head_get)();

  ASSERT(data->un);
  ASSERT(mat_scm_obj_type_get(data->un) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(data->un));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(data->un)) == &mat_scm_universe_type);
  ASSERT(mat_scm_obj_data_get(mat_scm_cons_car_get(data->un)));
}

static int
unmode_text ( void *funcdata, char *buf, int size )
{
  MatMenu0 *data = (MatMenu0 *) funcdata;
  char *text;

  ASSERT(data);

  switch (data->un_mode)
    {
    case MAT_MENU0_UN_MODE_JOIN:
      text = "Join existing universe";
      break;
    case MAT_MENU0_UN_MODE_NEW:
      ASSERT(data->un_type);
      ASSERT(mat_dlist_obj_get(data->un_type));
      ASSERT(mat_scm_obj_type_get(mat_dlist_obj_get(data->un_type)) == &mat_scm_universetype_type);
      text = mat_scm_string_data_get(mat_universetype_name_get(mat_dlist_obj_get(data->un_type)));
      break;
    }

  return list_text(text, buf, size);
}

static void
unmode_move ( void *funcdata, int dir )
{
  MatMenu0 *data = (MatMenu0 *) funcdata;
  char *name;

  unmode_rm(data);

  switch (data->un_mode)
    {
    case MAT_MENU0_UN_MODE_JOIN:
      data->un_mode = MAT_MENU0_UN_MODE_NEW;
      data->un_type = (dir <= 0 ? mat_universetype_tail_get : mat_universetype_head_get)();
      name = mat_scm_string_data_get(mat_universetype_name_get(mat_dlist_obj_get(data->un_type)));
      mat_widget_text_set(data->w_un_name, name, strlen(name));
      break;
    case MAT_MENU0_UN_MODE_NEW:
      data->un_type = (dir <= 0 ? mat_dlist_prev_get : mat_dlist_next_get)(data->un_type);
      if (!data->un_type)
        {
          if (mat_universe_head_get())
            data->un_mode = MAT_MENU0_UN_MODE_JOIN;
          else
            {
              data->un_type = dir <= 0 ? mat_universetype_tail_get() : mat_universetype_head_get();
              name = mat_scm_string_data_get(mat_universetype_name_get(mat_dlist_obj_get(data->un_type)));
              mat_widget_text_set(data->w_un_name, name, strlen(name));
            }
        }
      break;
    }

  unmode_set(data);
  mat_window_redraw(data->window);
}

static INLINE void
unmode_rm ( MatMenu0 *data )
{
  switch (data->un_mode)
    {
    case MAT_MENU0_UN_MODE_JOIN:
      mat_window_rm(data->window, data->w_un_list_label);
      mat_window_rm(data->window, data->w_un_list);
      break;
    case MAT_MENU0_UN_MODE_NEW:
      mat_window_rm(data->window, data->w_un_name_label);
      mat_window_rm(data->window, data->w_un_name);
      break;
    }
}

static void
unmode_set ( MatMenu0 *data )
{
  switch (data->un_mode)
    {
    case MAT_MENU0_UN_MODE_JOIN:
      mat_widget_window_add(data->w_un_list_label, data->window, data->w_un_mode_list);
      mat_widget_window_add(data->w_un_list, data->window, data->w_un_mode_list);
      break;
    case MAT_MENU0_UN_MODE_NEW:
      mat_widget_window_add(data->w_un_name_label, data->window, data->w_un_mode_list);
      mat_widget_window_add(data->w_un_name, data->window, data->w_un_mode_list);
      break;
    }
}

static void
quit ( MatScm *window, void *funcdata )
{
  mat_telnet_close(window, "User quit.");
}

static void
play ( MatScm *window, void *funcdata )
{
  MatMenu0 *data = (MatMenu0 *) funcdata;
  MatScm *un;

  ASSERT(data);

  if (arg_password && strncmp(data->password_buffer, arg_password, MAT_PASSWORD_MAX))
    {
      mat_widget_label_text_set(data->tip_label, mat_scm_string_data_new("Wrong password, try again!"));
      mat_widget_redraw(data->tip_label);
      mat_window_focus_set(data->window, data->password);
      return;
    }

  if (data->un_mode == MAT_MENU0_UN_MODE_NEW)
    un = mat_universe_new(mat_dlist_obj_get(data->un_type), mat_scm_string_new(data->un_name_buffer, strlen(data->un_name_buffer)));
  else
    un = mat_scm_cons_car_get(data->un);

  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universe_type);
  ASSERT(mat_scm_obj_data_get(un));

  mat_menu1_start(data->window, mat_scm_string_data_new(data->nickname_buffer), un);
}

static int
list_text ( char *src, char *dst, int n )
{
  int i;

  for (i = 0; src[i] && i < n; i ++)
    dst[i] = src[i];

  return i;
}
