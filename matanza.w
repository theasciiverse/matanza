@* Matanza - The Game.

@c
@<Portability@>
@<Definitions@>
@<Typedefs@>
@<Struct definitions@>
@<Function prototypes@>
@<Global variables@>
@<Functions@>
@<The main program@>

@* Struct definitions

@d MY_STATE_WILL      0x01
@d MY_WANT_STATE_WILL 0x02
@d MY_STATE_DO        0x04
@d MY_WANT_STATE_DO   0x08

@<Struct definitions@>=

#define AA_NORMAL_MASK 1	/*masks for attributes */
#define AA_DIM_MASK 2
#define AA_BOLD_MASK 4
#define AA_BOLDFONT_MASK 8
#define AA_REVERSE_MASK 16
#define AA_ALL 128
#define AA_EIGHT 256
#define AA_EXTENDED (AA_ALL|AA_EIGHT)

#define AA_ERRORDISTRIB 1	/*dithering types + AA_NONE */
#define AA_FLOYD_S 2
#define AA_DITHERTYPES 3

#define AA_NATTRS 5
#define AA_NPARAMS 5

struct parameters {
    unsigned int p[AA_NPARAMS];
};

typedef struct aa_font aa_font;
struct aa_font {
    unsigned char *data;
    int height;
    char *name;
    char *shortname;
};

struct aa_hardware_params {
    struct aa_font *font;
    int supported;
    int minwidth, minheight;
    int maxwidth, maxheight;
    int recwidth, recheight;
    int mmwidth, mmheight;
    int width, height;
};

typedef struct MatTerm             MatTerm;
typedef struct MatLocation         MatLocation;
typedef struct MatTeam             MatTeam;
typedef struct MatShip             MatShip;
typedef struct MatMovingObj        MatMovingObj;
typedef struct MatImage            MatImage;
typedef struct MatUniverse         MatUniverse;
typedef struct MatConnection       MatConnection;

@<Structs for smart objects@>
@<Struct for one subscription@>
@<Struct for the subscriptions in one machine@>
@<Struct for the terminals@>
@<Struct for the teams@>
@<Struct for the players@>
@<Struct for moving objects@>
@<Struct for the universes@>
@<Struct for connections@>

@ We need a struct to keep information about the terminals we support so it can
be easily accessed.

@<Struct for the terminals@>=

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

@

@d MAXCHATBUFFERLEN 60

@d MAXMESSAGES   5
@d MAXMESSAGELEN 80

@d MAT_CONNECTION_SHIP_HASH_SIZE 13

@<Struct for connections@>=

struct MatConnection
{
  MatSocketT           fd;
  char                *addr;

  /* Buffer for output. */
  char                *out_buf;
  int                  out_len;
  int                  out_siz;

  int                  state;

  MatMovingObj        *ship_hash[MAT_CONNECTION_SHIP_HASH_SIZE];
  MatMovingObj        *ship_list;
  int                  ship_count;

  MatTerm             *term;

  int                  xwinsize;
  int                  ywinsize;

  int                  graph_randomval;

  int                  graph_palette[256];

  unsigned char       *graph_imagebuffer;
  unsigned char       *graph_textbuffer;
  unsigned char       *graph_textbufferold;
  unsigned char       *graph_attrbuffer;
  unsigned char       *graph_attrbufferold;

  int                  graph_imgwidth;
  int                  graph_imgheight;

  struct aa_hardware_params params; /* has not been checked */

  int                  flags;

  char                 terminal_name[14];
  int                  telnet_args;
  int                  input_state;

  char                *buffer;
  int                  bufsiz;
  int                  buflen;
  int                  bufprc;

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

      MatSubMachine    subs;
    } m;

    /* Here we keep information when this connection uses telnet. */
    struct
    {
      int              clean;
    } t;

  } info;

  void               (*anim_func) (MatConnection *);
  void                *anim_data;

  void               (*key_handler) (MatConnection *, int);

  MatConnection       *next;
  MatConnection       *prev;
};

@ |players_head| and |players_tail| hold a list with all the players.

|players_real| is the number of players currently playing (in
|MAT_STATE_PLAYING| state) in this universe.

@d MAX_UNIVERSE_NAME    32
@d MAT_UNIVERSE_MAX    256

@<Struct for the universes@>=

struct MatUniverse
{
  int                  id;

  /* The next universe in the hash of universes. */
  MatUniverse         *next_hash;

  MatMovingObj        *players_head;
  MatMovingObj        *players_tail;
  int                  players_real;

  MatSub              *subs;

  int                  updates;

  int                  crash_damage[3];

  /* TODO: Comment out this field if no support for universes is included. */
  char                 name[MAX_UNIVERSE_NAME];
  char                *path;

  MatMovingObj        *bullets;

  MatMovingObj        *smart_objs;

  MatMovingObj        *ast;
  int                  ast_cur;

  int                  pause;

  /* Information for the different zones */
  /* TODO: Comment out this field if no support for loading universes is included. */
  MatLocation         *location[256];

  /* The initial health the ships have */
  int                  health;

  /* The size of the map for this universe. */
  int                  mapsize_x;
  int                  mapsize_y;

  /* Should we draw dots for this universe (background)? */
  int                  dots;

  MatMovingObj        *objs;

  /* Single linked list of all the objects (other than ships) in this universe
   * that should be removed as soon as possible.  To traverse the list, follow
   * the |rm_next| field of every node.  The last node in the list has
   * |rm_next| set to &main, not to NULL!
   *
   * Consult the documentation next ro |rm_next| in the |MatMovingObj| struct.
   *
   * Also, keep in mind that ships (objects of type |MAT_MOVING_OBJ_SHIP|)
   * should not be added to this list.
   */

  MatMovingObj        *objs_rm;

  MatImage            *bg;
  MatImage            *mask;
};

struct MatLocation
{
  /* Should the objects crash against this object? */
  int              crash[3];

  /* How much damage does this object deal to the player on each turn?  Usually
   * 0.  Can be positive or negative. */
  int              health;

  /* Does being on this area slow the objects down? The movement of the objects
   * is multiplied with this value once for every turn. */
  double           move[3];
};

@
@<Information about the different terminal types@>=

MatTerm term_vt100 = {

  { "\033[0;39m", "", "\033[1;39m", "\033[1;39m", "\033[0;7m",
    "\033[0;7m", "\033[0;31m", "\033[1;31m", "\033[0;34m" },
  {           7,   0,           7,            7,           6,
              6,            7,            7,           7 },
  "\033[2J",
  "\033[?25l", "\033[?25h", "\033[%d;%dH", "\033[%dC"
};

@

|MAT_TEAM_BLANK| must hold |MAT_TEAM_MAX| spaces.

@d MAT_TEAM_MAX 8
@d MAT_TEAM_BLANK "        "

@<Struct for the teams@>=
struct MatTeam
{
  int           max;
  int           cur;

  int           times_dead;

  char         *name;

  MatMovingObj *head;

  MatTeam      *next;
  MatTeam      *prev;
};

@ We keep one of this structs for every player.

If you change |MAT_USER_MAX|, change |MAT_USER_BLANK|: It should hold
|MAT_USER_MAX| spaces.

|bullets| is the number of bullets the player has shot that are currently on
the air.

@d MAT_STATE_REMOVE    1
@d MAT_STATE_PREPARE   2
@d MAT_STATE_NORMAL    3
@d MAT_STATE_PLAYING   4
@d MAT_STATE_MACHINE   5

@d MAT_USER_MAX   8
@d MAT_USER_BLANK "        "

@<Struct for the players@>=
struct MatShip
{
  char                 name[MAT_USER_MAX + 1];

  @<Fields for the ships@>

  MatTeam             *team;
  MatMovingObj        *team_next;

  int                  health;

  int                  visible;
  int                  visible_exp;

  int                  bullets_expire;
  int                  ship_bullets[4];

  int                  ship_speed_inc;

  char                 chat_buffer[MAXCHATBUFFERLEN + 1];
  int                  chat_buffer_len;

  int                  times_dead;

  /* game_over is evaluated to know if the player lost. */
  int                  game_over;

  int                  messages_count;
  int                  messages_start;
  char                 messages[MAXMESSAGES][MAXMESSAGELEN];
  int                  messages_expire[MAXMESSAGES];
  int                  messages_pos;
  int                  messages_enabled;

  /* These are used to know the size of the window for the connection that
   * belongs to this player.  The unit here is characters. */
  int                  win_x_s;
  int                  win_x_e;
  int                  win_y_s;
  int                  win_y_e;

  /* How big is his screen? How many dots fit it? */
  int                  imgwi;
  int                  imghe;

  /* How much of the world does he see? How many dots should we fit into his screen? */
  int                  world_wi;
  int                  world_he;
};

@<Data for an image@>
@<Data for bullets@>
@<Data for asteroids@>

typedef struct MatFont MatFont;

struct MatFont
{
  int width, height;
  char *data;
};

@ This is used to identify the ship in the connection.

|conn_id| is the id for this ship among the ships for this connection.  |conn|
points to the connection or, if it was lost, its set to |NULL| (this can
happen, for example, if the connection is gone but it is impossible to destroy
the ship because there are references to it in other objects).

|conn_hash_next| and |conn_hash_prev| point to the previous and next nodes in
the list in the hash of ships for the connection, while |conn_list_next| and
|conn_list_prev| are used to traverse the list with all the ships for the
connection.

@<Fields for the ships@>=
  MatConnection       *conn;
  int                  conn_id;
  MatMovingObj        *conn_hash_next;
  MatMovingObj        *conn_hash_prev;
  MatMovingObj        *conn_list_next;
  MatMovingObj        *conn_list_prev;

@ We need to keep track of the current view for the player controling this
ship.  Basically, he may see the objects surrounding any of the ships.
|view| is the object the player is currently viewing.

Note that, for the moment, |view| must point to an object of type
|MAT_MOVING_OBJ_SHIP|.

@<Fields for the ships@>=
  MatMovingObj        *view;

@ We need a way to keep track of what other players are seeing the world as it
looks like around any given ship.  This is important cause when the player
disconnects, we need a fast way to know what other players where looking to the
world through him.

|viewers| points to a list with all the ships viewing the world as the current.
The list is traversed following the |view_next| and |view_prev| fields.

@<Fields for the ships@>=
  MatMovingObj        *viewers;
  MatMovingObj        *view_next;
  MatMovingObj        *view_prev;

@
@<Struct for moving objects@>=

#define MAT_MOVING_OBJ_SHIP       0
#define MAT_MOVING_OBJ_BULLET     1
#define MAT_MOVING_OBJ_AST        2
#define MAT_MOVING_OBJ_SMART      3

#define MAT_MOVING_OBJ_MOVE_NORMAL 1
#define MAT_MOVING_OBJ_MOVE_DEAD   2

struct MatMovingObj
{
  @<Fields for moving objects@>

  MatUniverse   *un;

  /* If draw is 0, we don't draw this object. */
  int            draw;

  /* If crash is 0, this object won't crash against others. */
  int            crash;

  /* pos_x and pos_y specify the position of the object, relative to the place
   * where its image begins (upper left corner), not the center of the object.
   */
  double         pos_x;
  double         pos_y;

  MatImage      *img;

  /* sp_x and sp_y hold the speed in the x and y coordinate.  sp holds the
   * composite speed (that is, sp * sp = sp_x * sp_x + sp_y * sp_y). */
  double         sp_x;
  double         sp_y;
  double         sp;

  double         tmp_sp_x;
  double         tmp_sp_y;

  /* ang is the current angle of this object, such that 0 <= ang < arg_ang.
   * sp_ang is the speed of rotation. */
  double         ang;
  double         sp_ang;

  unsigned char *pal;

  MatMovingObj  *next;
  MatMovingObj  *prev;

  MatMovingObj  *nexttype;
  MatMovingObj  *prevtype;

  /* rm_next is used to maintain a single linked list of objects that should be
   * removed.  The last node of the object has rm_next == &main.  This is so we
   * can make assertions on all objects that should not be removed having
   * rm_next == NULL.  That is, to know if an object should be removed,
   * evaluate rm_next != NULL.
   *
   * Also, it is worth noticing that ships are not kept in this list.  Other
   * objects are kept in the |objs_rm| list for their universe (see the
   * definition of |MatUniverse|).
   */

  MatMovingObj  *rm_next;

  int            type;

  union
  {
    MatShip        player;
    MatBullet      bullet;
    MatAst         ast;
    MatSmartObj    smart;
  } info;
};

@ There are times when it is impossible to free an object because there are
references to it in other structs.  Consider, for example, that a player might
shot a missile and disconnect.  The missile has a reference to the ship so
removing the ship before the missile will probably cause a crash (when the
missile hits another player, the reference is followed to find out the name of
the player who shot it).  To solve that problem, we use reference counting on
the moving objects, only freeing one when it is no longer referenced.

References to the objects in the universe they are in are not counted, since
there is no way for an object to exist outside of a universe.

On the other hand, references to a ship from a connection should be counted.
This is because a ship may exist after its connection has been gone.

Don't forget to set this field to 0 everytime you create a new object.
Actually, since at creation time a ship will always be referenced from a
connection, you'll likely set it to 1 directly for ships.

@<Fields for moving objects@>=
  int                    refs;

@ Now we need code to increase and decrease the number of references to a ship.

@d MAT_MOVING_OBJ_REF(sh)
  sh->refs ++;

@<Function prototypes@>=
void mat_moving_obj_unref ( MatMovingObj *ship );

@
@<Functions@>=
void
mat_moving_obj_unref ( MatMovingObj *ship )
{
  ASSERT(ship->refs > 0);

  ship->refs --;
  if (ship->refs == 0)
    {
      MAT_SHIP_VIEW_CHECK(ship->un);

      MAT_SHIP_VIEW_RM(ship);
      @<Reset all the viewers in |ship|@>;

      ship->un->players_real --;

      VERBOSE("Killing player: %s\n", ship->info.player.name);

      if (ship->prevtype)
        ship->prevtype->nexttype = ship->nexttype;
      else
        ship->un->players_head = ship->nexttype;

      if (ship->nexttype)
        ship->nexttype->prevtype = ship->prevtype;
      else
        ship->un->players_tail = ship->prevtype;

      if (ship->prev)
        ship->prev->next = ship->next;
      else
        ship->un->objs = ship->next;

      if (ship->next)
        ship->next->prev = ship->prev;

      MAT_SHIP_VIEW_CHECK(ship->un);

      free(ship);
    }
}

@
@<Remove |sh| from its view's list@>=
{
  if (sh->info.player.view_prev)
    sh->info.player.view_prev->info.player.view_next = sh->info.player.view_next;
  else
}

@
@<Reset all the viewers in |ship|@>=
{
  MatMovingObj *i = ship->info.player.viewers;

  while (i)
    {
      MatMovingObj *tmp = i->info.player.view_next;

      ASSERT(i != ship);

      sprintf(message_get(i), "-=> Zip.  %s disappears.", ship->info.player.name);

      i->info.player.view = i;

      if (i->info.player.viewers)
        i->info.player.viewers->info.player.view_prev = i;

      i->info.player.view_prev = NULL;
      i->info.player.view_next = i->info.player.viewers;

      i->info.player.viewers = i;

      i = tmp;
    }
}

@* Global variables

@d MAT_UNIVERSES_HASH_SIZE 1

@<Global variables@>=

typedef void (*MatCrashHandler) (MatMovingObj *, MatMovingObj *);

MatCrashHandler crash_handler[] =
{
  player_crash_handler,
  bullet_crash_handler,
     ast_crash_handler
};

@<Variables for the command line arguments@>
@<Information about the different terminal types@>

MatUniverse     *mat_universes[MAT_UNIVERSES_HASH_SIZE];
int              mat_universes_count;

MatConnection   *connections = NULL;

MatLocation      mat_location_default = { {0, 0, 0}, 0, {1, 1, 1} };

MatFont          font;

/* Socket for incoming players. */
MatSocketT       mat_mainsock;

/* Socket for incoming machines. */
MatSocketT       mat_mainsock_machine;

int              players_count = 0;

@<Variables for the fonts@>

struct aa_font      *graph_font         = &aa_font16;
unsigned short      *graph_table;
unsigned short      *graph_filltable;
struct parameters   *graph_parameters;

@

@d MAT_UNIVERSES_FIND(xr, xid)
  for (xr = mat_universes[xid % MAT_UNIVERSES_HASH_SIZE]; xr && xr->id != xid; xr = xr->next_hash);

@d MAT_UNIVERSES_ADD(un)
{
  int i;

  un->id = mat_universes_count ++;

  i = un->id % MAT_UNIVERSES_HASH_SIZE;

  un->next_hash = mat_universes[i];
  mat_universes[i] = un;
}

@<Initialize hash of universes@>=
{
  int i;

  for (i = 0; i < MAT_UNIVERSES_HASH_SIZE; i ++)
    mat_universes[i] = NULL;
}

@

If arg_chat evaluates to false, the players won't be able to chat.

If you change |MAT_PASSWORD_MAX|, change |MAT_PASSWORD_BLANK|. It should hold
|MAT_PASSWORD_MAX| spaces.

@d MAT_PASSWORD_MAX 8
@d MAT_PASSWORD_BLANK "        "

@d MAT_BACKGROUND_DOTS 1
@d MAT_BACKGROUND_IMG  2

@d MAT_IMGWIDTH_DEFAULT  80 * 4
@d MAT_IMGHEIGHT_DEFAULT 24 * 4

@<Variables for the command line arguments@>=

char             *program_name;

int               arg_compress             = 1;

unsigned long     arg_usleep               = 100000;

int               arg_bg_dots              = 1;
int               arg_bg_size              = 0;
int               arg_bg_adj               = 0;
int               arg_bg_color_max         = -1;

int               arg_max_players          = 0;
int               arg_bold                 = 0;

char             *arg_ship_graphic_path    = NULL;

double            arg_ship_friction        = 0;

int               arg_imgwidth             = MAT_IMGWIDTH_DEFAULT;

int               arg_health               = 1000;

int               arg_port                 = 7993;
int               arg_port_machine         = 7793;

int               arg_mapsize_x            = 2000;
int               arg_mapsize_y            = 1600;

int               arg_limits               = 0;

int               arg_lives                = 5;

int               arg_ang                  = 64;
int               arg_chat                 = 1;

int               arg_ppc                  = 1;

int               arg_teams_count          = 0;
int               arg_teams_safe           = 0;
int               arg_teams_share          = 1;
MatTeam          *arg_teams_head           = NULL;
MatTeam          *arg_teams_tail           = NULL;

int               arg_radar                = 1;
int               arg_visible_init         = 0;

double            arg_ang_speed_max        = 8.0 / STEPS_MOVEMENT;
int               arg_fly_back             = 0;
int               arg_brake                = 1;
int               arg_space                = 1;
double            arg_speed_max            = 9.0 / STEPS_MOVEMENT;

char             *arg_password             = NULL;

int               arg_ast_num              = 4;

@ By default, when asked to terminate, we won't free anything (to avoid
possibly causing page faults).  However, when running Matanza under a memory
leaks detector, freeing everything makes things easier.  We will use parameter
|arg_free_at_exit| to know whether we should free memory when terminating the
program.

@<Variables for the command line arguments@>=

int               arg_free_at_exit         = 0;

@ Matanza can perform motion blur on objects that are moving.  The amount of
blur is proportional to their speeds.  |arg_motion_blur| is used to control
whether Matanza should apply motion blur to objects or just print them as they
are.

@<Variables for the command line arguments@>=
int               arg_motion_blur          = 1;

@* Functions

@<Functions@>=
@<AALib Functions@>
@<Function to send the buffer to a player@>
@<Function to read and process input from players |p|@>
@

@<Function prototypes@>=
@<AALib prototypes@>
void mat_input (MatConnection *p);

@* The main program

@<The main program@>=
int
main (int argc, char **argv)
{
  @<Initialize environment@>;

  while (1)
    if (players_count == 0)
      {
        @<Wait for a connection@>;
        @<Accept incoming connections@>;
      }
    else
      {
        unsigned long old, now;

        GETMICROSECOND_GENERIC(old);

        @<Accept incoming connections@>;
        @<Process input from players@>;
        @<Update all universes@>;
        @<Send output to players@>;

        GETMICROSECOND_GENERIC(now);

        @<Wait for a short interval@>;
      }
}

@
@<Accept incoming connections@>=
{
  VERBOSE("-=> accept incomming connections\n");
  mat_accept(mat_mainsock,         0);
  mat_accept(mat_mainsock_machine, 1);
  VERBOSE("<=- accept incomming connections\n");
}

@
@<Update all universes@>=
{
  int                   i;
  MatUniverse          *un;

  VERBOSE("-=> update universes\n");
  for (i = 0; i < MAT_UNIVERSES_HASH_SIZE; i ++)
    for (un = mat_universes[i]; un; un = un->next_hash)
      @<Update universe@>;
  VERBOSE("<=- update universes\n");
}

@
@<Wait for a connection@>=
{
  fd_set i;

  ASSERT(players_count == 0);

  FD_ZERO(&i);
  FD_SET(mat_mainsock,         &i);
  FD_SET(mat_mainsock_machine, &i);

  VERBOSE("-=> select\n");
  select(MAX(mat_mainsock, mat_mainsock_machine) + 1, &i, NULL, NULL, NULL);
  VERBOSE("<=- select\n");
}

@
@<Check if |obj| crashes against a wall, leaving the result in |crash_wall|@>=
{
  MatImage *img;

  img = un->mask;

  if (!img)
    crash_wall = 0;
  else
    {
      crash_x = ((int) obj->pos_x + obj->img->w / 2) % un->mapsize_x;
      crash_y = ((int) obj->pos_y + obj->img->h / 2) % un->mapsize_y;

      if (crash_x >= img->w || crash_y >= img->h)
        crash_wall = 0;
      else
        {
          ASSERT(img->img);
          ASSERT(un->location[img->img[crash_x + crash_y * img->w]]);

          crash_wall = un->location[img->img[crash_x + crash_y * img->w]]->crash[obj->type];
        }
    }
}

@
@<Handle crash of |obj| against a wall@>=
{
  switch (obj->type)
    {
    case MAT_MOVING_OBJ_BULLET:
      /* Remove the bullet and go on. */
      obj->info.bullet.expires = 0;
      break;

    case MAT_MOVING_OBJ_SHIP:
      if (arg_space)
        @<Modify the speed of |obj| after a crash@>;
      break;

    case MAT_MOVING_OBJ_AST:
      if (arg_space)
        {
          obj->sp_x = -obj->sp_x;
          obj->sp_y = -obj->sp_y;
        }
      break;
    }
}

@
@<Modify the speed of |obj| after a crash@>=
{
  int pos0, pos1, pos2;
  double spx, spy;
  
  @<Get information from the matrix for the world@>;

  /*
   * +-+-+
   * |0|1|
   * +-+-+
   * |3|2|
   * +-+-+
   *
   * 3 is where the crash took place.
   */
}

@
@<Get information from the matrix for the world@>=
{
  int x0, y0, x1, y1, x2, y2;

  spx = obj->sp_x;
  spy = obj->sp_y;

  if (spx > 0)
    {
      if (spy > 0)
        {
          x0 = crash_x - 1;
          y0 = crash_y;
          x1 = crash_x - 1;
          y1 = crash_y - 1;
          x2 = crash_x;
          y2 = crash_y - 1;
        }
      else
        {
          x0 = crash_x;
          y0 = crash_y + 1;
          x1 = crash_x - 1;
          y1 = crash_y + 1;
          x2 = crash_x - 1;
          y2 = crash_y;
        }
    }
  else
    {
      if (spy > 0)
        {
          x0 = crash_x;
          y0 = crash_y - 1;
          x1 = crash_x + 1;
          y1 = crash_y - 1;
          x2 = crash_x + 1;
          y2 = crash_y;
        }
      else
        {
          x0 = crash_x + 1;
          y0 = crash_y;
          x1 = crash_x + 1;
          y1 = crash_y + 1;
          x2 = crash_x;
          y2 = crash_y + 1;
        }
    }
}

@ We don't bother to update the world if we have no players or if the game is
paused.

@d STEPS_MOVEMENT 10

@<Update universe@>=
{
  if (!un->pause && un->players_real)
    {
      un->updates++;

      @<Update players in universe |un|@>;
      @<Make smart objects in |un| think@>;
      @<Move objects@>;
      @<Get rid of bullets that have expired@>;
      @<Remove queued objects@>;
    }
}

@

@<Move objects@>=
{
  MatMovingObj *obj;

  for (i = 0; i < STEPS_MOVEMENT; i ++)
    for (obj = un->objs; obj; obj = obj->next)
      if (!obj->rm_next)
        {
          if (obj->crash)
            {
              int crash_wall, crash_x, crash_y;

              @<Check if |obj| crashes against a wall...@>;

              if (crash_wall)
                @<Handle crash of |obj| against a wall@>@;
              else
                @<Check if |obj| crashes into another object@>;
            }
          @<Apply movement modifiers to player |obj|@>;
          @<Move |obj|@>;
        }
}

@
@<Get rid of bullets that have expired@>=
{
  MatMovingObj *b;

  ASSERT(un);

  for (b = un->bullets; b; b = b->nexttype)
    {
      ASSERT(b->info.bullet.expires >= 0);
      if (b->info.bullet.expires -- == 0)
        {
          REMOVE_QUEUE_ADD(b);

          ASSERT(b->info.bullet.src);
          ASSERT(b->info.bullet.src->refs > 0);

          mat_moving_obj_unref(b->info.bullet.src);
        }
    }
}

@
@<Update players in universe |un|@>=
{
  MatMovingObj *pl;

  for (pl = un->players_head; pl; pl = pl->nexttype)
    {
      if (pl->info.player.bullets_expire > 0)
        pl->info.player.bullets_expire--;

      if (pl->info.player.ship_speed_inc > 0)
        pl->info.player.ship_speed_inc--;

      if (pl->alpha_mod)
        pl->alpha_mod(pl);

      if (!pl->info.player.visible && pl->info.player.visible_exp -- <= 0)
        {
          pl->info.player.visible = 1;
          VISIBILITY_BROADCAST(pl);
        }
    }
}

@

@<Apply movement modifiers to player |obj|@>=
{
  MatImage *img;

  img = un->mask;
  if (img)
    {
      int x, y;

      x = (int) obj->pos_x + obj->img->w / 2 % obj->un->mapsize_x;
      y = (int) obj->pos_y + obj->img->h / 2 % obj->un->mapsize_y;

      if (x < img->w && y < img->h)
        {
          double index;

          index = un->location[img->img[x + y * img->w]]->move[obj->type];
          if (index != 1.0)
            {
              (obj)->sp_x *= (index);
              (obj)->sp_y *= (index);
              (obj)->sp    = sqrt(obj->sp_x * obj->sp_x + obj->sp_y * obj->sp_y);
            }
        }
    }
}

@

@d REMOVE_QUEUE_ADD(obj)
{
  obj->rm_next = obj->un->objs_rm;
  obj->un->objs_rm = obj;
}

@<Remove queued objects@>=
{
  MatMovingObj *tmp;

  while (un->objs_rm != (void *) &main)
    {
      MatMovingObj *next;

      next = un->objs_rm->rm_next;
      ASSERT(next);

      if (un->objs_rm->nexttype)
        un->objs_rm->nexttype->prevtype = un->objs_rm->prevtype;

      if (un->objs_rm->prevtype)
        un->objs_rm->prevtype->nexttype = un->objs_rm->nexttype;
      else
        switch (un->objs_rm->type)
          {
          case MAT_MOVING_OBJ_SHIP:
            break;
          case MAT_MOVING_OBJ_BULLET:
            un->bullets = un->bullets->nexttype;
            break;
          case MAT_MOVING_OBJ_AST:
            un->ast     = un->ast->nexttype;
            break;
          default:
            ASSERT(0);
          }

      DEL_MOVING_OBJ(un->objs_rm);

      free(un->objs_rm);
      un->objs_rm = next;
    }

  ASSERT_FOR(tmp = un->players_head; tmp; tmp = tmp->nexttype)
    ASSERT(!tmp->rm_next);
  ASSERT_FOR(tmp = un->bullets; tmp; tmp = tmp->nexttype)
    ASSERT(!tmp->rm_next);
  ASSERT_FOR(tmp = un->ast; tmp; tmp = tmp->nexttype)
    ASSERT(!tmp->rm_next);
}

@

@<Check if |obj| crashes into another object@>=
{
  MatMovingObj *i;

  ASSERT((int) obj->pos_x >= 0);
  ASSERT((int) obj->pos_x < un->mapsize_x);

  ASSERT((int) obj->pos_y >= 0);
  ASSERT((int) obj->pos_y < un->mapsize_y);

  for (i = obj->next; i; i = i->next)
    if (!i->rm_next && i->crash)
      {
        int crash = 0;

        if (obj->img && i->img && @<Objects are near@>)
          {
            MatMovingObj *src, *dst;
            int xmod, ymod, xbeg, xend, ybeg, yend, xtmp, ytmp;

            ASSERT((int)i->pos_x >= 0);
            ASSERT((int)i->pos_x < un->mapsize_x);

            ASSERT((int)i->pos_y >= 0);
            ASSERT((int)i->pos_y < un->mapsize_y);

            if (obj->img->h * obj->img->w <= i->img->h * i->img->w)
              src = obj, dst =   i;
            else
              src =   i, dst = obj;

            xmod = dst->pos_x - src->pos_x;
            if      (xmod > 0 && xmod >  un->mapsize_x / 2)
              xmod =  un->mapsize_x - xmod;
            else if (xmod < 0 && xmod < -un->mapsize_x / 2)
              xmod = -un->mapsize_x - xmod;

            ASSERT(ABS(xmod) < src->img->w || ABS(xmod) < dst->img->w);

            ymod = dst->pos_y - src->pos_y;
            if      (ymod > 0 && ymod >  un->mapsize_y / 2)
              ymod =  un->mapsize_y - ymod;
            else if (ymod < 0 && ymod < -un->mapsize_y / 2)
              ymod = -un->mapsize_y - ymod;

            ASSERT(ABS(ymod) < src->img->h || ABS(ymod) < dst->img->h);

            xbeg = MAX(0, xmod);
            xend = MIN(src->img->w, xmod + dst->img->w);

            ybeg = MAX(0, ymod);
            yend = MIN(src->img->h, ymod + dst->img->h);

            for (ytmp = ybeg; ytmp < yend; ytmp ++)
              for (xtmp = xbeg; xtmp < xend; xtmp ++)
                {
                  ASSERT(xtmp < src->img->w);
                  ASSERT(ytmp < src->img->h);
                  ASSERT(xtmp - xmod < dst->img->w);
                  ASSERT(ytmp - ymod < dst->img->h);
                  if (   src->pal[src->img->img[(xtmp       ) + (ytmp       ) * src->img->w]] > 0
                      && dst->pal[dst->img->img[(xtmp - xmod) + (ytmp - ymod) * dst->img->w]] > 0)
                    {
                      crash = 1;
                      xtmp = xend;
                      ytmp = yend;
                    }
                }
          }
        else if (obj->img || i->img)
          {
            MatMovingObj *src, *b;

            src = obj->img ? obj : i  ;
            b   = obj->img ? i   : obj;

            @<Check if bullet at |b| hits object at |src|@>;
          }

         if (crash)
           {
             ASSERT(  i->crash);
             ASSERT(obj->crash);
             crash_handler[i->type](i, obj);
           }
      }
}

@ I optimized this as much as I could.  It should be very fast. :)

|CHECK_HIT_BULLET| takes as arguments a variable where it is to leave the
results (cache), the bullet's position (bpos), the object's position (opos), the
object's size (osize) and the map's size (msize) and checks to see if the
bullet is close to the object.  If it isn't, cache is left with a value of -1.
If it is, cache is left with the coordinate the bullet is on inside the
object's image.

@d CHECK_HIT_BULLET(cache, bpos, opos, osize, msize)
{
  cache = bpos - opos;  

  if      (0 <= cache &&         cache < osize)
    cache = cache        ;
  else if (              msize + cache < osize)
    cache = cache + msize;
  else
    cache = -1;

  ASSERT(cache == -1 || (cache >= 0 && cache < osize));
}

@<Check if bullet at |b| hits object at |src|@>=
{
  int posx;

  CHECK_HIT_BULLET(posx, b->pos_x, src->pos_x, src->img->w, un->mapsize_x);

  if (posx != -1)
    {
      int posy;

      CHECK_HIT_BULLET(posy, b->pos_y, src->pos_y, src->img->h, un->mapsize_y);

      if (posy != -1)
        {
          ASSERT(!crash);
          crash = src->pal[src->img->img[posx + posy * src->img->w]] > 0;
        }
    }
}
    
@
@<Function prototypes@>=
void    ast_crash_handler ( MatMovingObj *, MatMovingObj * );
void bullet_crash_handler ( MatMovingObj *, MatMovingObj * );
void player_crash_handler ( MatMovingObj *, MatMovingObj * );

@
@<Functions@>=
void
ast_crash_handler ( MatMovingObj *ast, MatMovingObj *obj )
{
  ASSERT(obj);
  switch (obj->type)
    {
    case MAT_MOVING_OBJ_AST:
      break;

    case MAT_MOVING_OBJ_BULLET:
      crash_handler[obj->type](obj, ast);
      break;

    case MAT_MOVING_OBJ_SHIP:
      crash_handler[obj->type](obj, ast);
      break;

    case MAT_MOVING_OBJ_SMART:
      break;

    default:
      ASSERT(0);
    }
}

@
@<Functions@>=
void
bullet_crash_handler ( MatMovingObj *b, MatMovingObj *obj )
{
  ASSERT(obj);
  switch (obj->type)
    {
    case MAT_MOVING_OBJ_BULLET:
      break;

    case MAT_MOVING_OBJ_AST:
      @<Bullet |b| hits asteroid |obj|@>;
      break;

    case MAT_MOVING_OBJ_SHIP:
      @<Bullet |b| hits player |obj|@>;
      break;

    case MAT_MOVING_OBJ_SMART:
      break;

    default:
      ASSERT(0);
    }
}

@
@<Functions@>=
void
player_crash_handler ( MatMovingObj *p, MatMovingObj *obj )
{
  ASSERT(obj);
  ASSERT(p->type == MAT_MOVING_OBJ_SHIP);
  switch (obj->type)
    {
    case MAT_MOVING_OBJ_AST:
      @<Asteroid |obj| hits player |p|@>;
      break;

    case MAT_MOVING_OBJ_SHIP:
      @<Player |obj| crashes against player |p|@>;
      break;

    case MAT_MOVING_OBJ_BULLET:
      crash_handler[obj->type](obj, p);
      break;

    case MAT_MOVING_OBJ_SMART:
      break;

    default:
      ASSERT(0);
    }
}

@

@d CRASH_SHIP_DAMAGE 400

@d MESSAGE_CRASH_SHIP(x, y)
{
  if (x->info.player.health > 0)
    {
      if (dmg > 0)
        sprintf(message_get(x), "-=> You crashed against player %s ! [Health:%d%%]",
                y->info.player.name, HEALTH(x));
    }
  else
    {
      int sent_out;
      MatMovingObj *i;

      PLAYER_DEAD_REGISTER(sent_out, x);

      for (i = x->un->players_head; i; i = i->nexttype)
        if (arg_teams_count > 0)
          {
            sprintf(message_get(i),
                    "-=> BOOM ! %s of %s died in crash against %s of %s.",
                    x->info.player.name, x->info.player.team->name,
                    y->info.player.name, y->info.player.team->name);
            if (sent_out)
              sprintf(message_get(i),
                      "-=> %s of %s is sent out of the game.",
                      x->info.player.name, x->info.player.team->name);
          }
        else
          {
            sprintf(message_get(i),
                    "-=> BOOM ! %s died in crash against %s.",
                    x->info.player.name, y->info.player.name);
            if (sent_out)
              sprintf(message_get(i),
                      "-=> %s is sent out of the game.",
                      x->info.player.name);
          }

      if (!sent_out)
        {
          MatMovingObj *o;

          if (arg_teams_count && arg_teams_share)
            for (o = x->info.player.team->head; o; o = o->info.player.team_next)
              sprintf(message_get(o), "-=> [%s] Arg, we have lost %d ships now !",
                      x->info.player.team->name, x->info.player.team->times_dead);
          else
            sprintf(message_get(x), "-=> You have lost %d ships now !", x->info.player.times_dead);

          start_game(x);
        }
      else
        GAME_OVER_MODE(x);
    }
}

@<Player |obj| crashes against player |p|@>=
{
  int dmg;

  ASSERT(p->un == obj->un);

  if (!arg_space)
    return;

  dmg = p->un->crash_damage[MAT_MOVING_OBJ_SHIP];
  obj->info.player.health -= dmg;
    p->info.player.health -= dmg;

  if (arg_space)
    {
        p->sp_x = -  p->sp_x;
        p->sp_y = -  p->sp_y;
      obj->sp_x = -obj->sp_x;
      obj->sp_y = -obj->sp_y;
    }

  MESSAGE_CRASH_SHIP(  p, obj);
  MESSAGE_CRASH_SHIP(obj,   p);
}

@
@<Asteroid |obj| hits player |p|@>=
{
  ASSERT(obj);
  ASSERT(obj->type == MAT_MOVING_OBJ_AST);

  ASSERT(p);
  ASSERT(p->type == MAT_MOVING_OBJ_SHIP);

  if (!arg_space)
    return;

  /* Make damage. */
  p->info.player.health -= 200;

  if (arg_space)
    {
      p->sp_x   = -p->sp_x;
      p->sp_y   = -p->sp_y;

      obj->sp_x = -obj->sp_x;
      obj->sp_y = -obj->sp_y;
    }

  if (p->info.player.health <= 0)
    @<Player |p| has been killed by asteroid |obj|@>@;
  else
    sprintf(message_get(p), "-=> You crashed against an asteroid ! [Health:%d%%]", HEALTH(p));
}

@
@<Bullet |b| hits asteroid |obj|@>=
{
  ASSERT(b->info.bullet.expires >= 0);

  if (!b->info.bullet.expires)
    return;

  /* Remove the bullet. */
  b->info.bullet.expires = 0;

  /* Make damage. */
  obj->info.ast.health -= mat_bullets_damage[b->info.bullet.type];

  if (obj->info.ast.health <= 0)
    @<Asteroid |obj| has been broken@>@;
  else
    sprintf(message_get(b->info.bullet.src), "%s", mat_bullets_hit_ast[b->info.bullet.type]);
}

@
@<Asteroid |obj| has been broken@>=
{
  VERBOSE("Boom, asteroid broke\n");

  ASSERT(obj);
  ASSERT(obj->type == MAT_MOVING_OBJ_AST);

  ASSERT(b);
  ASSERT(b->type == MAT_MOVING_OBJ_BULLET);

  ASSERT(b->info.bullet.src);

  if (b->info.bullet.src->type == MAT_MOVING_OBJ_SHIP)
    sprintf(message_get(b->info.bullet.src), "-=> BOOM, you broke an asteroid to pieces !");

  REMOVE_QUEUE_ADD(obj);

  if (obj->info.ast.size < 1)
    @<Add new smaller asteroids@>;
}

@ When a player has crashed a big asteroid, it breaks down into parts.  The
asteroid is removed and four new asteroids are created.

@d RND_SP()
  (((double) (rand() % 20)) / 100.0 + 0.1)
@<Add new smaller asteroids@>=
{
  int ns;

  ns = obj->info.ast.size + 1;

  AST_ADD_POS(ns, obj->pos_x, obj->pos_y, obj->sp_x + RND_SP(), obj->sp_y + RND_SP(), obj->un);
  AST_ADD_POS(ns, obj->pos_x, obj->pos_y, obj->sp_x + RND_SP(), obj->sp_y - RND_SP(), obj->un);
  AST_ADD_POS(ns, obj->pos_x, obj->pos_y, obj->sp_x - RND_SP(), obj->sp_y + RND_SP(), obj->un);
  AST_ADD_POS(ns, obj->pos_x, obj->pos_y, obj->sp_x - RND_SP(), obj->sp_y - RND_SP(), obj->un);
}

@
@<Bullet |b| hits player |obj|@>=
{
  if (!b->info.bullet.expires || b->info.bullet.src == obj)
    return;

  /* Remove the bullet. */
  b->info.bullet.expires = 0;

  /* Make damage. */
  obj->info.player.health -= mat_bullets_damage[b->info.bullet.type];

  if (obj->info.player.health <= 0)
    @<Player |obj| has been killed by bullet |b|@>@;
  else
    switch(b->info.bullet.src->type)
      {
      case MAT_MOVING_OBJ_SHIP:
        sprintf(message_get(obj), "-=> Argh! %s got you! [Health:%d%%]",
                b->info.bullet.src->info.player.name, HEALTH(obj));
        sprintf(message_get(b->info.bullet.src),
                mat_bullets_hit[b->info.bullet.type], obj->info.player.name);
        break;

      case MAT_MOVING_OBJ_SMART:
        sprintf(message_get(obj), "-=> Argh! An alien got you! [Health:%d%%]", HEALTH(obj));
        break;

      default:
        ASSERT(0);
      }
}

@
@<Objects are near@>=
(    (   ((int)(obj->pos_x -   i->pos_x) >= 0 && (int)(obj->pos_x -   i->pos_x) <   i->img->w)
      || ((int)(  i->pos_x - obj->pos_x) >= 0 && (int)(  i->pos_x - obj->pos_x) < obj->img->w)
      || (un->mapsize_x + (int)(obj->pos_x -   i->pos_x) <   i->img->w)
      || (un->mapsize_x + (int)(  i->pos_x - obj->pos_x) < obj->img->w) )
  && (   ((int)(obj->pos_y -   i->pos_y) >= 0 && (int)(obj->pos_y -   i->pos_y) <   i->img->h)
      || ((int)(  i->pos_y - obj->pos_y) >= 0 && (int)(  i->pos_y - obj->pos_y) < obj->img->h)
      || (un->mapsize_y + (int)(obj->pos_y -   i->pos_y) <   i->img->h)
      || (un->mapsize_y + (int)(  i->pos_y - obj->pos_y) < obj->img->h) ) )
 
@

@<Move |obj|@>=
{
  ASSERT(- un->mapsize_x < (int) obj->sp_x);
  ASSERT((int) obj->sp_x < un->mapsize_x);
  ASSERT((int) obj->pos_x >= 0);
  ASSERT((int) obj->pos_x < un->mapsize_x);

  ASSERT(- un->mapsize_y < (int) obj->sp_y);
  ASSERT((int) obj->sp_y < un->mapsize_y);
  ASSERT((int) obj->pos_y >= 0);
  ASSERT((int) obj->pos_y < un->mapsize_y);

  obj->pos_x += obj->sp_x;
  obj->pos_y += obj->sp_y;

  ASSERT(- un->mapsize_x <= (int) obj->pos_x);
  ASSERT((int) obj->pos_x < 2 * un->mapsize_x);

  ASSERT(- un->mapsize_y <= (int) obj->pos_y);
  ASSERT((int) obj->pos_y < 2 * un->mapsize_y);

  if ((int) obj->pos_x < 0)
    {
      if (!arg_limits || obj->type != MAT_MOVING_OBJ_SHIP)
        {
          obj->pos_x += un->mapsize_x;
          ASSERT(0 <= (int) obj->pos_x);
        }
      else
        {
          obj->pos_x = 0;
          obj->sp_x  = 0;
        }
    }
  else if ((int) obj->pos_x >= un->mapsize_x)
    {
      if (!arg_limits || obj->type != MAT_MOVING_OBJ_SHIP)
        {
          obj->pos_x -= un->mapsize_x;
          ASSERT((int) obj->pos_x < un->mapsize_x);
        }
      else
        {
          obj->pos_x = un->mapsize_x - 1;
          obj->sp_x  = 0;
        }
    }

  if ((int) obj->pos_y < 0)
    {
      if (!arg_limits || obj->type != MAT_MOVING_OBJ_SHIP)
        {
          obj->pos_y += un->mapsize_y;
          ASSERT(0 <= (int) obj->pos_y);
        }
      else
        {
          obj->pos_y = 0;
          obj->sp_y  = 0;
        }
    }
  else if ((int) obj->pos_y >= un->mapsize_y)
    {
      if (!arg_limits || obj->type != MAT_MOVING_OBJ_SHIP)
        {
          obj->pos_y -= un->mapsize_y;
          ASSERT((int) obj->pos_y < un->mapsize_y);
        }
      else
        {
          obj->pos_y = un->mapsize_y;
          obj->sp_y  = 0;
        }
    }

  ASSERT((int) obj->pos_x >= 0);
  ASSERT((int) obj->pos_x < un->mapsize_x);

  ASSERT((int) obj->pos_y >= 0);
  ASSERT((int) obj->pos_y < un->mapsize_y);

  ASSERT((int) obj->ang >= 0);
  ASSERT((int) obj->ang < arg_ang);

  obj->ang += obj->sp_ang;
  if ((int) obj->ang >= arg_ang)
    obj->ang -= arg_ang;
  else if ((int) obj->ang < 0)
    obj->ang += arg_ang;

  if (!arg_space && obj->sp_ang != 0 && obj->type == MAT_MOVING_OBJ_SHIP)
    {
      obj->sp_x = obj->sp * mat_sin[(int) obj->ang];
      obj->sp_y = obj->sp * mat_cos[(int) obj->ang];
    }

  ASSERT((int) obj->ang >= 0);
  ASSERT((int) obj->ang < arg_ang);
}

@

@d PLAYER_DEAD_REGISTER(sent_out, pl)
{
  pl->info.player.times_dead ++;
  
  sent_out = pl->info.player.times_dead >= arg_lives;

  if (arg_teams_count)
    {
      pl->info.player.team->times_dead ++;

      if (arg_teams_share && pl->info.player.team->times_dead >= arg_lives)
        sent_out = 1;
    }
}

@
@d GAME_OVER_MODE(x)
{
  ASSERT((x)->type == MAT_MOVING_OBJ_SHIP);
  ASSERT((x)->alpha_mod == NULL);

  (x)->draw                  = 0;
  (x)->crash                 = 0;
  (x)->alpha                 = 0;
  (x)->info.player.game_over = 1;

  (x)->sp = 0;
  (x)->sp_x = (x)->sp_y = 0;
}

@

@<Player |p| has been killed by asteroid |obj|@>=
{
  int sent_out;
  MatMovingObj *i;

  VERBOSE("xxxx player killed by asteroid\n");

  ASSERT(p);
  ASSERT(p->type == MAT_MOVING_OBJ_SHIP);

  ASSERT(obj);
  ASSERT(obj->type == MAT_MOVING_OBJ_AST);

  ASSERT(p->un == obj->un);

  PLAYER_DEAD_REGISTER(sent_out, p);

  for (i = p->un->players_head; i; i = i->nexttype)
    if (arg_teams_count > 0)
      {
        sprintf(message_get(i),
                "-=> KABOOM ! %s of %s is destroyed by an asteroid . . .",
                p->info.player.name, p->info.player.team->name);
        if (sent_out)
          sprintf(message_get(i),
                  "-=> %s of %s is sent out of the game.",
                  p->info.player.name, p->info.player.team->name);
      }
    else
      {
        sprintf(message_get(i),
                "-=> KABOOM ! %s is destroyed by an asteroid . . .",
                p->info.player.name);
        if (sent_out)
          sprintf(message_get(i),
                  "-=> %s is sent out of the game.", p->info.player.name);
      }

  if (!sent_out)
    {
      if (arg_teams_count && arg_teams_share)
        for (i = p->info.player.team->head; i; i = i->info.player.team_next)
          sprintf(message_get(i), "-=> [%s] Arg, we have lost %d ships now !",
                  p->info.player.team->name, p->info.player.team->times_dead);
      else
        sprintf(message_get(p), "-=> You have lost %d ships now !", p->info.player.times_dead);

      start_game(p);
    }
  else
    GAME_OVER_MODE(p);
}

@ TODO: Don't make his ship disappear immediately! One explotion would be very
nice. Or at least have him fade into nothingness.

@<Player |obj| has been killed by bullet |b|@>=
{
  int sent_out;
  MatMovingObj *i;

  ASSERT(obj);
  ASSERT(obj->type == MAT_MOVING_OBJ_SHIP);

  ASSERT(b);
  ASSERT(b->type == MAT_MOVING_OBJ_BULLET);

  ASSERT(b->info.bullet.src);
  ASSERT(b->info.bullet.src->type == MAT_MOVING_OBJ_SHIP);

  ASSERT(obj != b->info.bullet.src);

  ASSERT(obj->un == b->un);

  PLAYER_DEAD_REGISTER(sent_out, obj);

  for (i = obj->un->players_head; i; i = i->nexttype)
    if (arg_teams_count > 0)
      {
        sprintf(message_get(i),
                "-=> KABOOM ! %s of %s is destroyed by %s of %s . . .",
                obj->info.player.name, obj->info.player.team->name,
                b->info.bullet.src->info.player.name,
                b->info.bullet.src->info.player.team->name);
        if (sent_out)
          sprintf(message_get(i),
                  "-=> %s of %s is sent out of the game.",
                  obj->info.player.name, obj->info.player.team->name);
      }
    else
      {
        sprintf(message_get(i),
                "-=> KABOOM ! %s is destroyed by %s . . .",
                obj->info.player.name, b->info.bullet.src->info.player.name);
        if (sent_out)
          sprintf(message_get(i),
                  "-=> %s is sent out of the game.", obj->info.player.name);
      }

  if (!sent_out)
    {
      if (arg_teams_count && arg_teams_share)
        for (i = obj->info.player.team->head; i; i = i->info.player.team_next)
          sprintf(message_get(i), "-=> [%s] Arg, we have lost %d ships now !",
                  obj->info.player.team->name, obj->info.player.team->times_dead);
      else
        sprintf(message_get(obj), "-=> You have lost %d ships now !", obj->info.player.times_dead);

      start_game(obj);
    }
  else
    {
      GAME_OVER_MODE(obj);
      /* YYYY */
      //un->players_real --;
      //obj->info.player.conn->state = MAT_STATE_DEAD;
      //main_menu_begin(obj->info.player.conn);
    }
}

@
@<Send output to players@>=
{
  MatConnection *i;

  for (i = connections; i; i = i->next)
    if (i->state != MAT_STATE_REMOVE && i->anim_func)
      i->anim_func(i);
}

@

@<Wait for a short interval@>=
{
  unsigned long         usec;
  unsigned long         elapsed;

  elapsed = now - old;

  usec = elapsed < arg_usleep ? arg_usleep - elapsed : 0;

  ASSERT(usec <= arg_usleep);

  if (usec > 0)
    USLEEP_GENERIC(usec);
}

@

@<Remove connection |con| from the list@>=
{
  ASSERT(con);
  ASSERT(con->state == MAT_STATE_REMOVE);

  if (con->prev)
    con->prev->next = con->next;
  else
    connections = con->next;

  if (con->next)
    con->next->prev = con->prev;

  players_count --;

  free (con);
}

@

@<Process input from players@>=
{
  MatConnection *con;

  con = connections;
  while (con)
    if (con->state == MAT_STATE_REMOVE)
      {
        MatConnection *tmp;

        tmp = con->next;
        @<Remove connection |con| from the list@>;
        con = tmp;
      }
    else
      {
        mat_input(con);
        con = con->next;
      }
}

@

@<Function to read and process input from players |p|@>=
void
mat_input (MatConnection *con)
{
  @<Make sure input buffer for player |con| has space@>;
  @<Read data for player |con|@>;
  @<Process and discard data for connection |con|@>;
}

@ Here we make sure there are at least 128 bytes of space in the player's
buffer.  Otherwise, we increase its size.

@<Make sure input buffer for player |con| has space@>=
  if (con->bufsiz - con->buflen < 128)
    {
      con->bufsiz = con->bufsiz ? con->bufsiz * 2 : 128;
      con->buffer = realloc(con->buffer, con->bufsiz + 1);
    }

@

@<Function prototypes@>=
void mat_accept ( MatSocketT fildes, int machine );

@ Here we loop accepting connections.  When no more connections are ready, we
return.

|machine| is evaluated to know if this socket receives connections from players
or from machines.

TODO: Make some checks against DoS attacks: we shouldn't allow more than X
connections from the same host.

@<Functions@>=
void
mat_accept ( MatSocketT source, int machine )
{
  MatSocketT          result;
  socklen_t           in_con_size = sizeof(struct sockaddr_in);
  struct sockaddr_in  in_con;

  while (1)
    {
      do
        {
          VERBOSE("-=> accept [fd:%d]\n", source);
          result = accept(source, (struct sockaddr*)&in_con, &in_con_size);
          VERBOSE("<=- accept [fd:%d] [result:%d]\n", source, result);
        }
      while (result == SOCKET_ERROR && result == SOCKET_CALL_INTR);

      if (result == SOCKET_ERROR)
        @<Handle error in call to |accept|@>@;
      else
        @<Handle a new accepted connection@>;
    }
}

@
@<Handle error in call to |accept|@>=
{
  VERBOSE("error in accept: %d\n", SOCKET_ERRNO());
  switch (SOCKET_ERRNO())
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
      fprintf(stderr, "%s: code: %d\n", program_name, SOCKET_ERRNO());
      fprintf(stderr, "%s: accept: %s\n", program_name, strerror(SOCKET_ERRNO()));
      exit(EXIT_FAILURE);
    }
}

@ This code gets executed right after a new connection has been accepted
(calling |accept|).  The new file descriptor is available at |result|.
|machine| can be evaluated to know whether the peer is a player (telnet
client) or a machine (speaking MMP).

@<Handle a new accepted connection@>=
{
  MatConnection *con;
  int            on = 1;

  con = malloc(sizeof(MatConnection));
  if (!con)
    {
      /* TODO: Kill this connection rather than the whole program. */
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  con->fd = result;

  @<Get IP address for |in_con|@>;

#if HAVE_SETSOCKOPT
#  ifdef SO_KEEPALIVE
#    ifdef SOL_SOCKET
  VERBOSE("setsockopt [fd:%d][option:SO_KEEPALIVE]\n", con->fd);
  if (setsockopt(con->fd, SOL_SOCKET, SO_KEEPALIVE, (void *) &on, sizeof(on)) == -1)
    fprintf(stderr, "%s: Warning: setsockopt: SO_KEEPALIVE: %s\n", program_name, strerror(errno));
#    endif
#  endif
#endif

  /* Make socket nonblocking */
  NONBLOCKING_GENERIC(result);

  con->out_buf = NULL;
  con->out_len = 0;
  con->out_siz = 0;

  ASSERT(source == mat_mainsock || source == mat_mainsock_machine);

  if (arg_max_players == 0 || players_count < arg_max_players)
    {
      if (source == mat_mainsock)
        @<Initialize connection |con|@>@;
      else
        @<Initialize machine connection |con|@>@;
    }
  else
    @<Shutdown connection |con| when the server is full@>;
}

@ This code gets executed when a connection arrives but the server is crowded.
We simply send a message, close the socket and free some memory.

Note that the actual message we send depends on whether the connection is that
of a player or a machine.

TODO: If the connection is that of a player, send a nicer message.

@<Shutdown connection |con| when the server is full@>=
{
  ASSERT(con);

  if (source == mat_mainsock)
    mat_printf(con, "%s", "Sorry, the server is full. Please try again later.\n");
  else
    mat_printf(con, "%c", MMP_SERVER_INITIAL_FULL);

  mat_out_flush(con);
  
  free(con->out_buf);
  free(con);
}

@ Here we don't reverse lookup the address to the name because it may take long
amounts of time: The whole game would freeze during resolution.

If |inet_ntop| is available, we use it.  On those systems, |inet_ntoa| is
deprecated.  It supports IPV6.  On older machines where it's not available, we
try with |inet_ntoa|.  To be safe, we make sure |inet_ntoa| is actually
available and, if it isn't, we report a bogus address.

All this macros should leave |real_buffer| pointing to the address.  For the
time being, we free |real_buffer|.  In the future, we will add it to the
players' structures.

@<Get IP address for |in_con|@>=
{
#if HAVE_INET_NTOP
  @<Get incoming address for |in_con| using |inet_ntop|@>;
#elif HAVE_INET_NTOA
  @<Get incoming address for |in_con| using |inet_ntoa|@>;
#else
  @<Report bogus IP address@>;
#endif

  ASSERT(con->addr);

  VERBOSE("Connection from: %s\n", con->addr);
}

@ If we don't know how to get the IP address, we use a default string.

@<Report bogus IP address@>=
{
  con->addr = strdup("Unknown");
  if (!con->addr)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }
}

@

@<Get incoming address for |in_con| using |inet_ntoa|@>=
{
  char *tmp;

  tmp = inet_ntoa(in_con.sin_addr);

  ASSERT(tmp);

  con->addr = strdup(tmp);
  if (!con->addr)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }
}

@

@<Get incoming address for |in_con| using |inet_ntop|@>=
{
  char buffer[256];

  if (!inet_ntop(in_con.sin_family, &in_con.sin_addr, buffer, sizeof(buffer)))
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  con->addr = strdup(buffer);
  if (!con->addr)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }
}

@ Here we start to negotiate the telnet options.

We send the default options we want and set the key handler.

@<Begin telnet negotiation@>=
{
  mat_printf(con, "%s" "%c%c%c" "%c%c%c" "%c%c%c" "%c%c%c" "%c%c%c",
             "-=> Please stand by . . .\r\n"
             "-=> Matanza is preparing your machine to maximize your experience . . . ",
             IAC, DO,   TELOPT_TTYPE,
             IAC, DO,   TELOPT_NAWS,
             IAC, DONT, TELOPT_LINEMODE,
             IAC, WILL, TELOPT_SGA,
             IAC, WILL, TELOPT_ECHO);

  mat_out_flush(con);
}

@ Please send me more names.

@<Select a random name@>=
{
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
    "Hax0r", "Lamer", "Cracker", "Dood", "Kiddo", "Waine", "Kent", "Wright",
    "Stephen", "Fred", "Tintin", "Haddock", "Milu", "Shiva", "Apollo", "Mars",
    "Jupiter", "Butthead", "Beavis", "ElCHAVO", "Laid", "Rex", "Riff",
    "Hunter", "Elvis", "Hendrix", "MajorTom", "Moe", "Musician", "Cat",
    "Vladimir", "Tricky", "Darwin", "Freud", "Einstein", "Newton", "Hitman",
    "Tato", "Pigidik", "NOD"
  };

  int ch;

  ch = rand() % (sizeof(name) / sizeof(char *));

  menu->buflen[0] = strlen(name[ch]);
  strncpy(menu->buffer[0], name[ch], menu->buflen[0] + 1);
}

@ Here we set information about the machine connection.

@<Initialize machine connection |con|@>=
{
  con->state = MAT_STATE_MACHINE;

  con->buffer = NULL;
  con->bufsiz = con->buflen = con->bufprc = 0;

  con->key_handler = machine_input_initial;

  con->input_state = MAT_KEY_DATA;
  con->anim_func = machine_update;

  con->next = connections;
  con->prev = NULL;

  @<Initialize the container of ships for |con|@>;

  MAT_SUB_MACHINE_INIT(con);

  if (connections)
    connections->prev = con;

  connections = con;

  players_count ++;
}

@
@<Initialize the container of ships for |con|@>=
{
  int i;

  con->ship_count = 0;

  for (i = 0; i < MAT_CONNECTION_SHIP_HASH_SIZE; i++)
    con->ship_hash[i] = NULL;

  con->ship_list = 0;
}

@

@<Initialize connection |con|@>=
{
  @<Begin telnet negotiation@>;

  con->state = MAT_STATE_PREPARE;

  con->info.t.clean = 1;

  con->terminal_name[0] = 0;
  con->term = &term_vt100;

  con->xwinsize = con->ywinsize = -1;

  con->buffer = NULL;
  con->bufsiz = con->buflen = con->bufprc = 0;

  con->key_handler = NULL;
  con->anim_func   = NULL;

  con->input_state = MAT_KEY_DATA;

  /* Give him a random default name. */
  //strcpy(con->playername, name[rand() % (sizeof(name) / sizeof(char *))]);

  @<AALib: Initialize context for player |con|@>;

  con->next = connections;
  con->prev = NULL;

  if (connections)
    connections->prev = con;

  connections = con;

  players_count ++;
}

@ In this function we will check to see if the telnet negotiation is done.

@<Functions@>=
void
check_telnet_negotiation ( MatConnection *con )
{
  if (con->state == MAT_STATE_PREPARE)
    {
      if (con->terminal_name[0] == 0 || con->xwinsize == -1 || con->ywinsize == -1)
        return;

      @<Choose terminal type@>;

      TERM_CLEAR_SCREEN(con);
      TERM_HIDE_CURSOR(con);

      con->state = MAT_STATE_NORMAL;

      @<Animation 000: Play@>;

      con->key_handler = mat_animation_cancel;
    }
}

@ For the moment we only support vt100.  In the future other terminal types
might be added.  We should check |con->terminal_name|.

@<Choose terminal type@>=
{
  ASSERT(con);
  con->term = &term_vt100;
}

@ Here a function to cancel the animations when the user hits <Return>.

@<Functions@>=
void
mat_animation_cancel (MatConnection *con, int x)
{
  ASSERT(con);

  switch (x)
    {
    case 13:
      free(con->anim_data);
      MAT_DEFAULT_PALETTE(con);
      main_menu_begin(con);
      break;
    default:
      VERBOSE("Input from player during animations: %d\n", x);
    }
}

@
@<Function prototypes@>=
void check_telnet_negotiation ( MatConnection *p );
void mat_animation_cancel     ( MatConnection *p, int x );

@

@d TERM_CLEAR_SCREEN(con)
  mat_printf(con, "%s", con->term->cls);

@d TERM_CLEAR_IMAGE(con)
  ASSERT(con);
  ASSERT(con->graph_imagebuffer);
  memset(con->graph_imagebuffer, 0, con->graph_imgwidth * con->graph_imgheight)

@d TERM_HIDE_CURSOR(con)
  mat_printf(con, "%s", con->term->cursor_hide);

@d TERM_SHOW_CURSOR(con)
  mat_printf(con, "%s", con->term->cursor_show);

@d TERM_MOVE_CURSOR(con, y, x)
  mat_printf(con, con->term->cursor_pos, y, x);

@<Initialize environment@>=
{
  @<Set the program name@>;
  @<Parse command line arguments@>;
  srand(time(NULL));
  @<Initialize the caches for results of trigonometric functions@>;
  @<AALib: Initialize global@>;
  @<Prepare images@>;
  @<Make palettes@>;
  @<Prepare the universes@>;
  @<Set the signal handlers@>;
  @<Initialize the sockets system@>;
  @<Display label@>;

#if MATANZA_VERBOSE
  if (!arg_verbose)
#endif
    @<Fork and exit@>;

  /* TODO: Should we change this to _chdir on Windows? */
  chdir ("/");

  mat_mainsock         = mat_listen(arg_port        );
  mat_mainsock_machine = mat_listen(arg_port_machine);

}

@
@<Display label@>=
{
  printf("-=> Matanza has been successfully started.\n");
  printf("-=> Connect to port %d to play.\n", arg_port);
}

@
@<Fork and exit@>=
#if HAVE_FORK
{
  pid_t childpid;

  /* Create a new process and exit. */
  childpid = fork ();
  if (childpid < 0)
    {
      fprintf(stderr, "%s: fork: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }
  else if (childpid > 0) 
    exit(EXIT_SUCCESS);

#if HAVE_SETSID
  /* Create a new session. */
  if (setsid() < 0)
    {
      fprintf(stderr, "%s: setsid: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  /* Create a new process and exit. */
  childpid = fork ();
  if (childpid < 0)
    {
      fprintf(stderr, "%s: fork: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }
  else if (childpid > 0) 
    exit(EXIT_SUCCESS);
#endif
}
#endif

#if 0
{
  int fd;
  int dev_null;

  /* Open the data sink. */
  dev_null = open("/dev/null", O_RDWR);
  if (dev_null < 0)
    {
      fprintf(stderr, "%s: open: /dev/null: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  /* Make it so that all reads to stdin will return EOF. */
  if (dup2(dev_null, 0) < 0)
    {
      fprintf(stderr, "%s: dup2: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  if (hbd_log_msg)
    {
      /* Open the server messages log. */
      fd = open(hbd_log_msg, O_CREAT|O_WRONLY|O_APPEND, 0666);
      if (fd < 0)
	{
	  fprintf(stderr, "%s: open: %s: %s\n", program_name, hbd_log_msg, strerror(errno));
	  exit(EXIT_FAILURE);
	}
    }
  else
    fd = dev_null;

  /* Make it so writes to stdout will go to the messages log or /dev/null. */
  if (dup2(fd, 1) < 0)
    {
      fprintf(stderr, "%s: dup2: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  if (hbd_log_err)
    {
      /* Open the server errors log. */
      fd = open(hbd_log_err, O_CREAT|O_WRONLY|O_APPEND, 0666);
      if (fd < 0)
	{
	  fprintf(stderr, "%s: open: %s: %s\n", program_name,
		  hbd_log_err, strerror(errno));
	  exit(EXIT_FAILURE);
	}
    }
  else
    fd = dev_null;

  /* Make it so writes to stderr get stored in the server errors log. */
  if (dup2(fd, 2) < 0)
    {
      fprintf(stderr, "%s: dup2: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  /* Close all other open file descriptors. */
  for (fd = getdtablesize() - 1; fd > 2; fd--)
    close(fd);
}
#endif
@
@<Prepare the universes@>=
{
  if (mat_universes_count)
    {
      MatUniverse    *un;
      int             i;

      for (i = 0; i < MAT_UNIVERSES_HASH_SIZE; i ++)
        for (un = mat_universes[i]; un; un = un->next_hash)
          @<Load universe@>;
    }
  else
    @<Initialize universe@>;
}

@ If they didn't specify any universes, we create a sane default.  Most of the
parameters for this universe come from the command line arguments.

@<Initialize universe@>=
{
  MatUniverse *un;
  int i;

  un = malloc(sizeof(MatUniverse));
  if (!un)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  MAT_UNIVERSES_ADD(un);

  @<Set the default parameters for |un|@>;

  for (i = 0; i < 256; i ++)
    un->location[i] = NULL;

  un->path = NULL;

  un->crash_damage[MAT_MOVING_OBJ_SHIP] = 200;

  un->bg = un->mask = NULL;

  @<Flood with asteroids@>;

#if 0
  MAT_SMART_OBJ_ADD_KASHKA(un);
  MAT_SMART_OBJ_ADD_KASHKA(un);
  MAT_SMART_OBJ_ADD_KASHKA(un);

  MAT_SMART_OBJ_ADD_WOOZKA(un);
  MAT_SMART_OBJ_ADD_WOOZKA(un);
  MAT_SMART_OBJ_ADD_WOOZKA(un);
#endif

  @<Fix problems in universe |un|@>;

  ASSERT(!un->bg || un->bg->w <= un->mapsize_x);
  ASSERT(!un->bg || un->bg->h <= un->mapsize_x);
}

@
@<Prepare images@>=
{
  @<Load background images@>;
  @<Rotate the ship in the different angles@>;
  @<Rotate the missiles in the different angles@>;
  @<Rotate the asteroids in the different angles@>;
}

@
@<Free images@>=
{
  @<Free the images for the ships@>;
  @<Free the images for the missiles@>;
  @<Free the images for the asteroids@>;
}

@ We block |SIGPIPE| so we won't die when a given player closes his connection.

We also block |SIGINT| and |SIGTERM| for a nice termination (printing some
messages to stdout and sending some information to the players before actually
terminating).

@<Set the signal handlers@>=
{
#if HAVE_SIGNAL
#  ifdef SIGPIPE
  signal(SIGPIPE, SIG_IGN);
#  endif
#  ifdef SIGINT
  signal(SIGINT,  term_handler);
#  endif
#  ifdef SIGTERM
  signal(SIGTERM, term_handler);
#  endif
#  ifdef SIGTTOU
  signal(SIGTTOU, SIG_IGN);
#  endif
#  ifdef SIGTTIN
  signal(SIGTTIN, SIG_IGN);
#  endif
#  ifdef SIGTSTP
  signal(SIGTSTP, SIG_IGN);
#  endif
#endif
}

@

@<Functions@>=
void
term_handler (int sig)
{
  MatConnection *c;

  printf("Terminating on signal %d\n", sig);

  for (c = connections; c; c = c->next)
    mat_connection_free(c, MMP_SERVER_CLOSE_SHUTDOWN, "The server is being shutdown.");

  if (arg_free_at_exit)
    @<Free everything@>;

  exit(EXIT_SUCCESS);
}

@ Here we are supposed to free all the memory we can trace.

@<Free everything@>=
{
  int i;

  @<Free images@>;

  free(font.data);

  for (i = 0; i < MAT_UNIVERSES_HASH_SIZE; i ++)
    while (mat_universes[i])
      {
        MatUniverse *d;

        d = mat_universes[i];
        mat_universes[i] = d->next_hash;

        @<Free universe |d|@>;
      }

  while (connections)
    {
      MatConnection *tmp;

      ASSERT(connections->state == MAT_STATE_REMOVE);

      tmp = connections->next;
      free(connections);
      connections = tmp;
    }
 
  @<Destroy tables used for rendering@>;
  @<Destroy palettes@>;
  @<Destroy the caches for results of trigonometric functions@>;
}

@
@<Free universe |d|@>=
{
  while (d->ast)
    {
      MatMovingObj *a;
      
      a = d->ast;
      d->ast = a->nexttype;
      
      free(a);
    }
  free(d);
}

@
@<Parse command line arguments@>=
{
  int iterate         = 1;
  int show_help       = 0;
  int show_version    = 0;
  int prompt_password = 0;

  while (iterate)
    {
      int oi = 0;
      static struct option lopt[] =
      {
        { "help",               no_argument,       NULL, 'h' },
        { "version",            no_argument,       NULL, 'v' },
        { "max-players",        required_argument, NULL, 'm' },
        { "bold",               no_argument,       NULL, 'b' },
        { "imgwidth",           required_argument, NULL, 'W' },
        { "health",             required_argument, NULL,  1  },
        { "mapsize-x",          required_argument, NULL, 'x' },
        { "mapsize-y",          required_argument, NULL, 'y' },
        { "lives",              required_argument, NULL, 'l' },
        { "angles",             required_argument, NULL, 'A' },
        { "no-chat",            no_argument,       NULL,  2  },
        { "team",               required_argument, NULL, 't' },
        { "password",           optional_argument, NULL, 'p' },
        { "teams-safe",         no_argument,       NULL,  3  },
        { "no-share",           no_argument,       NULL,  4  },
        { "ang-speed-max",      required_argument, NULL,  5  },
        { "fly-back",           no_argument,       NULL,  6  },
        { "no-brake",           no_argument,       NULL,  7  },
        { "air",                no_argument,       NULL,  8  },
        { "speed-max",          required_argument, NULL,  9  },
        { "asteroids",          required_argument, NULL, 10  },
        { "limits",             no_argument,       NULL, 11  },
        { "no-radar",           no_argument,       NULL, 12  },
        { "invisible",          required_argument, NULL, 13  },
        { "ppc",                required_argument, NULL, 14  },
        { "bg",                 required_argument, NULL, 15  },
        { "mapsize-img",        no_argument,       NULL, 16  },
        { "ship-friction",      required_argument, NULL, 17  },
        { "dots",               no_argument,       NULL, 18  },
        { "nodots",             no_argument,       NULL, 26  },
        { "bg-adjust",          no_argument,       NULL, 19  },
        { "no-compress",        no_argument,       NULL, 20  },
        { "bg-highcol",         required_argument, NULL, 21  },
        { "space",              no_argument,       NULL, 22  },
        { "port",               required_argument, NULL, 23  },
        { "usleep",             required_argument, NULL, 24  },
        { "port-machine",       required_argument, NULL, 25  },
        { "ship-img",           required_argument, NULL, 27  },
        { "free-at-exit",       no_argument,       NULL, 28  },
        { "verbose",            no_argument,       NULL, 29  },
	{ 0, 0, 0, 0 }
      };

      switch (getopt_long(argc, argv, "hvm:bW:x:y:l:A:t:p::", lopt, &oi))
	{
	case -1:
	  iterate = 0;
	  break;
        case 'h':
          show_help = 1;
          break;
        case 'v':
          show_version = 1;
          break;
        case 'm':
          arg_max_players = atoi(optarg);
          break;
        case 'b':
          arg_bold = 1;
          break;
        case 'W':
          arg_imgwidth = atoi(optarg);
          break;
        case 1:
          arg_health = atoi(optarg);
          break;
        case 'x':
          arg_mapsize_x = atoi(optarg);
          break;
        case 'y':
          arg_mapsize_y = atoi(optarg);
          break;
        case 'l':
          arg_lives = atoi(optarg);
          break;
        case 'A':
          arg_ang = atoi(optarg);
          break;
        case 2:
          arg_chat = 0;
          break;
        case 't':
          @<Add a new team@>;
          break;
        case 'p':
          if (optarg)
            arg_password = optarg;
          else
            prompt_password = 1;
          break;
        case 3:
          arg_teams_safe = 1;
          break;
        case 4:
          arg_teams_share = 0;
          break;
        case 5:
          arg_ang_speed_max = ((double)atoi(optarg)) / STEPS_MOVEMENT;
          break;
        case 6:
          arg_fly_back = 1;
          break;
        case 7:
          arg_brake = 0;
          break;
        case 8:
          arg_space = 0;
          break;
        case 9:
          arg_speed_max = (double) atoi(optarg);
          break;
        case 10:
          arg_ast_num = atoi(optarg);
          break;
        case 11:
          arg_limits = 1;
          break;
        case 12:
          arg_radar = 0;
          break;
        case 13:
          arg_visible_init = atoi(optarg);
          break;
        case 14:
          arg_ppc = atoi(optarg);
          break;
        case 15:
          @<Add background image form |optarg|@>;
          break;
        case 16:
          arg_bg_size = 1;
          break;
        case 17:
          arg_ship_friction = atof(optarg);
          mat_location_default.move[MAT_MOVING_OBJ_SHIP] = arg_ship_friction;
          break;
        case 18:
          arg_bg_dots = 1;
          break;
        case 19:
          arg_bg_adj = 1;
          break;
        case 20:
          arg_compress = 0;
          break;
        case 21:
          arg_bg_color_max = atoi(optarg);
          break;
        case 22:
          arg_space = 1;
          break;
        case 23:
          arg_port = atoi(optarg);
          break;
        case 24:
          arg_usleep = atol(optarg);
          break;
        case 25:
          arg_port_machine = atoi(optarg);
          break;
        case 26:
          arg_bg_dots = 0;
          break;
        case 27:
          arg_ship_graphic_path = optarg;
          break;
        case 28:
          arg_free_at_exit = 1;
          break;
        case 29:
#if MATANZA_VERBOSE
          arg_verbose = 1;
#endif
          break;
	default:
	  printf("Try '%s --help' for more information.\n", program_name);
	  exit(EXIT_FAILURE);
	}
    }

  if (show_help)
    {
      printf("Usage: %s [OPTION]... [FILE]\n"
             "Start the Matanza server, reading the universe from FILE\n\n"
             "  -h, --help                  Show this information and exit\n"
             "  -v, --version               Show version number and exit\n"
             "  -m, --max-players=NUM       Never allow more than NUM players\n"
	     "  -b, --bold                  Use bold characters too\n"
             "  -W, --imgwidth=NUM          Image width for default zoom (320)\n"
             "  --health=NUM                Health of ships (1000)\n"
             "  -x, --mapsize-x=NUM         Map size in the x coordinate\n"
             "  -y, --mapsize-y=NUM         Map size in the y coordinate\n"
             "  -l, --lives=NUM             Number of lives for the players\n"
             "  -A, --angles=NUM            Number of angles for rotations\n"
             "  --no-chat                   Disable the chat features\n"
             "  -t, --team=STR[:NUM]        Add team STR with maximum NUM players\n"
             "  -p, --password[=STR]        Perform authentication using STR as the password\n"
             "  --teams-safe                Bullets won't hit players of the same team\n"
             "  --no-share                  Players in the same team don't share lives\n"
             "  --ang-speed-max=NUM         Maximum angular speed for ships (try 0)\n"
             "  --fly-back                  Players will be able to fly backwards\n"
             "  --no-brake                  Players will not be able to brake\n"
             "  --air                       Ships' speed depend on their angle\n"
             "  --speed-max=NUM             Maximum speed (0 = Unlimited)\n"
             "  --asteroids=NUM             The number of asteroids at a given moment\n"
             "  --limits                    Give the world limits; make it end\n"
             "  --no-radar                  When enemy is away, don't show the direction\n"
             "  --invisible=NUM             Allow players to become invisible for NUM rounds\n"
             "  --ppc=NUM                   Set the number of Players Per Connection\n"
             "  --bg=PATH                   Load background image from FILE\n"
             "  --mapsize-img               Set the map's size to the background's size\n"
             "  --ship-friction=NUM         Set the ship's friction (0 <= NUM <= 1)\n"
             "  --dots                      Show dots on the background\n"
             "  --nodots                    Don't show dots on the background\n"
             "  --bg-adjust                 Adjust the backgrounds to the mapsize\n"
             "  --no-compress               Don't perform bandwidth optimizations\n"
             "  --bg-highcol=NUM            Adjust the colors in the background\n"
             "  --space                     Ships' speed doesn't depend on their angle\n"
             "  --port=NUM                  Use TCP port NUM for telnet\n"
             "  --usleep=NUM                Sleep NUM microseconds between updates\n"
             "  --port-machine=NUM          Use TCP port NUM for machines' friendly protocol\n"
             "  --ship-img=PATH             Load image of the ship from PATH\n"
             "\n"
             "Report bugs to <bachue@@bachue.com>\n", program_name);
      exit(EXIT_SUCCESS);
    }

  if (show_version)
    {
      printf("Freaks Unidos' %s %s\n"
             "Copyright (C) 2000 Alejandro Forero Cuervo\n"
             "Report any bugs to <bachue@@bachue.com>\n"
             "Check <http://bachue.com/matanza> for updates\n",
             PACKAGE, VERSION);
      exit(EXIT_SUCCESS);
    }

  if (prompt_password)
    {
      fprintf(stderr, "%s: Prompting for a password is not supported (yet)\n", program_name);
      fprintf(stderr, "%s: Try --password=<string>\n", program_name);
      exit(EXIT_FAILURE);
    }

  while (optind < argc)
    {
      MatUniverse *un;

      un = malloc(sizeof(MatUniverse));
      if (!un)
        {
          fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
          exit(EXIT_FAILURE);
        }

      un->path = argv[optind ++];

      MAT_UNIVERSES_ADD(un);
    }

  if (arg_ship_graphic_path)
    @<Load the image for the ship@>;

  @<Make sure parameters are sane@>;
}

@
@<Load the image for the ship@>=
{
  FILE *in;
  int   i, j;
  int   wr = 0;

  in = fopen(arg_ship_graphic_path, "r");
  if (!in)
    {
      fprintf(stderr, "%s: open: %s: %s\n", program_name, arg_ship_graphic_path, strerror(errno));
      exit(EXIT_FAILURE);
    }

  arg_ship_graphic = malloc(MAT_SHIPSIZE_X * MAT_SHIPSIZE_Y);
  if (!arg_ship_graphic)
    {
      fprintf(stderr, "%s: malloc: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  for (i = 0; i < MAT_SHIPSIZE_Y; i ++)
    for (j = 0; j < MAT_SHIPSIZE_X; j ++)
      {
        int c;

        ASSERT(wr == i * MAT_SHIPSIZE_X + j);

        c = getc(in);
        switch (c)
          {
          case '\n':
            memset(arg_ship_graphic + wr, ' ', MAT_SHIPSIZE_X - j);
            wr += MAT_SHIPSIZE_X - j;
            j = MAT_SHIPSIZE_X - 1;
            break;

          case '#':
            while ((c = getc(in)) != EOF)
              if (c == '\n')
                break;

            if (j == 0)
              j--;
            else
              {
                memset(arg_ship_graphic + wr, ' ', MAT_SHIPSIZE_X - j);
                wr += MAT_SHIPSIZE_X - j;
                j = MAT_SHIPSIZE_X - 1;
              }

            break;

          case ' ':
          case '*':
          case '1':
          case '2':
          case 'X':
          case '.':
            arg_ship_graphic[wr ++] = c;
            break;

          case EOF:
            memset(arg_ship_graphic + wr, ' ', MAT_SHIPSIZE_X * MAT_SHIPSIZE_Y - wr);
            j = MAT_SHIPSIZE_X - 1;
            i = MAT_SHIPSIZE_Y - 1;
            break;

          default:

            fprintf(stderr, "%s: %s: Invalid character: %c\n",
                    program_name, arg_ship_graphic_path, c);
            exit(EXIT_FAILURE);
          }

        ASSERT(wr == i * MAT_SHIPSIZE_X + j + 1);
      }

  fclose(in);
}

@ Here we add a new team with the parameters specified in optarg.

@<Add a new team@>=
{
  MatTeam *t;
  char    *m;

  t = malloc(sizeof(MatTeam));
  if (!t)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  m = strrchr(optarg, ':');

  if (m)
    {
      *m = 0;
      if (m > optarg + MAT_TEAM_MAX)
        {
          fprintf(stderr, "%s: %s: Name too long\n", program_name, optarg);
          exit(EXIT_FAILURE);
        }
      t->max = atoi(m + 1);
    }
  else
    t->max = 0;

  t->name       = optarg;
  t->cur        = 0;
  t->times_dead = 0;
  t->head       = NULL;
  t->next       = arg_teams_head;
  t->prev       = NULL;

  if (arg_teams_head)
    arg_teams_head->prev = t;
  else
    arg_teams_tail = t;

  arg_teams_head = t;

  arg_teams_count ++;
}

@
@<Make sure parameters are sane@>=
{
  if (!arg_teams_count)
    {
      if (arg_teams_safe)
        fprintf(stderr, "%s: --teams-safe used in individual mode\n", program_name);
      if (!arg_teams_share)
        fprintf(stderr, "%s: --no-share used in individual mode\n", program_name);
    }

  if (arg_imgwidth <= 0)
    {
      fprintf(stderr, "%s: width <= 0\n", program_name);
      arg_imgwidth = 80 * 4;
    }

#if 0
  if (arg_mapsize_x < arg_imgwidth  + MAT_SHIPSIZE_X)
    {
      fprintf(stderr, "%s: mapsize-x < imgwidth + %d\n", program_name, MAT_SHIPSIZE_X);
      arg_mapsize_x = arg_imgwidth  + MAT_SHIPSIZE_X;
    }

  if (arg_mapsize_y < arg_imgheight + MAT_SHIPSIZE_Y)
    {
      fprintf(stderr, "%s: mapsize-y < imgheight + %d\n", program_name, MAT_SHIPSIZE_Y);
      arg_mapsize_y = arg_imgheight + MAT_SHIPSIZE_Y;
    }
#endif

  if (arg_ang < 1)
    {
      fprintf(stderr, "%s: angles < 1\n", program_name);
      arg_ang = 1;
    }

  if (arg_port < 1)
    {
      fprintf(stderr, "%s: port < 1\n", program_name);
      arg_port = 7993;
    }

  if (arg_port_machine < 1)
    {
      fprintf(stderr, "%s: port < 1\n", program_name);
      arg_port_machine = 7993;
    }

  if (arg_speed_max < 0)
    {
      fprintf(stderr, "%s: speed-max < 0\n", program_name);
      arg_speed_max = 0;
    }

  if (arg_fly_back && !arg_brake)
    {
      fprintf(stderr, "%s: fly-back && !arg_brake\n", program_name);
      arg_brake = 1;
    }

  if (arg_lives < 1)
    {
      fprintf(stderr, "%s: lives < 1\n", program_name);
      arg_lives = 1;
    }

  if (arg_ppc != 1 && arg_ppc != 2)
    {
      fprintf(stderr, "%s: ppc != 1 && ppc != 2\n", program_name);
      fprintf(stderr, "%s: 1 and 2 are the only supported values\n", program_name);
      arg_ppc = 1;
    }

  if (arg_ppc != 1)
    arg_chat = 0;

  if (arg_ship_friction < 0)
    {
      fprintf(stderr, "%s: ship-friction < 0\n", program_name);
      arg_ship_friction = 0.0;
    }
  else if (arg_ship_friction > 1)
    {
      fprintf(stderr, "%s: ship-friction > 1\n", program_name);
      arg_ship_friction = 1.0;
    }

  arg_ship_friction = 1 - arg_ship_friction;
}

@ Here we set the global variable |program_name|.  It is used across the
program at the beginning of all the error messages.

If the program name contains slashes, we discard everything before the last of
them.

@<Set the program name@>=
  program_name = strrchr(argv[0], '/');
  if (program_name)
    program_name++;
  else
    program_name = argv[0];

@ This can be still cleaned up much, removing uncompressed flushes.

@d MAT_FLUSH_ALL(con)
{
  if (con->state != MAT_STATE_MACHINE && !con->info.t.clean)
    {
      con->info.t.clean = 1;
      TERM_CLEAR_SCREEN(con);
      memset(con->graph_textbufferold, (char) ' '       , con->xwinsize * con->ywinsize);
      memset(con->graph_attrbufferold, (char) MAT_NORMAL, con->xwinsize * con->ywinsize);
    }
  mat_flush(con, 0, 0, con->xwinsize, con->ywinsize);
}

@d MESSAGE_FROM(con, reason)
   MESSAGE_FROM_REAL(con, reason, 0)

@d MESSAGE_FROM_IP(con, reason)
   MESSAGE_FROM_REAL(con, reason, 1)

@d MESSAGE_FROM_REAL(con, reason, showip)
{
  MatMovingObj *sh;
  MatShip *pl;

  for (sh = con->ship_list; sh; sh = sh->info.player.conn_list_next)
    {
      ASSERT(sh->type == MAT_MOVING_OBJ_SHIP);
      pl = &sh->info.player;

      if (showip)
        message_broadcast(sh->un, "-=> %s <%s> %s.", pl->name, pl->conn->addr, reason);
      else
        message_broadcast(sh->un, "-=> %s %s.", pl->name, reason);
    }
}

@<Function to send the buffer to a player@>=
int
mat_flush ( MatConnection *con, int x, int y, int w, int h )
{
  unsigned char *buf, *att;
  int   term_state;

  term_state = MAT_NORMAL;

  ASSERT(x < w);
  ASSERT(y < h);
  ASSERT(x < con->xwinsize);
  ASSERT(y < con->ywinsize);
  ASSERT(w <= con->xwinsize);
  ASSERT(h <= con->ywinsize);

  buf = con->graph_textbuffer + y * con->xwinsize;
  att = con->graph_attrbuffer + y * con->xwinsize;

  ASSERT(buf);
  ASSERT(att);

  if (arg_compress)
    @<Send frame compressed@>@;
  else
    @<Send frame uncompressed@>;

  SET_TERMINAL(MAT_NORMAL);

  @<Call |mat_out_flush| and check its return value@>;
}

@

@<Send frame uncompressed@>=
{
  int len, i;

  TERM_MOVE_CURSOR(con, y + 1, x + 1);

  for (; y < h; y++)
    {
      for (i = x; i < w; i += len)
        {
          for (len = 1; len + i < w && att[i + len] == att[i]; len ++);
          SET_TERMINAL((int)att[i]);
          mat_write(con, buf + i, len);
        }

      if (y < h - 1)
        {
          mat_printf(con, "%c%c", '\r', '\n');
          buf += con->xwinsize;
          att += con->xwinsize;
        }
    }
}

@

@<Call |mat_out_flush| and check its return value@>=
{
  if (!mat_out_flush(con))
    {
      if (con->state == MAT_STATE_PLAYING)
        MESSAGE_FROM(con, "has gone netdead");
      mat_connection_free(con, MMP_SERVER_CLOSE_SYSERROR, strerror(errno));
      return 0;
    }

  return 1;
}

@

@<Send frame compressed@>=
{
  int   i, j;
  int   advl;
  char *bufold, *attold;
  int   w_x = -1, w_y = -1;

  if (!con->graph_textbufferold)
    {
      con->graph_textbufferold = malloc(con->xwinsize * con->ywinsize);
      con->graph_attrbufferold = malloc(con->xwinsize * con->ywinsize);
      memset(con->graph_textbufferold, (char) ' '       , con->xwinsize * con->ywinsize);
      memset(con->graph_attrbufferold, (char) MAT_NORMAL, con->xwinsize * con->ywinsize);
      TERM_CLEAR_SCREEN(con);
    }

  bufold = con->graph_textbufferold + y * con->xwinsize;
  attold = con->graph_attrbufferold + y * con->xwinsize;

  ASSERT(bufold);
  ASSERT(attold);

  advl = strlen(con->term->cursor_adv);

  for (; y < h; y++)
    {
      for (i = x; i < w;)
        {
          for (j = i; j < w && bufold[j] == buf[j] && (attold[j] == att[j] || buf[j] == ' '); j++);

          if (j < w)
            @<Flush compressed@>@;
          else
            i = j;
        }

      buf += con->xwinsize;
      att += con->xwinsize;
      bufold += con->xwinsize;
      attold += con->xwinsize;
    }
}


@

@d TERM_SET_CURSOR(npos_y, npos_x)
{
  if (w_x == -1)
    {
      TERM_MOVE_CURSOR(con, npos_y + 1, npos_x + 1);
    }
  else if (npos_y == w_y && npos_x == w_x)
    {
      /* Noop. */
    }
  else if (npos_y != w_y)
    {
      if (npos_x == 0 && npos_y == w_y + 1)
        mat_printf(con, "\r\n");
      else
        TERM_MOVE_CURSOR(con, npos_y + 1, npos_x + 1);
    }
  else
    {
      TERMINAL_ADVANCE_CURSOR(npos_x - w_x);
    }

  w_x = npos_x;
  w_y = npos_y;
}

@
@<Flush compressed@>=
{
  SET_TERMINAL((int)att[j]);

  ASSERT(i <= j);

  TERM_SET_CURSOR(y, j);

  mat_write(con, buf + j, 1);
  w_x ++;

  attold[j] = att[j];
  bufold[j] = buf[j];

  i = j + 1;
}

@

@d TERMINAL_ADVANCE_CURSOR(length)
  mat_printf(con, con->term->cursor_adv, length);

@d SET_TERMINAL(term_type)
  if (term_type != term_state)
    {
      mat_printf(con, con->term->code[term_type]);
      term_state = term_type;
    }

@d SET_TERMINAL_LEN(term_type)
  con->term->code_len[term_type]

@* Graphic functions

@d MAT_NORMAL     0
@d MAT_DIM        1
@d MAT_BOLD       2
@d MAT_BOLDFONT   3
@d MAT_REVERSE    4
@d MAT_SPECIAL    5
@d MAT_RED        6
@d MAT_RED_BOLD   7
@d MAT_BLUE       8
@d MAT_TERM_CODES 9

@<AALib: Function to print text@>=
void mat_print (MatConnection *p, int x, int y, int attr, char *s, int bl)
{
  char s1[10000];
  int pos, pos1;
  int x1, y1;

  ASSERT(x >= 0);
  ASSERT(y >= 0);
  ASSERT(x < p->xwinsize);
  ASSERT(y < p->ywinsize);

  x1 = x;
  y1 = y;
  for (pos = 0; s[pos] != 0 && pos < 10000; pos++)
    {
      s1[pos] = s[pos];
      pos1 = x1 + y1 * p->xwinsize;
      ASSERT(x1 < p->xwinsize);
      ASSERT(y1 < p->ywinsize);
      ASSERT(pos1 < p->xwinsize * p->ywinsize);
      p->graph_textbuffer[pos1] = s[pos];
      p->graph_attrbuffer[pos1] = attr;
      x1++;
      if (x1 >= p->xwinsize)
        {
          if (!bl)
            break;
          x1 = 0;
          y1++;
          if (y1 >= p->ywinsize)
            break;
        }
    }
}

@ Here we initialize AALib.

@d CLRSCR(p)
   memset(p->graph_imagebuffer, 0, p->graph_imgwidth * p->graph_imgheight)

@d TEXTCLRSCR(p)
   memset(p->graph_textbuffer, ' ',        p->xwinsize * p->ywinsize),
   memset(p->graph_attrbuffer, MAT_NORMAL, p->xwinsize * p->ywinsize)

@<AALib: Initialize global@>=
{
//  if (!aa_parseoptions(NULL, NULL, &argc, argv))
//    {
//      fprintf(stderr, "%s: aalib: aa_parseoptions failed\n", program_name);
//      exit(EXIT_FAILURE);
//    }

  @<Build tables used for rendering@>;

  uncompressfont(&aa_font16);
}

@

@<AALib: Function to render centered text@>=
void
mat_print_center(MatConnection *p, int x, int y, double size, MatFont *font, int color, char *text)
{
  double height, width;
  
  height = p->graph_imgheight / size;
  width  = p->graph_imgwidth / size;

  print(p,
        (int) ((double) x - width * (double) strlen(text) / 2.0),
        (int) ((double) y - height / 2.0),
        (int) width,
        (int) height,
        font, color, text);
}

@

@<AALib: Function to render text@>=
void
print (MatConnection *c, int x, int y, int width, int height, MatFont *f, int color, char *text)
{
  int i;

  for (i = 0; text[i]; i++)
    mat_graph_pscale(c, x + i * width, y, x + (i + 1)* width, y + height,
           f->data + f->width * (f->height * text[i]), f->width, f->height,
           color);
}

@ This is a mess. Heheheh.

@<Function to print data without checking position@>=
void
mat_graph_pscale ( MatConnection *con, int x1, int y1, int x2, int y2,
                   char *data, int w, int h, int color )
{
  float step;
  int xx1, xx2, yy1, yy2;

  /* If it is in valid coordinates */
  if (x1 >= 0 && x2 < con->graph_imgwidth && y1 >= 0 && y2 <= con->graph_imgheight)
    fastscale(data, con->graph_imagebuffer + x1 + con->graph_imgwidth * y1,
              w, x2 - x1, h, y2 - y1, w, con->graph_imgwidth, color);

  /* If it is outside of the view */
  if (x2 <= 0 || x1 >= con->graph_imgwidth || y2 <= 0 || y1 >= con->graph_imgheight)
    return;

  /* Step es la proporción: width real / width que se quiere mostrar */
  step = w / (float) (x2 - x1); /* width / number of bytes */

  /* xx1: desde que byte en x*step se empieza a imprimir */
  if (x1 >= 0)
    xx1 = 0; /* desde el comienzo */
  else
    {
      xx1 = (int) (-step * x1); /* desde los bytes que quedan por adentro */
      x1 = 0;
    }

  /* xx2: Cuantos bytes se imprimen */
  if (x2 <= con->graph_imgwidth)
    xx2 = w;
  else
    {
      xx2 = (int) (step * (con->graph_imgwidth - x1));
      x2 = con->graph_imgwidth - 1;
    }

  /* Proporción en y */
  step = h / (float) (y2 - y1);
  if (y1 < 0)
    {
      yy1 = (int) (-step * y1);
      y1 = 0;
    }
  else
    yy1 = 0;

  if (y2 > con->graph_imgheight)
    {
      yy2 = (int) (step * (con->graph_imgheight - y1));
      y2 = con->graph_imgheight - 1;
    }
  else
    yy2 = h;

  fastscale(data + xx1 + yy1 * w,
            con->graph_imagebuffer + x1 + con->graph_imgwidth * y1, xx2 - xx1,
            x2 - x1, yy2 - yy1, y2 - y1, w, con->graph_imgwidth, color);
}

void fastscale(char *b1, char *b2, int x1, int x2, int y1,
               int y2, int width1, int width2, int color)
{
    int ddx1, ddx, spx = 0, ex;
    int ddy1, ddy, spy = 0, ey;
    int x;
    char *bb1 = b1;
    width2 -= x2;
    if (!x1 || !x2 || !y1 || !y2)
	return;
    ddx = x1 + x1;
    ddx1 = x2 + x2;
    if (ddx1 < ddx)
	spx = ddx / ddx1, ddx %= ddx1;
    ddy = y1 + y1;
    ddy1 = y2 + y2;
    if (ddy1 < ddy)
	spy = (ddy / ddy1) * width1, ddy %= ddy1;
    ey = -ddy1;
    for (; y2; y2--) {
	ex = -ddx1;
	for (x = x2; x; x--) {
	    if (*b1)
		*b2 = color;
	    b2++;
	    b1 += spx;
	    ex += ddx;
	    if (ex > 0) {
		b1++;
		ex -= ddx1;
	    }
	}
	b2 += width2;
	bb1 += spy;
	ey += ddy;
	if (ey > 0) {
	    bb1 += width1;
	    ey -= ddy1;
	}
	b1 = bb1;
    }
}

@
@<Functions@>=
void
uncompressfont (aa_font *src)
{
  int i, y;

  font.width = 8;
  font.height = src->height;
  font.data = calloc(1, 256 * 8 * src->height);
  for (i = 0, y = 0; i < 256 * font.height; i++, y += 8)
    {
      char c = src->data[i];
      font.data[y    ] = (char) c & (1 << 7);
      font.data[y + 1] = (char) c & (1 << 6);
      font.data[y + 2] = (char) c & (1 << 5);
      font.data[y + 3] = (char) c & (1 << 4);
      font.data[y + 4] = (char) c & (1 << 3);
      font.data[y + 5] = (char) c & (1 << 2);
      font.data[y + 6] = (char) c & (1 << 1);
      font.data[y + 7] = (char) c & (1 << 0);
    }
}

@
@<AALib Functions@>=
@<AALib: Function to print text@>
@<AALib: Function to render centered text@>
@<AALib: Function to render text@>
@<Function to print data without checking position@>
@<AALib: Function to render an image@>
@<AALib: Function to resize context of a player@>

@

@<AALib prototypes@>=
void fastscale(char *b1, char *b2, int x1, int x2, int y1,
               int y2, int width1, int width2, int color);
void print (MatConnection *con,
            int x, int y, int width, int height, MatFont *f,
            int color, char *text);

void        mat_graph_resize            ( MatConnection  *con,
                                          int             width,
                                          int             height );

void        mat_graph_pscale            ( MatConnection  *con,
                                          int             x1,
                                          int             y1,
                                          int             x2,
                                          int             y2,
                                          char           *data,
                                          int             w,
                                          int             h,
                                          int             color );

void        mat_graph_render            ( MatConnection  *con,
                                          int             x1,
                                          int             y1,
                                          int             x2,
                                          int             y2 );

@

@d aa_validmode(x,y,params)
  (    (         (params)->minwidth
              || (params)->maxwidth 
         ||      (params)->width == (x)
              || !(params)->width   )
    && (         (params)->minheight
              || (params)->maxheight  
         ||      (params)->height == (y)
              || !(params)->height   )
    && ( (params)->minwidth  ? (params->minwidth)  <= (x) : 1 )
    && ( (params)->minheight ? (params->minheight) <= (x) : 1 )
    && ( (params)->maxwidth  ? (params->maxwidth)  >= (x) : 1 )
    && ( (params)->maxheight ? (params->maxheight) >= (x) : 1 ) )

@
@<AALib: Destroy the context of player |con|@>=
{
  free(con->graph_imagebuffer);
  free(con->graph_textbuffer);
  free(con->graph_textbufferold);
  free(con->graph_attrbuffer);
  free(con->graph_attrbufferold);

  TERM_CLEAR_SCREEN(con);
  TERM_SHOW_CURSOR(con);
  TERM_MOVE_CURSOR(con, 0, 0);
}

@
@<AALib: Function to resize context of a player@>=
void
mat_graph_resize ( MatConnection *con, int width, int height )
{
  int c;

  if (height <= 0)
    height = 24;
  if (width <= 0)
    width = 80;

  ASSERT(width  > 0);
  ASSERT(height > 0);

  if (con->xwinsize != width || con->ywinsize != height)
    {
      if (con->graph_imagebuffer)
        free(con->graph_imagebuffer);
      if (con->graph_textbuffer)
        free(con->graph_textbuffer);
      if (con->graph_textbufferold)
        free(con->graph_textbufferold);
      if (con->graph_attrbuffer)
        free(con->graph_attrbuffer);
      if (con->graph_attrbufferold)
        free(con->graph_attrbufferold);

      con->xwinsize = width;
      con->ywinsize = height;

      con->graph_imgwidth  = con->xwinsize * 2;
      con->graph_imgheight = con->ywinsize * 2;

      c = width * height;

      con->graph_imagebuffer = malloc(c * 4);
      if (!con->graph_imagebuffer)
        {
          /* TODO: Don't exit, just kill the player. */
          fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
          exit(EXIT_FAILURE);
        }

      con->graph_textbuffer = malloc(c);
      if (!con->graph_textbuffer)
        {
          /* TODO: Don't exit, just kill the player. */
          fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
          exit(EXIT_FAILURE);
        }

      con->graph_attrbuffer = malloc(c);
      if (!con->graph_attrbuffer)
        {
          /* TODO: Don't exit, just kill the player. */
          fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
          exit(EXIT_FAILURE);
        }

      memset(con->graph_textbuffer, ' ',        c);
      memset(con->graph_attrbuffer, MAT_NORMAL, c);
      TERM_CLEAR_IMAGE(con);

      con->graph_textbufferold = NULL;
      con->graph_attrbufferold = NULL;

      if (con->state == MAT_STATE_PLAYING)
        @<Calculate the size for the screens for the ships@>;
    }
}

@

@<AALib: Initialize context for player |con|@>=
{
  ASSERT(con);

  con->graph_textbuffer    = NULL;
  con->graph_textbufferold = NULL;
  con->graph_attrbuffer    = NULL;
  con->graph_attrbufferold = NULL;
  con->graph_imagebuffer   = NULL;

  MAT_DEFAULT_PALETTE(con);
}

@
@d MAT_DEFAULT_PALETTE(p)
{
  int i;

  ASSERT(p);

  for (i = 0; i < 256; i++)
    p->graph_palette[i] = i;
}

@ Here we receive the character C and we look it in the description of the
font, to see how many points are on in each of its four zones, leaving the
results in |v1|, |v2|, |v3| and |v4|.

We also make adjustments for bold, dim and reverse characters.

@d  isset(n,i) (((i)&1<<(n))!=0)
@d canset(n,i) (!isset(n,i)&&isset(n-1,i))
@d MUL 8

@d MAT_GRAPH_DIMMUL  5.3
@d MAT_GRAPH_BOLDMUL 2.7

@<Calculate the values for character |c|@>=
{
  int i;
  int car;
  unsigned const char *font;

  font = graph_font->data;

  car = (c % 256) * graph_font->height;

  v1 = 0;
  v2 = 0;
  v3 = 0;
  v4 = 0;

  for (i = 0; i < graph_font->height / 2; i++)
    {
      v1 += (  isset(0, font[car + i])
             + isset(1, font[car + i])
             + isset(2, font[car + i])
             + isset(3, font[car + i]));
      v2 += (  isset(4, font[car + i])
             + isset(5, font[car + i])
             + isset(6, font[car + i])
             + isset(7, font[car + i]));
    }

  for (; i < graph_font->height; i++)
    {
      v3 += (  isset(0, font[car + i])
             + isset(1, font[car + i])
             + isset(2, font[car + i])
             + isset(3, font[car + i]));
      v4 += (  isset(4, font[car + i])
             + isset(5, font[car + i])
             + isset(6, font[car + i])
             + isset(7, font[car + i]));
    }

  v1 *= MUL;
  v2 *= MUL;
  v3 *= MUL;
  v4 *= MUL;

  switch (c / 256)
    {
    case MAT_REVERSE:
      v1 = graph_font->height * 2 * MUL - v1;
      v2 = graph_font->height * 2 * MUL - v2;
      v3 = graph_font->height * 2 * MUL - v3;
      v4 = graph_font->height * 2 * MUL - v4;
      break;
    case MAT_DIM:
      v1 = (v1 + 1) / MAT_GRAPH_DIMMUL;
      v2 = (v2 + 1) / MAT_GRAPH_DIMMUL;
      v3 = (v3 + 1) / MAT_GRAPH_DIMMUL;
      v4 = (v4 + 1) / MAT_GRAPH_DIMMUL;
      break;
    case MAT_BOLD:
      v1 *= MAT_GRAPH_BOLDMUL;
      v2 *= MAT_GRAPH_BOLDMUL;
      v3 *= MAT_GRAPH_BOLDMUL;
      v4 *= MAT_GRAPH_BOLDMUL;
      break;
    case MAT_BOLDFONT:
      for (i = 0; i < graph_font->height / 2; i++)
        {
          v1 += (   isset(0, font[car + i])
                 + canset(1, font[car + i])
                 + canset(2, font[car + i])
                 + canset(3, font[car + i])) * MUL;
          v2 += (  isset(4, font[car + i])
                 + canset(5, font[car + i])
                 + canset(6, font[car + i])
                 + canset(7, font[car + i])) * MUL;
        }
      for (; i < graph_font->height; i++)
        {
          v3 += (   isset(0, font[car + i])
                 + canset(1, font[car + i])
                 + canset(2, font[car + i])
                 + canset(3, font[car + i])) * MUL;
          v4 += (   isset(4, font[car + i])
                 + canset(5, font[car + i])
                 + canset(6, font[car + i])
                 + canset(7, font[car + i])) * MUL;
        }
    }
}

@ Here we iterate through all the characters, calculating the minimum and
maximum values.

@<Get the maximum and minimum values for all the characters@>=
{
  int v1, v2, v3, v4;
  int c;

  for (c = 0; c < NCHARS; c++)
    {
      if (!ALOWED(c, MAT_GRAPH_SUPPORTED))
        continue;

      @<Calculate the values for character |c|@>;

      if (v1 > ma1)
        ma1 = v1;
      if (v2 > ma2)
        ma2 = v2;
      if (v3 > ma3)
        ma3 = v3;
      if (v4 > ma4)
        ma4 = v4;

      if (v1 + v2 + v3 + v4 > msum)
        msum = v1 + v2 + v3 + v4;

      if (v1 < mi1)
        mi1 = v1;
      if (v2 < mi2)
        mi2 = v2;
      if (v3 < mi3)
        mi3 = v3;
      if (v4 < mi4)
        mi4 = v4;

      if (v1 + v2 + v3 + v4 < misum)
        misum = v1 + v2 + v3 + v4;

      parameters[c].p[0] = v1;
      parameters[c].p[1] = v2;
      parameters[c].p[2] = v3;
      parameters[c].p[3] = v4;
    }
  msum -= misum;

#if 0
  mi1 = misum / 4;
  mi2 = misum / 4;
  mi3 = misum / 4;
  mi4 = misum / 4;

  ma1 = msum / 4;
  ma2 = msum / 4;
  ma3 = msum / 4;
  ma4 = msum / 4;
#endif
}

@ First, we begin to fill |parameters| with the right values. We remember the
maximum and minimum value seen for every point and for the sum of all four
points of a given character.  Then we make adjustments to the parameters using
those maximum and minimum values.

@<Calculate |parameters|@>=
{
  int c;
  int ma1 = 0, ma2 = 0, ma3 = 0, ma4 = 0, msum = 0;
  int mi1 = 50000, mi2 = 50000, mi3 = 50000, mi4 = 50000, misum = 50000;

  @<Get the maximum and minimum values for all the characters@>;

  for (c = 0; c < NCHARS; c++)
    @<Adjust |parameters| for character |c|@>;
}

@ Here we make adjustments to the |parameters| for the character |c|, depending
on the maximum and minimum values seen.

@<Adjust |parameters| for character |c|@>=
{
  parameters[c].p[4] = (double)
    (  parameters[c].p[0]
     + parameters[c].p[1]
     + parameters[c].p[2]
     + parameters[c].p[3]
     - misum) * (1020 / (double) msum) + 0.5;

  parameters[c].p[0] = ((double) (parameters[c].p[0] - mi1) * (255 / (double) ma1) + 0.5);
  parameters[c].p[1] = ((double) (parameters[c].p[1] - mi2) * (255 / (double) ma2) + 0.5);
  parameters[c].p[2] = ((double) (parameters[c].p[2] - mi3) * (255 / (double) ma3) + 0.5);
  parameters[c].p[3] = ((double) (parameters[c].p[3] - mi4) * (255 / (double) ma4) + 0.5);

  @<Make sure the values in |parameters[c]| are valid@>;
}

@ We don't want values over 255 or under 0.

@<Make sure the values in |parameters[c]| are valid@>=
  if (parameters[c].p[0] > 255)
    parameters[c].p[0] = 255;
  if (parameters[c].p[1] > 255)
    parameters[c].p[1] = 255;
  if (parameters[c].p[2] > 255)
    parameters[c].p[2] = 255;
  if (parameters[c].p[3] > 255)
    parameters[c].p[3] = 255;
  if (parameters[c].p[0] < 0)
    parameters[c].p[0] = 0;
  if (parameters[c].p[1] < 0)
    parameters[c].p[1] = 0;
  if (parameters[c].p[2] < 0)
    parameters[c].p[2] = 0;
  if (parameters[c].p[3] < 0)
    parameters[c].p[3] = 0;

@
@<Destroy tables used for rendering@>=
{
  free(graph_parameters);
  free(graph_table);
  free(graph_filltable);
}

@

@d TABLESIZE 65536

@d pow2(i) ((i)*(i))

@d add(i)
  if(next[(i)]==(i)&&last!=(i))
    {
      if (last!=-1)
        next[last]=(i),last=(i);
      else
        last=first=(i);
    }
@d dist(i1,i2,i3,i4,i5,y1,y2,y3,y4,y5)
  ( 2 * (   pow2((int)(i1) - (int)(y1))
          + pow2((int)(i2) - (int)(y2))
          + pow2((int)(i3) - (int)(y3))
          + pow2((int)(i4) - (int)(y4)))
    + pow2((int)(i5)-(int)(y5)) )

@d dist1(i1,i2,i3,i4,i5,y1,y2,y3,y4,y5)
  (  (   pow2((int)(i1) - (int)(y1))
       + pow2((int)(i2)-(int)(y2))
       + pow2((int)(i3)-(int)(y3))
       + pow2((int)(i4)-(int)(y4)) )
   + 2 * pow2((int)(i5)-(int)(y5)))

@d pos(i1,i2,i3,i4)
  (  ((int)(i1) << 12)
   + ((int)(i2) <<  8)
   + ((int)(i3) <<  4)
   + ((int)(i4) <<  0) )

@d postoparams(pos,i1,i2,i3,i4)
  ( (i1) = ((pos) >> 12)     ),
  ( (i2) = ((pos) >>  8) & 15),
  ( (i3) = ((pos) >>  4) & 15),
  ( (i4) = ((pos) >>  0) & 15)

@d NCHARS (256*AA_NATTRS)

@d ALOWED(i,s)
  (    (   isgraph((i) & 0xff)
        || (((i) & 0xff) == ' ')
        || (((i) & 0xff) > 160 && ((s) & AA_EIGHT))
        || (( (s) & AA_ALL ) && ((i) & 0xff)))
    && ( (s) & TOMASK(((i) >> 8))))

@d TOMASK(i) (1 << (i))

@d MAT_GRAPH_SUPPORTED (AA_NORMAL_MASK | (arg_bold ? MAT_BOLD | AA_BOLD_MASK : 0))

@<Build tables used for rendering@>=
{
  unsigned short *next;
  struct parameters *parameters;
  unsigned short *table;
  unsigned short *filltable;

  static int priority[] = { 4, 5, 3, 2, 1 };
  int i;
  int i1, i2, i3, i4;
  int sum, pos;
  int first = -1;
  int last = -1;

  VERBOSE("Building tables used for rendering...");
  fflush(stdout);

  next       = malloc(sizeof(*next) * TABLESIZE);
  parameters = malloc(sizeof(struct parameters) * (NCHARS + 1));
  table      = malloc(TABLESIZE * sizeof(*table));
  filltable  = malloc(256 * sizeof(*filltable));

  first = -1;
  last = -1;

  for (i = 0; i < TABLESIZE; i++)
    next[i] = i, table[i] = 0;

  @<Calculate |parameters|@>;

  for (i = 0; i < NCHARS; i++)
    {
      if (ALOWED(i, MAT_GRAPH_SUPPORTED))
        {
          int p1, p2, p3, p4;

	  i1 = parameters[i].p[0];
	  i2 = parameters[i].p[1];
	  i3 = parameters[i].p[2];
	  i4 = parameters[i].p[3];

	  p1 = i1 >> 4;
	  p2 = i2 >> 4;
	  p3 = i3 >> 4;
	  p4 = i4 >> 4;

	  sum = parameters[i].p[4];
	  pos = pos(p1, p2, p3, p4);

	  if (table[pos])
            {
	      int sum;

	      p1 = (p1 << 4) + p1;
	      p2 = (p2 << 4) + p2;
	      p3 = (p3 << 4) + p3;
	      p4 = (p4 << 4) + p4;

	      sum = p1 + p2 + p3 + p4;

	if ((p1=dist(parameters[i].p[0],
		 parameters[i].p[1],
		 parameters[i].p[2],
			 parameters[i].p[3],
			 parameters[i].p[4],
			 p1, p2, p3, p4, sum)) >=
		    (p1=dist(parameters[table[pos]].p[0],
			 parameters[table[pos]].p[1],
			 parameters[table[pos]].p[2],
			 parameters[table[pos]].p[3],
			 parameters[table[pos]].p[4],
			 p1, p2, p3, p4, sum))&&(p1!=p2||priority[i/256]<=priority[table[pos]/256]))
		    goto skip;
	    }
	    table[pos] = i;
	    add(pos);
	  skip:;

	}
    }
    for (pos = 0; pos < 256; pos++) {
	int mindist = INT_MAX, d1;
	for (i = 0; i < NCHARS; i++) {
	    if (ALOWED(i, MAT_GRAPH_SUPPORTED)) {
	       d1 = dist1(parameters[i].p[0],
				parameters[i].p[1],
				parameters[i].p[2],
				parameters[i].p[3],
				parameters[i].p[4],
				pos, pos, pos, pos, pos*4);
               if (d1 <= mindist
                   && ( d1 != mindist
                        || priority[i/256] > priority[filltable[pos]/256]))
		    filltable[pos] = i, mindist = d1;
	    }
	}
    }
    do {
	int blocked;
	if (last != -1)
	    next[last] = last;
	else
	    break;
	blocked = last;
	i = first;
	if (i == -1)
	    break;
	first = last = -1;
	do {
	    int m1, m2, m3, m4, ii, dm;
	    unsigned short c = table[i];
	    postoparams(i, m1, m2, m3, m4);
	    for (dm = 0; dm < 4; dm++)
		for (ii = -1; ii <= 1; ii += 2) {
		    int dist, dist1, index;
		    unsigned short ch;
		    i1 = m1;
		    i2 = m2;
		    i3 = m3;
		    i4 = m4;
		    switch (dm) {
		    case 0:
			i1 += ii;
			if (i1 < 0 || i1 >= 16)
			    continue;
			break;
		    case 1:
			i2 += ii;
			if (i2 < 0 || i2 >= 16)
			    continue;
			break;
		    case 2:
			i3 += ii;
			if (i3 < 0 || i3 >= 16)
			    continue;
			break;
		    case 3:
			i4 += ii;
			if (i4 < 0 || i4 >= 16)
			    continue;
			break;

		    }
		    index = pos(i1, i2, i3, i4);
		    ch = table[index];
		    if (ch == c || index == blocked)
			continue;
		    if (ch) {
			int ii1 = (i1 << 4) + i1;
			int ii2 = (i2 << 4) + i2;
			int ii3 = (i3 << 4) + i3;
			int ii4 = (i4 << 4) + i4;
			int iisum = ii1 + ii2 + ii3 + ii4;
			dist = dist(
				       ii1, ii2, ii3, ii4, iisum,
				       parameters[c].p[0],
				       parameters[c].p[1],
				       parameters[c].p[2],
				       parameters[c].p[3],
				       parameters[c].p[4]);
			dist1 = dist(
					ii1, ii2, ii3, ii4, iisum,
					parameters[ch].p[0],
					parameters[ch].p[1],
					parameters[ch].p[2],
					parameters[ch].p[3],
					parameters[ch].p[4]);
		    }
		    if (!ch || dist < dist1) {
			table[index] = c;
			add(index);
		    }
		}
	    i1 = i;
	    i = next[i];
	    next[i1] = i1;
	}
	while (i != i1);
    }
  while (last != -1);

  free (next);

  graph_table      = table;
  graph_filltable  = filltable;
  graph_parameters = parameters;

  VERBOSE("Done\n");
}

@ Here we check |x1|, |x2|, |y1| and |y2| and make sure they are inside the
screen. If they aren't, we fix them.

Also, if they are attempting to render something outside of the screen, we
return.

@<AALib: Make sure rendering coordinates are inside the screen@>=
{
  if (x2 < 0 || y2 < 0 || x1 > con->xwinsize || y1 > con->ywinsize)
    return;

  if (x2 >= con->xwinsize)
    x2 = con->xwinsize;

  if (y2 >= con->ywinsize)
    y2 = con->ywinsize;

  if (x1 < 0)
    x1 = 0;

  if (y1 < 0)
    y1 = 0;
}

@

@<Variables for the fonts@>=

/* binary image of font8x16 follows */
static unsigned char font16data[] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x81,
    0xa5, 0x81, 0x81, 0xbd, 0x99, 0x81, 0x81, 0x7e, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7e, 0xff, 0xdb, 0xff, 0xff, 0xc3,
    0xe7, 0xff, 0xff, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x6c, 0xfe, 0xfe, 0xfe, 0xfe, 0x7c, 0x38, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38,
    0x7c, 0xfe, 0x7c, 0x38, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x3c, 0x3c, 0xe7, 0xe7, 0xe7, 0x18,
    0x18, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18,
    0x3c, 0x7e, 0xff, 0xff, 0x7e, 0x18, 0x18, 0x3c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x3c,
    0x3c, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xe7, 0xc3, 0xc3, 0xe7, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c,
    0x66, 0x42, 0x42, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xc3, 0x99, 0xbd, 0xbd, 0x99,
    0xc3, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x1e, 0x0e,
    0x1a, 0x32, 0x78, 0xcc, 0xcc, 0xcc, 0xcc, 0x78, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x3c,
    0x18, 0x7e, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3f, 0x33, 0x3f, 0x30, 0x30, 0x30, 0x30, 0x70, 0xf0, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x63, 0x7f, 0x63,
    0x63, 0x63, 0x63, 0x67, 0xe7, 0xe6, 0xc0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x18, 0xdb, 0x3c, 0xe7, 0x3c, 0xdb,
    0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0,
    0xf0, 0xf8, 0xfe, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x02, 0x06, 0x0e, 0x1e, 0x3e, 0xfe, 0x3e,
    0x1e, 0x0e, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x3c, 0x7e, 0x18, 0x18, 0x18, 0x7e, 0x3c, 0x18, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66,
    0x66, 0x66, 0x66, 0x00, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7f, 0xdb, 0xdb, 0xdb, 0x7b, 0x1b, 0x1b, 0x1b,
    0x1b, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0x60,
    0x38, 0x6c, 0xc6, 0xc6, 0x6c, 0x38, 0x0c, 0xc6, 0x7c, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xfe, 0xfe, 0xfe, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x3c, 0x7e, 0x18, 0x18, 0x18, 0x7e, 0x3c, 0x18, 0x7e,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x3c, 0x7e, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e,
    0x3c, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x18, 0x0c, 0xfe, 0x0c, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x60, 0xfe,
    0x60, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xfe, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24,
    0x66, 0xff, 0x66, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x38, 0x7c, 0x7c, 0xfe,
    0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xfe, 0xfe, 0x7c, 0x7c, 0x38, 0x38, 0x10, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x3c, 0x3c, 0x3c, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x24, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x6c, 0x6c, 0xfe, 0x6c, 0x6c, 0x6c, 0xfe,
    0x6c, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x7c, 0xc6,
    0xc2, 0xc0, 0x7c, 0x06, 0x06, 0x86, 0xc6, 0x7c, 0x18, 0x18,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc2, 0xc6, 0x0c, 0x18,
    0x30, 0x60, 0xc6, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x6c, 0x6c, 0x38, 0x76, 0xdc, 0xcc, 0xcc, 0xcc, 0x76,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x18, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x18,
    0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x18, 0x30, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x3c, 0xff,
    0x3c, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x06, 0x0c, 0x18,
    0x30, 0x60, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x7c, 0xc6, 0xc6, 0xce, 0xde, 0xf6, 0xe6, 0xc6, 0xc6, 0x7c,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0x78, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7c, 0xc6, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0,
    0xc6, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6,
    0x06, 0x06, 0x3c, 0x06, 0x06, 0x06, 0xc6, 0x7c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0c, 0x1c, 0x3c, 0x6c, 0xcc, 0xfe,
    0x0c, 0x0c, 0x0c, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xfe, 0xc0, 0xc0, 0xc0, 0xfc, 0x06, 0x06, 0x06, 0xc6, 0x7c,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x60, 0xc0, 0xc0,
    0xfc, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xfe, 0xc6, 0x06, 0x06, 0x0c, 0x18, 0x30, 0x30,
    0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6,
    0xc6, 0xc6, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7e, 0x06,
    0x06, 0x06, 0x0c, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18,
    0x00, 0x00, 0x00, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x30, 0x18,
    0x0c, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x7e, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x06,
    0x0c, 0x18, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x7c, 0xc6, 0xc6, 0x0c, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6,
    0xde, 0xde, 0xde, 0xdc, 0xc0, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x10, 0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6,
    0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x66,
    0x66, 0x66, 0x7c, 0x66, 0x66, 0x66, 0x66, 0xfc, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0xc2, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc2, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xf8, 0x6c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x6c, 0xf8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x66, 0x62, 0x68,
    0x78, 0x68, 0x60, 0x62, 0x66, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xfe, 0x66, 0x62, 0x68, 0x78, 0x68, 0x60, 0x60,
    0x60, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66,
    0xc2, 0xc0, 0xc0, 0xde, 0xc6, 0xc6, 0x66, 0x3a, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xfe, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x0c, 0x0c, 0x0c,
    0x0c, 0x0c, 0xcc, 0xcc, 0xcc, 0x78, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xe6, 0x66, 0x66, 0x6c, 0x78, 0x78, 0x6c, 0x66,
    0x66, 0xe6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x60,
    0x60, 0x60, 0x60, 0x60, 0x60, 0x62, 0x66, 0xfe, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc3, 0xe7, 0xff, 0xff, 0xdb, 0xc3,
    0xc3, 0xc3, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc6, 0xe6, 0xf6, 0xfe, 0xde, 0xce, 0xc6, 0xc6, 0xc6, 0xc6,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x60, 0x60,
    0x60, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xd6, 0xde, 0x7c, 0x0c, 0x0e,
    0x00, 0x00, 0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x7c, 0x6c,
    0x66, 0x66, 0x66, 0xe6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x7c, 0xc6, 0xc6, 0x60, 0x38, 0x0c, 0x06, 0xc6, 0xc6, 0x7c,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xdb, 0x99, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
    0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c, 0x18, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb,
    0xdb, 0xff, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc3, 0xc3, 0x66, 0x3c, 0x18, 0x18, 0x3c, 0x66, 0xc3, 0xc3,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0xc3, 0xc3, 0x66,
    0x3c, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xff, 0xc3, 0x86, 0x0c, 0x18, 0x30, 0x60, 0xc1,
    0xc3, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x70, 0x38,
    0x1c, 0x0e, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x6c, 0xc6, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x30, 0x30, 0x18, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x0c, 0x7c,
    0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xe0, 0x60, 0x60, 0x78, 0x6c, 0x66, 0x66, 0x66, 0x66, 0x7c,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c,
    0xc6, 0xc0, 0xc0, 0xc0, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x1c, 0x0c, 0x0c, 0x3c, 0x6c, 0xcc, 0xcc, 0xcc,
    0xcc, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x7c, 0xc6, 0xfe, 0xc0, 0xc0, 0xc6, 0x7c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x38, 0x6c, 0x64, 0x60, 0xf0, 0x60,
    0x60, 0x60, 0x60, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x76, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x7c,
    0x0c, 0xcc, 0x78, 0x00, 0x00, 0x00, 0xe0, 0x60, 0x60, 0x6c,
    0x76, 0x66, 0x66, 0x66, 0x66, 0xe6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x06,
    0x00, 0x0e, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x66, 0x66,
    0x3c, 0x00, 0x00, 0x00, 0xe0, 0x60, 0x60, 0x66, 0x6c, 0x78,
    0x78, 0x6c, 0x66, 0xe6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe6,
    0xff, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x66, 0x66, 0x66, 0x66,
    0x66, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x66, 0x66,
    0x66, 0x66, 0x66, 0x7c, 0x60, 0x60, 0xf0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x76, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x7c,
    0x0c, 0x0c, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdc,
    0x76, 0x66, 0x60, 0x60, 0x60, 0xf0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0x60, 0x38, 0x0c,
    0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x30,
    0x30, 0xfc, 0x30, 0x30, 0x30, 0x30, 0x36, 0x1c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcc, 0xcc,
    0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c, 0x18,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3,
    0xc3, 0xc3, 0xdb, 0xdb, 0xff, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x66, 0x3c, 0x18, 0x3c,
    0x66, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7e, 0x06, 0x0c,
    0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xcc, 0x18,
    0x30, 0x60, 0xc6, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0e, 0x18, 0x18, 0x18, 0x70, 0x18, 0x18, 0x18, 0x18, 0x0e,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18,
    0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x70, 0x18, 0x18, 0x18, 0x0e, 0x18, 0x18, 0x18,
    0x18, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0xdc,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x6c, 0xc6,
    0xc6, 0xc6, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3c, 0x66, 0xc2, 0xc0, 0xc0, 0xc0, 0xc2, 0x66, 0x3c, 0x0c,
    0x06, 0x7c, 0x00, 0x00, 0x00, 0x00, 0xcc, 0x00, 0x00, 0xcc,
    0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x0c, 0x18, 0x30, 0x00, 0x7c, 0xc6, 0xfe, 0xc0, 0xc0,
    0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x6c,
    0x00, 0x78, 0x0c, 0x7c, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xcc, 0x00, 0x00, 0x78, 0x0c, 0x7c,
    0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60,
    0x30, 0x18, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0xcc, 0xcc, 0x76,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x6c, 0x38, 0x00, 0x78,
    0x0c, 0x7c, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x60, 0x60, 0x66, 0x3c,
    0x0c, 0x06, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x6c,
    0x00, 0x7c, 0xc6, 0xfe, 0xc0, 0xc0, 0xc6, 0x7c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc6, 0x00, 0x00, 0x7c, 0xc6, 0xfe,
    0xc0, 0xc0, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60,
    0x30, 0x18, 0x00, 0x7c, 0xc6, 0xfe, 0xc0, 0xc0, 0xc6, 0x7c,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x38,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x18, 0x3c, 0x66, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x30, 0x18,
    0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xc6, 0x00, 0x10, 0x38, 0x6c, 0xc6, 0xc6,
    0xfe, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00, 0x38, 0x6c,
    0x38, 0x00, 0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0xc6,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x30, 0x60, 0x00, 0xfe, 0x66,
    0x60, 0x7c, 0x60, 0x60, 0x66, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x6e, 0x3b, 0x1b, 0x7e, 0xd8,
    0xdc, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x6c,
    0xcc, 0xcc, 0xfe, 0xcc, 0xcc, 0xcc, 0xcc, 0xce, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x10, 0x38, 0x6c, 0x00, 0x7c, 0xc6, 0xc6,
    0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc6, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 0x00, 0x7c,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x30, 0x78, 0xcc, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
    0xcc, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x30, 0x18,
    0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc6, 0x00, 0x00, 0xc6, 0xc6, 0xc6,
    0xc6, 0xc6, 0xc6, 0x7e, 0x06, 0x0c, 0x78, 0x00, 0x00, 0xc6,
    0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x00, 0xc6, 0xc6, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x18, 0x18, 0x7e, 0xc3, 0xc0, 0xc0, 0xc0, 0xc3, 0x7e,
    0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x6c, 0x64,
    0x60, 0xf0, 0x60, 0x60, 0x60, 0x60, 0xe6, 0xfc, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc3, 0x66, 0x3c, 0x18, 0xff, 0x18,
    0xff, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc,
    0x66, 0x66, 0x7c, 0x62, 0x66, 0x6f, 0x66, 0x66, 0x66, 0xf3,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x1b, 0x18, 0x18, 0x18,
    0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0xd8, 0x70, 0x00, 0x00,
    0x00, 0x18, 0x30, 0x60, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0xcc,
    0xcc, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x18, 0x30,
    0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x30, 0x60, 0x00, 0x7c, 0xc6, 0xc6,
    0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18,
    0x30, 0x60, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x76,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0xdc, 0x00, 0xdc,
    0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x76, 0xdc, 0x00, 0xc6, 0xe6, 0xf6, 0xfe, 0xde, 0xce, 0xc6,
    0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x6c, 0x6c,
    0x3e, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x38, 0x6c, 0x6c, 0x38, 0x00, 0x7c, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x30, 0x30, 0x00, 0x30, 0x30, 0x60, 0xc0, 0xc6, 0xc6, 0x7c,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xfe, 0xc0, 0xc0, 0xc0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x06, 0x06, 0x06,
    0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0xc2,
    0xc6, 0xcc, 0x18, 0x30, 0x60, 0xce, 0x9b, 0x06, 0x0c, 0x1f,
    0x00, 0x00, 0x00, 0xc0, 0xc0, 0xc2, 0xc6, 0xcc, 0x18, 0x30,
    0x66, 0xce, 0x96, 0x3e, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x3c, 0x3c, 0x3c, 0x18,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36,
    0x6c, 0xd8, 0x6c, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xd8, 0x6c, 0x36, 0x6c, 0xd8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x44, 0x11, 0x44,
    0x11, 0x44, 0x11, 0x44, 0x11, 0x44, 0x11, 0x44, 0x11, 0x44,
    0x11, 0x44, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa,
    0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0xdd, 0x77,
    0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77,
    0xdd, 0x77, 0xdd, 0x77, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xf8, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0xf8, 0x18, 0xf8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0xf6,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8,
    0x18, 0xf8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x36, 0x36, 0x36, 0x36, 0x36, 0xf6, 0x06, 0xf6, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x06, 0xf6,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0xf6, 0x06, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x18, 0x18, 0x18, 0x18, 0xf8, 0x18, 0xf8, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xf8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1f,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xff, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1f, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xff,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x1f, 0x18, 0x1f, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x37, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 0x30, 0x3f, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x3f, 0x30, 0x37, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0xf7, 0x00, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xff, 0x00, 0xf7, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x37,
    0x30, 0x37, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x36, 0x36, 0x36,
    0x36, 0xf7, 0x00, 0xf7, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x18, 0x18, 0x18, 0x18, 0x18, 0xff, 0x00, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
    0x00, 0xff, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1f, 0x18, 0x1f,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1f, 0x18, 0x1f, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x3f, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0xff, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x18, 0x18, 0x18, 0x18,
    0x18, 0xff, 0x18, 0xff, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xf8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xf0, 0xf0, 0xf0,
    0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
    0xf0, 0xf0, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76,
    0xdc, 0xd8, 0xd8, 0xd8, 0xdc, 0x76, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x78, 0xcc, 0xcc, 0xcc, 0xd8, 0xcc, 0xc6, 0xc6,
    0xc6, 0xcc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xc6,
    0xc6, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x6c, 0x6c, 0x6c,
    0x6c, 0x6c, 0x6c, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xfe, 0xc6, 0x60, 0x30, 0x18, 0x30, 0x60, 0xc6, 0xfe,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e,
    0xd8, 0xd8, 0xd8, 0xd8, 0xd8, 0x70, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c,
    0x60, 0x60, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x76, 0xdc, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x18, 0x3c, 0x66, 0x66,
    0x66, 0x3c, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0x6c, 0x38,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x6c, 0xc6, 0xc6,
    0xc6, 0x6c, 0x6c, 0x6c, 0x6c, 0xee, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x1e, 0x30, 0x18, 0x0c, 0x3e, 0x66, 0x66, 0x66,
    0x66, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x7e, 0xdb, 0xdb, 0xdb, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x06, 0x7e, 0xdb, 0xdb,
    0xf3, 0x7e, 0x60, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1c, 0x30, 0x60, 0x60, 0x7c, 0x60, 0x60, 0x60, 0x30, 0x1c,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0xfe, 0x00, 0x00,
    0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x00, 0xff, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x18, 0x0c, 0x06, 0x0c,
    0x18, 0x30, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x0c, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0c, 0x00, 0x7e,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x1b, 0x1b, 0x1b,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xd8, 0xd8,
    0xd8, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x18, 0x00, 0x7e, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0xdc, 0x00,
    0x76, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38,
    0x6c, 0x6c, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0c, 0x0c,
    0x0c, 0x0c, 0x0c, 0xec, 0x6c, 0x6c, 0x3c, 0x1c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xd8, 0x6c, 0x6c, 0x6c, 0x6c, 0x6c, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70,
    0xd8, 0x30, 0x60, 0xc8, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x7c,
    0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

struct aa_font aa_font16 =
{font16data, 16, "Standard vga 8x16 font", "vga16"};

@ Here we prepare a temporal palette, applying adjustments from |graph_bright|,
|graph_inversion| and |graph_gamma|.

In the future, rather than doing this during render time, it should be done
when |graph_bright|, |graph_inversion| or |graph_gamma| are modified.

@<Prepare the temporal palette from |pal| into |table|@>=
{
  int j, col;

  for (j = 0; j < 256; j++)
    {
      col = palette[j] + con->graph_bright;

      @<Check that |col| has a valid color@>;
      @<Apply the contrast in player |con| to |col|@>;

      if (gamma)
        y = pow(y / 255.0, con->graph_gamma) * 255 + 0.5;

      if (con->graph_inversion)
        y = 255 - y;

      @<Check that |col| has a valid color@>;

      table[j] = col;
    }
}

@

@<Apply the contrast in player |con| to |col|@>=
  if (con->graph_contrast)
    {
      if (y - con->graph_contrast < 0)
        y = 0;
      else if (y + con->graph_contrast > 256)
        y = 255;
      else
        y = (y - con->graph_contrast) * (255 / (255 - 2 * con->graph_contrast));
    }

@

@<Check that |col| has a valid color@>=
  if (col > 255)
    col = 255;
  if (col < 0)
    col = 0;

@ We use a very simple random numbers generator.

@d MYLONG_MAX 0xffffffff
@d MYRAND() (((state * 1103515245) + 12345) & MYLONG_MAX)

@<Apply randomization to points |i1|, |i2|, |i3| and |i4|@>=
  if (gamma)
    {
      static int state;

      state = MYRAND();

      i1 += (state >>  0) % con->graph_randomval - gamma;
      i2 += (state >>  8) % con->graph_randomval - gamma;
      i3 += (state >> 16) % con->graph_randomval - gamma;
      i4 += (state >> 24) % con->graph_randomval - gamma;

      @<Make sure points |i1|, |i2|, |i3| and |i4| have a valid color@>;
    }

@ Here we make sure the points have a color in the valid range.  This is
optimized for the common case.

@<Make sure points |i1|, |i2|, |i3| and |i4| have a valid color@>=

  if ((i1 | i2 | i3 | i4) & (~255))
    {
      if (i1 < 0)
        i1 = 0;
      else if (i1 > 255)
        i1 = 255;
      if (i2 < 0)
        i2 = 0;
      else if (i2 > 255)
        i2 = 255;
      if (i3 < 0)
        i3 = 0;
      else if (i3 > 255)
        i3 = 255;
      if (i4 < 0)
        i4 = 0;
      else if (i4 > 255)
        i4 = 255;
    }

@ We render four points in a square at a time.  In this section, we get four
points from |graph_imagebuffer| at position |pos| and store their colors in
|i1|, |i2|, |i3| and |i4|.

@<Grab four points into |i1|, |i2|, |i3| and |i4@>=
  i1 = con->graph_palette[((((int)con->graph_imagebuffer[pos         ])))];
  i2 = con->graph_palette[((((int)con->graph_imagebuffer[pos + 1     ])))];
  i3 = con->graph_palette[((((int)con->graph_imagebuffer[pos     + wi])))];
  i4 = con->graph_palette[((((int)con->graph_imagebuffer[pos + 1 + wi])))];

@

@d MAT_GRAPH_RENDER_ALL(con)
  mat_graph_render(con, 0, 0, (con)->xwinsize, (con)->ywinsize);

@d MAT_GRAPH_RENDER_NOWIPE_ALL(con)
  mat_graph_render_nowipe(con, 0, 0, (con)->xwinsize, (con)->ywinsize);

@<AALib: Function to render an image@>=
void
mat_graph_render ( MatConnection *con, int x1, int y1, int x2, int y2 )
{
  int x, y;
  int val;
  int wi = con->graph_imgwidth;
  int pos, pos1;
  int i1, i2, i3, i4;
  float gamma;

  @<AALib: Make sure rendering coordinates are inside the screen@>;

  if (con->graph_randomval > 0)
    gamma = con->graph_randomval / 2;
  else
    gamma = 0;

  for (y = y1; y < y2; y++)
    {
      pos = 2 * y * wi;
      pos1 = y * con->xwinsize;
      for (x = x1; x < x2; x++)
        {
          @<Grab four points into |i1|, |i2|, |i3| and |i4@>;
          @<Apply randomization to points |i1|, |i2|, |i3| and |i4|@>;

          val = graph_table[((i1>>4)<<8)+((i2>>4)<<12) + ((i3>>4)<<0) + ((i4>>4)<<4)];

          con->graph_imagebuffer[pos         ] = 0;
          con->graph_imagebuffer[pos + 1     ] = 0;
          con->graph_imagebuffer[pos     + wi] = 0;
          con->graph_imagebuffer[pos + 1 + wi] = 0;

          con->graph_attrbuffer[pos1] = val >> 8;
          con->graph_textbuffer[pos1] = val & 0xff;
          pos += 2;
          pos1++;
        }
    }
}

void
mat_graph_render_nowipe ( MatConnection *con, int x1, int y1, int x2, int y2 )
{
  int x, y;
  int val;
  int wi = con->graph_imgwidth;
  int pos, pos1;
  int i1, i2, i3, i4;
  float gamma;

  @<AALib: Make sure rendering coordinates are inside the screen@>;

  if (con->graph_randomval > 0)
    gamma = con->graph_randomval / 2;
  else
    gamma = 0;

  for (y = y1; y < y2; y++)
    {
      pos = 2 * y * wi;
      pos1 = y * con->xwinsize;
      for (x = x1; x < x2; x++)
        {
          @<Grab four points into |i1|, |i2|, |i3| and |i4@>;
          @<Apply randomization to points |i1|, |i2|, |i3| and |i4|@>;

          val = graph_table[((i1>>4)<<8)+((i2>>4)<<12) + ((i3>>4)<<0) + ((i4>>4)<<4)];

          con->graph_attrbuffer[pos1] = val >> 8;
          con->graph_textbuffer[pos1] = val & 0xff;
          pos += 2;
          pos1++;
        }
    }
}

#if 0
#define VAL (13)

void
mat_graph_render_good ( MatConnection *con, int x1, int y1, int x2, int y2)
{
  static int state;
  int x, y;
  int val;
  int wi = con->graph_imgwidth;
  int pos;
  int i;
  int pos1;
  int i1, i2, i3, i4, esum;
  int *errors[2];
  int cur = 0;
  int mval;
  int gamma /*= p->gamma != 1.0*/;
  unsigned char table[256];

  errors[0] = calloc(1, (x2 + 5) * sizeof(int));
  errors[0] += 3;
  errors[1] = calloc(1, (x2 + 5) * sizeof(int));
  errors[1] += 3;
  cur = 0;

#if 0
  for (i = 0; i < 256; i++)
    {
      y = con->graph_palette[i] + 0; /*p->bright;*/

      if (y < 0)
        y = 0;
      else if (y > 255)
        y = 255;

      /*if (p->contrast)
        y = DO_CONTRAST(y, p->contrast);*/

      /*if (gamma)
        y = pow(y / 255.0, p->gamma) * 255 + 0.5;*/

      /*if (p->inversion)
        y = 255 - y;*/
      /*if (y > 255)
        y = 255;
      else if (y < 0)
        y = 0;*/

      table[i] = y;
   }
#endif

  if (con->graph_randomval)
    gamma = con->graph_randomval / 2;
  else
    gamma = 0;

  mval = (graph_parameters[graph_filltable[255]].p[4]);
  for (y = y1; y < y2; y++)
    {
      pos = 2 * y * wi;
      pos1 = y * con->xwinsize;
      esum = 0;
      for (x = x1; x < x2; x++)
        {
          @<Grab four points into |i1|, |i2|, |i3| and |i4@>;
          @<Apply randomization to points |i1|, |i2|, |i3| and |i4|@>;

          if (i1 | i2 | i3 | i4)
            {
              errors[cur][x - 2] += esum >> 4;
              errors[cur][x - 1] += (5 * esum) >> 4;
              errors[cur][x] = (3 * esum) >> 4;
              esum = (7 * esum) >> 4;
              esum += errors[cur ^ 1][x];
              i1 += (esum + 1) >> 2;
              i2 += (esum) >> 2;
              i3 += (esum + 3) >> 2;
              i4 += (esum + 2) >> 2;
            }

          esum = i1 + i2 + i3 + i4;
          val = (esum) >> 2;
          if ((abs(i1 - val) < VAL &&
               abs(i2 - val) < VAL &&
               abs(i3 - val) < VAL &&
               abs(i4 - val) < VAL))
            {
              if (esum >= 4 * 256)
                val = 255, esum = 4 * 256 - 1;
              if (val < 0)
                val = 0;
              val = graph_filltable[val];
            }
          else
            {
              @<Make sure points |i1|, |i2|, |i3| and |i4| have a valid color@>;
              esum = i1 + i2 + i3 + i4;
              i1 >>= 4;
              i2 >>= 4;
              i3 >>= 4;
              i4 >>= 4;
              val = graph_table[((i1>>4)<<8) + ((i2>>4)<<12) + ((i3>>4)<<0) + ((i4>>4)<<4)];
            }
          esum = (esum - (graph_parameters[val].p[4]) * 1020 / mval);

          con->graph_attrbuffer[pos1] = val >> 8;
          con->graph_textbuffer[pos1] = val & 0xff;
          pos += 2;
          pos1++;
        }

      if (x2 - 1 > x1)
        errors[cur][x2 - 2] += (esum) >> 4;
      if (x2 > x1)
        errors[cur][x2 - 1] += (5 * esum) >> 4;
      cur ^= 1;
      errors[cur][x1] = 0;
      errors[cur ^ 1][-1] = 0;
    }
  free(errors[0] - 3);
  free(errors[1] - 3);
}
#endif

@* Animations

In the section we'll place some animations to send to the players.

@<Functions@>=
@<Animation 000: Function@>
@<Animation 001: Function@>
@<Animation 002: Function@>
@<Main Menu: Animate@>
@<Main Menu: Key Handler@>

@

@<Struct definitions@>=
@<Animation 000: Data@>
@<Animation 001: Data@>
@<Animation 002: Data@>
@<Main Menu: Data@>

@

@<Function prototypes@>=
void mat_anim_000 ( MatConnection * );
void mat_anim_001 ( MatConnection * );
void mat_anim_002 ( MatConnection * );
void mat_mainmenu_anim ( MatConnection * );

void mat_mainmenu_key  ( MatConnection *, int );

@

@d DATA000(x) (((MatAnim000*)con->anim_data)->x)

@<Animation 000: Data@>=

typedef struct MatAnim000 MatAnim000;

struct MatAnim000
{
  int width, height;
  int mode;
  int colors[12];
  int delay;
  int next;
  int pos_x, pos_y;
};

@

@<Animation 000: Play@>=
{
  int i;

  ASSERT(con);

  con->graph_randomval = 60;

  con->anim_func = mat_anim_000;
  con->anim_data = malloc(sizeof(MatAnim000));

  for (i = 0; i < 12; i ++)
    DATA000(colors[i] = 0);

  DATA000(next) = 0;
  DATA000(delay) = 30;

  DATA000(width)  = con->graph_imgwidth  / 7;
  DATA000(height) = con->graph_imgheight / 2;

  DATA000(pos_x)  = con->graph_imgwidth  / 14;
  DATA000(pos_y)  = con->graph_imgheight / 2;
}

@

@<Animation 000: Function@>=

void
mat_anim_000 ( MatConnection *con )
{
  static char *text0[] = { "F", "r", "e", "a", "k", "s" };
  static char *text1[] = { "U", "n", "i", "d", "o", "s" };
  int i;

  ASSERT(con);

  for (i = 0; i < 6; i ++)
    if (DATA000(colors[i]))
      print(con, DATA000(pos_x) + DATA000(width) * i, 0,
            DATA000(width), DATA000(height),
            &font, DATA000(colors[i]), text0[i]);

  for (i = 6; i < 12; i ++)
    if (DATA000(colors[i]))
      print(con, DATA000(pos_x) + DATA000(width) * (i - 6), DATA000(pos_y),
            DATA000(width), DATA000(height),
            &font, DATA000(colors[i]), text1[i - 6]);

  MAT_GRAPH_RENDER_ALL(con);
  MAT_FLUSH_ALL(con);

  @<Animation 000: Update@>;
}

@

@<Animation 000: Update@>=
  if (DATA000(delay) == 0)
    {
      if (DATA000(next) != 6)
        {
          DATA000(colors[DATA000(next)]) = 252;
          DATA000(colors[11 - DATA000(next)]) = 252;
          DATA000(next)++;
          DATA000(delay) = 8;
        }
      else
        DATA000(delay)--;
    }
  else
    {
      int i;

      if (DATA000(delay) == -1)
        {
          if (DATA000(colors[5]) == 0)
            {
              free(con->anim_data);
              @<Animation 001: Play@>;
              return;
            }
        }
      else
        DATA000(delay) --;

      for (i = 0; i < 12; i ++)
        if (DATA000(colors[i]))
          DATA000(colors[i]) -= 6;
    }
@

@d DATA001(x) (((MatAnim001*)con->anim_data)->x)

@<Animation 001: Data@>=

typedef struct MatAnim001 MatAnim001;

struct MatAnim001
{
  double head, delta, delay;
  int width, height, pos_x, pos_y;
};

@

@<Animation 001: Play@>=
{
  con->graph_randomval = 60;

  con->anim_func = mat_anim_001;
  con->anim_data = malloc(sizeof(MatAnim001));

  DATA001(delay) = 0;
  DATA001(delta) = 0.3;
  DATA001(head)  = -7;

  DATA001(width)  = con->graph_imgwidth  / 9;
  DATA001(height) = con->graph_imgheight / 2;

  DATA001(pos_x)  = con->graph_imgwidth  / 81;
  DATA001(pos_y)  = con->graph_imgheight / 4;
}

@

@<Animation 001: Function@>=

void
mat_anim_001 ( MatConnection *con )
{
  static char *text[] = { "P", "r", "e", "s", "e", "n", "t", "s" };
  int i;
  double c;

  for (i = 0; i < 8; i++)
    {
      c = 36.0 * (((double)i) - ((double)DATA001(head)));
      c = i >= DATA001(head) ? 255 - c : 255 + c;
      c = c > 0 ? c : 0;

      print(con, DATA001(pos_x) + (DATA001(pos_x) + DATA001(width)) * i,
            DATA001(pos_y), DATA001(width), DATA001(height),
            &font, (int) c, text[i]);
    }

  MAT_GRAPH_RENDER_ALL(con);
  MAT_FLUSH_ALL(con);

  @<Animation 001: Update@>;
}

@

@<Animation 001: Update@>=
  if (DATA001(head) <= -3)
    DATA001(delta) = +0.4;
  else if (DATA001(head) >= 10)
    {
      if (DATA001(delay) < 2)
        DATA001(delta) = -0.4;
      else if (DATA001(delay) > 20)
        {
          free(con->anim_data);
          @<Animation 002: Play@>;
        }

      DATA001(delay)++;
    }
  DATA001(head) += DATA001(delta);

@

@d MAXTABLE (256*5)
@d DATA002(x) (((MatAnim002*)con->anim_data)->x)

@<Animation 002: Data@>=
typedef struct MatAnim002 MatAnim002;

struct MatAnim002
{
  unsigned int table[MAXTABLE];
  int height, loop, sloop;
  int done, curr, text_pos;
};

@

@<Animation 002: Play@>=
{
  int minus;
  unsigned int i;

  ASSERT(con);

  con->graph_randomval = 0;

  con->anim_func = mat_anim_002;
  con->anim_data = malloc(sizeof(MatAnim002));

  minus = 800 / (con->graph_imgheight - 4);

  if (minus == 0)
    minus = 1;

  for (i = 0; i < MAXTABLE; i++)
    {
      if (i > minus)
        DATA002(table)[i] = (i - minus) / 5;
      else
        DATA002(table)[i] = 0;
    }

  @<Prepare palette for the fire effect@>;

  DATA002(loop) = DATA002(sloop) = DATA002(height) = 0;

  DATA002(text_pos) = 0;
  DATA002(done) = con->graph_imgheight / 3;
  DATA002(curr) = 0;
}

@ Here we initialize the palette for the fire effect.

@d COLORRGB(r,g,b) (((r)*30+(g)*59+(b)*11)>>8)

@d FP(i) (fire_pal[j * 3 + i] * 4)

@<Prepare palette for the fire effect@>=
{
  int j;

  ASSERT(con);

  for (j = 0; j < 256; j++)
    con->graph_palette[j] = COLORRGB(FP(0), FP(1), FP(2));
}

@

@d min(x,y) ((x)<(y)?(x):(y))

@d CALC(m)
  (con->graph_imagebuffer + con->graph_imgwidth * (con->graph_imgheight - m))

@d CALC1(y,x) (*(c + y * con->graph_imgwidth + x))

@<Animation 002: Function@>=
void
mat_anim_002 ( MatConnection *con )
{
  unsigned int i, last1, i1, i2;
  unsigned char *c;
  static char *text = "Welcome to Matanza, the multiplayer, action, "
  "network-based, real time, ASCII art based, spaceship, slogan lacking game."
  "                                                      "
  "Matanza is currently under development by Alejandro Forero Cuervo of the "
  "Freaks Unidos.  You can find more information on this organization on the "
  "web at <http://bachue.com>."
  "                                                      ";

  ASSERT(con);

  DATA002(height)++;
  DATA002(loop)--;

  if (DATA002(loop) < 0)
    {
      DATA002(loop) = rand() % 3;
      DATA002(sloop++);
    }

  i1 = 1;
  i2 = 4 * con->graph_imgwidth + 1;

  ASSERT(CALC(3) - CALC(4) == con->graph_imgwidth);

  for (c = CALC(4); c < CALC(3); c++, i1 += 4, i2 -= 4)
    {
      last1 = rand() % min(i1, min(i2, DATA002(height)));
      i = rand() % 6;
      while (c < CALC(3) && i != 0)
        {
	  *(c                          ) = last1;
          last1 += rand() % 6 - 2;
          *(c +     con->graph_imgwidth) = last1;
          last1 += rand() % 6 - 2;
          *(c + 2 * con->graph_imgwidth) = last1;
          last1 += rand() % 6 - 2;

          c++;
          i--;
          i1 += 4;
          i2 -= 4;
        }
    }

    {
      unsigned char *c;

      c = con->graph_imagebuffer;

      while (c <= CALC(4))
        *(c++) = DATA002(table)[CALC1(1,-1)+CALC1(1,1)+CALC1(1,0)+CALC1(2,-1)+CALC1(2,1)];
    }

  {
    char buffer[2];

    if (!DATA002(done) --)
      {
        DATA002(done) = con->graph_imgheight / 3;

        if (DATA002(curr) >= 7)
          {
            if (DATA002(curr) ++ == 13)
              DATA002(curr) = 0;
          }
        else
          {
            buffer[0] = "Matanza"[DATA002(curr)++];
            buffer[1] = 0;

            print(con, con->graph_imgwidth / 8, con->graph_imgheight / 10,
                  6 * con->graph_imgwidth / 8, 8 * con->graph_imgheight / 10,
                  &font, 255, buffer);
          }
      }
  }

  MAT_GRAPH_RENDER_NOWIPE_ALL(con);

  mat_print(con, MAX(con->xwinsize - DATA002(text_pos) - 1, 0), 0, MAT_BOLD,
            text + MAX(DATA002(text_pos) - con->xwinsize + 1, 0), 0);

  DATA002(text_pos)++;
  if (text[MAX(DATA002(text_pos) - con->xwinsize + 1, 0)] == 0)
    DATA002(text_pos) = 0;

  MAT_FLUSH_ALL(con);
}

@

@d DATAMENU(field) (((MatStateMainMenu*)con->anim_data)->field)

@d MENU_STATE_DATA    0
@d MENU_STATE_KEY     1
@d MENU_STATE_KEYREAL 2

@<Main Menu: Data@>=
typedef struct MatStateMainMenu MatStateMainMenu;

struct MatStateMainMenu
{
  char     buffer[2][17];
  int      buflen[2];

  char    *texterr;

  int      focus;
  int      state;

  int      text_pos;
  int      text_cur;

  MatTeam *team;
};

@
@<Function prototypes@>=
void main_menu_begin ( MatConnection *p );

@
@<Functions@>=
void
main_menu_begin ( MatConnection *con )
{
  int c;

  MatStateMainMenu *menu;

  ASSERT(con);

  TERM_CLEAR_IMAGE(con);

  c = con->xwinsize * con->ywinsize;

  memset(con->graph_textbuffer, ' ', c);
  memset(con->graph_attrbuffer, MAT_NORMAL, c);

  TERM_CLEAR_SCREEN(con);

  @<Main Menu: Initialize |menu|@>;

  con->anim_func   = mat_mainmenu_anim;
  con->anim_data   = menu;
  con->key_handler = mat_mainmenu_key;

  @<Main Menu: Draw input fields@>;
}

@
@<Give player |p| a random team@>=
{
  int i;

  p->info.player.team = arg_teams_head;
  if (arg_teams_count)
    for (i = rand() % arg_teams_count; i > 0; i--)
      p->info.player.team = p->info.player.team->next;
}

@

@<Main Menu: Initialize |menu|@>=
{
  ASSERT(con);

  menu = malloc(sizeof(MatStateMainMenu));

  menu->focus = 0;
  menu->state = MENU_STATE_DATA;

  @<Select a random name@>;

  menu->buflen[1] = 0;
  menu->buffer[1][0] = 0;

  menu->text_pos = 0;
  menu->text_cur = 0;

  menu->texterr  = NULL;
}

@

|MAT_TEXTERR_BLANK| should have |MAT_TEXTERR_MAX| blanks.

@d MAT_TEXTERR_MAX 29
@d MAT_TEXTERR_BLANK "                            "

@<Main Menu: Key Handler@>=
void
mat_mainmenu_key ( MatConnection *con, int input )
{
  MatStateMainMenu *menu;

  ASSERT(con->state == MAT_STATE_NORMAL);

  menu = con->anim_data;

  ASSERT(menu);
  ASSERT((menu->focus != 0 && menu->focus != 1) || menu->buflen[menu->focus] >= 0);

  menu->texterr = MAT_TEXTERR_BLANK;

  switch(menu->state)
    {
    case MENU_STATE_DATA:
      switch (input)
        {
        case 13:
          if (menu->focus == 3)
            {
              if (strlen(menu->buffer[0]) == 0)
                {
                  menu->focus = 0;
                  menu->texterr = "Enter a user name.";
                }
              else if (arg_password && strcmp(arg_password, menu->buffer[1]))
                {
                  menu->buflen[1] = 0;
                  menu->buffer[1][0] = 0;

                  menu->focus = 1;
                  menu->texterr = "Invalid password; try again.";
                }
              else
                {
                  @<Connection |con| enters the game@>;
                  free(con->anim_data);
                  con->anim_data = NULL;
                  return;
                }
            }
          else
            do
              {
                menu->focus++;
              }
            while ((menu->focus == 1 && !arg_password) || (menu->focus == 2 && !arg_teams_count));

          break;

        case 21:
          if (menu->focus == 0 || menu->focus == 1)
            {
              menu->buflen[menu->focus]    = 0;
              menu->buffer[menu->focus][0] = 0;
            }
          break;

        case 27:
          menu->state = MENU_STATE_KEY;
          break;

        case 3:
          mat_connection_free(con, MMP_SERVER_CLOSE_CLIENT, "User quit.");
          return;

        case 127:
        case 8:
          if ((menu->focus == 0 || menu->focus == 1) && menu->buflen[menu->focus] > 0)
            {
              menu->buflen[menu->focus]--;
              menu->buffer[menu->focus][menu->buflen[menu->focus]] = 0;
            }
          break;

        default:
          if (menu->focus == 2)
            {
              switch(input)
                {
                case 'h':
                  menu->team = menu->team->prev;
                  if (!menu->team)
                    menu->team = arg_teams_tail;
                  break;
                case 'l':
                  menu->team = menu->team->next;
                  if (!menu->team)
                    menu->team = arg_teams_head;
                  break;
                }
            }
          else if (isalnum(input) && (isalpha(input) || menu->buflen[menu->focus] > 0)
                   && (menu->focus != 0 || menu->buflen[0] < MAT_USER_MAX)
                   && (menu->focus != 1 || menu->buflen[1] < MAT_PASSWORD_MAX)
                   && menu->focus != 3)
            {
              menu->buffer[menu->focus][menu->buflen[menu->focus]++] = input;
              menu->buffer[menu->focus][menu->buflen[menu->focus]] = 0;
            }
          else
            VERBOSE("Invalid character: %d\n", input);

          break;
        }
      break;

    case MENU_STATE_KEY:
      if (input != 91)
        {
          VERBOSE("%s:%d: Unknown input: %d\n", __FILE__, __LINE__, input);
          menu->state = MENU_STATE_DATA;
        }
      else
        menu->state = MENU_STATE_KEYREAL;
      break;

    case MENU_STATE_KEYREAL:
      switch(input)
        {
        case 65:
          do
            {
              menu->focus--;
            }
          while ((menu->focus == 1 && !arg_password) || (menu->focus == 2 && !arg_teams_count));
          if (menu->focus == -1)
            menu->focus = 3;
          break;

        case 66:
          do
            {
              menu->focus++;
            }
          while ((menu->focus == 1 && !arg_password) || (menu->focus == 2 && !arg_teams_count));
          if (menu->focus == 4)
            menu->focus = 0;
          break;

        case 67:
          if (menu->focus == 2)
            {
#if 0
              p->info.player.team = p->info.player.team->prev;
              if (!p->info.player.team)
                p->info.player.team = arg_teams_tail;
#endif
            }
          break;

        case 68:
          if (menu->focus == 2)
            {
#if 0
              p->info.player.team = p->info.player.team->next;
              if (!p->info.player.team)
                p->info.player.team = arg_teams_head;
#endif
            }
          break;

        default:
          VERBOSE("%s:%d: Unknown input: %d\n", __FILE__, __LINE__, input);
        }
      menu->state = MENU_STATE_DATA;
    }

  @<Main Menu: Draw input fields@>;
}

@
@<Calculate the size for the screens for the ships@>=
{
  MatMovingObj *sh;
  int size_y;
  int slot = 0;

  size_y = con->ywinsize / con->ship_count;

  ASSERT(arg_imgwidth > 0);

  for (sh = con->ship_list; sh; sh = sh->info.player.conn_list_next)
    {
      ASSERT(sh->type == MAT_MOVING_OBJ_SHIP);

      sh->info.player.win_x_s = 0;
      sh->info.player.win_x_e = con->xwinsize;
      sh->info.player.imgwi   = con->xwinsize * 2;

      sh->info.player.win_y_s = size_y * (slot + 0);
      sh->info.player.win_y_e = size_y * (slot + 1);
      sh->info.player.imghe   = 2 * (sh->info.player.win_y_e - sh->info.player.win_y_s);


      ASSERT(sh->info.player.imgwi > 0);
      ASSERT(sh->info.player.imghe > 0);

      sh->info.player.world_wi = arg_imgwidth;
      sh->info.player.world_he = GET_WORLD_HE(sh->info.player, arg_imgwidth);

      ASSERT(sh->info.player.world_wi > 0);
      ASSERT(sh->info.player.world_he > 0);

      slot ++;
    }
}

@
@<Connection |con| enters the game@>=
{
  int                    slot;
  MatUniverse           *un;

  con->graph_randomval = 60;

  ASSERT(arg_ppc > 0);

  MAT_UNIVERSES_FIND(un, 0);

  ASSERT(un);

  con->state           = MAT_STATE_PLAYING;
  con->graph_randomval = 0;
  con->anim_func       = draw_world;
  con->key_handler     = arg_ppc == 1 ? player_input_default : player_input_multiple;
  con->ship_count      = 0;

  @<Initialize the container of ships for |con|@>;

  MAT_SHIP_VIEW_CHECK(un);

  for (slot = 0; slot < arg_ppc; slot ++)
    @<Create ship for connection |con|@>@;

  un->players_real += con->ship_count;

  MAT_SHIP_VIEW_CHECK(un);

  @<Calculate the size for the screens for the ships@>;

#if 0
  YYYY
  if (p->info.player.team)
    {
      p->info.player.team_next       = p->info.player.team->head;
      p->info.player.team->head      = p;
    }
#endif

  @<Send a message informing about the new players@>;
}

@

@d GET_WORLD_HE(xpl, xwi)
  (int) ((double) xwi * CORRECTION * (double) (xpl).imghe / (double) (xpl).imgwi)

@<Create ship for connection |con|@>=
{

  MatMovingObj *s;
  s = malloc(sizeof(MatMovingObj));
  if (!s)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  s->type = MAT_MOVING_OBJ_SHIP;

  @<Initialize the messages buffer for a new ship@>;
  MAT_SHIP_VIEW_DEFAULT(s);

  s->refs = 1;

  s->img   = mat_ship;
  s->draw  = 1;

  s->un = un;

  ADD_MOVING_OBJ(s, un);

  s->rm_next = NULL;

  s->prevtype = NULL;
  s->nexttype = un->players_head;

  if (un->players_head)
    un->players_head->prevtype = s;
  else
    un->players_tail = s;

  un->players_head = s;

  strcpy(s->info.player.name, menu->buffer[0]);
  s->info.player.times_dead = 0;
  s->info.player.game_over  = 0;

  s->info.player.chat_buffer_len = 0;
  s->info.player.chat_buffer[0]  = 0;

  MAT_CONNECTION_SHIP_ADD(con, s);

  start_game(s);
}

@
@d MAT_SHIP_VIEW_DEFAULT(s)
{
  s->info.player.view = s;

  s->info.player.viewers = s;
  s->info.player.view_next = s->info.player.view_prev = NULL;
}

@
@<Initialize the messages buffer for a new ship@>=
{
  s->info.player.messages_start = 0;
  s->info.player.messages_count = 0;
  s->info.player.messages_pos = 0;
  s->info.player.messages_enabled = 1;
}

@
@d MAT_CONNECTION_SHIP_ADD(con, s)
{
  int pos;

  s->info.player.conn = con;

  pos = con->ship_count % MAT_CONNECTION_SHIP_HASH_SIZE;

  s->info.player.conn_id = con->ship_count ++;

  s->info.player.conn_hash_next = con->ship_hash[pos];
  s->info.player.conn_hash_prev = NULL;

  if (con->ship_hash[pos])
    con->ship_hash[pos]->prev = s;

  con->ship_hash[pos] = s;

  s->info.player.conn_list_next = con->ship_list;
  s->info.player.conn_list_prev = NULL;

  if (con->ship_list)
    con->ship_list->prev = s;

  con->ship_list = s;
}

@

@<Main Menu: Draw input fields@>=
{
  int p = 10;

  ASSERT(menu);

  mat_print(con, 10, p, menu->focus == 0 ? MAT_RED_BOLD : MAT_RED, "User:", 0);
  mat_print(con, 21, p, MAT_BLUE, menu->focus == 0 ? "-=> [" : "    [", 0);
  mat_print(con, 28 + MAT_USER_MAX, p, MAT_BLUE,
            menu->focus == 0 ? "] <=-" : "]    ", 0);
  mat_print(con, 27, p, MAT_NORMAL, MAT_USER_BLANK, 0);
  mat_print(con, 27, p, MAT_NORMAL, menu->buffer[0], 0);
  mat_flush(con, 10, p, 33 + MAT_USER_MAX, p + 1);

  p += 2;

  if (arg_password)
    {
      mat_print(con, 10, p, menu->focus == 1 ? MAT_RED_BOLD : MAT_RED, "Password:", 0);
      mat_print(con, 21, p, MAT_BLUE, menu->focus == 1 ? "-=> [" : "    [", 0);
      mat_print(con, 28 + MAT_PASSWORD_MAX, p, MAT_BLUE, menu->focus == 1 ? "] <=-" : "]    ", 0);
      mat_print(con, 27, p, MAT_NORMAL, MAT_PASSWORD_BLANK, 0);
      mat_print(con, 27, p, MAT_NORMAL, menu->buffer[1], 0);
      mat_flush(con, 10, p, 33 + MAT_USER_MAX, p + 1);

      p += 2;
    }

  if (arg_teams_count)
    {
#if 0
YYYYY
      ASSERT(con->ship->info.player.team);
      ASSERT(con->ship->info.player.team->name);

      mat_print(&p->info.player, 10, pos, menu->focus == 2 ? MAT_RED_BOLD : MAT_RED, "Team:");
      mat_print(&p->info.player, 21, pos, MAT_BLUE, menu->focus == 2 ? "-=> [" : "    [");
      mat_print(&p->info.player, 28 + MAT_TEAM_MAX, pos, MAT_BLUE,
                menu->focus == 2 ? "] <=-" : "]    ");
      mat_print(&p->info.player, 27, pos, MAT_NORMAL, MAT_TEAM_BLANK);
      mat_print(&p->info.player, 27, pos, MAT_NORMAL, p->info.player.team->name);
      mat_flush(p, 10, pos, 33 + MAT_TEAM_MAX, pos + 1);

      pos += 2;
#endif
    }

  if (menu->texterr)
    {
      ASSERT(strlen(menu->texterr) < MAT_TEXTERR_MAX);
      mat_print(con, 10, p, MAT_RED_BOLD, menu->texterr, 0);
      mat_flush(con, 10, p, 8 + MAT_TEXTERR_MAX, p + 1);
    }
}

@

@<Main Menu: Animate@>=
void
mat_mainmenu_anim ( MatConnection *con )
{
  static char *text[] = {
    "Welcome to Matanga... ",
    NULL
  };

  ASSERT(con);

  mat_print(con, MAX(con->xwinsize - DATAMENU(text_pos) - 1, 0),
            con->ywinsize - 1, MAT_BOLD,
            text[DATAMENU(text_cur)] + MAX(DATAMENU(text_pos) - con->xwinsize + 1, 0), 0);

  mat_flush(con, 0, con->ywinsize - 1, con->xwinsize, con->ywinsize);

  if (text[DATAMENU(text_cur)][MAX(DATAMENU(text_pos) - con->xwinsize, 0)] == 0)
    {
      DATAMENU(text_pos) = 0;
      DATAMENU(text_cur) ++;
      if (!text[DATAMENU(text_cur)])
        DATAMENU(text_cur) = 0;
    }
  else
    DATAMENU(text_pos++);
}

@ Now we check to see if connection |con| is a machine or a player and call the
appropriate macro.

@<Process and discard data for connection |con|@>=
{
  if (con->state == MAT_STATE_MACHINE)
    @<Process and discard data for machine |con|@>@;
  else
    @<Process and discard data for player |con|@>@;
}

@

@<Process and discard data for machine |con|@>=
{
  int i;

  ASSERT(con);

  for (i = con->bufprc; i < con->buflen && con->state != MAT_STATE_REMOVE; i++)
    if (con->key_handler)
      {
        /* VERBOSE("Got: %d (%c) [%d]\n", *(con->buffer + i), *(con->buffer + i), i); */
        con->key_handler(con, *(con->buffer + i));
      }

  memmove(con->buffer, con->buffer + i, con->buflen - i);
  con->buflen -= i;
  con->bufprc = 0;
}

@ The actual processing for every character in the input buffer depends on
the player's state.  We will call the player's |key_handler| for every
character on his buffer.

After this is done, we remove the bytes from the buffer.

@d MAT_KEY_DATA  0
@d MAT_KEY_IAC   1
@d MAT_KEY_WILL  2
@d MAT_KEY_WONT  3
@d MAT_KEY_DO    4
@d MAT_KEY_DONT  5
@d MAT_KEY_SB    6
@d MAT_KEY_SE    7
@d MAT_KEY_27    8
@d MAT_KEY_79    9
@d MAT_KEY_91   10

@d KEY_ARROW_UP    -10
@d KEY_ARROW_DOWN  -11
@d KEY_ARROW_RIGHT -12
@d KEY_ARROW_LEFT  -13

@d GOT_INPUT(con, c)
  if (con->key_handler)
    con->key_handler(con, c);

@<Process and discard data for player |con|@>=
{
  int i, c;

  ASSERT(con);

  for (i = con->bufprc; i < con->buflen; i++)
    {
      c = *(con->buffer + i) & 0377;
      switch (con->input_state)
        {
        case MAT_KEY_DATA:
          switch (c)
            {
            case IAC:
              con->input_state = MAT_KEY_IAC;
              break;
            case 27:
              con->input_state = MAT_KEY_27;
              break;
            case 0:
              break;
            default:
              GOT_INPUT(con, c);
            }
          break;
  
        case MAT_KEY_IAC:
          switch(c)
            {
            case WILL:
              con->input_state = MAT_KEY_WILL;
              break;
            case WONT:
              con->input_state = MAT_KEY_WONT;
              break;
            case DO:
              con->input_state = MAT_KEY_DO;
              break;
            case DONT:
              con->input_state = MAT_KEY_DONT;
              break;
            case IAC:
              break;
            case IP:
              if (con->state == MAT_STATE_PLAYING)
                MESSAGE_FROM(con, "has left for the real world.")
              mat_connection_free(con, MMP_SERVER_CLOSE_CLIENT, "User interrupt.");
              break;
            case BREAK:
              VERBOSE("Got IAC BREAK\n");
              break;
            case AYT:
              VERBOSE("Got IAC AYT\n");
              break;
            case AO:
              VERBOSE("Got IAC AO\n");
              break;
            case SB:
              con->input_state   = MAT_KEY_SB;
              con->telnet_args    = i + 1;
              break;
            default:
              VERBOSE("Ignored Input: [key:%d][state:%d]\n", c, con->input_state);
              con->input_state = MAT_KEY_DATA;
            }
          break;

        case MAT_KEY_27:
          switch (c)
            {
            case 79:
              con->input_state = MAT_KEY_79;
              break;

            case 91:
              con->input_state = MAT_KEY_91;
              break;

            default:
              con->input_state = MAT_KEY_DATA;
              VERBOSE("Ignored Input: [key:%d][state:%d]\n", c, con->input_state);
            }
          break;

        case MAT_KEY_79:
        case MAT_KEY_91:
          switch(c)
            {
            case 65:
              GOT_INPUT(con, KEY_ARROW_UP);
              break;
            case 66:
              GOT_INPUT(con, KEY_ARROW_DOWN);
              break;
            case 67:
              GOT_INPUT(con, KEY_ARROW_RIGHT);
              break;
            case 68:
              GOT_INPUT(con, KEY_ARROW_LEFT);
              break;
            default:
              VERBOSE("Ignored Input: [key:%d][state:%d]\n", c, con->input_state);
            }
          con->input_state = MAT_KEY_DATA;
          break;

        case MAT_KEY_SB:
          if (c == IAC)
            con->input_state = MAT_KEY_SE;
          break;

        case MAT_KEY_SE:
          @<Process data for player |con| in state |MAT_KEY_SE|@>;
          break;

        case MAT_KEY_WILL:
          @<Will telnet option for player |con|@>;
          con->input_state = MAT_KEY_DATA;
          break;

        case MAT_KEY_WONT:
          @<Wont telnet option for player |con|@>;
          con->input_state = MAT_KEY_DATA;
          break;

        case MAT_KEY_DO:
          @<Do telnet option for player |con|@>;
          con->input_state = MAT_KEY_DATA;
          break;

        case MAT_KEY_DONT:
          @<Dont telnet option for player |con|@>;
          con->input_state = MAT_KEY_DATA;
          break;

        default:
          ASSERT(0);
        }
    }
  
  memmove(con->buffer, con->buffer + i, con->buflen - i);
  con->buflen -= i;
  con->bufprc = 0;
}

@

@<Remove players of connection |con| from the game@>=
{
  MatMovingObj *tmp;

  while (con->ship_list)
    {
      @<Send |MMP_SERVER_DEFAULT_SHIPRM| messages to machines in this universe@>;

      con->ship_list->info.player.conn = NULL;

      tmp = con->ship_list->info.player.conn_list_next;
      mat_moving_obj_unref(con->ship_list);
      con->ship_list = tmp;
    }
}

@
@<Send |MMP_SERVER_DEFAULT_SHIPRM| messages to machines in this universe@>=
{
  MatUniverse       *un;
  MatSub            *c;
  char              *name;

  name = con->ship_list->info.player.name;
  un   = con->ship_list->un;

  for (c = un->subs; c; c = c->u_next)
    mat_printf(c->m, "%c%c%c%s%c",
               MMP_SERVER_DEFAULT_SHIPRM,
               un->id & 0xff00, un->id & 0x00ff, name, 0);
}

@ Now we will add a function to remove a player.

It doesn't actually remove it from the list because that may mess up things for
the caller.  It sets its |state| to |MAT_STATE_REMOVE| so it gets removed
eventually.

@<Functions@>=
void
mat_connection_free ( MatConnection *con, unsigned char reason, char *desc )
{
  ASSERT(con->state != MAT_STATE_REMOVE);

  if (con->state == MAT_STATE_PLAYING || con->state == MAT_STATE_MACHINE)
    @<Remove players of connection |con| from the game@>;

  if (con->state != MAT_STATE_MACHINE)
    {
      @<AALib: Destroy the context of player |con|@>;

      free(con->buffer);

      mat_printf(con,
                "You have been disconnected from the server.\r\nReason: %s\r\n\r\n"
                "Thanks for playing Matanza.\r\n"
                "FU Matanza %s\r\n%s\r\n",
                desc, VERSION, "Copyright 2000 Alejandro Forero Cuervo");
    }
  else
    {
      mat_sub_machine_rm(con);
      mat_printf(con, "%c%c", MMP_SERVER_CLOSE, reason);
      if (reason == MMP_SERVER_CLOSE_SYSERROR || reason == MMP_SERVER_CLOSE_MMPERROR)
        mat_printf(con, "%s%c", desc, 0);
    }

  mat_out_flush(con);

  con->state = MAT_STATE_REMOVE;

  free(con->addr);
  free(con->out_buf);
  free(con->anim_data);
  SOCKET_CLOSE_GENERIC(con->fd);
}

@
@<Function prototypes@>=
void mat_connection_free ( MatConnection *, unsigned char, char * );

@
@<Parse terminal speed@>=
{
  int xspeed, rspeed;

  //if (his_state_is_wont(TELOPT_TSPEED))
  //  break;

  ASSERT(con->buflen > con->telnet_args);

  if ((con->buffer[con->telnet_args++] & 0xff) != TELQUAL_IS)
    ASSERT(0); /* TODO: Just kill this player. */

  xspeed = atoi(con->buffer + con->telnet_args);

  while (con->buffer[con->telnet_args] != ',' && con->buflen > con->telnet_args)
    con->telnet_args++;

  ASSERT(con->buflen > con->telnet_args);

  rspeed = atoi(con->buffer + con->telnet_args);

  VERBOSE("Ignoring TSPEED: %d %d\n", xspeed, rspeed);
}

@
@<Parse terminal type@>=
{
  int j;

  //if (his_state_is_wont(TELOPT_TTYPE))        /* Ignore if option disabled */
  //  break;

  ASSERT(con->buflen > con->telnet_args);

  if ((con->buffer[con->telnet_args++] & 0xff) != TELQUAL_IS)
    ASSERT(1); /* TODO: Just kill this player. */

  for (j = 0; j < 14 && con->telnet_args + 1 < i; j ++, con->telnet_args++)
    con->terminal_name[j] = tolower(con->buffer[con->telnet_args]);

  con->terminal_name[j] = 0;

  VERBOSE("Got terminal name: %s\n", con->terminal_name);
}

@

@<Parse line mode@>=
{
  ASSERT(con->buflen > con->telnet_args);
  con->telnet_args++;
  ASSERT(con->buflen > con->telnet_args);

  if ((con->buffer[con->telnet_args] & 0xff) == LM_SLC)
    {
      VERBOSE("Disabling TELOPT_LINEMODE for player.\n");
      mat_printf(con, "%c%c%c%c%c%c", IAC, SB, TELOPT_LINEMODE, LM_SLC, IAC, SE);
    }
  else if (con->buffer[con->telnet_args] == LM_MODE)
    {
      VERBOSE("Use edit mode: %d\n", con->buffer[con->telnet_args++]);
    }
  else
    {
      ASSERT(con->buflen > con->telnet_args);

      VERBOSE("Telnet: Invalid char received in TELOPT_LINEMODE: %s:%d\n", __FILE__, __LINE__);
    }
}

@
@<Parse window size@>=
{
  int width, height;

  //if (his_state_is_wont(TELOPT_NAWS)) /* Ignore if option disabled */
  //  break;

  ASSERT(con->buflen > con->telnet_args);
  width = (con->buffer[con->telnet_args++] & 0xff) << 8;
  ASSERT(con->buflen > con->telnet_args);
  width |= (con->buffer[con->telnet_args++] & 0xff);
  ASSERT(con->buflen > con->telnet_args);
  height = (con->buffer[con->telnet_args++] & 0xff) << 8;
  ASSERT(con->buflen > con->telnet_args);
  height |= (con->buffer[con->telnet_args++] & 0xff);

  VERBOSE("Got window size: %d x %d\n", width, height);

  if (width > 0 && height > 0)
    mat_graph_resize(con, width, height);
}

@
@<Process data for player |con| in state |MAT_KEY_SE|@>=
  if (c != SE)
    {
      ASSERT(c == IAC);
      con->input_state = MAT_KEY_IAC;
    }
  else
    {
      switch (con->buffer[con->telnet_args++])
        {
        case TELOPT_TSPEED:
          @<Parse terminal speed@>;
          break;

        case TELOPT_TTYPE:
          @<Parse terminal type@>;
          break;

        case TELOPT_NAWS:
          @<Parse window size@>;
          break;

        case TELOPT_LINEMODE:
          @<Parse line mode@>;
          break;

        default:
          VERBOSE("Unknown telnet parameter: %d\n", con->buffer[con->telnet_args - 1]);
          break;
        }
      con->input_state = MAT_KEY_DATA;
      check_telnet_negotiation(con);
   }

@

@<Dont telnet option for player |con|@>=
{
  VERBOSE("Telnet: Got DONT %d\n", c);
  @<Send wont ...@>;
}

@

@<Do telnet option for player |con|@>=
{
  ASSERT(con);

  switch (c)
    {
    case TELOPT_LOGOUT:
      @<Send will ...@>;
      if (con->state == MAT_STATE_PLAYING)
        MESSAGE_FROM(con, "has left for the real world");
      mat_connection_free(con, MMP_SERVER_CLOSE_CLIENT, "User logout.");
      break;
    case TELOPT_ECHO:
      VERBOSE("Telnet: Got DO ECHO\n");
      break;
    case TELOPT_SGA:
      VERBOSE("Telnet: Got DO SGA\n");
      @<Send will ...@>;
      break;
    default:
      VERBOSE("Telnet: Got DO %d\n", c);
      @<Send wont ...@>;
    }
}

@

@<Will telnet option for player |con|@>=
{
  ASSERT(con);

  switch (c)
    {
    case TELOPT_TSPEED:
    case TELOPT_TTYPE:
    case TELOPT_NAWS:
      mat_printf(con, "%c%c%c%c%c%c", IAC, SB, c, TELQUAL_SEND, IAC, SE);
      mat_out_flush(con);
      break;
    case TELOPT_LINEMODE:
      VERBOSE("Telnet: Player TELOPT_LINEMODE\n");
      break;
    default:
      VERBOSE("Telnet: Got WILL %d\n", c);
      @<Send dont ...@>;
    }
}

@

@<Wont telnet option for player |con|@>=
{
  ASSERT(con);

  switch (c)
    {
    case TELOPT_TTYPE:
      strcpy(con->terminal_name, "dumb");
      check_telnet_negotiation(con);
      break;
    case TELOPT_NAWS:
      mat_graph_resize(con, 80, 24);
      check_telnet_negotiation(con);
      break;
    case TELOPT_ECHO:
      VERBOSE("Telnet: Client won't echo\n");
      @<Send wont ...@>;
      break;
    case TELOPT_LINEMODE:
      VERBOSE("Telnet: Client won't do linemode (may do buffering)\n");
      break;
    default:
      @<Send dont ...@>;
      VERBOSE("Telnet: Got WONT %d\n", c);
    }
}

@
@<Send will |option| to player |con|@>=
{
  ASSERT(con);

  mat_printf(con, "%c%c%c", IAC, WILL, c);
  mat_out_flush(con);
}

@
@<Send wont |option| to player |con|@>=
{
  ASSERT(con);

  mat_printf(con, "%c%c%c", IAC, WONT, c);
  mat_out_flush(con);
}

@
@<Send dont |option| to player |con|@>=
{
  ASSERT(con);

  mat_printf(con, "%c%c%c", IAC, DONT, c);
  mat_out_flush(con);
}

@
@<Send do |option| to player |con|@>=
{
  ASSERT(con);

  mat_printf(con, "%c%c%c", IAC, DO, c);
  mat_out_flush(con);
}

@ The following values are used to generate the palette used to draw fire.

@<Global variables@>=
static int fire_pal[] =
{
  0, 0, 0, 0, 0, 6, 0, 0, 6, 0, 0, 7, 0, 0, 8, 0, 0, 8, 0, 0, 9, 0, 0, 10,
  2, 0, 10, 4, 0, 9, 6, 0, 9, 8, 0, 8, 10, 0, 7, 12, 0, 7, 14, 0, 6, 16, 0, 5,
  18, 0, 5, 20, 0, 4, 22, 0, 4, 24, 0, 3, 26, 0, 2, 28, 0, 2, 30, 0, 1, 32, 0, 0,
  32, 0, 0, 33, 0, 0, 34, 0, 0, 35, 0, 0, 36, 0, 0, 36, 0, 0, 37, 0, 0, 38, 0, 0,
  39, 0, 0, 40, 0, 0, 40, 0, 0, 41, 0, 0, 42, 0, 0, 43, 0, 0, 44, 0, 0, 45, 0, 0,
  46, 1, 0, 47, 1, 0, 48, 2, 0, 49, 2, 0, 50, 3, 0, 51, 3, 0, 52, 4, 0, 53, 4, 0,
  54, 5, 0, 55, 5, 0, 56, 6, 0, 57, 6, 0, 58, 7, 0, 59, 7, 0, 60, 8, 0, 61, 8, 0,
  63, 9, 0, 63, 9, 0, 63, 10, 0, 63, 10, 0, 63, 11, 0, 63, 11, 0, 63, 12, 0, 63, 12, 0,
  63, 13, 0, 63, 13, 0, 63, 14, 0, 63, 14, 0, 63, 15, 0, 63, 15, 0, 63, 16, 0, 63, 16, 0,
  63, 17, 0, 63, 17, 0, 63, 18, 0, 63, 18, 0, 63, 19, 0, 63, 19, 0, 63, 20, 0, 63, 20, 0,
  63, 21, 0, 63, 21, 0, 63, 22, 0, 63, 22, 0, 63, 23, 0, 63, 24, 0, 63, 24, 0, 63, 25, 0,
  63, 25, 0, 63, 26, 0, 63, 26, 0, 63, 27, 0, 63, 27, 0, 63, 28, 0, 63, 28, 0, 63, 29, 0,
  63, 29, 0, 63, 30, 0, 63, 30, 0, 63, 31, 0, 63, 31, 0, 63, 32, 0, 63, 32, 0, 63, 33, 0,
  63, 33, 0, 63, 34, 0, 63, 34, 0, 63, 35, 0, 63, 35, 0, 63, 36, 0, 63, 36, 0, 63, 37, 0,
  63, 38, 0, 63, 38, 0, 63, 39, 0, 63, 39, 0, 63, 40, 0, 63, 40, 0, 63, 41, 0, 63, 41, 0,
  63, 42, 0, 63, 42, 0, 63, 43, 0, 63, 43, 0, 63, 44, 0, 63, 44, 0, 63, 45, 0, 63, 45, 0,
  63, 46, 0, 63, 46, 0, 63, 47, 0, 63, 47, 0, 63, 48, 0, 63, 48, 0, 63, 49, 0, 63, 49, 0,
  63, 50, 0, 63, 50, 0, 63, 51, 0, 63, 52, 0, 63, 52, 0, 63, 52, 0, 63, 52, 0, 63, 52, 0,
  63, 53, 0, 63, 53, 0, 63, 53, 0, 63, 53, 0, 63, 54, 0, 63, 54, 0, 63, 54, 0, 63, 54, 0,
  63, 54, 0, 63, 55, 0, 63, 55, 0, 63, 55, 0, 63, 55, 0, 63, 56, 0, 63, 56, 0, 63, 56, 0,
  63, 56, 0, 63, 57, 0, 63, 57, 0, 63, 57, 0, 63, 57, 0, 63, 57, 0, 63, 58, 0, 63, 58, 0,
  63, 58, 0, 63, 58, 0, 63, 59, 0, 63, 59, 0, 63, 59, 0, 63, 59, 0, 63, 60, 0, 63, 60, 0,
  63, 60, 0, 63, 60, 0, 63, 60, 0, 63, 61, 0, 63, 61, 0, 63, 61, 0, 63, 61, 0, 63, 62, 0,
  63, 62, 0, 63, 62, 0, 63, 62, 0, 63, 63, 0, 63, 63, 1, 63, 63, 2, 63, 63, 3, 63, 63, 4,
  63, 63, 5, 63, 63, 6, 63, 63, 7, 63, 63, 8, 63, 63, 9, 63, 63, 10, 63, 63, 10, 63, 63, 11,
  63, 63, 12, 63, 63, 13, 63, 63, 14, 63, 63, 15, 63, 63, 16, 63, 63, 17, 63, 63, 18, 63, 63, 19,
  63, 63, 20, 63, 63, 21, 63, 63, 21, 63, 63, 22, 63, 63, 23, 63, 63, 24, 63, 63, 25, 63, 63, 26,
  63, 63, 27, 63, 63, 28, 63, 63, 29, 63, 63, 30, 63, 63, 31, 63, 63, 31, 63, 63, 32, 63, 63, 33,
  63, 63, 34, 63, 63, 35, 63, 63, 36, 63, 63, 37, 63, 63, 38, 63, 63, 39, 63, 63, 40, 63, 63, 41,
  63, 63, 42, 63, 63, 42, 63, 63, 43, 63, 63, 44, 63, 63, 45, 63, 63, 46, 63, 63, 47, 63, 63, 48,
  63, 63, 49, 63, 63, 50, 63, 63, 51, 63, 63, 52, 63, 63, 52, 63, 63, 53, 63, 63, 54, 63, 63, 55,
  63, 63, 56, 63, 63, 57, 63, 63, 58, 63, 63, 59, 63, 63, 60, 63, 63, 61, 63, 63, 62, 63, 63, 63
};

int fire_palette[256];

@ Now some mechanisms for assertions.

The function assert_exit is used so you can have your debugger stop execution
when an assertion fails.

@<Definitions@>=

#if MATANZA_ASSERT
#define ASSERT(x) \
  if(!(x)) \
    fprintf(stderr, "%s:%d: ASSERT failed\n", __FILE__, __LINE__), assert_exit()

#define ASSERT_FOR(x) for(x)
#else
#define ASSERT(x)
#define ASSERT_FOR(x)
#endif

@
@<Functions@>=
#if MATANZA_ASSERT
void assert_exit ( void )
{
  exit(-1);
}
#endif

@ To get rid of the current verbosity, define VERBOSE to empty.  Matanza might
then throw a lot of warnings during compilation, but will work.

@<Global variables@>=
#if MATANZA_VERBOSE
#define VERBOSE(...) if (arg_verbose) printf(__VA_ARGS__)
int arg_verbose = 0;
#else
#define VERBOSE(...)
#endif

@
@<Function prototypes@>=
void assert_exit ( void );

@ Lets include all the files we can.  We should check this and make sure it
really is what we want.

@<Include files@>=

#include <stdarg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <math.h>
#include <limits.h>

/* Headers for Windows */

#if HAVE_WINSOCK2_H
#include <winsock2.h>
#endif

#if HAVE_WINDOWS_H
#include <windows.h>
#endif

#if HAVE_DIRECT_H
#include <direct.h>
#endif

/* Headers for BeOS */

#if HAVE_OS_H
#include <OS.h>
#endif

#if HAVE_ERRORS_H
#include <Errors.h>
#endif

/* Headers for Unix */

#if HAVE_NETDB_H
#include <netdb.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#if HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_ARPA_TELNET_H
#include <arpa/telnet.h>
#else
@<Telnet definitions@>@;
#endif

/* The following files are bundled with Matanza.  If you get errors with
 * the following lines, make sure you have the directory where your Matanza
 * source code resides as part of your standard includes (for gcc, this is
 * specified using the parameter -I). */

#include <xmlparse.h>
#include <getopt.h>
#include <shipreal.c>
#include <mmp.h>

@
@<Telnet definitions@>=
#define	SE	240		/* end sub negotiation */
#define	BREAK	243		/* break */
#define	IP	244		/* interrupt process--permanently */
#define	AO	245		/* abort output--but let prog finish */
#define	AYT	246		/* are you there */
#define	SB	250		/* interpret as subnegotiation */
#define	WILL	251		/* I will use option */
#define	WONT	252		/* I won't use option */
#define	DO	253		/* please, you use option */
#define	DONT	254		/* you are not to use option */
#define	IAC	255		/* interpret as command: */

#define TELOPT_ECHO	1	/* echo */
#define	TELOPT_SGA	3	/* suppress go ahead */
#define	TELOPT_LOGOUT	18	/* force logout */
#define	TELOPT_TTYPE	24	/* terminal type */
#define	TELOPT_NAWS	31	/* window size */
#define	TELOPT_TSPEED	32	/* terminal speed */
#define TELOPT_LINEMODE	34	/* Linemode option */

#define	LM_MODE		1
#define	LM_SLC		3

#define	TELQUAL_IS	0	/* option is... */
#define	TELQUAL_SEND	1	/* send option */

@

@<Data for an image@>=
struct MatImage
{
  int            h, w;
  unsigned char *img; /* Color (0 = black, 255 = white) */
};

@ And we also need one struct to keep count of the bullets.

|angle_x| and |angle_y| are always greater or equal than zero, and |dir_x| and
|dir_y| are used to know if the bullet is headed left or right and up or down.

If |img| is |NULL|, the image is a point.

@<Data for bullets@>=

typedef struct MatBullet MatBullet;

struct MatBullet
{
  int           expires;
  int           bright;

  MatMovingObj *src;

  int           type;
};

@

@d MAP_EMPTY;

@<Global variables@>=

MatImage        *mat_ship;
unsigned char   *mat_ship_palette[3];

MatImage *mat_ast              [2];
MatImage *mat_missile          [3];

@

@<Global variables@>=

int       mat_bullets_damage   [4] = { 15, 100, 400, 3000 };

double    mat_bullets_speed    [4] = {
   8.0 / STEPS_MOVEMENT,
  11.0 / STEPS_MOVEMENT,
  14.5 / STEPS_MOVEMENT,
   8.0 / STEPS_MOVEMENT
};

int       mat_bullets_back     [4] = {  1,   0,   0,    0 };
int       mat_bullets_bexp     [4] = { 20,  40, 200, 3000 };
int       mat_bullets_pexp     [4] = {  1,   3,   6,   10 };

char     *mat_bullets_name     [4] = { "bullets", "small missiles", "missiles", "torpedoes" };

char     *mat_bullets_hit      [4] = {
  "-=> You got %s !",
  "-=> Your small missile got %s !",
  "-=> Bang, your missile got %s !",
  "-=> BOOM, your torpedo got %s !"
};

char     *mat_bullets_hit_ast  [4] = {
  "-=> You reached the asteroid !",
  "-=> Your small missile reached the asteroid !",
  "-=> Bang, your missile hit the asteroid !",
  "-=> BOOM, your torpedo got the asteroid !"
};

@
@<Free the images for the asteroids@>=
{
  int i;

  if (arg_ast_num > 0)
    {
      for (i = 0; i < arg_ang; i ++)
        {
          free(mat_ast[0][i].img);
          free(mat_ast[1][i].img);
        }

      free(mat_ast[0]);
      free(mat_ast[1]);
    }
}

@

@d POS_AST(j,i)
  (int)(j + MAT_AST_SIZE_Y / 2) * MAT_AST_SIZE_X + (int)(i + MAT_AST_SIZE_X / 2)

@<Rotate the asteroids in the different angles@>=
{
  int i, j, ang;
  double x, y, c, s;

  if (arg_ast_num > 0)
    {
      VERBOSE("Calculating rotations of asteroids... ");
      fflush(stdout);

      mat_ast[0] = malloc(sizeof(MatImage) * arg_ang);
      mat_ast[1] = malloc(sizeof(MatImage) * arg_ang);
      if (!mat_ast[0] || !mat_ast[1])
        {
          fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
          exit(EXIT_FAILURE);
        }

      for (ang = 0; ang < arg_ang; ang ++)
        {
          c = mat_cos[ang];
          s = mat_sin[ang];

          mat_ast[0][ang].w = MAT_AST_SIZE_X;
          mat_ast[1][ang].w = MAT_AST_SIZE_X;

          mat_ast[0][ang].h = MAT_AST_SIZE_Y;
          mat_ast[1][ang].h = MAT_AST_SIZE_Y;

          mat_ast[0][ang].img = malloc(MAT_AST_SIZE_X * MAT_AST_SIZE_Y);
          mat_ast[1][ang].img = malloc(MAT_AST_SIZE_X * MAT_AST_SIZE_Y);
          if (!mat_ast[0][ang].img || !mat_ast[1][ang].img)
            {
              fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
              exit(EXIT_FAILURE);
            }

          for (j = -MAT_AST_SIZE_Y / 2; j * 2 < MAT_AST_SIZE_Y; j ++)
            for (i = -MAT_AST_SIZE_X / 2; i * 2 < MAT_AST_SIZE_X; i ++)
              {
                x = i * c - j * s;
                y = i * s + j * c;

                if (    x < -MAT_AST_SIZE_X / 2 || x > MAT_AST_SIZE_X / 2
                     || y < -MAT_AST_SIZE_Y / 2 || y > MAT_AST_SIZE_Y / 2 )
                  {
                    mat_ast[0][ang].img[POS_AST(j,i)] = ' ';
                    mat_ast[1][ang].img[POS_AST(j,i)] = ' ';
                  }
                else
                  {
                    mat_ast[0][ang].img[POS_AST(j,i)] = mat_graph_ast[0][POS_AST(y,x)];
                    mat_ast[1][ang].img[POS_AST(j,i)] = mat_graph_ast[1][POS_AST(y,x)];
                  }
              }
        }
      VERBOSE("Done\nOptimizing images for the asteroids... [%dx%d] [%dx%d]",
              mat_ast[0][0].h, mat_ast[0][0].w, mat_ast[1][0].h, mat_ast[1][0].w);
      fflush(stdout);

      mat_image_optimize(mat_ast[0]);
      mat_image_optimize(mat_ast[1]);

      VERBOSE(" -> [%dx%d] [%dx%d]\n",
              mat_ast[0][0].h, mat_ast[0][0].w, mat_ast[1][0].h, mat_ast[1][0].w);
    }
}

@
@<Free the images for the missiles@>=
{
  int i;

  if (arg_ast_num > 0)
    {
      for (i = 0; i < arg_ang; i ++)
        {
          free(mat_missile[0][i].img);
          free(mat_missile[1][i].img);
          free(mat_missile[2][i].img);
        }

      free(mat_missile[0]);
      free(mat_missile[1]);
      free(mat_missile[2]);
    }
}
@

@d POS_MISSILE(j,i)
  (int)(j + MAT_MISSILESIZE_Y / 2) * MAT_MISSILESIZE_X + (int)(i + MAT_MISSILESIZE_X / 2)

@<Rotate the missiles in the different angles@>=
{
  int i, j, ang;
  double x, y, c, s;

  VERBOSE("Calculating rotations of missiles... ");
  fflush(stdout);

  mat_missile[0] = malloc(sizeof(MatImage) * arg_ang);
  mat_missile[1] = malloc(sizeof(MatImage) * arg_ang);
  mat_missile[2] = malloc(sizeof(MatImage) * arg_ang);
  if (!mat_missile[0] || !mat_missile[1] || !mat_missile[2])
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  for (ang = 0; ang < arg_ang; ang ++)
    {
      c = mat_cos[ang];
      s = mat_sin[ang];

      mat_missile[0][ang].w = MAT_MISSILESIZE_X;
      mat_missile[1][ang].w = MAT_MISSILESIZE_X;
      mat_missile[2][ang].w = MAT_MISSILESIZE_X;

      mat_missile[0][ang].h = MAT_MISSILESIZE_Y;
      mat_missile[1][ang].h = MAT_MISSILESIZE_Y;
      mat_missile[2][ang].h = MAT_MISSILESIZE_Y;

      mat_missile[0][ang].img = malloc(MAT_MISSILESIZE_X * MAT_MISSILESIZE_Y);
      mat_missile[1][ang].img = malloc(MAT_MISSILESIZE_X * MAT_MISSILESIZE_Y);
      mat_missile[2][ang].img = malloc(MAT_MISSILESIZE_X * MAT_MISSILESIZE_Y);
      if (!mat_missile[0][ang].img || !mat_missile[1][ang].img || !mat_missile[2][ang].img)
        {
          fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
          exit(EXIT_FAILURE);
        }

      for (j = -MAT_MISSILESIZE_Y / 2; j * 2 < MAT_MISSILESIZE_Y; j ++)
        for (i = -MAT_MISSILESIZE_X / 2; i * 2 < MAT_MISSILESIZE_X; i ++)
          {
            x = i * c - j * s;
            y = i * s + j * c;

            if (    x < -MAT_MISSILESIZE_X / 2 || x > MAT_MISSILESIZE_X / 2
                 || y < -MAT_MISSILESIZE_Y / 2 || y > MAT_MISSILESIZE_Y / 2 )
              {
                mat_missile[0][ang].img[POS_MISSILE(j,i)] = ' ';
                mat_missile[1][ang].img[POS_MISSILE(j,i)] = ' ';
                mat_missile[2][ang].img[POS_MISSILE(j,i)] = ' ';
              }
            else
              {
                mat_missile[0][ang].img[POS_MISSILE(j,i)] = mat_graph_missile[0][POS_MISSILE(y,x)];
                mat_missile[1][ang].img[POS_MISSILE(j,i)] = mat_graph_missile[1][POS_MISSILE(y,x)];
                mat_missile[2][ang].img[POS_MISSILE(j,i)] = mat_graph_missile[2][POS_MISSILE(y,x)];
              }
          }
    }
  VERBOSE("Done\nOptimizing images for [%dx%d] [%dx%d] [%dx%d]",
          mat_missile[0][0].h, mat_missile[0][0].w,
          mat_missile[1][0].h, mat_missile[1][0].w,
          mat_missile[2][0].h, mat_missile[2][0].w);
  fflush(stdout);

  mat_image_optimize(mat_missile[0]);
  mat_image_optimize(mat_missile[1]);
  mat_image_optimize(mat_missile[2]);

  VERBOSE(" -> [%dx%d] [%dx%d] [%dx%d]\n",
          mat_missile[0][0].h, mat_missile[0][0].w,
          mat_missile[1][0].h, mat_missile[1][0].w,
          mat_missile[2][0].h, mat_missile[2][0].w);
}

@ Here we rotate the matrix for the ships.

@d ANGLE_SPEED_MAX 1
@d PI 3.141592

@d POS_SHIP(j,i)
  (int)(j + size / 2) * size + (int)(i + size / 2)

@d POS_SHIP_ORIGINAL(j,i)
  (int)(j + MAT_SHIPSIZE_Y / 2) * MAT_SHIPSIZE_X + (int)(i + MAT_SHIPSIZE_X / 2)

@<Rotate the ship in the different angles@>=
{
  int i, j, ang, size;

  VERBOSE("Calculating rotations of ships... ");
  fflush(stdout);

  mat_ship = malloc(sizeof(MatImage) * arg_ang);
  if (!mat_ship)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  size = (int) sqrt(MAT_SHIPSIZE_X * MAT_SHIPSIZE_X + MAT_SHIPSIZE_Y * MAT_SHIPSIZE_Y) + 1;

  for (ang = 0; ang < arg_ang; ang ++)
    {
      double c, s;

      c = mat_cos[ang];
      s = mat_sin[ang];

      mat_ship[ang].w = size;
      mat_ship[ang].h = size;

      mat_ship[ang].img = malloc(mat_ship[ang].w * mat_ship[ang].h);
      if (!mat_ship[ang].img)
        {
          fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
          exit(EXIT_FAILURE);
        }

      for (j = - size / 2; j < size / 2; j ++)
        for (i = - size / 2; i < size / 2; i ++)
          {
            double x, y;

            x = i * c - j * s;
            y = i * s + j * c;

            ASSERT(0 <= POS_SHIP(j, i));
            ASSERT(POS_SHIP(j, i) < mat_ship[ang].w * mat_ship[ang].h);

            if (    (int) x < -MAT_SHIPSIZE_X / 2 || (int) x >= MAT_SHIPSIZE_X / 2
                 || (int) y < -MAT_SHIPSIZE_Y / 2 || (int) y >= MAT_SHIPSIZE_Y / 2 )
              {
                mat_ship[ang].img[POS_SHIP(j, i)] = ' ';
              }
            else
              {
                ASSERT((int) y + MAT_SHIPSIZE_Y / 2 < MAT_SHIPSIZE_Y);
                ASSERT((int) x + MAT_SHIPSIZE_X / 2 < MAT_SHIPSIZE_X);
                ASSERT(0 <= POS_SHIP_ORIGINAL(y, x));
                ASSERT(POS_SHIP_ORIGINAL(y, x) < MAT_SHIPSIZE_X * MAT_SHIPSIZE_Y);
                mat_ship[ang].img[POS_SHIP(j, i)] = arg_ship_graphic[POS_SHIP_ORIGINAL(y,x)];
              }
          }
    }
  VERBOSE("Done\nOptimizing images for the ships... [%dx%d]",
          mat_ship[0].h, mat_ship[0].w);
  fflush(stdout);

  mat_image_optimize(mat_ship);

  VERBOSE(" -> [%dx%d]\n", mat_ship[0].h, mat_ship[0].w);
}

@
@<Free the images for the ships@>=
{
  int i;

  if (arg_ast_num > 0)
    {
      for (i = 0; i < arg_ang; i ++)
        {
          free(mat_ship[i].img);
        }

      free(mat_ship);
    }
}

@ A function to set up a player in the game.

This is called whenever he is about to begin a new life.

@<Functions@>=
void
start_game ( MatMovingObj *ship )
{
  ASSERT(ship);
  ASSERT(ship->type == MAT_MOVING_OBJ_SHIP);
  ASSERT(ship->un);

  ship->crash     = 0;
  ship->alpha     = 0.0;
  ship->alpha_mod = mat_alpha_mod_ship_appear;

  @<Set the startup location for ship |ship|@>;
  @<Set the initial number of bullets for ship |ship|@>;

  ship->info.player.visible        = 1;
  ship->info.player.visible_exp    = arg_visible_init;
  ship->info.player.health         = ship->un->health;
  ship->info.player.ship_speed_inc = 0;
  ship->pal                        = mat_ship_palette[2];
}

@
@<Set the initial number of bullets for ship |ship|@>=
{
  ship->info.player.ship_bullets[0]  = 20000;
  ship->info.player.ship_bullets[1]  = 7;
  ship->info.player.ship_bullets[2]  = 2;
  ship->info.player.ship_bullets[3]  = 0;

  ship->info.player.bullets_expire  = 0;
}

@

@d MESSAGE_TO(con, msg)
{
  MatMovingObj        *sh;

  for (sh = con->ship_list; sh; sh = sh->info.player.conn_list_next)
    sprintf(message_get(sh), msg);
}

@

@<Send a message informing about the new players@>=
{
  MatMovingObj       *sh;

  ASSERT(con->state == MAT_STATE_PLAYING);

  for (sh = con->ship_list; sh; sh = sh->info.player.conn_list_next)
    {
      ASSERT(sh->type == MAT_MOVING_OBJ_SHIP);
      ASSERT(sh->un->players_real >= 1);

      if (sh->un->players_real == 1)
        sprintf(message_get(sh), "-=> You are all alone . . .");
      else
        {
          MatSub *i;

          MESSAGE_FROM_IP(con, "joins the game");

          sprintf(message_get(sh),
                  "-=> There are %d ships in this universe.",
                  sh->un->players_real);

          for (i = sh->un->subs; i; i = i->u_next)
            mat_printf(i->m, "%c%c%c%s%c", MMP_SERVER_DEFAULT_SHIPADD_OTHER,
                       0, 0, sh->info.player.name, 0);
        }

      if (un->pause)
        MESSAGE_TO(con, "-=> Time is frozen . . .");
    }
}

@

@<Functions@>=
void
player_input_buffer ( MatConnection *con, int key )
{
  ASSERT(con);
  ASSERT(con->state == MAT_STATE_PLAYING);
  ASSERT(con->ship_count == 1);

  switch(key)
    {
    case 13:
      ship_key_chat_end(con);
      break;

    case 21:
      ship_key_chat_wipe(con->ship_list);
      break;

    case 127:
    case 8:
      ship_key_chat_backspace(con->ship_list);
      break;

    default:
      ship_key_chat_add(con->ship_list, key);
      break;
    }
}

@ Now a function for a very simple state. When the player hits `q', he enters
the `quit' state that this function handles... Basically, he gets asked for
confirmation.  If he enters `y' or `q', he quits.  Otherwise, he is allowed to
continue playing.

@<Functions@>=
void
player_input_quit ( MatConnection *con, int key )
{
  ASSERT(con->state == MAT_STATE_PLAYING);

  switch (tolower(key))
    {
    case 'q':
    case 'y':
      ship_key_quit(con, 1);
      break;
    default:
      ship_key_quit_cancel(con);
      break;
    }
}

@ In the future, this function should handle more than two players per
connection.  Right?

@d GET_SHIP(con, id)
  con->ship_hash[id]

@<Functions@>=
void
player_input_multiple ( MatConnection *con, int key )
{
  ASSERT(con);

  ASSERT(con->ship_count == 2);

  ASSERT(con->ship_hash[0]);
  ASSERT(con->ship_hash[0]->type == MAT_MOVING_OBJ_SHIP);
  ASSERT(con->ship_hash[0]->info.player.conn_id == 0);
  ASSERT(con->ship_hash[0]->refs >= 0);

  ASSERT(con->ship_hash[1]);
  ASSERT(con->ship_hash[1]->type == MAT_MOVING_OBJ_SHIP);
  ASSERT(con->ship_hash[1]->info.player.conn_id == 1);
  ASSERT(con->ship_hash[1]->refs >= 0);

  ASSERT(con->ship_hash[0]->un == con->ship_hash[1]->un);

  switch (key)
    {
    case KEY_ARROW_UP:
      ship_key_up(con->ship_hash[0]);
      break;
    case KEY_ARROW_DOWN:
      ship_key_down(con->ship_hash[0]);
      break;
    case KEY_ARROW_RIGHT:
      ship_key_right(con->ship_hash[0]);
      break;
    case KEY_ARROW_LEFT:
      ship_key_left(con->ship_hash[0]);
      break;
    case '1':
      mat_bullet_new_player(con->ship_hash[0], 0, 1);
      break;
    case '2':
      mat_bullet_new_player(con->ship_hash[0], 1, 1);
      break;
    case '3':
      mat_bullet_new_player(con->ship_hash[0], 2, 1);
      break;
    case 'k':
      ship_key_up(con->ship_hash[1]);
      break;
    case 'j':
      ship_key_down(con->ship_hash[1]);
      break;
    case 'l':
      ship_key_right(con->ship_hash[1]);
      break;
    case 'h':
      ship_key_left(con->ship_hash[1]);
      break;
    case ' ':
      mat_bullet_new_player(con->ship_hash[1], 0, 1);
      break;
    case 'n':
      mat_bullet_new_player(con->ship_hash[1], 1, 1);
      break;
    case 'm':
      mat_bullet_new_player(con->ship_hash[1], 2, 1);
      break;
    case 'z':
      ship_key_stats(con->ship_hash[1]);
      break;
    case '0':
      ship_key_stats(con->ship_hash[0]);
      break;
    case 'p':
      ship_key_pause(con->ship_hash[0]);
      break;
    case '-':
      ship_key_zoom_out(con->ship_hash[1]);
      break;
    case '=':
      ship_key_zoom_in(con->ship_hash[1]);
      break;
    case '/':
      ship_key_zoom_out(con->ship_hash[0]);
      break;
    case '*':
      ship_key_zoom_in(con->ship_hash[0]);
      break;
    }
}

@ The following function handles the initialization for a machine.

@<Functions@>=
void
machine_input_initial ( MatConnection *m, int key )
{
  switch (key)
    {
    case MMP_CLIENT_INITIAL_ID:
      @<Receive identification from client@>;
      break;

    default:
      @<Handle unknown command from machine |m|@>;
      break;
    }
}

@
@<Receive identification from client@>=
{
  ASSERT(m);

  m->key_handler = machine_input_string;

  m->info.m.string_maxlength = 256;
  m->info.m.string           = NULL;
  m->info.m.string_len       = 0;
  m->info.m.string_siz       = 0;

  m->info.m.handler          = machine_input_arg_id_name;
}

@
@<Functions@>=
void
machine_input_arg_id_name ( MatConnection *m )
{
  VERBOSE("[Machine:%s]\n", m->info.m.string);

  free(m->info.m.string);

  m->key_handler = machine_input_string;

  m->info.m.string_maxlength = 256;
  m->info.m.string           = NULL;
  m->info.m.string_len       = 0;
  m->info.m.string_siz       = 0;

  m->info.m.handler          = machine_input_arg_id_ver;
}

@
@<Functions@>=
void
machine_input_arg_id_ver ( MatConnection *m )
{
  VERBOSE("[Version:%s]\n", m->info.m.string);

  free(m->info.m.string);

  mat_printf(m, "%c%s%c%s%c", MMP_SERVER_INITIAL_CONNECTED, PACKAGE, 0, VERSION, 0);
  mat_out_flush(m);

  m->key_handler = machine_input_default;
}

@
@<Functions@>=
void
machine_input_default ( MatConnection *m, int key )
{
  switch (key)
    {
    case MMP_CLIENT_DEFAULT_JOIN:
      VERBOSE("Command: join\n");
      @<Allow machine |m| to join to a universe@>;
      break;

    case MMP_CLIENT_DEFAULT_SHIPADD:
      VERBOSE("Command: shipadd\n");
      @<Add a ship controled by machine |m|@>;
      break;

    default:
      @<Handle unknown command from machine |m|@>;
      break;

#if 0
    case MMP_CLIENT_DEFAULT_UNIVERSES:
      <Send a list with all the universes to machine |m|>;
      break;
#endif
    }
}

@ If the machine sends a message we can't understand, we shut down its
connection.  We are very exigent and only talk to those we can understand.

@<Handle unknown command from machine |m|@>=
{
  char buffer[sizeof("Client sent unknown command: ?") + 1];

  VERBOSE("Unknown command from machine: %d (%c)\n", key, key);

  sprintf(buffer, "Client sent unknown command: %c", key);

  mat_connection_free(m, MMP_SERVER_CLOSE_MMPERROR, buffer);
}

@ Here we add a ship.  The arguments are the universe where we should add it
(number) and the name given to the ship (string).

@<Add a ship controled by machine |m|@>=
{
  ASSERT(m);

  m->key_handler = machine_input_number;

  m->info.m.number_byte = 2;
  m->info.m.number      = 0;
  m->info.m.handler     = machine_input_arg_ship_add_string;
}

@ TODO: Make sure they can only enter alnums on the ship's name!

@<Functions@>=
void
machine_input_arg_ship_add_string ( MatConnection *m )
{
  ASSERT(m);

  m->key_handler = machine_input_string;

  m->info.m.string_maxlength = 8;
  m->info.m.string           = NULL;
  m->info.m.string_len       = 0;
  m->info.m.string_siz       = 0;

  m->info.m.handler          = machine_input_arg_ship_add;
}

@ This one is used to read a number from the network.

I like it very much.

it keeps on reading bytes and adding them until number_byte becomes 0.  That
makes it very easy to set the length (in bytes) of the number.  Once done
reading the number, it calls the handler.

@<Functions@>=
void
machine_input_number ( MatConnection *m, int key )
{
  ASSERT(m->info.m.number_byte > 0);

  ASSERT(0 <= key);
  ASSERT(key < 255);

  m->info.m.number = m->info.m.number * 255 + key;

  if (-- m->info.m.number_byte == 0)
    m->info.m.handler(m);
}

@ And now one to read strings.

We keep on reading bytes and adding them to the buffer until we come across a
0.  We then call the handler.

If we already have the maximum number of characters allowed, we ignore the
rest.

@<Functions@>=
void
machine_input_string ( MatConnection *m, int key )
{
  if (m->info.m.string_len == m->info.m.string_maxlength)
    return;

  if (m->info.m.string_len == m->info.m.string_siz)
    @<Resize buffer for string in |m|@>@;

  m->info.m.string[m->info.m.string_len ++] = key;

  if (key == 0)
    m->info.m.handler(m);
}

@
@<Resize buffer for string in |m|@>=
{
  m->info.m.string_siz = m->info.m.string_siz ? m->info.m.string_siz * 2 : 256;
  m->info.m.string = realloc(m->info.m.string, m->info.m.string_siz + 1);
  if (!m->info.m.string)
    {
      /* TODO: Shutdown this connection rather than the whole program! */
      fprintf(stderr, "%s: realloc: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }
}

@
@<Functions@>=
void
machine_input_arg_ship_add ( MatConnection *m )
{
  MatUniverse *un;

  ASSERT(m);

  m->key_handler = machine_input_default;

  MAT_UNIVERSES_FIND(un, m->info.m.number);

  if (!un)
    {
      MAT_PROTOCOL_ERROR_SHIPADD(m, MMP_SERVER_SHIPADDFAIL_UNEXISTANT, (char *) NULL);
      return;
    }

  VERBOSE("Adding ship to universe: %d\n", un->id);

  @<Add a new ship for machine |m|@>;
  @<Inform machines of a new ship added by |m|@>;

  mat_printf(m, "%c", MMP_SERVER_DEFAULT_SHIPADD_CLIENT);
  mat_out_flush(m);
}

@
@<Inform machines of a new ship added by |m|@>=
{
  MatSub *i;

  ASSERT(un);

  for (i = un->subs; i; i = i->u_next)
    if (i->m != m)
      mat_printf(i->m, "%c%c%c%s%c",
                 MMP_SERVER_DEFAULT_SHIPADD_OTHER,
                 0, 0, m->info.m.string, 0);
}

@ TODO: We ought to make sure the name entered is valid (only contains alnum).

@<Add a new ship for machine |m|@>=
{
  MatMovingObj *s;

  s = malloc(sizeof(MatMovingObj));
  if (!s)
    {
      mat_connection_free(m, MMP_SERVER_CLOSE_SYSERROR, strerror(errno));
      return;
    }

  s->type = MAT_MOVING_OBJ_SHIP;

  @<Initialize the messages buffer for a new ship@>;
  MAT_SHIP_VIEW_DEFAULT(s);

  s->refs = 1;

  s->img   = mat_ship;
  s->draw  = 1;

  s->un = un;

  un->players_real++;

  ADD_MOVING_OBJ(s, un);

  s->rm_next = NULL;

  s->prevtype = NULL;
  s->nexttype = un->players_head;

  if (un->players_head)
    un->players_head->prevtype = s;
  else
    un->players_tail = s;

  un->players_head = s;

  ASSERT(m->info.m.string_len <= MAT_USER_MAX);

  strcpy(s->info.player.name, m->info.m.string);

  s->info.player.times_dead = 0;
  s->info.player.game_over  = 0;

  s->info.player.chat_buffer_len = 0;
  s->info.player.chat_buffer[0]  = 0;

  MAT_CONNECTION_SHIP_ADD(m, s);

  MAT_SHIP_VIEW_CHECK(s->un);

  start_game(s);

  message_broadcast(un, "-=> %s (%s) joins the game", s->info.player.name, m->addr);
}

@ With this command, a machine can join an existing universe.

@<Allow machine |m| to join to a universe@>=
{
  ASSERT(m);

  m->key_handler = machine_input_number;

  m->info.m.number_byte = 2;
  m->info.m.number      = 0;
  m->info.m.handler     = machine_input_arg_join;
}

@ This one adds a universe to a connection.

TODO: Add a maximum-per-machine limit to the number of subscriptions so a
single mean machine won't be able to eat all our bandwidth.

@<Functions@>=
void
machine_input_arg_join ( MatConnection *m )
{
  MatUniverse *un;

  ASSERT(m);

  m->key_handler = machine_input_default;

  MAT_UNIVERSES_FIND(un, m->info.m.number)

  if (!un)
    {
      MAT_PROTOCOL_ERROR_JOIN(m, MMP_SERVER_JOINFAIL_UNEXISTANT);
      return;
    }

  VERBOSE("Machine attempts to join universe: [%d]\n", un->id);

  @<Add |m| to |un|@>;
  @<Send message informing |m| about the state of |un|@>;
}

@ Here we report an error that takes place when a client attempts to join to a
universe.

@d MAT_PROTOCOL_ERROR_JOIN(m, reason)
{
  mat_printf(m, "%c%c", MMP_SERVER_DEFAULT_JOIN_FAIL, reason);
  mat_out_flush(m);
}

@ And while we are at it, lets make a macro to report errors when the client
attempts to join a ship to a given universe.

@d MAT_PROTOCOL_ERROR_SHIPADD(m, reason, string)
{
  mat_printf(m, "%c%c", MMP_SERVER_DEFAULT_SHIPADD_FAIL, reason);
  if (reason == MMP_SERVER_SHIPADDFAIL_BADNAME)
    mat_printf(m, "%s%c", string, 0);
  mat_out_flush(m);
}


@ Here we add machine |m| to the universe specified in |m->info.m.number|.  We
already know that such universe exists.

The id of the universe is stored in m.info.m.number.

@<Add |m| to |un|@>=
{
  if (!mat_sub_add(m, un))
    {
      mat_connection_free(m, MMP_SERVER_CLOSE_SYSERROR, strerror(errno));
      return;
    }
}

@ Once a machine subscribes to a universe, we must sent a reply message
MMP_SERVER_DEFAULT_JOIN_OK.

@<Send message informing |m| about the state of |un|@>=
{
  MatMovingObj *pl;

  mat_printf(m, "%c", MMP_SERVER_DEFAULT_JOIN_OK);

  for (pl = un->players_head; pl; pl = pl->nexttype)
    mat_printf(m, "%s%c", pl->info.player.name, 0);

  mat_printf(m, "%c", 0);

  mat_out_flush(m);
}

@ In the future, it should be possible for a machine to obtain a list of all
the universes.  For the moment, we ignore his request.

@<Send a list with all the universes to machine |m|@>=
{

}

@
@<Functions@>=
void
player_input_default ( MatConnection *con, int key )
{
  ASSERT(con);
  ASSERT(con->state == MAT_STATE_PLAYING);
  ASSERT(con->ship_count == 1);

  ASSERT(con->ship_hash[0]);
  ASSERT(con->ship_hash[0]->type == MAT_MOVING_OBJ_SHIP);
  ASSERT(con->ship_hash[0]->info.player.conn_id == 0);
  ASSERT(con->ship_hash[0]->refs >= 0);

  switch (key)
    {
    case '[':
      ship_key_view_prev(con->ship_hash[0]);
      break;

    case ']':
      ship_key_view_next(con->ship_hash[0]);
      break;

    case '-':
    case '_':
      ship_key_zoom_out(con->ship_hash[0]);
      break;

    case '=':
    case '+':
      ship_key_zoom_in (con->ship_hash[0]);
      break;

    case 'q':
      ship_key_quit(con, 0);
      break;

    case 'Q':
      ship_key_quit(con, 1);
      break;

    case 'p':
      ship_key_pause(con->ship_hash[0]);
      break;

    case 'w':
      ship_key_who(con->ship_hash[0]);
      break;

    case 'i':
      ship_key_visibility(con->ship_hash[0]);
      break;

    case 'l':
    case KEY_ARROW_RIGHT:
      ship_key_right(con->ship_hash[0]);
      break;

    case KEY_ARROW_LEFT:
    case 'h':
      ship_key_left(con->ship_hash[0]);
      break;

    case 'k':
    case KEY_ARROW_UP:
      ship_key_up(con->ship_hash[0]);
      break;

    case 'j':
    case KEY_ARROW_DOWN:
      ship_key_down(con->ship_hash[0]);
      break;

    case ' ':
      mat_bullet_new_player(con->ship_hash[0], 0,  1);
      break;

    case 'b':
      mat_bullet_new_player(con->ship_hash[0], 0, -1);
      break;

    case 'n':
      mat_bullet_new_player(con->ship_hash[0], 1,  1);
      break;

    case 'm':
      mat_bullet_new_player(con->ship_hash[0], 2,  1);
      break;

    case 's':
      ship_key_swappos(con->ship_hash[0]);
      break;

    case 'c':
      ship_key_messages_less(con->ship_hash[0]);
      break;

    case 'z':
      ship_key_stats(con->ship_hash[0]);
      break;

    case 'C':
      ship_key_messages_clean(con->ship_hash[0]);
      break;

    case 'v':
      ship_key_messages_view(con->ship_hash[0]);
      break;

    case 13:
      ship_key_chat_start(con);
      break;

    default:
      VERBOSE("Ignored Input: [key:%d][state:player_input_default]\n", key);
    }
}

@
@<Functions@>=
void
ship_key_quit ( MatConnection *con, int force )
{
  if (!force)
    {
      MatMovingObj  *sh;

      /* TODO: Get rid of this assert! */
      ASSERT(con->ship_count > 0);

      con->key_handler = player_input_quit;
      for (sh = con->ship_list; sh; sh = sh->info.player.conn_list_next)
        sprintf(message_get(sh), "-=> Are you sure you want to quit?");
    }
  else
    {
      MESSAGE_FROM(con, "quits the game.");
      mat_connection_free(con, MMP_SERVER_CLOSE_CLIENT, "User quit.  Bye bye.");
    }
}

@
@<Functions@>=
void
ship_key_quit_cancel ( MatConnection *con )
{
  MatMovingObj     *sh;
  static char      *msg[] = 
    {
      "-=> I knew it!",
      "-=> Good!  Have fun! :)",
      "-=> Hahahah, I knew you couldn't quit!",
      "-=> Great!",
      "-=> Wonderful...",
      "-=> Ooooer, stay stay stay! :)",
      "-=> I knew you weren't such a chicken!",
      "-=> Phew, you scared me.",
      "-=> Good.  Now kill them all, you bastard!"
    };

  ASSERT(con->ship_count > 0);

  con->key_handler = player_input_default;

  for (sh = con->ship_list; sh; sh = sh->info.player.conn_list_next)
    sprintf(message_get(sh), "%s", msg[rand() % (sizeof(msg) / sizeof(char *))]);
}

@
@<Functions@>=
void
ship_key_chat_wipe ( MatMovingObj *ship )
{
  ship->info.player.chat_buffer_len = 0;
  ship->info.player.chat_buffer[0] = 0;
}

@
@<Functions@>=
void
ship_key_chat_backspace ( MatMovingObj *ship )
{
  if (ship->info.player.chat_buffer_len > 0)
    {
      ship->info.player.chat_buffer_len--;
      ship->info.player.chat_buffer[ship->info.player.chat_buffer_len] = 0;
    }
}

@
@<Functions@>=
void
ship_key_messages_clean ( MatMovingObj *ship )
{
  ship->info.player.messages_count = 0;
}

@
@<Functions@>=
void
ship_key_messages_view ( MatMovingObj *ship )
{
  ship->info.player.messages_enabled = !ship->info.player.messages_enabled;
}

@
@<Functions@>=
void
ship_key_chat_start ( MatConnection *con )
{
  if (arg_chat)
    con->key_handler = player_input_buffer;
}

@

@<Functions@>=
void
ship_key_chat_end ( MatConnection *con )
{
  MatShip *pl;

  ASSERT(con->ship_count == 1);

  con->key_handler = player_input_default;

  pl = &con->ship_hash[0]->info.player;

  pl->chat_buffer[pl->chat_buffer_len] = 0;
  if (pl->chat_buffer_len > 0)
    {
      message_broadcast(con->ship_hash[0]->un, "%s> %s", pl->name, pl->chat_buffer);
      pl->chat_buffer_len = 0;
      pl->chat_buffer[0] = 0;
      pl->messages_enabled = 1;
    }
}

@
@<Functions@>=
void
ship_key_messages_less ( MatMovingObj *ship )
{
  if (ship->info.player.messages_count > 0)
    {
      ship->info.player.messages_count -= 1;
      ship->info.player.messages_start += 1;
      ship->info.player.messages_start %= MAXMESSAGES;
    }
}

@
@<Functions@>=
void
ship_key_swappos ( MatMovingObj *ship )
{
  ship->info.player.messages_pos = !ship->info.player.messages_pos;
}

@
@<Functions@>=
void
ship_key_pause ( MatMovingObj *ship )
{
  char *msg;

  ship->un->pause = !ship->un->pause;

  msg = ship->un->pause ? "freezes time" : "makes time go on";
  message_broadcast(ship->un, "-=> %s %s . . .", ship->info.player.name, msg);
}

@ This function tells a player what are the other players in the universe he's
in.

@<Functions@>=
void
ship_key_who ( MatMovingObj *ship )
{
  MatMovingObj *pl;
  char *msg;
  int   i = MAXMESSAGELEN;

  ASSERT(ship->type == MAT_MOVING_OBJ_SHIP);
  ASSERT(MAT_USER_MAX < MAXMESSAGELEN);

  msg = message_get(ship);
  i = sizeof("-=> Players: ") - 1;

  sprintf(msg, "-=> Players: ");
  for (pl = ship->un->players_head; pl; pl = pl->nexttype)
    {
      char *name;

      ASSERT(pl->type == MAT_MOVING_OBJ_SHIP);
      ASSERT(pl->un == ship->un);

      name = pl->info.player.name;
      ASSERT(name);

      if (i + strlen(name) + 2 + 3 >= MAXMESSAGELEN)
        {
          msg[i] = 0;
          msg = message_get(ship);
          i = 4;

          sprintf(msg, "    ");
        }

      sprintf(msg + i, "[%s:%02d]", name, LIVES(pl));

      i += strlen(name) + 2 + 3;
    }
}

@

@d LIVES(ship)
  arg_lives - ((arg_teams_count && arg_teams_share) ?
    ship->info.player.team->times_dead :
    ship->info.player.times_dead)

@d HEALTH(x)
  ((int)(100 * (double)(x)->info.player.health / (double)(x)->un->health))

@<Functions@>=
void
ship_key_stats ( MatMovingObj *ship )
{
  ship->info.player.messages_enabled = 1;

  sprintf(message_get(ship), "[lives:%02d][health:%02d%%][players:%02d]",
          LIVES(ship), ship->info.player.health > 0 ? HEALTH(ship) : 0, ship->un->players_real);
}

@

@d SHIP_KEY_VIEW_LABEL(x)
{
  if (x->info.player.view == x)
    sprintf(message_get(x), "-=> Centering view around yourself.");
  else
    {
      char *name = x->info.player.view->info.player.name;
      sprintf(message_get(x), "-=> Centering view around player %s", name);
    }
}

@
@d MAT_SHIP_VIEW_RM(SHIP)
{
  if (SHIP->info.player.view_prev)
    SHIP->info.player.view_prev->info.player.view_next = SHIP->info.player.view_next;
  else
    {
      ASSERT(SHIP->info.player.view->info.player.viewers == SHIP);
      SHIP->info.player.view->info.player.viewers = SHIP->info.player.view_next;
    }

  if (SHIP->info.player.view_next)
    SHIP->info.player.view_next->info.player.view_prev = SHIP->info.player.view_prev;
}

@
@<Add the ship to the list of viewers@>=
{
  if (ship->info.player.view->info.player.viewers)
    ship->info.player.view->info.player.viewers->info.player.view_prev = ship;

  ship->info.player.view_prev = NULL;
  ship->info.player.view_next = ship->info.player.view->info.player.viewers;

  ship->info.player.view->info.player.viewers = ship;
}

@

@<Definitions@>=
#if MATANZA_ASSERT
#define MAT_SHIP_VIEW_CHECK(X) mat_ship_view_check(X)
#else
#define MAT_SHIP_VIEW_CHECK(X)
#endif

@
@<Function prototypes@>=
#if MATANZA_ASSERT
void mat_ship_view_check ( MatUniverse *un );
#endif

@
@<Functions@>=
#if MATANZA_ASSERT
void
mat_ship_view_check ( MatUniverse *un )
{
  MatMovingObj *ship;
  int count_total = 0, count_view = 0;

  for (ship = un->players_head; ship; ship = ship->nexttype)
    {
      MatMovingObj *i;

      count_total ++;

      for (i = ship->info.player.viewers; i; i = i->info.player.view_next)
        {
          count_view ++;
          ASSERT(i->info.player.view == ship);

          if (i->info.player.view_next)
            ASSERT(i->info.player.view_next->info.player.view_prev == i);

          if (!i->info.player.view_prev)
            ASSERT(i == ship->info.player.viewers);
        }
    }

  ASSERT(count_total == count_view);
}
#endif

@
@<Make sure there are other players in this universe@>=
{
  if (ship->un->players_real == 1)
    {
      sprintf(message_get(ship), "-=> You are the only player in this universe!\n");
      return;
    }
}

@
@<Functions@>=
void
ship_key_view_prev ( MatMovingObj *ship )
{
  ASSERT(ship);
  ASSERT(ship->refs > 0);
  ASSERT(ship->info.player.view);

  ASSERT(ship->info.player.view->refs > 0);

  MAT_SHIP_VIEW_CHECK(ship->un);

  @<Make sure there are other players in this universe@>;

  MAT_SHIP_VIEW_RM(ship);

  ASSERT(ship->un->players_tail);

  ship->info.player.view = ship->info.player.view->prevtype;
  if (!ship->info.player.view)
    ship->info.player.view = ship->un->players_tail;

  ASSERT(ship->info.player.view);

  @<Add the ship to the list of viewers@>;

  /* Print a message informing about the change of view. */
  SHIP_KEY_VIEW_LABEL(ship);

  MAT_SHIP_VIEW_CHECK(ship->un);
}

@
@<Functions@>=
void
ship_key_view_next ( MatMovingObj *ship )
{
  ASSERT(ship);
  ASSERT(ship->refs > 0);
  ASSERT(ship->info.player.view);

  ASSERT(ship->info.player.view->refs > 0);

  MAT_SHIP_VIEW_CHECK(ship->un);

  @<Make sure there are other players in this universe@>;

  MAT_SHIP_VIEW_RM(ship);

  ASSERT(ship->un->players_head);

  ship->info.player.view = ship->info.player.view->nexttype;
  if (!ship->info.player.view)
    ship->info.player.view = ship->un->players_head;

  ASSERT(ship->info.player.view);

  @<Add the ship to the list of viewers@>;

  /* Print a message informing about the change of view. */
  SHIP_KEY_VIEW_LABEL(ship);

  MAT_SHIP_VIEW_CHECK(ship->un);
}

@
@d CORRECTION 1.5

@<Functions@>=
void
ship_key_zoom_out ( MatMovingObj *ship )
{
  double nw, nh;

  nw = (double) ship->info.player.world_wi * 1.1;
  nh = GET_WORLD_HE(ship->info.player, nw);

  if ((int) nw < ship->un->mapsize_x && (int) nh < ship->un->mapsize_y)
    {
      ship->info.player.world_wi = (int) nw;
      ship->info.player.world_he = (int) nh;
    }
}

@
@<Functions@>=
void
ship_key_zoom_in  ( MatMovingObj *ship )
{
  double nw, nh;

  nw = (double) ship->info.player.world_wi / 1.1;
  nh = GET_WORLD_HE(ship->info.player, nw);

  if ((int) nw > mat_ship[0].w - 2 && (int) nh > mat_ship[0].h - 2)
    {
      ship->info.player.world_wi = (int) nw;
      ship->info.player.world_he = (int) nh;
    }

}

@
@<Function prototypes@>=
void ship_key_chat_start     ( MatConnection * );
void ship_key_chat_end       ( MatConnection * );

void ship_key_view_next      ( MatMovingObj *ship );
void ship_key_view_prev      ( MatMovingObj *ship );
void ship_key_zoom_out       ( MatMovingObj *ship );
void ship_key_zoom_in        ( MatMovingObj *ship );
void ship_key_stats          ( MatMovingObj *ship );
void ship_key_messages_clean ( MatMovingObj *ship );
void ship_key_messages_view  ( MatMovingObj *ship );
void ship_key_chat_backspace ( MatMovingObj *ship );
void ship_key_chat_wipe      ( MatMovingObj *ship );
void ship_key_pause          ( MatMovingObj *ship );
void ship_key_who            ( MatMovingObj *ship );
void ship_key_messages_less  ( MatMovingObj *ship );
void ship_key_swappos        ( MatMovingObj *ship );
void ship_key_up             ( MatMovingObj *ship );
void ship_key_down           ( MatMovingObj *ship );
void ship_key_left           ( MatMovingObj *ship );
void ship_key_right          ( MatMovingObj *ship );
void ship_key_visibility     ( MatMovingObj *ship );

void ship_key_quit           ( MatConnection *con, int force );
void ship_key_quit_cancel    ( MatConnection *con );

void ship_key_chat_add       ( MatMovingObj *ship, int key );

@
@<Functions@>=
void
ship_key_chat_add ( MatMovingObj *ship, int key )
{
  MatShip *p = &ship->info.player;

  if (isprint(key) && p->chat_buffer_len < MAXCHATBUFFERLEN)
    {
      p->chat_buffer[p->chat_buffer_len++] = (char)key;
      p->chat_buffer[p->chat_buffer_len] = 0;
    }
  else
    VERBOSE("Ignored Input: [key:%d][state:player_input_buffer]\n", key);
}

@
@<Functions@>=
void
ship_key_up ( MatMovingObj *ship )
{
  if (ship->info.player.game_over)
    {
      double mod_y, mod_t;

      if (ship->sp_y <= 0)
        mod_y = ship->sp_y - 1.0 / STEPS_MOVEMENT;
      else
        mod_y = 0;

      mod_t = sqrt(ship->sp_x * ship->sp_x + mod_y * mod_y);

      if (arg_speed_max == 0 || (-arg_speed_max < mod_t && mod_t < arg_speed_max))
        {
          ship->sp   = mod_t;
          ship->sp_y = mod_y;
        }
    }
  else if (ship->info.player.ship_speed_inc == 0)
    {
      ship->info.player.ship_speed_inc = 3;
      if (arg_space)
        {
          double mod_x, mod_y, mod_t;

          mod_x = ship->sp_x - 1.0 * mat_sin[(int)ship->ang] / STEPS_MOVEMENT;
          mod_y = ship->sp_y - 1.0 * mat_cos[(int)ship->ang] / STEPS_MOVEMENT;
          mod_t = sqrt(mod_x * mod_x + mod_y * mod_y);

          if (arg_speed_max == 0 || (-arg_speed_max < mod_t && mod_t < arg_speed_max))
            {
              ship->sp   = mod_t;
              ship->sp_x = mod_x;
              ship->sp_y = mod_y;
            }
        }
      else if (arg_speed_max == 0 || ship->sp + 1.0 / STEPS_MOVEMENT < (double)arg_speed_max)
        {
          ship->sp -= 1.0 / STEPS_MOVEMENT;
          ship->sp_x = ship->sp * mat_sin[(int)ship->ang];
          ship->sp_y = ship->sp * mat_cos[(int)ship->ang];
        }
    }
}

@

@<Functions@>=
void
ship_key_down ( MatMovingObj *ship )
{
  if (ship->info.player.game_over)
    {
      double mod_y, mod_t;

      if (ship->sp_y >= 0)
        mod_y = ship->sp_y + 1.0 / STEPS_MOVEMENT;
      else
        mod_y = 0;

      mod_t = sqrt(ship->sp_x * ship->sp_x + mod_y * mod_y);

      if (arg_speed_max == 0 || (-arg_speed_max < mod_t && mod_t < arg_speed_max))
        {
          ship->sp   = mod_t;
          ship->sp_y = mod_y;
        }
    }
  else if (ship->info.player.ship_speed_inc == 0 && arg_brake)
    {
      ship->info.player.ship_speed_inc = 3;
      if (arg_space)
        {
          double mod_x, mod_y, mod_t;

          mod_x = ship->sp_x + 1.0 * mat_sin[(int)ship->ang] / STEPS_MOVEMENT;
          mod_y = ship->sp_y + 1.0 * mat_cos[(int)ship->ang] / STEPS_MOVEMENT;
          mod_t = sqrt(mod_x * mod_x + mod_y * mod_y);

          if (arg_speed_max == 0 || (-arg_speed_max < mod_t && mod_t < arg_speed_max))
            {
              ship->sp   = mod_t;
              ship->sp_x = mod_x;
              ship->sp_y = mod_y;
            }
        }
      else if (arg_speed_max == 0 || -arg_speed_max < ship->sp - 1.0 / STEPS_MOVEMENT)
        {
          ship->sp += 1.0 / STEPS_MOVEMENT;
          if (!arg_fly_back && 0.0 < ship->sp)
            ship->sp = 0;
          ship->sp_x = ship->sp * mat_sin[(int)ship->ang];
          ship->sp_y = ship->sp * mat_cos[(int)ship->ang];
        }
    }
}

@
@<Functions@>=
void
ship_key_right ( MatMovingObj *ship )
{
  if (ship->info.player.game_over)
    {
      double mod_x, mod_t;

      if (ship->sp_x >= 0)
        mod_x = ship->sp_x + 1.0 / STEPS_MOVEMENT;
      else
        mod_x = 0;

      mod_t = sqrt(mod_x * mod_x + ship->sp_y * ship->sp_y);

      if (arg_speed_max == 0 || (-arg_speed_max < mod_t && mod_t < arg_speed_max))
        {
          ship->sp   = mod_t;
          ship->sp_x = mod_x;
        }
    }
  else if (arg_ang_speed_max > 0)
    {
      if (ship->sp_ang > 0)
        ship->sp_ang = 0;
      else if (ship->sp_ang > -arg_ang_speed_max)
        ship->sp_ang -= 1.0 / STEPS_MOVEMENT;
    }
  else
    {
      ship->ang -= 1;
      if ((int)ship->ang < 0)
        ship->ang += arg_ang;
      if (!arg_space)
        {
          ship->sp_x = ship->sp * mat_sin[(int)ship->ang];
          ship->sp_y = ship->sp * mat_cos[(int)ship->ang];
        }
    }
}

@
@<Functions@>=
void
ship_key_left ( MatMovingObj *ship )
{
  if (ship->info.player.game_over)
    {
      double mod_x, mod_t;

      if (ship->sp_x <= 0)
        mod_x = ship->sp_x - 1.0 / STEPS_MOVEMENT;
      else
        mod_x = 0;

      mod_t = sqrt(mod_x * mod_x + ship->sp_y * ship->sp_y);

      if (arg_speed_max == 0 || (-arg_speed_max < mod_t && mod_t < arg_speed_max))
        {
          ship->sp   = mod_t;
          ship->sp_x = mod_x;
        }
    }
  else if (arg_ang_speed_max > 0)
    {
      if (ship->sp_ang < 0)
        ship->sp_ang = 0;
      else if (ship->sp_ang < arg_ang_speed_max)
        ship->sp_ang += 1.0 / STEPS_MOVEMENT;
    }
  else
    {
      ship->ang += 1;
      if ((int)ship->ang > arg_ang - 1)
        ship->ang -= arg_ang;
      if (!arg_space)
        {
          ship->sp_x = ship->sp * mat_sin[(int)ship->ang];
          ship->sp_y = ship->sp * mat_cos[(int)ship->ang];
        }
    }
}

@

@d VISIBILITY_BROADCAST(obj)
{
  MatMovingObj *i;
  char *msg;

  ASSERT(obj->type == MAT_MOVING_OBJ_SHIP);

  msg = obj->info.player.visible ?
          "-=> %s appears in the middle of nowhere !" :
          "-=> %s becomes invisible !";

  for (i = obj->un->players_head; i; i = i->nexttype)
    {
      ASSERT(i->un == obj->un);
      if (i != obj && OBJECT_INSIDE(i, obj))
        sprintf(message_get(i), msg, obj->info.player.name);
    }
}

@<Functions@>=
void
ship_key_visibility ( MatMovingObj *ship )
{
  if (ship->info.player.visible_exp > 0)
    {
      ship->info.player.visible = !ship->info.player.visible;
      VISIBILITY_BROADCAST(ship);
    }
}

@ We keep a list of maximum |MAXMESSAGES| messages.  The messages expire when
they are too old.

@d NM ((p->info.player.messages_start + p->info.player.messages_count - 1) % MAXMESSAGES)

@<Functions@>=
char *
message_get (MatMovingObj *p)
{
  static char buffer[MAXMESSAGELEN];

  if (p->type != MAT_MOVING_OBJ_SHIP
      || !p->info.player.conn
      || p->info.player.conn->state != MAT_STATE_PLAYING)
    return buffer;

  ASSERT(p);
  ASSERT(p->type == MAT_MOVING_OBJ_SHIP);
  ASSERT(p->info.player.conn);
  ASSERT(p->info.player.conn->state == MAT_STATE_PLAYING);

  if (p->info.player.messages_count == MAXMESSAGES)
    p->info.player.messages_start = (p->info.player.messages_start + 1) % MAXMESSAGES;
  else
    p->info.player.messages_count ++;

  p->info.player.messages_expire[NM] = 200;
  return p->info.player.messages[NM];
}

void
message_broadcast (MatUniverse *u, char *fmt, ...)
{
  MatMovingObj *p;
  va_list arg;

  ASSERT(u);
  ASSERT(fmt);

  for (p = u->players_head; p; p = p->nexttype)
    {
      va_start(arg, fmt);
      vsprintf(message_get(p), fmt, arg);
      va_end(arg);
    }
}

@
@<Function prototypes@>=
int mat_bullet_new_player ( MatMovingObj *p, int type, int dir );

@ This function makes a new bullet.

|p| is the player shooting the bullet. |sp| is the speed of the bullet. |dm| is
the damage the bullet will do when it hits other players. |xp| is the number of
times events should be processed before the bullet is removed. |pxp| is the
number of ticks before the player will be allowed to shot again.

@<Functions@>=
int
mat_bullet_new_player ( MatMovingObj *p, int type, int dir )
{
  double sp_x, sp_y;

  ASSERT(dir == 1 || dir == -1);

  ASSERT(p->type == MAT_MOVING_OBJ_SHIP);
  ASSERT(p->refs >= 0);

  @<Make sure the player can shot@>;

  sp_x = - dir * mat_bullets_speed[type] * mat_sin[(int)p->ang] + p->sp_x;
  sp_y = - dir * mat_bullets_speed[type] * mat_cos[(int)p->ang] + p->sp_y;

  return mat_bullet_new(p, type, sp_x, sp_y, p->ang);
}

@
@<Function prototypes@>=

int                  mat_bullet_new                  ( MatMovingObj         *p,
                                                       int                   type,
                                                       double                sp_x,
                                                       double                sp_y,
                                                       double                ang );

@
@<Functions@>=
int
mat_bullet_new ( MatMovingObj *p, int type, double sp_x, double sp_y, double ang )
{
  MatMovingObj *n;

  n = malloc(sizeof(MatMovingObj));
  if (!n)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      return 0;
    }

  MAT_MOVING_OBJ_REF(p);

  n->un = p->un;

  p->info.player.ship_bullets[type] --;
  if (p->info.player.ship_bullets[type] == 0)
    sprintf(message_get(p), "-=> Ooops, you ran out of %s !", mat_bullets_name[type]);

  if (type == 2)
    p->pal = mat_ship_palette[p->info.player.ship_bullets[2]];

  p->info.player.bullets_expire += mat_bullets_pexp[type];

  n->info.bullet.src     = p;
  n->info.bullet.type    = type;
  n->info.bullet.expires = mat_bullets_bexp[type];

  n->type          = MAT_MOVING_OBJ_BULLET;
  n->rm_next       = NULL;
  n->ang           = ang;
  n->sp_ang        = 0;

  n->crash         = 1;
  n->draw          = 1;

  n->alpha         = 1.0;
  n->alpha_mod     = NULL;

  n->img           = type > 0 ? mat_missile[type - 1] : NULL;

  n->sp_x          = sp_x;
  n->sp_y          = sp_y;

  ASSERT(p->img);

  n->pos_x         = p->pos_x + (p->img[0].w - (type > 0 ? n->img[0].w : 0)) / 2;
  n->pos_y         = p->pos_y + (p->img[0].h - (type > 0 ? n->img[0].h : 0)) / 2;

  if ((int)n->pos_x >= n->un->mapsize_x)
    n->pos_x -= n->un->mapsize_x;
  if ((int)n->pos_y >= n->un->mapsize_y)
    n->pos_y -= n->un->mapsize_y;

  ASSERT((int)n->pos_x >= 0);
  ASSERT((int)n->pos_y >= 0);
  ASSERT((int)n->pos_x < n->un->mapsize_x);
  ASSERT((int)n->pos_y < n->un->mapsize_y);

  n->pal = mat_ship_palette[0];

  @<Add bullet |b| to the list of bullets@>;
  return 1;
}

@
@<Add bullet |b| to the list of bullets@>=
{
  MatUniverse *un;

  un = n->un;

  n->prevtype = NULL;
  n->nexttype = un->bullets;

  if (un->bullets)
    un->bullets->prevtype = n;

  un->bullets = n;

  ADD_MOVING_OBJ(n, un);
}

@

@d DEL_MOVING_OBJ(obj)
{
  if ((obj)->next)
    (obj)->next->prev = (obj)->prev;

  if ((obj)->prev)
    (obj)->prev->next = (obj)->next;
  else
    (obj)->un->objs = (obj)->un->objs->next;
}

@d ADD_MOVING_OBJ(obj, un)
{
  (obj)->prev = NULL;
  (obj)->next = un->objs;

  if (un->objs)
    un->objs->prev = (obj);

  un->objs = (obj);
}

@
@<Make sure the player can shot@>=
{
  if (p->info.player.bullets_expire > mat_bullets_pexp[type] / 2
      || p->info.player.ship_bullets[type] <= 0
      || p->info.player.game_over)
    return 0;
}

@ This should be optimized.  We should get rid of universe_bitmask and keep
this subscriptions as a list.

This will break with the IDs for the universes when more than one universe gets
supported.

@<Functions@>=
void
machine_update ( MatConnection *self )
{
  int     i;
  MatSub *un;

  for (i = 0; i < MAT_SUB_MACHINE_HASH_SIZE; i ++)
    for (un = self->info.m.subs[i]; un; un = un->m_next)
      {
        mat_printf(self, "%c%c%c", MMP_SERVER_DEFAULT_FRAME_SHIPS,
                   un->u->id & 0xff00, un->u->id, 0x00ff);
        @<Send information about all the ships@>;
        mat_printf(self, "%c", MMP_SERVER_FRAME_DONE);
      }

  mat_out_flush(self);
}

@ In this macro we send information about all the ships in the

@d SEND_NUMBER(x) mat_printf(self, "%c%c", ((int)x) & 0xff00, ((int)x) & 0x00ff)

@d SEND_STRING(x) mat_printf(self, "%s%c", ((char *)x), 0);

@<Send information about all the ships@>=
{
  MatMovingObj *sh;

  for (sh = un->u->players_head; sh; sh = sh->nexttype)
    mat_printf(self, "%c%c" "%c%c" "%c%c",
               ((int) sh->pos_x) & 0xff00, ((int) sh->pos_x) & 0x00ff,
               ((int) sh->pos_y) & 0xff00, ((int) sh->pos_y) & 0x00ff,
               ((int) sh->ang  ) & 0xff00, ((int) sh->ang  ) & 0x00ff);
}

@
@<Functions@>=
void
draw_world ( MatConnection *self )
{
  int           game_moving;

  ASSERT(self);
  ASSERT(self->state == MAT_STATE_PLAYING);

  @<Find out whether the game is paused for all the players in |self|@>;

  if (game_moving)
    @<Draw the images for connection |self|@>@;

  MAT_GRAPH_RENDER_NOWIPE_ALL(self);

  @<Draw the text for connection |self|@>;

  MAT_FLUSH_ALL(self);
}

@
@<Find out whether the game is paused for all the players in |self|@>=
{
  MatMovingObj *sh = self->ship_list;

  game_moving = 0;

  do
    {
      if (!sh->un->pause)
        {
          game_moving = 1;
          break;
        }
      else
        sh = sh->info.player.conn_list_next;
    }
  while (sh);
}

@

@<Draw the text for connection |self|@>=
{
  MatMovingObj            *i;
  MatMovingObj            *sh;
  MatShip                 *pl;
  MatShip                 *pl_view;
  MatUniverse             *un;

  for (i = self->ship_list; i; i = i->info.player.conn_list_next)
    {
      ASSERT(i->type == MAT_MOVING_OBJ_SHIP);

      pl = &i->info.player;

      sh = i->info.player.view;

      ASSERT(sh);
      ASSERT(sh->type == MAT_MOVING_OBJ_SHIP);

      un = sh->un;
      ASSERT(un);

      pl_view = &sh->info.player;

      if (arg_radar && un->updates % 4)
        {
          MatMovingObj *obj;

          for (obj = un->ast; obj; obj = obj->nexttype)
            if (!OBJECT_INSIDE(sh, obj))
              @<Draw the direction to reach object |obj|@>;

          for (obj = un->players_head; obj; obj = obj->nexttype)
            if (!OBJECT_INSIDE(sh, obj) && !obj->info.player.game_over)
              @<Draw the direction to reach object |obj|@>;
        }

      @<Draw the messages@>;
    }
}

@ Here we draw everything that gets rendered.

@<Draw the images for connection |self|@>=
{
  int                       startx;
  int                       starty;
  MatMovingObj             *sh;
  MatMovingObj             *i;
  MatMovingObj             *p;
  MatMovingObj             *obj;
  MatShip                  *pl;
  MatShip                  *pl_view;
  MatUniverse              *un;

  TERM_CLEAR_IMAGE(self);

  for (i = self->ship_list; i; i = i->info.player.conn_list_next)
    {
      ASSERT(i->type  == MAT_MOVING_OBJ_SHIP);

      pl = &i->info.player;

      sh = i->info.player.view;

      ASSERT(sh);
      ASSERT(sh->type == MAT_MOVING_OBJ_SHIP);

      pl_view = &sh->info.player;

      un = sh->un;
      ASSERT(un);

      ASSERT((int)sh->pos_x >= 0);
      ASSERT((int)sh->pos_x < un->mapsize_x);
      ASSERT((int)sh->pos_y >= 0);
      ASSERT((int)sh->pos_y < un->mapsize_y);

      ASSERT(pl->world_wi > 0);
      ASSERT(pl->world_he > 0);

      startx = sh->pos_x - (pl->world_wi - sh->img[0].w) / 2;
      starty = sh->pos_y - (pl->world_he - sh->img[0].h) / 2;

      if (startx < 0)
        startx += un->mapsize_x;

      if (starty < 0)
        starty += un->mapsize_y;

      ASSERT(startx >= 0);
      ASSERT(starty >= 0);
      ASSERT(startx < un->mapsize_x);
      ASSERT(starty < un->mapsize_y);

      @<Draw background image@>;

      for (obj = un->ast; obj; obj = obj->nexttype)
        @<Draw |obj| in the screen of |sh|@>;

      for (obj = un->smart_objs; obj; obj = obj->nexttype)
        @<Draw |obj| in the screen of |sh|@>;

      for (p = un->players_head; p; p = p->nexttype)
        if (/*OBJECT_INSIDE(sh, p) &&*/ p->info.player.visible && p->draw)
          @<Draw ship for player |p| in screen for |i|@>;

      for (obj = un->bullets; obj; obj = obj->nexttype)
        @<Draw |obj| in the screen of |sh|@>;

      if (pl_view->game_over && pl->messages_enabled)
        @<Draw Game Over label@>;
    }
}


@
TODO: Get this to work with ppc != 1

@<Draw Game Over label@>=
{
  int imgwi, imghe;

  imgwi = pl->win_x_e - pl->win_x_s;
  imghe = pl->win_y_e - pl->win_y_s;

  print(self,
        pl->win_x_s * 2 + imgwi - 2 * 2 * imgwi / 5,
        pl->win_y_s * 2 + imghe / 2,
        2 * imgwi / 5,
        2 * imghe / 4,
        &font, 255, "Game");

  print(self,
        pl->win_x_s * 2 + imgwi - 2 * 2 * (imgwi / 5),
        pl->win_y_s * 2 + imghe,
        2 * imgwi / 5,
        2 * imghe / 4,
        &font, 255, "Over");
}

@

@d POSB()
  (pl->messages_pos ? pl->win_y_s + 1 : pl->win_y_e - 2)

@d NMESG(x)
  ((pl->messages_start + x) % MAXMESSAGES)

@<Draw the messages@>=
{
  int i, pos;

  ASSERT(pl);

  pos = pl->messages_pos ? pl->win_y_e - pl->messages_count - 1 : pl->win_y_s + 1;

  for (i = 0; i < pl->messages_count; i ++)
    {
      if (pl->messages_enabled)
        mat_print(self, 1, pos + i, MAT_NORMAL, pl->messages[NMESG(i)], 0);

      if (pl->messages_expire[NMESG(i)] -- == 0)
        {
          pl->messages_start = NMESG(1);
          pl->messages_count --;
        }
    }

  if (self->ship_count == 1 && self->key_handler == player_input_buffer)
    {
      mat_print(self, 1, POSB(), MAT_NORMAL, "> ", 0);
      mat_print(self, 3, POSB(), MAT_NORMAL, pl->chat_buffer, 0);
    }
}

@

@<Draw background image@>=
{
  MatImgWorld *obj;

  for (obj = mat_bg_head; obj; obj = obj->next)
    {
      @<Draw image for the world in |obj|@>;
    }

  if (un->bg)
    {
      ASSERT(un->bg->w <= un->mapsize_x);
      ASSERT(un->bg->h <= un->mapsize_y);

      MAT_IMAGE_DRAW_ST(self, i, un->bg, startx, starty, mat_stdpal, arg_bg_adj, 1.0);
    }

  if (un->dots)
    {
      int i, j;

      ASSERT(self);
      ASSERT(self->graph_imagebuffer);

      for (i = 20 - starty % 20; i < pl->imghe; i += 20)
        for (j = 40 - startx % 40; j < pl->imgwi; j += 40)
          MAT_GRAPH_DRAW(self, pl, j, i, 100);
    }
}

@ If the player |self| can see the object |obj|, we draw it.

@<Draw |obj| in the screen of |sh|@>=
{
  int x, y;

  ASSERT(obj);
  ASSERT(!obj->rm_next);

  ASSERT(self);

  ASSERT((int) obj->pos_x >= 0);
  ASSERT((int) obj->pos_y >= 0);
  ASSERT((int) obj->pos_x < un->mapsize_x);
  ASSERT((int) obj->pos_y < un->mapsize_y);

  if (obj->draw)
    {
      if (obj->img == NULL)
        {
          x = (int) obj->pos_x - startx;
          y = (int) obj->pos_y - starty;

          if (x < 0)
            x += un->mapsize_x;
          if (y < 0)
            y += un->mapsize_y;

          ASSERT(x < un->mapsize_x);
          ASSERT(y < un->mapsize_y);

          ASSERT(x >= 0);
          ASSERT(y >= 0);

          if (x < pl->world_wi && y < pl->world_he)
            {
              @<Scale |x| and |y| to the screen of |pl|@>;

              ASSERT(x >= 0);
              ASSERT(x < self->graph_imgwidth);
              ASSERT(y >= 0);
              ASSERT(y < self->graph_imgheight);

              MAT_GRAPH_DRAW(self, pl, x, y, 255);
            }
        }
      else
        {
          MatImage *img;
          int x, y;

          img = &obj->img[(int) obj->ang];

          x = (int) obj->pos_x;
          y = (int) obj->pos_y;

          MAT_IMAGE_DRAW_DY(self, i, img, x, y, obj, startx, starty, 0);
        }
    }
}

@
@<Global variables@>=
unsigned char mat_stdpal[256] = {
  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
 20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
 40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
 60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,
100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255 };

@ This macro is used to adjust the colors in a given image so the minimum color
will be projected to col_min and the maximum to col_max.

@<Adjust the colors in image |img|@>=
{
  unsigned char col_r_min, col_r_max;
  int bufsize = img->h * img->w;

  @<Calculate the lowest and highest colors in |img|@>;

  if (col_r_min != col_min || col_r_max != col_max)
    @<Modify the colors in |img| to adjust them@>;
}

@

@<Modify the colors in |img| to adjust them@>=
{
  double rate;
  int i;

  ASSERT(bufsize == img->h * img->w);

  if (col_r_max == col_r_min)
    rate = 0;
  else
    rate = ((double)col_max - (double)col_min) / ((double)col_r_max - (double)col_r_min);

  for (i = 0; i < bufsize; i ++)
    {
      img->img[i] = col_min + (img->img[i] - col_r_min) * rate;
      ASSERT(col_min <= img->img[i]);
      ASSERT(img->img[i] <= col_max);
    }
}

@

@<Calculate the lowest and highest colors in |img|@>=
{
  int i;

  col_r_min = col_r_max = img->img[0];

  for (i = 1; i < bufsize; i ++)
    {
      unsigned char tmp = img->img[i];

      if (tmp < col_r_min)
        col_r_min = tmp;
      else if (col_r_max < tmp)
        col_r_max = tmp;
    }
}

@ Here we prepare the palettes used to draw stuff.

@<Make palettes@>=
{
  int i;

  for (i = 0; i < 3; i ++)
    {
      mat_ship_palette[i] = malloc(sizeof(int) * 256);
      if (!mat_ship_palette[i])
        {
          fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
          exit(EXIT_FAILURE);
        }

      mat_ship_palette[i]['*'] = 100;
      mat_ship_palette[i]['X'] = 255;
      mat_ship_palette[i]['.'] = 0;
      mat_ship_palette[i][' '] = 0;
    }

  mat_ship_palette[2]['1'] = 255;
  mat_ship_palette[2]['2'] = 255;
  mat_ship_palette[2]['3'] = 255;
  mat_ship_palette[2]['4'] = 255;

  mat_ship_palette[1]['1'] = 0;
  mat_ship_palette[1]['2'] = 255;
  mat_ship_palette[1]['3'] = 100;
  mat_ship_palette[1]['4'] = 255;

  mat_ship_palette[0]['1'] = 0;
  mat_ship_palette[0]['2'] = 0;
  mat_ship_palette[0]['3'] = 100;
  mat_ship_palette[0]['4'] = 100;
}

@
@<Destroy palettes@>=
{
  free(mat_ship_palette[0]);
  free(mat_ship_palette[1]);
  free(mat_ship_palette[2]);
}

@
@<Scale |x| and |y| to the screen of |pl|@>=
{
  if (pl->imgwi != pl->world_wi)
    x = (double) x * (double) pl->imgwi / (double) pl->world_wi;

  if (pl->imghe != pl->world_he)
    y = (double) y * (double) pl->imghe / (double) pl->world_he;
}

@ We use MAT_SHIPSIZE_. / . because the ships don't take the whole
MAT_SHIPSIZE_.  yet. In the future, as the matrixes for the ships get
optimized, this should be changed to MAT_SHIPSIZE_. / 2.

@d ABS(x) ((x) >= 0 ? (x) : -(x))

@d MAXWI(p) p->info.player.world_wi / 2 + p->img[0].w / 2
@d MAXHE(p) p->info.player.world_he / 2 + p->img[0].h / 2

@d OBJECT_INSIDE(p, q)
  (   p == q
   || (    (                       ABS(q->pos_x - p->pos_x) < MAXWI(p)
             || p->un->mapsize_x - ABS(q->pos_x - p->pos_x) < MAXWI(p) )
        && (                       ABS(q->pos_y - p->pos_y) < MAXHE(p)
             || p->un->mapsize_y - ABS(q->pos_y - p->pos_y) < MAXHE(p) ) ) )

@ Here we draw the first letter of the name of player |obj| in the direction self
should move to reach him.

@<Draw the direction to reach object |obj|@>=
{
  double x, y, m;
  int    wi, he, dx, dy, draw = 1;
  char buffer[2];

  ASSERT(obj != sh);

  @<Find out the distance between |obj| and |sh|@>;

  m = y / x;

  ASSERT(x >= 0);
  ASSERT(y >= 0);

  ASSERT(pl->imgwi > 0);
  ASSERT(pl->imghe > 0);

  if (m <= (double) pl->imghe / (double) pl->imgwi)
    {
      wi = dx > 0 ? pl->imgwi - 1: 0;
      he = (pl->imghe + dy * m * pl->imgwi) / 2;
    }
  else
    {
      he = dy > 0 ? pl->imghe - 1: 0;
      wi = (pl->imgwi + dx * pl->imghe / m) / 2;
    }

  ASSERT(0 <= he);
  ASSERT(he < pl->imghe);
  ASSERT(0 <= wi);
  ASSERT(wi < pl->imgwi);
  ASSERT(he == 0 || he == pl->imghe - 1 || wi == 0 || wi == pl->imgwi - 1);

  @<Fill buffer according to the distance in |x| and |y|@>;

  if (draw)
    mat_print(self, wi / 2 + pl->win_x_s, he / 2 + pl->win_y_s, MAT_NORMAL, buffer, 0);
}

@

@d ADJUST_POSITION(res, pos, w, map)
  {
    ASSERT((int)pos >= 0);
    ASSERT((int)pos < map);
    
    ASSERT(w < map);
    ASSERT(w >= 0);

    res = pos + w / 2;

    if (res >= map)
      res -= map;

    ASSERT(res >= 0);
    ASSERT(res < map);
  }

@<Find out the distance between |obj| and |sh|@>=
{
  double px, sx, py, sy;
  double x1, x2, y1, y2;

  ASSERT(obj);
  ASSERT(sh);

  ASSERT(obj->un == sh->un);

  ASSERT(!obj->rm_next);
  ASSERT(!sh->rm_next);

  ASSERT(obj->img);
  ASSERT(sh->img);

  ADJUST_POSITION(px, obj->pos_x, obj->img->w, obj->un->mapsize_x);
  ADJUST_POSITION(sx,  sh->pos_x,  sh->img->w, obj->un->mapsize_x);

  x1 = ABS(px - sx);
  x2 = obj->un->mapsize_x - x1;

  if (x1 < x2)
    x = x1, dx = 1;
  else
    x = x2, dx = -1;

  dx = sx <= px ? dx : -dx;

  ADJUST_POSITION(py, obj->pos_y, obj->img->h, obj->un->mapsize_y);
  ADJUST_POSITION(sy,  sh->pos_y,  sh->img->h, obj->un->mapsize_y);

  y1 = ABS(py - sy);
  y2 = obj->un->mapsize_y - y1;

  if (y1 < y2)
    y = y1, dy = 1;
  else
    y = y2, dy = -1;

  dy = sy <= py ? dy : -dy;
}

@

@d LENAWAY (pl->world_wi * pl->world_wi + pl->world_he * pl->world_he)
@d LENNEAR (pl->world_wi * pl->world_wi + pl->world_he * pl->world_he) / 2

@<Fill buffer according to the distance in |x| and |y|@>=
{
  int dist;

  ASSERT(obj);

  dist = x * x + y * y;

  if (dist > LENAWAY)
    {
      if (obj->type == MAT_MOVING_OBJ_SHIP)
        buffer[0] = tolower(obj->info.player.name[0]);
      else
        draw = 0;
    }
  else if (dist > LENNEAR)
    {
      if (obj->type == MAT_MOVING_OBJ_SHIP)
        buffer[0] = toupper(obj->info.player.name[0]);
      else
        buffer[0] = '.';
    }
  else
    {
      if (obj->type == MAT_MOVING_OBJ_SHIP)
        buffer[0] = '*';
      else
        buffer[0] = '+';
    }

  buffer[1] = 0;
}

@
@<Scale |pos_x| and |pos_y|@>=

  if (self->graph_imgwidth != self->world_wi)
    pos_x = pos_x * (double)self->graph_imgwidth / (double) self->world_wi;
  if (self->graph_imgheight != self->world_he)
    pos_y = pos_y * (double)self->graph_imgheight / (double) self->world_he;

@ |ships.c| contains an array with the matrix for all the possible angles.

@d MAT_GRAPH_CORD(p,s,x,y)
  ((int)(((x) + 2 * (s)->win_x_s) + ((y) + 2 * (s)->win_y_s) * (p)->graph_imgwidth))

@d MAT_GRAPH_DRAW(p,s,x,y,b)
  ((p)->graph_imagebuffer[MAT_GRAPH_CORD(p,s,x,y)] = (b))

@d MAT_GRAPH_DRAW_ALPHA(p,s,x,y,b,a)
{
  unsigned char *xcol = &(p)->graph_imagebuffer[MAT_GRAPH_CORD(p, s, x, y)];

  ASSERT(0.0 <= a);
  ASSERT(a <= 1.0);

  ASSERT(0 <= b);
  ASSERT(b <= 255);

  ASSERT(0 <= (int)((double)(b) * (a) + (double)*xcol * (1 - (a))));
  ASSERT((int)((double)(b) * (a) + (double)*xcol * (1 - (a))) <= 255);

  *xcol = (char)((double)(b) * (a) + (double)*xcol * (1 - (a)));
}

@<Draw ship for player |p| in screen for |i|@>=
{
  MatImage *img;
  int x, y;

  ASSERT(p);
  ASSERT(p->type == MAT_MOVING_OBJ_SHIP);

  ASSERT(p->info.player.ship_bullets[2] >= 0);
  ASSERT(p->info.player.ship_bullets[2] <= 2);

  ASSERT(!p->rm_next);

  x = (int) p->pos_x;
  y = (int) p->pos_y;

  ASSERT(0 <= x);
  ASSERT(x < i->un->mapsize_x);

  ASSERT(0 <= y);
  ASSERT(y < i->un->mapsize_y);

  img = &p->img[(int)p->ang];
  ASSERT(img);

  MAT_IMAGE_DRAW_DY(self, i, img, x, y, p, startx, starty, 0);
}

@
@<Function prototypes@>=
void          machine_update             ( MatConnection     *self );
void          draw_world                 ( MatConnection     *p );
void          start_game                 ( MatMovingObj      *p );
char         *message_get                ( MatMovingObj      *p );
void          message_broadcast          ( MatUniverse       *, char *, ...);

@
@<Function prototypes@>=
void         player_input_quit                         ( MatConnection *p, int key );
void         player_input_default                      ( MatConnection *p, int key );
void         player_input_buffer                       ( MatConnection *p, int key );
void         player_input_multiple                     ( MatConnection *p, int key );

void        machine_input_initial                      ( MatConnection *p, int key );
void        machine_input_default                      ( MatConnection *p, int key );
void        machine_input_string                       ( MatConnection *m, int key );
void        machine_input_number                       ( MatConnection *m, int key );

void        machine_input_arg_join                     ( MatConnection *m );
void        machine_input_arg_ship_add                 ( MatConnection *m );
void        machine_input_arg_ship_add_string          ( MatConnection *m );
void        machine_input_arg_id_name                  ( MatConnection *m );
void        machine_input_arg_id_ver                   ( MatConnection *m );

@ For the moment, a random location.  In the future we might pick up one from a
list of locations or it might depend on the player's team or something.

@<Set the startup location for ship |ship|@>=
{
  ship->pos_x         = (double)(rand() % ship->un->mapsize_x);
  ship->pos_y         = (double)(rand() % ship->un->mapsize_y);

  ship->ang           = (double)(rand() % arg_ang);
  ship->sp_ang        = arg_ang_speed_max == 0 ? 0 : (double)(rand() % 5 - 2) / 10;
  ship->sp            = 0;
  ship->sp_x          = 0;
  ship->sp_y          = 0;
}

@* Asteroids

The struct containing asteroids:

@<Data for asteroids@>=

typedef struct MatAst MatAst;

struct MatAst
{
  int     size;
  int     health;
};

@

@<Global variables@>=

int              mat_ast_health[] = { 500, 300 };

@ Here we add a few asteroids at random positions in the game.  In the future,
we ought to make sure we are not adding asteroids in places where players can
see them appear all of a sudden.

@<Flood with asteroids@>=
{
  ASSERT(un);

  while (un->ast_cur < arg_ast_num)
    AST_ADD_RANDOMPOS(0, un);
    
#if 0
    arg_ast_num = mat_ast_cur;
    fprintf(stderr, "%s: Resetting number of asteroids to %d\n", program_name, arg_ast_num);
#endif
}

@ We need a function that takes the asteroid size and adds a given asteroid.

@<Function prototypes@>=
int ast_add ( int );

@

@d AST_ADD_RANDOMPOS(size, un)
{
  ASSERT(un->mapsize_x > 0);
  ASSERT(un->mapsize_y > 0);

  AST_ADD_POS(size,
    rand() % un->mapsize_x, rand() % un->mapsize_y,
    ((double)((rand() % 10) - 5)) / STEPS_MOVEMENT,
    ((double)((rand() % 10) - 5)) / STEPS_MOVEMENT,
    un);

  ASSERT(un->ast_cur > 0);
}

@d AST_ADD_POS(size, x, y, sx, sy, un)
{
  MatMovingObj *ast;

  AST_ADD(ast, size, un);

  ASSERT(x >= 0);
  ASSERT(x < un->mapsize_x);

  ASSERT(y >= 0);
  ASSERT(y < un->mapsize_y);

  ast->pos_x = x;
  ast->pos_y = y;
  ast->sp_x  = sx;
  ast->sp_y  = sy;
}

@d AST_ADD(ast_new, sz, unx)
{
  ast_new = malloc(sizeof(MatMovingObj));

  ast_new->type            = MAT_MOVING_OBJ_AST;
  ast_new->rm_next         = NULL;

  ASSERT(mat_ast[sz]);
  ast_new->img             = mat_ast[sz];
  ast_new->pal             = mat_ship_palette[0];

  ast_new->ang             =  (double)(rand() % arg_ang);
  ast_new->sp_ang          = ((double)(rand() % arg_ang) - arg_ang / 2) / (20 * STEPS_MOVEMENT);

  ast_new->info.ast.size   = sz;
  ast_new->info.ast.health = mat_ast_health[sz];

  ast_new->crash           = 1;
  ast_new->draw            = 1;

  ast_new->alpha           = 1.0;
  ast_new->alpha_mod       = NULL;

  ast_new->un              = unx;

  ast_new->nexttype        = unx->ast;
  ast_new->prevtype        = NULL;

  if (unx->ast)
    unx->ast->prevtype = ast_new;

  unx->ast = ast_new;

  ADD_MOVING_OBJ(ast_new, unx);

  unx->ast_cur ++;

  ASSERT(ast_new->img);
}

@
@<Struct definitions@>=
typedef struct MatImgWorld MatImgWorld;

struct MatImgWorld
{
  MatImage      *img;
  //MatImage      *trans;
  char          *path;

  MatImgWorld   *next;
  MatImgWorld   *prev;

  //MatMovingObj  *mv;
};

@

@<Global variables@>=
MatImgWorld     *mat_bg_head = NULL;
MatImgWorld     *mat_bg_tail = NULL;

MatImgWorld     *mat_fg_head = NULL;
MatImgWorld     *mat_fg_tail = NULL;

@
TODO: Make sure the images are big enough.

@<Load background images@>=
{
  MatImgWorld *tmp;

  for (tmp = mat_bg_head; tmp; tmp = tmp->next)
    {
      tmp->img = mat_png_load(tmp->path);
      if (arg_bg_color_max != -1)
        {
          int col_min = 0;
          int col_max = arg_bg_color_max;

          MatImage *img = tmp->img;

          @<Adjust the colors in image |img|@>;
        }
    }

  for (tmp = mat_fg_head; tmp; tmp = tmp->next)
    tmp->img = mat_png_load(tmp->path);

  if (arg_bg_size && (mat_bg_head || mat_fg_tail))
    arg_mapsize_x = arg_mapsize_y = 0;

  for (tmp = mat_bg_head; tmp; tmp = tmp->next)
    {
      arg_mapsize_x = MAX(arg_mapsize_x, tmp->img->w);
      arg_mapsize_y = MAX(arg_mapsize_y, tmp->img->h);
    }

  for (tmp = mat_fg_head; tmp; tmp = tmp->next)
    {
      arg_mapsize_x = MAX(arg_mapsize_x, tmp->img->w);
      arg_mapsize_y = MAX(arg_mapsize_y, tmp->img->h);
    }
}

@

@<Draw image for the world in |obj|@>=
{
  ASSERT(obj->img->w <= sh->un->mapsize_x);
  ASSERT(obj->img->h <= sh->un->mapsize_y);

  MAT_IMAGE_DRAW_ST(self, i, obj->img, startx, starty, mat_stdpal, arg_bg_adj, 1.0);
}

@
@<Add background image form |optarg|@>=
{
  MatImgWorld *tmp;

  tmp = malloc(sizeof(MatImgWorld));
  if (!tmp)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  tmp->path = optarg;

  tmp->next = NULL;
  tmp->prev = mat_bg_tail;

  if (mat_bg_tail)
    mat_bg_tail->next = tmp;
  else
    mat_bg_head = tmp;

  mat_bg_tail = tmp;
}

@* Code for reading PNG images

This is just glue that allows Matanza to read PNG images.  It depends on
libpng.

@<Include files@>=
#if HAVE_PNG_H
#include <png.h>
#endif

@ We have just one function, mat_png_load.  It is given a pathname and
returns a pointer to an image (|MatImage|).  Should an error take place, it
aborts (I think).

@<Function prototypes@>=
MatImage *mat_png_load ( char const *path );

@ The PNG code goes here.  It is only compiled in if they have the PNG
library.

@<Functions@>=

#if HAVE_LIBPNG
@<PNG_Functions@>@;
#else
@<PNG_Wrappers@>@;
#endif

@
@<PNG_Wrappers@>=
MatImage *
mat_png_load ( char const *path )
{
  fprintf(stderr, "%s: Support for loading PNG files not included\n", program_name);
  exit(EXIT_FAILURE);
}

@
@<PNG_Functions@>=
MatImage *
mat_png_load ( char const *path )
{
  png_structp  png_ptr;
  png_infop   info_ptr;
  FILE *fp;

  MatImage *nimg;

  int bd, color;

  fp = fopen(path, "rb");
  if (!fp)
    {
      fprintf(stderr, "%s: %s: %s\n", program_name, path, strerror(errno));
      exit(EXIT_FAILURE);
    }

  @<PNG read: Initialize structs@>;
  @<PNG read: Check the image@>;
  @<PNG read: Set data transformations@>;
  @<PNG read: Read the image@>;
  @<PNG read: Clean up@>;

  return nimg;
}

@
@<PNG read: Check the image@>=
{
  if (color != PNG_COLOR_TYPE_GRAY && color != PNG_COLOR_TYPE_GRAY_ALPHA)
    {
      fprintf(stderr, "%s: Only grayscale PNG files are supported\n", program_name);
      exit(EXIT_FAILURE);
    }
}

@
@<PNG read: Clean up@>=
{
  /* read rest of file, and get additional chunks in info_ptr - REQUIRED */
  png_read_end(png_ptr, info_ptr);

  /* clean up after the read, and free any memory allocated - REQUIRED */
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}

@
@<PNG read: Read the image@>=
{
  int y, len, dst;
  png_bytep row_pointers;

  len = png_get_rowbytes(png_ptr, info_ptr);
  row_pointers = malloc(len);

  ASSERT(color == PNG_COLOR_TYPE_GRAY || color == PNG_COLOR_TYPE_GRAY_ALPHA);

  for (y = dst = 0; y < nimg->h; y++)
    {
      int rd, stop;

      png_read_rows(png_ptr, &row_pointers, NULL, 1);

      for (rd = 0, stop = dst + nimg->w; dst != stop; rd ++)
        {
          png_byte p;
          p = row_pointers[rd];
          nimg->img[dst ++] = p;
        }
    }

  free(row_pointers);
}

@
@<PNG read: Initialize structs@>=
{
  png_uint_32 w, h;
  int dummy;

  /* Create and initialize the png_struct with the desired error handler
   * functions.  If you want to use the default stderr and longjump method,
   * you can supply NULL for the last three parameters.  We also supply the
   * the compiler header file version, so that we know if the application
   * was compiled with a compatible version of the library.  REQUIRED
   */

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!png_ptr)
    return 0;

  /* Allocate/initialize the memory for image information.  REQUIRED. */
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      exit(EXIT_FAILURE);
    }

  if (setjmp(png_ptr->jmpbuf))
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      exit(EXIT_FAILURE);
    }

  png_init_io(png_ptr, fp);

  png_read_info(png_ptr, info_ptr);
  png_get_IHDR(png_ptr, info_ptr, &w, &h, &bd, &color, &dummy, NULL, NULL);

  nimg = malloc(sizeof(MatImage));
  if (!nimg)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  nimg->w = w;
  nimg->h = h;

  nimg->img = malloc(w * h);
  if (!nimg->img)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }
}

@ Set up the data transformations you want.

@<PNG read: Set data transformations@>=
{
  png_color_16 my_background, *image_background;

  /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
   * byte into separate bytes (useful for paletted and grayscale images).
   */
  png_set_packing(png_ptr);

  /* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
  if ((color == PNG_COLOR_TYPE_GRAY && bd < 8) || png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_expand(png_ptr);

  /* Set the background color to draw transparent and alpha images over.
   * It is possible to set the red, green, and blue components directly
   * for paletted images instead of supplying a palette index.  Note that
   * even if the PNG file supplies a background, you are not required to
   * use it - you should use the (solid) application background if it has one.
   */

  if (png_get_bKGD(png_ptr, info_ptr, &image_background))
    png_set_background(png_ptr, image_background, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
  else
    png_set_background(png_ptr, &my_background, PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);

  if (png_set_interlace_handling(png_ptr) != 1)
    {
      fprintf(stderr, "%s: %s: Interlaced PNGs are not supported\n", program_name, path);
      exit(EXIT_FAILURE);
    }

  /* Optional call to gamma correct and add the background to the palette
   * and update info structure.  REQUIRED if you are expecting libpng to
   * update the palette for you (ie you selected such a transform above).
   */

  png_read_update_info(png_ptr, info_ptr);
}

@* Universes

In this section we will handle information about the universe.

@
@<libxml: Set the universe's name@>=
{
  char *tmp;
  tmp = xmlGetProp(doc->root, "name");
  strncpy(un->name, tmp ? tmp : "Unnamed Universe", MAX_UNIVERSE_NAME);
}

@

@d SET_CRASH_DAMAGE(obj, name)
{
  char *tmp;
  tmp = xmlGetProp(doc->root, name);
  un->crash_damage[obj] = tmp ? atoi(tmp) : 0;
}

@<libxml: Set the universe's damage when crashing other ships@>=
{
  SET_CRASH_DAMAGE(MAT_MOVING_OBJ_SHIP, "shipcrash");
}

@
@<libxml: Set the universe's map size@>=
{
  char *tmpx, *tmpy;

  tmpx = xmlGetProp(doc->root, "mapsizex");
  un->mapsize_x = tmpx ? atoi(tmpx) : arg_mapsize_x;

  tmpy = xmlGetProp(doc->root, "mapsizey");
  un->mapsize_y = tmpy ? atoi(tmpy) : arg_mapsize_y;
}

@
@<libxml: Set the universe's dots@>=
{
  char *tmp;

  tmp = xmlGetProp(doc->root, "dots");

  if (!tmp)
    un->dots = arg_bg_dots;
  else if (!strcasecmp(tmp, "yes"))
    un->dots = 1;
  else if (!strcasecmp(tmp, "no"))
    un->dots = 0;
  else
    un->dots = arg_bg_dots;
}

@
@<libxml: Set the universe's ships' initial health@>=
{
  char *tmp;

  tmp = xmlGetProp(doc->root, "health");
  un->health = tmp ? atoi(tmp) : arg_health;
}

@
@<Set the default parameters for |un|@>=
{
  int i;

  ASSERT(un);

  un->players_head = un->players_tail = NULL;
  un->bullets = un->ast = un->objs = un->smart_objs = NULL;

  un->objs_rm = (void *) &main;

  un->players_real = un->ast_cur = 0;
  un->pause = 0;

  un->subs = NULL;

  un->updates = 0;

  for (i = 0; i < 256; i ++)
    un->location[i] = NULL;

  ASSERT(sizeof("Default Universe") + 1 < MAX_UNIVERSE_NAME);

  strcpy(un->name, "Default Universe");

  un->mapsize_x = arg_mapsize_x;
  un->mapsize_y = arg_mapsize_y;

  un->dots   = arg_bg_dots;
  un->health = arg_health;
}

@
@<libxml: Read the universe@>=
{
  xmlDocPtr    doc;
  xmlNodePtr   node;

  ASSERT(un);
  ASSERT(un->path);

  doc = xmlParseFile(un->path);

  if (!doc || !doc->root || !doc->root->name || strcasecmp(doc->root->name, "Universe"))
    {
      fprintf(stderr, "%s: %s: Corrupted Universe\n", program_name, un->path);
      exit(EXIT_FAILURE);
    }

  @<Set the default parameters for |un|@>;

  @<libxml: Set the universe's name@>;
  @<libxml: Set the universe's map size@>;
  @<libxml: Set the universe's dots@>;
  @<libxml: Set the universe's ships' initial health@>;

  un->bg.src   = xmlGetProp(doc->root, "background");
  un->mask.src = xmlGetProp(doc->root, "mask");

  MAT_UNIVERSE_LOCATIONS_SETALL(un, NULL);

  for (node = doc->root->childs; node; node = node->next)
    {
      if (!strcasecmp(node->name, "Location"))
        {
          int color;

          @<libxml: Get color for the current location@>;

          if (un->location[color])
            free(un->location[color]);

          un->location[color] = malloc(sizeof(MatLocation));
          if (!un->location[color])
            {
              fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
              exit(EXIT_FAILURE);
            }

          @<libxml: Set parameters for the current location@>;
        }
      else
        fprintf(stderr, "%s: %s: %s: Ignoring tag\n", program_name, un->path, node->name);
    }

  @<Set |NULL| locations in |un| to the default@>;

#if 0
  <!ATTLIST Location color  "0">
  <!ATTLIST Location crash  (yes|no) "no">
  <!ATTLIST Location damage CDATA "0">
  <!ATTLIST Location friction CDATA "0">

  <!ATTLIST Universe brake      CDATA "0">
#endif
}

@

@d LIBXML_MULTIPLE_BOOL(type, prop, name)
{
  char *tmp;

  tmp = xmlGetProp(node, name);
  un->location[color]->prop[type] = tmp && !strcasecmp(tmp, "yes");
}

@d LIBXML_MULTIPLE_FLOAT(type, prop, name, default)
{
  char *tmp;

  tmp = xmlGetProp(node, name);
  un->location[color]->prop[type] = tmp ? atof(tmp) : default;
}


@<libxml: Set parameters for the current location@>=
{
  char *tmp_health;

  LIBXML_MULTIPLE_BOOL(MAT_MOVING_OBJ_SHIP,   crash, "shipcrash");
  LIBXML_MULTIPLE_BOOL(MAT_MOVING_OBJ_BULLET, crash, "astcrash");
  LIBXML_MULTIPLE_BOOL(MAT_MOVING_OBJ_AST,    crash, "bulletcrash");

  LIBXML_MULTIPLE_FLOAT(MAT_MOVING_OBJ_SHIP,   move, "shipmove",   1.0);
  LIBXML_MULTIPLE_FLOAT(MAT_MOVING_OBJ_BULLET, move, "astmove",    1.0);
  LIBXML_MULTIPLE_FLOAT(MAT_MOVING_OBJ_AST,    move, "bulletmove", 1.0);

  tmp_health   = xmlGetProp(node, "health");
  un->location[color]->health = tmp_health ? atoi(tmp_health) : 0;
}

@
@<Set |NULL| locations in |un| to the default@>=
{
  int i;
  for (i = 0; i < 256; i ++)
    if (un->location[i] == NULL)
      un->location[i] = &mat_location_default;
}

@

@d MAT_UNIVERSE_LOCATIONS_SETALL(un, val)
{
  int i;
  for (i = 0; i < 256; i ++)
    un->location[i] = val;
}

@
@<libxml: Get color for the current location@>=
{
  char *tmp;

  tmp = xmlGetProp(node, "color");

  if (tmp)
    {
      color = atoi(tmp);
      if (color < 0)
        color = 0;
      else if (color > 255)
        color = 255;
    }
  else
    color = 0;
}

@
TODO: Give verbose descriptions of the problems.

@<Fix problems in universe |un|@>=
{
  if (un->bg)
    {
      if (un->bg->w > un->mapsize_x)
        un->mapsize_x = un->bg->w;

      if (un->bg->h > un->mapsize_y)
        un->mapsize_y = un->bg->h;
    }
}

@* Portability Code

In this section we will define different ways to do things that are not
portable among our target operating systems.

The aim of this section is to make it easier to write code elsewhere, hopefully
isolating nonportable code.

@ For the moment, we are using |recv| everywhere, as |read| seems to be
unavailable on Windows.  We will leave the code that uses |read| around
in case we find any reasons to switch back to it on Unix.

@<Read data for player |con|@>=
  @<Use |recv| to read data for player |con|@>;

@
@<Use |recv| to read data for player |con|@>=
{
  int read_len;
  int got_input    = 0;
  int keep_reading = 1;

  VERBOSE("-=> recv cycle.\n");
  while (keep_reading)
    {
      VERBOSE("-=> recv\n");
      read_len = recv(con->fd, con->buffer + con->buflen, con->bufsiz - con->buflen, 0);
      VERBOSE("<=- recv\n");

      switch (read_len)
        {
        case SOCKET_ERROR:
          switch (SOCKET_ERRNO())
            {
            case SOCKET_CALL_INTR:

              break;

            @<Cases for errors when the connection is closed@>

              if (con->state == MAT_STATE_PLAYING)
                MESSAGE_FROM(con, "has gone netdead");

              mat_connection_free(con, MMP_SERVER_CLOSE_SYSERROR, strerror(SOCKET_ERRNO()));

              return;

            @<Cases for no more input ready@>

              keep_reading = 0;
              break;

            default:

              fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
              exit(EXIT_FAILURE);
            }
          break;
        case 0:

          keep_reading = 0;
          break;

        default:

          ASSERT(read_len > 0);
          got_input = 1;
          con->buflen += read_len;
          con->buffer[con->buflen] = 0;
        }
    }
  VERBOSE("<=- recv cycle\n");

  if (!got_input)
    return;
}

@ Here goes a list of errors when we try to read from a socket that mean the
connection was closed.

@<Cases for errors when the connection is closed@>=
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

@ Here we add a list of all the errors that happen when no more input is
available and we try to read from a socket.

@<Cases for no more input ready@>=
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

@ In the following code we call |read| to fill the buffer of player |p|
with incomming data.  The following situations are possible:

\item{$\bullet$} \bf Error |EINTR| takes place.\rm If the call was
interrupted by a signal, we try again.  This is very unlikely since the
socket is non-blocking, but you never know.

\item{$\bullet$} \bf Error |EAGAIN| takes place or |read| returns |0|.\rm
This means no more data is available to be read.  We leave the slot as it
is and return.

\item{$\bullet$} \bf Error |ECONNRESET| or |EPIPE| takes place.\rm If the
remote server closes the connection, we mark the request as either failed
or successful.

\item{$\bullet$} \bf No error is detected.\rm The data was read, we do
nothing in this case.

\item{$\bullet$} \bf An unknown error is detected.\rm Since \.{Matanza}
doesnt know how to handle the error, it terminates.

@<Use |read| to read data for player |con|@>=
{
  int read_len;

  do
    {
      VERBOSE("-=> read\n");
      read_len = read(con->fd, con->buffer + con->buflen, con->bufsiz - con->buflen);
      VERBOSE("<=- read\n");
    }
  while (read_len == -1 && errno == EINTR);

  switch (read_len)
    {
    case -1:
      switch (errno)
        {
#ifdef ECONNRESET
        case ECONNRESET:
#endif
#ifdef EPIPE
        case EPIPE:
#endif
          VERBOSE("read failed: connection closed\n");
          if (con->state == MAT_STATE_PLAYING)
            MESSAGE_FROM(con, "has gone netdead");
          mat_connection_free(con, MMP_SERVER_CLOSE_SYSERROR, strerror(errno));
          return;

        case EAGAIN:
          VERBOSE("no more data is available\n");
          return;

        default:
          fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
          exit(EXIT_FAILURE);
        }
      break;
    case 0:
      VERBOSE("no more data is available\n");
      return;
    }

  VERBOSE("read data [len:%d]\n", read_len);
  con->buflen += read_len;
  con->buffer[con->buflen] = 0;
}

@ Windows and BeOS lack usleep.  We need to find alternatives on them.

@<Define |USLEEP_GENERIC|@>=

#if HAVE_USLEEP
#  define USLEEP_GENERIC(x) \
   { VERBOSE("-=> usleep\n"); usleep(x); VERBOSE("<=- usleep\n"); }
#elif HAVE_SNOOZE_UNTIL
#  define USLEEP_GENERIC(x) USLEEP_SNOOZE_UNTIL(x)
#elif HAVE_SLEEP
#  define USLEEP_GENERIC(x) \
   { VERBOSE("-=> Sleep(%d)\n", x / 1000); Sleep(x / 1000); VERBOSE("<=- Sleep\n"); }
#else
#  error Could not find way to implement USLEEP_GENERIC.
#endif

@
@d USLEEP_SNOOZE_UNTIL(xusec)
{
  bigtime_t wake_at;
  status_t  status;

  wake_at = system_time() + xusec;

  VERBOSE("-=> snooze_until cycle\n");
  do
    {
      VERBOSE("-=> snooze_until\n");
      status = snooze_until(wake_at, B_SYSTEM_TIMEBASE);
      VERBOSE("<=- snooze_until\n");
    }
  while (status == EINTR);
  VERBOSE("<=- snooze_until cycle\n");
}

@ Here we define |SOCKET_CLOSE_GENERIC| depending on the operating system we
are being compiled on.

@<Define |SOCKET_CLOSE_GENERIC|@>=
#if HAVE_CLOSESOCKET
#  define SOCKET_CLOSE_GENERIC(s) closesocket(s)
#else
#  define SOCKET_CLOSE_GENERIC(s) close(s)
#endif

@ Here we check to see how can get we get the current millisecond.

@<Define |GETMICROSECOND_GENERIC|@>=
#if HAVE_SYSTEM_TIME
#  ifdef B_SYSTEM_TIMEBASE
#    ifdef EINTR
#      define HAVE_SYSTEM_TIME_CONST 1
#    else
#      define HAVE_SYSTEM_TIME_CONST 0
#    endif
#  else
#    define HAVE_SYSTEM_TIME_CONST 0
#  endif
#else
#  define HAVE_SYSTEM_TIME_CONST 0
#endif

/* If you get an error on the following line, there are some include files
 * missing.  Basically, we are on BeOS but we can't get B_SYSTEM_TIMEBASE or
 * EINTR. */

#if __BEOS__
#  ifndef EINTR
#    error Missing header files for BeOS: EINTR undefined
#  endif
#  ifndef B_SYSTEM_TIMEBASE
#    error Missing header files for BeOS: B_SYSTEM_TIMEBASE undefined
#  endif
#endif

#if HAVE_SYSTEM_TIME
#  define GETMICROSECOND_GENERIC(xx) GETMICROSECOND_SYSTEM_TIME(xx)
#elif HAVE_GETTIMEOFDAY
#  define GETMICROSECOND_GENERIC(xx) GETMICROSECOND_GETTIMEOFDAY(xx)
#elif HAVE_GETTICKCOUNT
#  define GETMICROSECOND_GENERIC(xx) GETMICROSECOND_GETTICKCOUNT(xx)
#else
#  error Could not find a way to implement GETMICROSECOND_GENERIC.
#endif

@
@d GETMICROSECOND_SYSTEM_TIME(xx)
{
  xx = (unsigned long) system_time();
}

@
@d GETMICROSECOND_GETTICKCOUNT(xx)
{
  xx = (unsigned long) GetTickCount() * 1000;
}

@

@d GETMICROSECOND_GETTIMEOFDAY(xx)
{
  struct timeval st;

  gettimeofday(&st, NULL);

  xx = (unsigned long) (st.tv_sec * 1000000 + st.tv_usec);
}

@ Here we check to see how can we make the socket nonblocking and execute
the appropriate code.

@<Define |NONBLOCKING_GENERIC|@>=
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

#if HAVE_SETSOCKOPT_NONBLOCK
#  define NONBLOCKING_GENERIC(result) NONBLOCKING_SETSOCKOPT(result)
#elif HAVE_IOCTLSOCKET
#  define NONBLOCKING_GENERIC(result) NONBLOCKING_IOCTLSOCKET(result)
#elif HAVE_FCNTL_NONBLOCK
#  define NONBLOCKING_GENERIC(result) NONBLOCKING_FCNTL(result)
#else
#  error Could not find a way to implement NONBLOCKING_GENERIC.
#endif

@
@d NONBLOCKING_IOCTLSOCKET(result)
{
  int dummy = 1;

  VERBOSE("ioctlsocket [fd:%d][option:FIONBIO]\n", result);
  if (ioctlsocket(result, FIONBIO, &dummy) == SOCKET_ERROR)
    {
      /* TODO: Make sure it's ok to call strerror like this. */
      fprintf(stderr, "%s: ioctlsocket: %s\n", strerror(SOCKET_ERRNO()));
      exit(EXIT_FAILURE);
    }
}

@d NONBLOCKING_SETSOCKOPT(result)
{
  char on = 1;
  VERBOSE("setsockopt [fd:%d][option:SO_NONBLOCK]\n", result);
  if (setsockopt(result, SOL_SOCKET, SO_NONBLOCK, (void *) &on, sizeof(char)) == -1)
    {
      fprintf(stderr, "%s: setsockopt: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }
}

@
@d NONBLOCKING_FCNTL(result)
{
  int opts;

  VERBOSE("fcntl [fd:%d][option:O_NONBLOCK]\n", result);
  opts = fcntl(result, F_GETFL);
  if (opts < 0)
    {
      fprintf(stderr, "%s: fcntl: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  opts = (opts | O_NONBLOCK);
  if (fcntl(result, F_SETFL, opts) < 0)
    {
      fprintf(stderr, "%s: fcntl: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }
}

@
@<Function prototypes@>=
MatSocketT mat_listen ( int port );

@
@<Functions@>=
MatSocketT
mat_listen ( int port )
{
  struct sockaddr_in in;
  MatSocketT result;
  int on = 1;

  VERBOSE("Listening on port %d...", port);
  fflush(stdout);

  memset(&in, 0, sizeof(struct sockaddr_in));
  in.sin_family = AF_INET;
  in.sin_addr.s_addr = INADDR_ANY;
  in.sin_port = htons(port);

#if HAVE_WSASOCKET
  result = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
#else
  result = socket(AF_INET, SOCK_STREAM, 0);
#endif

  if (result == SOCKET_ERROR)
    {
      @<Display error on call to |socket|@>;
      exit(EXIT_FAILURE);
    }

#if HAVE_SETSOCKOPT
#  ifdef SOL_SOCKET
#    ifdef SO_REUSEADDR
  VERBOSE("setsockopt [fd:%d][option:SO_REUSEADDR]\n", result);
  setsockopt(result, SOL_SOCKET, SO_REUSEADDR, (void *) &on, sizeof(on));
#    endif
#  endif
#endif

  if (bind(result, (struct sockaddr*)&in, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
    {
      fprintf(stderr, "%s: bind: %s\n", program_name, strerror(SOCKET_ERRNO()));

      exit(EXIT_FAILURE);
    }

  if (listen(result, 10) == SOCKET_ERROR)
    {
      exit(EXIT_FAILURE);
      fprintf(stderr, "%s: listen: %s\n", program_name, strerror(SOCKET_ERRNO()));
    }

  /* Make socket nonblocking. */
  NONBLOCKING_GENERIC(result);

  VERBOSE(" Done\n");

  return result;
}

@ There seems to be no portable way of describing an error.

On Windows, we have to check for every individual error.  We only check for
those we think might happen.

@<Display error on call to |socket|@>=
#if WIN32
  switch (SOCKET_ERRNO())
  {
  case WSAEMFILE:
    fprintf(stderr, "%s: socket: No more socket descriptors are available.\n", program_name);
    break;
  default:
    fprintf(stderr, "%s: socket: Unknown error (code %d)\n", program_name, SOCKET_ERRNO());
  }
#else
  fprintf(stderr, "%s: socket: %s\n", program_name, strerror(SOCKET_ERRNO()));
#endif

@ Portability information goes here.

@<Portability@>=

#ifdef WIN32
#  include <winconfig.h>
#else /* !WIN32 */
#  ifdef HAVE_CONFIG_H
#    include <config.h>
#  else /* !HAVE_CONFIG_H */
#    define PACKAGE "matanza"
#    define VERSION "{Unknown-Version}"
#  endif /* !HAVE_CONFIG_H */
#endif /* !WIN32 */

@<Include files@>

/* Here we find out a way to build nonblocking sockets. */
@<Define |NONBLOCKING_GENERIC|@>

/* Here we find out a way to get current millisecond. */
@<Define |GETMICROSECOND_GENERIC|@>
 
/* Here we find out how to close a socket. */
@<Define |SOCKET_CLOSE_GENERIC|@>

/* Here we find out how to sleep for a given time in microseconds. */
@<Define |USLEEP_GENERIC|@>

/* In windows, the type SOCKET is used for sockets.  int is used everywhere
 * else.  In this program, we use MatSocketT.  Here we define it as
 * appropriate. */

#ifdef WIN32
#define MatSocketT SOCKET
#else /* ! WIN32 */
#define MatSocketT int
#endif /* ! WIN32 */

/* Windows returns SOCKET_ERROR when an error with a socket operation takes
 * place.  UNIX returns -1.  Windows forces us to use SOCKET_ERROR
 * everywhere. */

#ifndef WIN32
#define SOCKET_ERROR -1
#endif
 
/* To get the reason why a socket call failed we simply look errno in UNIX.
 * On Windows, we are forced to call WSAGetLastError.  We define SOCKET_ERRNO
 * appropriately. */

#ifdef WIN32
#define SOCKET_ERRNO() WSAGetLastError()
#else
#define SOCKET_ERRNO() errno
#endif

/* On UNIX, when a call is interrupted by a signal, SOCKET_ERRNO() returns
 * EINTR.  On Windows, SOCKET_ERRNO() returns WSAEINTR.  We define the
 * constant SOCKET_CALL_INTR to the appropriate value. */

#ifdef WIN32
#  define SOCKET_CALL_INTR WSAEINTR
#else
#  define SOCKET_CALL_INTR EINTR
#endif

#ifndef MAX
#  define MAX(x,y) ((x)>(y)?(x):(y))
#endif

#ifndef MIN
#  define MIN(x,y) ((x)<(y)?(x):(y))
#endif

@ On Windows we need to call WSAStartup() before any sockets-related
functions.

@<Initialize the sockets system@>=
{
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
}

@* Socket output functions.

In this section we will define three functions useful to perform buffered
output to sockets.

In earlier versions, we used fdopen and the C library's fprintf and similar
functions.  It turned out it wasn't very portable so we had to write our
replacement functions.

|mat_printf| works like a printf, but only accepts the |%c| and |%s| format
options.  It has a restriction on the maximum number of strings that can be
print on a given call.

|mat_write| works like a fwrite.

|mat_flush| calls |send| to send the contents of the buffer through the socket.
It is harmless to call |mat_flush| when the buffer is empty: it checks for said
condition and does nothing if it evaluates to true.

@<Function prototypes@>=
int                mat_printf                ( MatConnection       *con,
                                               char                *format,
                                               ... );

int                mat_write                 ( MatConnection       *con,
                                               void                *buffer,
                                               size_t               len );

int                mat_out_flush             ( MatConnection       *con );

@ I think SOCKET_CALL_INTR will never happened on non-blocking sockets, but
lets be safe.

@<Functions@>=
int
mat_out_flush ( MatConnection *con )
{
  int retval;

  ASSERT(con);

  if (con->out_len == 0)
    return 1;

  ASSERT(con->out_buf);

#ifdef MSG_NOSIGNAL
#  define MAT_SEND_FLAGS MSG_NOSIGNAL
#else
#  define MAT_SEND_FLAGS 0
#endif

send_retry:

  VERBOSE("-=> send\n");
  retval = send(con->fd, con->out_buf, con->out_len, MAT_SEND_FLAGS);
  VERBOSE("<=- send\n");

  if (retval == SOCKET_ERROR)
    {
    switch (SOCKET_ERRNO())
      {
      case SOCKET_CALL_INTR:
        goto send_retry;

      case EPIPE:
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
#  if ECONNABORTED
      case ECONNABORTED:
#  endif
#endif
        VERBOSE("send failed: connection closed\n");
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
        VERBOSE("send failed: player lagging\n");
        con->info.t.clean = 0;
        break;

      default:
        fprintf(stderr, "%s: send: Unknown error (code %d)\n", program_name, SOCKET_ERRNO());
        exit(EXIT_FAILURE);
      }
    }

  VERBOSE("send successful\n");
  con->out_len = 0;

  return 1;
}

@
@<Functions@>=
int
mat_write ( MatConnection *con, void *buffer, size_t len )
{
  @<Make sure the output buffer in |con| has space@>;
  memcpy(con->out_buf + con->out_len, buffer, len);
  con->out_len += len;

  return 1;
}

@
@<Functions@>=
int
mat_printf ( MatConnection *con, char *format, ... )
{
  int slen[10];
  int len;

  ASSERT(con);
  ASSERT(format);

  @<Find the lengths of what they are printing@>;
  @<Make sure the output buffer in |con| has space@>;
  @<Print the arguments to the buffer@>;

  return 1;
}

@
@<Print the arguments to the buffer@>=
{
  char   *write_buf;
  int     strcur = 0;
  va_list arg;

  write_buf = con->out_buf + con->out_len;

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
          default:
            ASSERT(0);
          }
        format++;
      }
    else
      *(write_buf++) = *(format++);

  va_end(arg);

  ASSERT(write_buf >= con->out_buf);

  con->out_len = write_buf - con->out_buf;

  ASSERT(con->out_len >= 0);
}

@
@<Make sure the output buffer in |con| has space@>=
{
  int nlen;

  nlen = con->out_len + len;

  if (nlen >= con->out_siz)
    {
      char *nbuf;
      int   nsiz;

      nsiz = 2048 + nlen;
      nbuf = realloc(con->out_buf, nsiz);
      if (!nbuf)
        {
          fprintf(stderr, "%s: realloc: %s\n", program_name, strerror(errno));
          exit(EXIT_FAILURE);
        }

      VERBOSE("Resized buffer: %d\n", nsiz);

      con->out_buf = nbuf;
      con->out_siz = nsiz;
    }
}

@
@<Find the lengths of what they are printing@>=
{
  va_list  arg;
  char    *tmp;
  int      strcur = 0;
  char     dummy_c;
  int      dummy_d;

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
          dummy_d = va_arg(arg, int);
          len += 10;
          break;

        case 'c':
          len ++;
          dummy_c = (char) va_arg(arg, int);
          break;

        case 's':
          /* If you get an assert fail in next line, increase the length of slen. */
          ASSERT(strcur < sizeof(slen) / sizeof(int));
          slen[strcur] = strlen(va_arg(arg, char *));
          len += slen[strcur++];
          break;

        default:
          ASSERT(0);
        }
    else
      len ++;

  va_end(arg);
}

@* Here we will use a structure to efficently keep track of what machines are
subscribed to what universes.

It has to perform the following operations as fast as possible:

- Return a list of all the machines in a given universe.
- Return a list of all the universes for a given machine.
- Get rid of all the subscriptions for a given machine.
- Get rid off a given subscription for one universe for one machine.

@<Function prototypes@>=
int               mat_sub_add                      ( MatConnection         *m,
                                                     MatUniverse           *un );
void              mat_sub_machine_rm               ( MatConnection         *m );

@
@<Typedefs@>=
typedef struct MatSub MatSub;

@ The natural solution for this is a doubly linked list that can be traversed
by the universe (to get all the machines in a universe) or by the machine (to
get all the machines in a universe).

|m_prev| and |m_next| point to the node for the previous and next subscription
for the current machine.  Actually, not the next node but the next node in the
current list in the hash of subscriptions for the machine.

|u_prev| and |u_next| point to the node for the previous and next subscription
for the current universe.

|m| and |u| point to the current machine and universe.

@<Struct for one subscription@>=
struct MatSub
{
  MatConnection       *m;

  MatSub              *m_prev;
  MatSub              *m_next;

  MatUniverse         *u;

  MatSub              *u_prev;
  MatSub              *u_next;
};

@ Since we want to be able to erase a subscription for one machine and
universe, lets make a hash for all the subscriptions in a given machine.  The
code in this major section assumes that it is included in every machine as the
field |subs|.

We use an incredibly small value for the hash size since, for the moment, we
only support one universe.

TODO: Once support for multiple universes is added, increase the size of the
hash.

@d MAT_SUB_MACHINE_HASH_SIZE 1

@<Struct for the subscriptions in one machine@>=
typedef MatSub *MatSubMachine [MAT_SUB_MACHINE_HASH_SIZE];

@ Lets make a function to get rid of one given subscription.

This function is only called by code inside this section.

|s| is the subscription you want to get rid off. |hf| is the result of applying
the hashing function to the universe's id.

@<Function prototypes@>=

void              mat_sub_rm                       ( MatSub                *s,
                                                     int                    hf );

@ And now the actual implementation.

@d MAT_SUB_RM_U(s)
{
  if (s->u_prev)
    s->u_prev->u_next = s->u_next;
  else
    s->u->subs = s->u_next;

  if (s->u_next)
    s->u_next->u_prev = s->u_prev;
}

@<Functions@>=
void
mat_sub_rm ( MatSub *s, int hf )
{
  if (s->m_prev)
    s->m_prev->m_next = s->m_next;
  else
    s->m->info.m.subs[hf] = s->m_next;

  if (s->m_next)
    s->m_next = s->m_prev;

  MAT_SUB_RM_U(s);

  free(s);
}

@ Now lets make a function to get rid of all the subscriptions of a given
machine.

@<Functions@>=
void
mat_sub_machine_rm ( MatConnection *m )
{
  MatSub *c;
  int i;

  ASSERT(m->state == MAT_STATE_MACHINE);

  for (i = 0; i < MAT_SUB_MACHINE_HASH_SIZE; i ++)
    {
      c = m->info.m.subs[i];
      while (c)
        {
          MatSub *tmp;

          MAT_SUB_RM_U(c);

          tmp = c;

          c = tmp->m_next;

          free(tmp);
        }
    }
}

@ And now lets make a function that subscribes a machine to a universe
(depending on its id).

@<Functions@>=
int
mat_sub_add ( MatConnection *m, MatUniverse *un )
{
  MatSub *c;
  int     pos;

  c = malloc(sizeof(MatSub));
  if (!c)
    return 0;

  c->u = un;
  c->m = m;

  c->u_prev = NULL;
  c->u_next = un->subs;

  if (un->subs)
    un->subs->u_prev = c;

  un->subs = c;

  pos = un->id % MAT_SUB_MACHINE_HASH_SIZE;

  c->m_prev = NULL;
  c->m_next = m->info.m.subs[pos];

  if (m->info.m.subs[pos])
    m->info.m.subs[pos]->m_prev = c;

  m->info.m.subs[pos] = c;

  return 1;
}

@ Lets now make a macro to initialize the subscription of a machine.

@d MAT_SUB_MACHINE_INIT(xm)
{
  int mat_sub_machine_init = 0;
  while (mat_sub_machine_init < MAT_SUB_MACHINE_HASH_SIZE)
    xm->info.m.subs[mat_sub_machine_init++] = NULL;
}

@* Trigonometry Functions

During program execution, we will need the sine and cosine of the normal
angles.  Rather than calculate them everytime, we will calculate them on all
the standard angles (from 0 to |arg_ang|) and hold the results in the variables
|mat_cos| and |mat_sin|.

@<Global variables@>=

static double       *mat_cos;
static double       *mat_sin;

@ Here we calculate the values for all the angles we are interested on.

@<Initialize the caches for results of trigonometric functions@>=
{
  int i;

  mat_cos = malloc(sizeof(double) * arg_ang);
  mat_sin = malloc(sizeof(double) * arg_ang);
  if (!mat_cos || !mat_sin)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  for (i = 0; i < arg_ang; i ++)
    {
      double ang;

      ang = PI * 2.0 * (double) i / (double) arg_ang;
      mat_cos[i] = cos(ang);
      mat_sin[i] = sin(ang);
    }
}

@ Finally, here we destroy all our caches.

@<Destroy the caches for results of trigonometric functions@>=
{
  free(mat_cos);
  free(mat_sin);
}

@* Images

In this section we will put the high-level code that deals with images.

We define the following functions (actually, we define more, but all the others
are meant to be used internally by the code in this section):

@d MAT_IMAGE_DRAW_ST(xs, xsh, xi, startx, starty, mat_stdpal, arg_bg_adj, alpha)
  mat_image_draw(xs, xsh, xi, 0, 0, 0, 0, startx, starty, mat_stdpal, arg_bg_adj, alpha)

@d MAT_IMAGE_DRAW_DY(xs, xsh, xi, x, y, p, startx, starty, xadj)
  mat_image_draw(xs, xsh, xi, x, y, 
                 xsh->sp_x - p->sp_x, xsh->sp_y - p->sp_y,
                 startx, starty, p->pal, xadj, p->alpha)

@<Function prototypes@>=

void               mat_image_draw             ( MatConnection       *self,
                                                MatMovingObj        *ship,
                                                MatImage           *img,
                                                int                 x,
                                                int                 y,
                                                double              sp_x,
                                                double              sp_y,
                                                int                 startx,
                                                int                 starty,
                                                unsigned char      *pal,
                                                int                 adj,
                                                double              alpha );

void               mat_image_optimize         ( MatImage           *img );

@ Here we draw |img| in coordinates |x| and |y| to player |self|.

|pos_x| and |pos_y| hold the beginning of the image, relative to the beginning
of the screen of the player.

|sp| is the speed squared.

@<Functions@>=
void
mat_image_draw ( MatConnection *self, MatMovingObj *ship, MatImage *img,
                 int x, int y, double sp_x, double sp_y,
                 int startx, int starty, unsigned char *pal, int adj, double alpha )
{
  double height, width, sp;
  double rate_x, rate_y, pos_x, pos_y;
  int size_x, size_y;

  ASSERT(ship);
  ASSERT(ship->type == MAT_MOVING_OBJ_SHIP);

  ASSERT(img);
  ASSERT(img->w > 0);
  ASSERT(img->h > 0);

  ASSERT(ship->info.player.world_wi > 0);
  ASSERT(ship->info.player.imgwi > 0);

  if (alpha <= 0.0)
    return;

  if (ship->info.player.world_wi != ship->info.player.imgwi)
    {
      rate_x = (double)ship->info.player.world_wi / (double)ship->info.player.imgwi;
      width  = (double)img->w / rate_x;
    }
  else
    {
      rate_x = 1;
      width  = img->w;
    }

  ASSERT(rate_x > 0);

  ASSERT(ship->info.player.world_he > 0);
  ASSERT(ship->info.player.imghe > 0);

  if (ship->info.player.world_he != ship->info.player.imghe)
    {
      rate_y = (double)ship->info.player.world_he / (double)ship->info.player.imghe;
      height = (double)img->h / rate_y;
    }
  else
    {
      rate_y = 1;
      height = img->h;
    }

  ASSERT(rate_y > 0);

  ASSERT(img->w <= ship->un->mapsize_x);
  ASSERT(img->h <= ship->un->mapsize_y);

  if (adj)
    {
      double rate_x, rate_y;

      size_x = img->w;
      size_y = img->h;

      rate_x = (double) size_x / (double) ship->un->mapsize_x;
      rate_y = (double) size_y / (double) ship->un->mapsize_y;

      x      = (double) x      * rate_x;
      y      = (double) y      * rate_y;
      startx = (double) startx * rate_x;
      starty = (double) starty * rate_y;
    }
  else
    {
      size_x = ship->un->mapsize_x;
      size_y = ship->un->mapsize_y;
    }

  ASSERT(img->w <= size_x);
  ASSERT(img->h <= size_y);

  ASSERT(0 <= startx);
  ASSERT(startx < size_x);
  ASSERT(0 <= starty);
  ASSERT(starty < size_y);

  ASSERT(0 <= x);
  ASSERT(x < size_x);
  ASSERT(0 <= y);
  ASSERT(y < size_y);

  pos_x = x - startx;
  pos_y = y - starty;

  ASSERT(-size_x < (int)pos_x);
  ASSERT(-size_y < (int)pos_y);
  ASSERT((int)pos_x < size_x);
  ASSERT((int)pos_y < size_y);

  if (0 < (int)pos_x)
    pos_x -= size_x;
  if (0 < (int)pos_y)
    pos_y -= size_y;

  ASSERT((int)pos_x <= 0);
  ASSERT((int)pos_y <= 0);

  ASSERT(-size_x < (int)pos_x);
  ASSERT(-size_y < (int)pos_y);

  sp = sp_x * sp_x + sp_y * sp_y;

  if (!arg_motion_blur || sp < 1.0 / (double) STEPS_MOVEMENT)
    {
      if (alpha >= 1.0)
        @<Draw image without alpha@>@;
      else
        @<Draw image with current alpha@>@;
    }
  else
    @<Draw image with motion blur@>@;
}

@
@<Draw image with motion blur@>=
{
  int steps;
  int i;

  sp = sqrt(sp);

  steps = sp * STEPS_MOVEMENT;
  alpha = alpha / (double) steps;

  for (i = 0; i < steps; i ++)
    {
      @<Draw image with current alpha@>;
      @<Update the positions of the image for the motion blur@>;
    }
}

@
@<Draw image with current alpha@>=
{
#define MAT_IMAGE_DRAW_ALPHA(xx, yy) \
  mat_image_draw_alpha(self, ship, img, xx, yy, rate_x, rate_y, width, height, pal, alpha)

  ASSERT((int)pos_x <= 0);
  ASSERT((int)pos_y <= 0);

  ASSERT(-size_x < (int)pos_x);
  ASSERT(-size_y < (int)pos_y);

  if (0 < (int) pos_x + img->w)
    {
      if (0 < (int) pos_y + img->h)
        MAT_IMAGE_DRAW_ALPHA(pos_x,          pos_y         );

      if ((int) pos_y + size_y < ship->info.player.world_he)
        MAT_IMAGE_DRAW_ALPHA(pos_x,          pos_y + size_y);
    }

  if ((int) pos_x + size_x < ship->info.player.world_wi)
    {
      if (0 < (int) pos_y + img->h)
        MAT_IMAGE_DRAW_ALPHA(pos_x + size_x, pos_y         );

      if ((int) pos_y + size_y < ship->info.player.world_he)
        MAT_IMAGE_DRAW_ALPHA(pos_x + size_x, pos_y + size_y);
    }
}

@
@<Update the positions of the image for the motion blur@>=
{
  ASSERT(-size_x < (int) pos_x);
  ASSERT((int) pos_x <= 0);

  ASSERT(ABS(sp_x) <= size_x);

  pos_x += sp_x;

  ASSERT(2 * -size_x < (int) pos_x);
  ASSERT((int) pos_x <= size_x);

  if ((int) pos_x <= -size_x)
    pos_x += size_x;
  else if (0 < (int)pos_x)
    pos_x -= size_x;

  ASSERT(-size_x < (int) pos_x);
  ASSERT((int) pos_x <= 0);

  ASSERT(-size_y < (int) pos_y);
  ASSERT((int) pos_y <= 0);

  ASSERT(ABS(sp_y) <= size_y);

  pos_y += sp_y;

  ASSERT(2 * -size_y < (int) pos_y);
  ASSERT((int) pos_y <= size_y);

  if ((int) pos_y <= -size_y)
    pos_y += size_y;
  else if (0 < (int)pos_y)
    pos_y -= size_y;

  ASSERT(-size_y < (int) pos_y);
  ASSERT((int) pos_y <= 0);
}

@
@<Draw image without alpha@>=
{
#define MAT_IMAGE_DRAW_FAST(xx, yy) \
  mat_image_draw_fast(self, ship, img, xx, yy, rate_x, rate_y, width, height, pal)

  if (0 < (int)pos_x + img->w)
    {
      if (0 < (int)pos_y + img->h)
        MAT_IMAGE_DRAW_FAST(pos_x,          pos_y         );

      if ((int)pos_y + size_y < ship->info.player.world_he)
        MAT_IMAGE_DRAW_FAST(pos_x,          pos_y + size_y);
    }

  if ((int)pos_x + size_x < ship->info.player.world_wi)
    {
      if (0 < (int)pos_y + img->h)
        MAT_IMAGE_DRAW_FAST(pos_x + size_x, pos_y         );

      if ((int)pos_y + size_y < ship->info.player.world_he)
        MAT_IMAGE_DRAW_FAST(pos_x + size_x, pos_y + size_y);
    }
}

@
@<Function prototypes@>=
void
mat_image_draw_fast ( MatConnection *self, MatMovingObj *ship, MatImage *img,
                      double pos_x, double pos_y, double rate_x, double rate_y,
                      double width, double height, unsigned char *pal );
void
mat_image_draw_alpha ( MatConnection *self, MatMovingObj *ship, MatImage *img,
                       double pos_x, double pos_y, double rate_x, double rate_y,
                       double width, double height, unsigned char *pal, double alpha );

@
@<Functions@>=
void
mat_image_draw_alpha ( MatConnection *self, MatMovingObj *ship, MatImage *img,
                       double pos_x, double pos_y, double rate_x, double rate_y,
                       double width, double height, unsigned char *pal, double alpha )
{
  int col_x, col_y, col, i, j;
  double x1, y1;
  MatShip *sh;

  ASSERT(width  > 0);
  ASSERT(height > 0);

  ASSERT(rate_x > 0);
  ASSERT(rate_y > 0);

  ASSERT(ship);
  ASSERT(ship->type == MAT_MOVING_OBJ_SHIP);

  if (rate_x != 1)
    pos_x  = (double)pos_x / rate_x;
  if (rate_y != 1)
    pos_y  = (double)pos_y / rate_y;

  ASSERT(0 < pos_x + width );
  ASSERT(0 < pos_y + height);

  ASSERT(pos_x >= 0 || -pos_x < width );
  ASSERT(pos_y >= 0 || -pos_y < height);

  if (pos_x < 0)
    x1 = -pos_x;
  else
    x1 = 0;

  if (pos_y < 0)
    y1 = -pos_y;
  else
    y1 = 0;

  ASSERT(x1 >= 0);
  ASSERT(y1 >= 0);

  ASSERT(x1 < width);
  ASSERT(y1 < height);

  if ((int)pos_x + (int)width >= ship->info.player.imgwi)
    width  = ship->info.player.imgwi  - (int)pos_x - 1;

  ASSERT((int)pos_x + (int)width  < ship->info.player.imgwi);

  if ((int)pos_y + (int)height >= ship->info.player.imghe)
    height = ship->info.player.imghe - (int)pos_y - 1;

  ASSERT((int)pos_y + (int)height < ship->info.player.imghe);

  sh = &ship->info.player;

  for (i = (int)y1; i < (int)height; i ++)
    for (j = (int)x1; j < (int)width; j ++)
      {
        col = 0;
        for (col_y = 0; col_y < rate_y && (int)(i*rate_y) + col_y < img->h; col_y ++)
          for (col_x = 0; col_x < rate_x && (int)(j*rate_x) + col_x < img->w; col_x ++)
            col += pal[(int)img->img[((int)(i*rate_y)+col_y)*img->w+((int)(j*rate_x)+col_x)]];

        ASSERT(0 < col_x);
        ASSERT(0 < col_y);

        ASSERT(col >= 0);

        if (col > 0)
          MAT_GRAPH_DRAW_ALPHA(self, sh, (int)pos_x+j, (int)pos_y+i, col/(col_y*col_x), alpha);
      }
}
@
@<Functions@>=
void
mat_image_draw_fast ( MatConnection *self, MatMovingObj *ship, MatImage *img,
                      double pos_x, double pos_y, double rate_x, double rate_y,
                      double width, double height, unsigned char *pal )
{
  int col_x, col_y, col, i, j;
  double x1, y1;
  MatShip *sh;

  ASSERT(width  > 0);
  ASSERT(height > 0);

  ASSERT(rate_x > 0);
  ASSERT(rate_y > 0);

  ASSERT(ship);
  ASSERT(ship->type == MAT_MOVING_OBJ_SHIP);

  if (rate_x != 1)
    pos_x  = (double)pos_x / rate_x;
  if (rate_y != 1)
    pos_y  = (double)pos_y / rate_y;

  ASSERT(0 < pos_x + width );
  ASSERT(0 < pos_y + height);

  ASSERT(pos_x >= 0 || -pos_x < width );
  ASSERT(pos_y >= 0 || -pos_y < height);

  if (pos_x < 0)
    x1 = -pos_x;
  else
    x1 = 0;

  if (pos_y < 0)
    y1 = -pos_y;
  else
    y1 = 0;

  ASSERT(x1 >= 0);
  ASSERT(y1 >= 0);

  ASSERT(x1 < width);
  ASSERT(y1 < height);

  if ((int)pos_x + (int)width >= ship->info.player.imgwi)
    width  = ship->info.player.imgwi  - (int)pos_x - 1;

  ASSERT((int)pos_x + (int)width  < ship->info.player.imgwi);

  if ((int)pos_y + (int)height >= ship->info.player.imghe)
    height = ship->info.player.imghe - (int)pos_y - 1;

  ASSERT((int)pos_y + (int)height < ship->info.player.imghe);

  sh = &ship->info.player;

  for (i = (int)y1; i < (int)height; i ++)
    for (j = (int)x1; j < (int)width; j ++)
      {
        col = 0;
        for (col_y = 0; col_y < rate_y && (int)(i*rate_y) + col_y < img->h; col_y ++)
          for (col_x = 0; col_x < rate_x && (int)(j*rate_x) + col_x < img->w; col_x ++)
            col += pal[(int)img->img[((int)(i*rate_y)+col_y)*img->w+((int)(j*rate_x)+col_x)]];

        ASSERT(0 < col_x);
        ASSERT(0 < col_y);

        ASSERT(col >= 0);

        if (col > 0)
          MAT_GRAPH_DRAW(self, sh, (int)pos_x+j, (int)pos_y+i, col/(col_y*col_x));
      }
}

@ Receives an array of |arg_ang| images and optimizes it to reduce memory
consumption.

@<Functions@>=
void
mat_image_optimize ( MatImage *img )
{
  int xbeg, xend, ybeg, yend;

  @<Calculate maximum reductions allowed@>;
  @<Perform the reductions on the image@>;
}

@

@d MAT_IMAGE_REDUCTION_H(var, start, modify)
{
  int blank = 1, ang, i;

  for (var = start; blank; var += modify)
    for (ang = 0; ang < arg_ang; ang ++)
      for (i = 0; i < img[0].w; i ++)
        if (img[ang].img[var * img[ang].w + i] != ' ')
          blank = 0, i = img[ang].w, ang = arg_ang;

  var -= modify;
}

@d MAT_IMAGE_REDUCTION_X(var, start, modify)
{
  int blank = 1, ang, i;

  for (var = start; blank; var += modify)
    for (ang = 0; ang < arg_ang; ang ++)
      for (i = ybeg; i < yend; i ++)
        if (img[ang].img[i * img[ang].w + var] != ' ')
          blank = 0, i = img[ang].h, ang = arg_ang;

  var -= modify;
}

@<Calculate maximum reductions allowed@>=
{
  MAT_IMAGE_REDUCTION_H(ybeg,            0,  1);
  ASSERT(0 <= ybeg);
  ASSERT(ybeg < img[0].h);

  MAT_IMAGE_REDUCTION_H(yend, img[0].h - 1, -1);
  ASSERT(ybeg < yend);
  ASSERT(yend < img[0].h);

  MAT_IMAGE_REDUCTION_X(xbeg,            0,  1);
  ASSERT(0 <= xbeg);
  ASSERT(xbeg < img[0].w);

  MAT_IMAGE_REDUCTION_X(xend, img[0].w - 1, -1);
  ASSERT(xbeg < xend);
  ASSERT(xend < img[0].w);
}

@
@<Perform the reductions on the image@>=
{
  int       ang;

  for (ang = 0; ang < arg_ang; ang ++)
    {
      char     *tmp;
      int       pos = 0, x, y;

      for (y = ybeg; y < yend; y ++)
        for (x = xbeg; x < xend; x ++)
          img[ang].img[pos ++] = img[ang].img[y * img[ang].w + x];

      ASSERT(pos == (xend - xbeg) * (yend - ybeg));

      /* This is supposed to never fail, but if it fails, we can fix it. */
      tmp = realloc(img[ang].img, pos);
      if (tmp)
        img[ang].img = tmp;

      img[ang].h = yend - ybeg;
      img[ang].w = xend - xbeg;
    }
}

@* Aliens System

@<Typedefs@>=
typedef struct MatSmartObj MatSmartObj;
typedef struct MatSmartObjType MatSmartObjType;

@
@<Structs for smart objects@>=

struct MatSmartObjType
{
  char            *name;
  MatImage        *img;
  void           (*function) (MatMovingObj *obj);
};

struct MatSmartObj
{
  int       type;
};

@
@<Global variables@>=

MatSmartObjType mat_objects[] =
{
  { "Woozka", NULL, &mat_object_woozka },
  { "Kashka", NULL, &mat_object_kashka }
};

@
@<Function prototypes@>=
void mat_object_woozka ( MatMovingObj *obj );
void mat_object_kashka ( MatMovingObj *obj );

@
@<Functions@>=
void
mat_object_kashka ( MatMovingObj *obj )
{
  MatMovingObj         *dst;
  int                   dist_x, dist_dx, dist_dy, dist_y, dist_total;
  double                sp_x, sp_y;

  ASSERT(obj);
  ASSERT(obj->un);

  ASSERT(obj->type == MAT_MOVING_OBJ_SMART);
  ASSERT(obj->info.smart.type == 1);
  ASSERT(!obj->crash);

  if (!obj->un->players_real || obj->un->updates % 5)
    return;

  @<Find out the closest player to |obj|@>;
  ASSERT(dst);

  sp_x = (double) dist_dx * (double) dist_x / (double) (dist_x + dist_y);
  sp_y = (double) dist_dy * (double) dist_y / (double) (dist_x + dist_y);

  printf("Shoting: %fx%f\n", sp_x, sp_y);
  mat_bullet_new(obj, 0, sp_x, sp_y, 0);
}

@
@<Functions@>=
void
mat_object_woozka ( MatMovingObj *obj )
{
  MatMovingObj         *dst;
  int                   dist_x, dist_dx, dist_dy, dist_y, dist_total;

  ASSERT(obj);
  ASSERT(obj->un);

  ASSERT(obj->type == MAT_MOVING_OBJ_SMART);
  ASSERT(obj->info.smart.type == 0);
  ASSERT(!obj->crash);

  if (!obj->un->players_real)
    return;

  @<Find out the closest player to |obj|@>;
  ASSERT(dst);

  @<Acelerate |obj| in the direction of |dst|@>;

  ASSERT(!obj->crash);
}

@
@<Acelerate |obj| in the direction of |dst|@>=
{
  double des_sp, des_sp_x, des_sp_y;

#if 0
  printf("[x:%d][y:%d][sx:%f][sy:%f]\n", dist_dx * dist_x, dist_dy * dist_y, obj->sp_x, obj->sp_y);
#endif

  if (dist_x != 0 || dist_y != 0)
    {
      double dist_sum;

      dist_sum = (double) dist_x + dist_y;

      des_sp_x = obj->sp_x + 0.01 * (double) dist_dx * (double) dist_x / dist_sum;
      des_sp_y = obj->sp_y + 0.01 * (double) dist_dy * (double) dist_y / dist_sum;

      des_sp   = sqrt(des_sp_x * des_sp_x + des_sp_y * des_sp_y);

      if (des_sp * 3 < arg_speed_max)
        {
          obj->sp   = des_sp;
          obj->sp_x = des_sp_x;
          obj->sp_y = des_sp_y;
        }
      else
        {
          double tsp;

          /* TODO: Code here probably has bugs. */
          tsp = (double) arg_speed_max / 3;
          obj->sp_x = tsp * (double) dist_dx * (double) dist_x / dist_sum;
          obj->sp_y = tsp * (double) dist_dy * (double) dist_y / dist_sum;
          obj->sp   = sqrt(obj->sp_x * obj->sp_x + obj->sp_y * obj->sp_y);
        }
    }
}

@
@<Find out the closest player to |obj|@>=
{
  MatMovingObj *sh;

  ASSERT(obj->un->players_head);

  dst = NULL;

#if 0
  printf("[woozka: [x:%x][y:%x]]\n", obj->pos_x, obj->pos_y);
#endif

  for (sh = obj->un->players_head; sh; sh = sh->nexttype)
    {
      int x, y, dx, dy, total;

      @<Find out the distance between |obj| and |sh|@>;
      total = x * x + y * y;

      if (!dst || total < dist_total)
        {
#if 0
          printf("[obj: [x:%x][y:%x]]\n", sh->pos_x, sh->pos_y);
#endif
          dst = sh;
          dist_total = total;
          dist_x     = x;
          dist_dx    = -dx;
          dist_y     = y;
          dist_dy    = -dy;
        }
    }
}

@
@d MAT_SMART_OBJ_ADD_WOOZKA(xun)
  MAT_SMART_OBJ_ADD(xun, 0)

@d MAT_SMART_OBJ_ADD_KASHKA(xun)
  MAT_SMART_OBJ_ADD(xun, 1)

@d MAT_SMART_OBJ_ADD(xun, xtype)
{
  MatMovingObj *obj;
  obj = malloc(sizeof(MatMovingObj));
  if (!obj)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  obj->type            = MAT_MOVING_OBJ_SMART;
  obj->rm_next         = NULL;

  obj->refs            = 1;

  ASSERT(mat_ast[1]);

  obj->img             = mat_ast[1];
  obj->pal             = mat_ship_palette[0];

  obj->ang             = 0;
  obj->sp_ang          = 0;

  obj->info.smart.type = xtype;

  obj->crash           = 0;
  obj->draw            = 1;

  obj->alpha           = 1;
  obj->alpha_mod       = NULL;

  obj->un              = xun;

  obj->nexttype        = xun->smart_objs;
  obj->prevtype        = NULL;

  if (xun->smart_objs)
    xun->smart_objs->prevtype = obj;

  xun->smart_objs = obj;

  obj->sp = obj->sp_x = obj->sp_y = 0;

  obj->pos_x = (double) (rand() % xun->mapsize_x);
  obj->pos_y = (double) (rand() % xun->mapsize_y);

  ADD_MOVING_OBJ(obj, xun);
}

@
@<Make smart objects in |un| think@>=
{
  MatMovingObj *obj;

  for (obj = un->smart_objs; obj; obj = obj->nexttype)
    {
      ASSERT(obj->type == MAT_MOVING_OBJ_SMART);
      ASSERT(obj->info.smart.type < sizeof(mat_objects) / sizeof(MatSmartObjType));
      mat_objects[obj->info.smart.type].function(obj);
    }
}

/* TODO: Add smart_obj_handler */

@* XML Code

@<Load universe@>=
{
  XML_Parser *prs;
  FILE *in;
  char buf[BUFSIZ];
  int  done;

  @<Open the file to load the universe@>;

  @<Set the default parameters for |un|@>;

  prs = XML_ParserCreate(NULL);

  XML_SetUserData(prs, un);
  XML_SetElementHandler(prs, mat_xml_start, mat_xml_end);

  do
    {
      size_t len;

      len = fread(buf, 1, sizeof(buf), in);
      done = len < sizeof(buf);
      if (!XML_Parse(prs, buf, len, done))
        {
          fprintf(stderr, "%s: %s:%d: %s\n", program_name, un->path,
              XML_GetCurrentLineNumber(prs), XML_ErrorString(XML_GetErrorCode(prs)));
          exit(EXIT_FAILURE);
        }
    }
  while (!done);

  XML_ParserFree(prs);

  @<Set |NULL| locations in |un| to the default@>;
}

@
@<Function prototypes@>=

void                     mat_xml_start         ( void           *data,
                                                 const char     *name,
                                                 const char    **atts );

void                     mat_xml_end           ( void           *data,
                                                 const char     *name );

@
@<Functions@>=
void
mat_xml_start (void *data, const char *name, const char **atts)
{
  MatUniverse *un = data;

  if (!strcmp(name, "Universe"))
    @<Parse Universe tag@>@;
  else if (!strcmp(name, "Location"))
    @<Parse Location tag@>@;
  else
    {
      fprintf(stderr, "%s: %s: %s: Unknown tag\n", program_name, un->path, name);
      exit(EXIT_FAILURE);
    }
}

@
@<Parse Location tag@>=
{
  int i, color = 0;
  MatLocation *l;

  l = malloc(sizeof(MatLocation));
  if (!l)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  memcpy(l, &mat_location_default, sizeof(MatLocation));

  for (i = 0; atts[i]; i += 2)
    if (!strcmp(atts[i], "color"))
      color = atoi(atts[i + 1]);
    else if (!strcmp(atts[i], "shipcrash"))
      l->crash[MAT_MOVING_OBJ_SHIP] = strcmp(atts[i + 1], "yes") ? 0 : 1;
    else if (!strcmp(atts[i], "asteroidcrash"))
      l->crash[MAT_MOVING_OBJ_AST] = strcmp(atts[i + 1], "yes") ? 0 : 1;
    else if (!strcmp(atts[i], "health"))
      l->health = atoi(atts[i + 1]);
    else if (!strcmp(atts[i], "shipmove"))
      l->move[MAT_MOVING_OBJ_SHIP] = atof(atts[i + 1]);
    else
      {
        fprintf(stderr, "%s: %s: Unknown attribute for <Location>\n", program_name, atts[i]);
        exit(EXIT_FAILURE);
      }

  if (color < 0 || color > 255)
    {
      fprintf(stderr, "%s: %d: Invalid color specified\n", program_name, color);
      exit(EXIT_FAILURE);
    }

  un->location[color] = l;
}

@
@<Parse Universe tag@>=
{
  int i;
  for (i = 0; atts[i]; i += 2)
    if (!strcmp(atts[i], "mapsizex"))
      un->mapsize_x = atoi(atts[i + 1]);
    else if (!strcmp(atts[i], "mapsizey"))
      un->mapsize_y = atoi(atts[i + 1]);
    else if (!strcmp(atts[i], "name"))
      ;
    else if (!strcmp(atts[i], "background"))
      un->bg = mat_png_load(atts[i + 1]);
    else if (!strcmp(atts[i], "mask"))
      un->mask = mat_png_load(atts[i + 1]);
    else if (!strcmp(atts[i], "dots"))
      un->dots = strcmp(atts[i + 1], "no") ? 1 : 0;
    else if (!strcmp(atts[i], "health"))
      un->health = atoi(atts[i + 1]);
    else
      {
        fprintf(stderr, "%s: %s: Unknown attribute for <Universe>\n", program_name, atts[i]);
        exit(EXIT_FAILURE);
      }
}

@
@<Functions@>=
void
mat_xml_end (void *userData, const char *name)
{
}

@
@<Open the file to load the universe@>=
  in = fopen(un->path, "r");
  if (!in)
    {
      fprintf(stderr, "%s: %s: %s\n", program_name, un->path, strerror(errno));
      exit(EXIT_FAILURE);
    }

@* Alpha Transparency

We want to make it possible to draw objects transparently.  This is used
mostly for the times when they are appearing.

|alpha| goes from 0 to 1, 0 meaning the object will be invisible and 1 meaning
it will be solid.

If |alpha_mod| is not |NULL|, it will be called once for every turn and is
supposed to modify the alpha parameter.

@<Fields for moving objects@>=
  double         alpha;
  void         (*alpha_mod) (MatMovingObj *);

@
@<Function prototypes@>=
void              mat_alpha_mod_ship_appear                ( MatMovingObj         *s );
void              mat_alpha_mod_ship_disappear             ( MatMovingObj         *s );

@
@<Functions@>=
void
mat_alpha_mod_ship_appear ( MatMovingObj *s )
{
  s->alpha += 0.01;
  if (s->alpha >= 1.0)
    {
      s->alpha_mod = NULL;
      s->crash     = 1;
    }
}

@
@<Functions@>=
void
mat_alpha_mod_ship_disappear ( MatMovingObj *s )
{
  s->alpha -= 0.01;
  if (s->alpha <= 0)
    {
      s->alpha_mod = NULL;
    }
}
