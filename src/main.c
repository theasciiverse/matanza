#include <matanza.h>

#define ARG_UNIVERSE_UPDATE_STEPS 10

/* See matanza.h for a description of most of these variables. */

char             *program_name                  = "matanza";

int               arg_port_machine              = 7793;
int               arg_port                      = 7993;
unsigned long     arg_usleep                    = 100000;
int               arg_max_players               = 0;
int               arg_ang                       = 64;
int               arg_lives                     = 5;
int               arg_chat                      = 1;
char             *arg_password                  = NULL;
int               arg_space                     = 1;
int               arg_brake                     = 1;
int               arg_fly_back                  = 0;
int               arg_free_at_exit              = 0;
int               arg_bold                      = 0;
double            arg_ship_friction             = 0;
int               arg_imgwidth                  = MAT_IMGWIDTH_DEFAULT;
int               arg_health                    = 1000;
int               arg_mapsize_x                 = 2000;
int               arg_mapsize_y                 = 1600;
int               arg_ppc                       = 1;
int               arg_compress                  = 1;
int               arg_limits                    = 0;
int               arg_bg_dots                   = 1;
int               arg_bg_adj                    = 0;
int               arg_bg_color_max              = -1;
int               arg_teams_safe                = 0;
int               arg_teams_share               = 1;
int               arg_ast_num                   = 4;
int               arg_bg_size                   = 0;
int               arg_visible_init              = 0;
int               arg_image_scale_good          = 0;
int               mat_arg_radar                 = 1;
int               mat_arg_update_delay          = 3;
int               mat_arg_mist                  = 1;
int               mat_arg_motion_blur           = 0;
int               arg_universe_update_steps     = ARG_UNIVERSE_UPDATE_STEPS;
double            arg_ang_speed_max             = 8.0 / ARG_UNIVERSE_UPDATE_STEPS;
double            arg_speed_max                 = 20.0 / ARG_UNIVERSE_UPDATE_STEPS;
//MatImgWorld      *mat_bg_head                   = NULL;
//MatImgWorld      *mat_bg_tail                   = NULL;
//MatImgWorld      *mat_fg_head                   = NULL;
//MatImgWorld      *mat_fg_tail                   = NULL;
MatScm           *arg_ship_graphic              = NULL;

#if MATANZA_VERBOSE
int               mat_arg_verbose               = 0;
#endif /* MATANZA_VERBOSE */

/* Function prototypes. */

static void mat_init ( int argc, char **argv );

static void mat_init_label_done ( void );
static void mat_init_arg ( int argc, char **argv );
static void mat_init_arg_bg ( char *name );
static void mat_init_arg_check ( void );
static void mat_init_arg_version ( void );
static void mat_init_fork ( void );
static void mat_init_signal ( void );
static void mat_signal_term ( int sig );
static void mat_signal_term_free ( void );
static unsigned long mat_getmicrosecond ( void );

INLINE static void        mat_init_arg_verbose             ( char               *input );

static MatScm            *mat_env = NULL;

int
main (int argc, char **argv)
{
  mat_init(argc, argv);

  while (1)
    if (players_count == 0)
      {
        mat_network_wait_accept();
        mat_network_accept();
      }
    else
      {
        unsigned long old, now;
        MatScm *c;

        int                   i;
        MatScm               *un;

        old = mat_getmicrosecond();

        mat_network_accept();

        /* Read input from all connections. */
        c = mat_connection_head;
        while (c)
          {
            mat_connection_flush(mat_dlist_obj_get(c));
            mat_connection_input(mat_dlist_obj_get(c));
            c = mat_dlist_next_get(c);
          }

#if 0
          if (mat_connection_state_get(mat_dlist_obj_get(c)) == MAT_STATE_REMOVE)
            {
              MatConnection *tmp;

              tmp = mat_connection_next_get(c);
              mat_connection_remove(c);
              c = tmp;

              players_count --;
            }
#endif

        /* Update all the registered universes. */
        MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> update universes\n");
        for (un = mat_universe_head; un; un = mat_dlist_next_get(un))
          mat_universe_update(mat_dlist_obj_get(un));
        MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- update universes\n");

        /* Send output to the players. */
        for (c = mat_connection_head; c; c = mat_dlist_next_get(c))
          if (mat_connection_state_get(mat_dlist_obj_get(c)) != MAT_STATE_REMOVE)
            mat_connection_update(mat_dlist_obj_get(c));

        now = mat_getmicrosecond() - old;

        if (now < arg_usleep)
          usleep(arg_usleep - now);
      }
}

static void
mat_init ( int argc, char **argv )
{
  int i;

  /* Here we set the global variable program_name.  It is used across the
   * program at the beginning of all the error messages. */

  program_name = strrchr(argv[0], '/');
  if (program_name)
    program_name++;
  else
    program_name = argv[0];

  srand(time(NULL));

#if 0
  for (i = 0; i < MAT_UNIVERSES_HASH_SIZE; i ++)
    mat_universes[i] = NULL;
#endif

  mat_init_arg(argc, argv);

  mat_network_init();
  mat_asciirender_init();

  mat_init_signal();

  mat_init_label_done();

#if MATANZA_VERBOSE
  if (!mat_arg_verbose)
#endif
    mat_init_fork();

  chdir("/");
}

/* Show a label when Matanza is succesfully initialized. */
/* TODO: Add some spice, probably some ASCII art. */
static void
mat_init_label_done ( void )
{
  printf("-=> Matanza has been succesfully started.\n-=> [TCP Port: %d] [PID: %d] [UID: %d]\n", arg_port, getpid(), getuid());
}

static void
mat_init_arg ( int argc, char **argv )
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
        { "free-at-exit",       no_argument,       NULL, 28  },
        { "image-scale-good",   no_argument,       NULL, 29  },
        { "motion-blur",        no_argument,       NULL, 30  },
        { "update-steps",       required_argument, NULL, 31  },
        { "verbose",            required_argument, NULL, 'V' },
        { "update-delay",       required_argument, NULL, 32  },
	{ 0, 0, 0, 0 }
      };

      switch (getopt_long(argc, argv, "hvm:bW:x:y:l:A:p::V:", lopt, &oi))
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
          arg_ang_speed_max = ((double)atoi(optarg)) / arg_universe_update_steps;
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
        case 13:
          arg_visible_init = atoi(optarg);
          break;
        case 14:
          arg_ppc = atoi(optarg);
          break;
        case 15:
          mat_init_arg_bg(optarg);
          break;
        case 16:
          arg_bg_size = 1;
          break;
        case 17:
          arg_ship_friction = atof(optarg);
          /* TODO: Enable below. */
          //mat_location_default.move[MAT_MOVING_OBJ_SHIP] = arg_ship_friction;
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
        case 28:
          arg_free_at_exit = 1;
          break;
        case 29:
          arg_image_scale_good = 1;
          break;
        case 30:
          mat_arg_motion_blur = 1;
          break;
        case 31:
          arg_universe_update_steps = atoi(optarg);
          break;
        case 32:
          mat_arg_update_delay = atoi(optarg);
          break;
        case 'V':
#if MATANZA_VERBOSE
          mat_init_arg_verbose(optarg);
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
             "  -h, --help              Show this information and exit\n"
             "  -v, --version           Show version number and exit\n"
             "  -m, --max-players=NUM   Never allow more than NUM players\n"
	     "  -b, --bold              Use bold characters too\n"
             "  -W, --imgwidth=NUM      Image width for default zoom (320)\n"
             "  --health=NUM            Health of ships (1000)\n"
             "  -x, --mapsize-x=NUM     Map size in the x coordinate\n"
             "  -y, --mapsize-y=NUM     Map size in the y coordinate\n"
             "  -l, --lives=NUM         Number of lives for the players\n"
             "  -A, --angles=NUM        Number of angles for rotations\n"
             "  --no-chat               Disable the chat features\n"
             "  -t, --team=STR[:NUM]    Add team STR with maximum NUM players\n"
             "  -p, --password[=STR]    Perform authentication using STR as the password\n"
             "  --teams-safe            Bullets won't hit players of the same team\n"
             "  --no-share              Players in the same team don't share lives\n"
             "  --ang-speed-max=NUM     Maximum angular speed for ships (try 0)\n"
             "  --fly-back              Players will be able to fly backwards\n"
             "  --no-brake              Players will not be able to brake\n"
             "  --air                   Ships' speed depend on their angle\n"
             "  --speed-max=NUM         Maximum speed (0 = Unlimited)\n"
             "  --asteroids=NUM         The number of asteroids at a given moment\n"
             "  --limits                Give the world limits; make it end\n"
             "  --invisible=NUM         Allow players to become invisible for NUM rounds\n"
             "  --ppc=NUM               Set the number of Players Per Connection\n"
             "  --bg=PATH               Load background image from FILE\n"
             "  --mapsize-img           Set the map's size to the background's size\n"
             "  --ship-friction=NUM     Set the ship's friction (0 <= NUM <= 1)\n"
             "  --dots                  Show dots on the background\n"
             "  --nodots                Don't show dots on the background\n"
             "  --bg-adjust             Adjust the backgrounds to the mapsize\n"
             "  --no-compress           Don't perform bandwidth optimizations\n"
             "  --bg-highcol=NUM        Adjust the colors in the background\n"
             "  --space                 Ships' speed doesn't depend on their angle\n"
             "  --port=NUM              Use TCP port NUM for telnet\n"
             "  --usleep=NUM            Sleep NUM microseconds between updates\n"
             "  --port-machine=NUM      Use TCP port NUM for machines' friendly protocol\n"
             "  --image-scale-good      Use a good (but slow) algorithm for scaling images\n"
             "  --motion-blur           Apply motion blur to objects at high speeds\n"
             "  --update-steps=NUM      Update the universe in NUM steps\n"
             "\n"
             "Report bugs to <bachue@bachue.com>\n", program_name);
      exit(EXIT_SUCCESS);
    }

  if (show_version)
    mat_init_arg_version();

  if (prompt_password)
    {
      fprintf(stderr, "%s: Prompting for a password is not supported (yet)\n", program_name);
      fprintf(stderr, "%s: Try --password=<string>\n", program_name);
      exit(EXIT_FAILURE);
    }

  if (optind == argc)
    {
      fprintf(stderr, "%s: Missing universe specification\n", program_name);
      exit(EXIT_FAILURE);
    }

  mat_scm_init();

  mat_scm_obj_protect(&mat_env);

  mat_env = mat_scm_env_malloc();
  mat_scm_env_add(mat_env);

  while (optind < argc)
    mat_scm_load(argv[optind ++], mat_env);

  /* Make sure the code defined at least one universe type */
  ASSERT(mat_universetype_head_get());

  mat_init_arg_check();
}

static void
mat_init_arg_check ( void )
{
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
      fprintf(stderr, "%s: Telnet port < 1\n", program_name);
      arg_port = 7993;
    }

  if (arg_port_machine < 1)
    {
      fprintf(stderr, "%s: MMP port < 1\n", program_name);
      arg_port_machine = 7793;
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

static void
mat_init_arg_version ( void )
{
  printf("Freaks Unidos' %s %s\n"
         "Copyright (C) 2000 Alejandro Forero Cuervo\n"
         "Report any bugs to <bachue@bachue.com>\n"
         "Check <http://bachue.com/matanza> for updates\n",
         PACKAGE, VERSION);
  exit(EXIT_SUCCESS);
}

static void
mat_init_fork ( void )
{
#if HAVE_FORK
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
#endif
}

/* Set the signal handlers. */
static void
mat_init_signal ( void )
{
  /* We block SIGPIPE so we won't die when a given player closes his
   * connection.
   *
   * We also block SIGINT and SIGTERM in order to provide a nice termination
   * (printing some messages to stdout and sending some information to the
   * players before actually closing their connections). */

#if HAVE_SIGNAL
#  ifdef SIGPIPE
  signal(SIGPIPE, SIG_IGN);
#  endif
#  ifdef SIGINT
  signal(SIGINT,  mat_signal_term);
#  endif
#  ifdef SIGTERM
  signal(SIGTERM, mat_signal_term);
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

static void
mat_signal_term ( int sig )
{
  MatScm *c;

  printf("%s: Terminating on signal %d\n", program_name, sig);

  for (c = mat_connection_head; c; c = mat_dlist_next_get(c))
    mat_connection_free(mat_dlist_obj_get(c), MMP_SERVER_CLOSE_SHUTDOWN, "The server is being shutdown.");

  /* Most of the time we won't want to call free since the operating system
   * will reclaim all the memory we held and we could otherwise trigger page
   * faults and make things slow.
   *
   * Why then properly cleaning some memory space that is going to get
   * destroyed in the next function call?  Because we want to be able to run
   * Matanza under a memory leaks detector such as ccmalloc, that prints memory
   * areas that we allocated but didn't free. */

  if (arg_free_at_exit)
    mat_signal_term_free();

  exit(EXIT_SUCCESS);
}

static void
mat_signal_term_free ( void )
{
  int i;

#if 0
  /* Free all universes. */
  for (i = 0; i < MAT_UNIVERSES_HASH_SIZE; i ++)
    while (mat_universes[i])
      {
        MatUniverse *d;

        d = mat_universes[i];
        mat_universes[i] = mat_universe_hash_next_get(d);

        mat_universe_free(d);
      }
#endif

#if 0
  /* Close all connections. */
  while (connections)
    {
      MatConnection *tmp;

      ASSERT(mat_connection_state_get(connections) == MAT_STATE_REMOVE);

      tmp = mat_connection_next_get(connections);
      free(connections);
      connections = tmp;
    }
#endif
 
  mat_asciirender_free();
}

static void
mat_init_arg_bg ( char *name )
{
#if 0
  MatImgWorld *tmp;

  tmp = malloc(sizeof(MatImgWorld));
  if (!tmp)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  tmp->path = name;

  tmp->next = NULL;
  tmp->prev = mat_bg_tail;

  if (mat_bg_tail)
    mat_bg_tail->next = tmp;
  else
    mat_bg_head = tmp;

  mat_bg_tail = tmp;
#endif
}

/* Look for a way to implement GETMICROSECOND_GENERIC. */

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

/* Notice that on Windows we haven't figured out how to get the microsecond so
 * we have to multiply by 1000.  Windows sucks. */

static unsigned long
mat_getmicrosecond ( void )
{
#if HAVE_SYSTEM_TIME
  return system_time();
#elif HAVE_GETTIMEOFDAY
  struct timeval st;
  gettimeofday(&st, NULL);
  return ((unsigned long) st.tv_sec * 1000000 + (unsigned long) st.tv_usec);
#elif HAVE_GETTICKCOUNT
  return GetTickCount() * 1000;
#else
#  error Could not find a way to implement mat_getmicrosecond
#endif
}

/* Windows and BeOS lack usleep.  We need to find alternatives on them.  Notice
 * that on Windows we have to divide the time by 1000.  Windows sucks!  */

#if !HAVE_USLEEP
void
usleep ( unsigned long usec )
{
#  if HAVE_SNOOZE_UNTIL
  bigtime_t wake_at;
  status_t  status;

  wake_at = system_time() + usec;

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> snooze_until cycle\n");
  do
    {
      MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> snooze_until\n");
      status = snooze_until(wake_at, B_SYSTEM_TIMEBASE);
      MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- snooze_until\n");
    }
  while (status == EINTR);
  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- snooze_until cycle\n");
#  elif HAVE_SLEEP
  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> Sleep(%d)\n", usec / 1000);
  Sleep(usec / 1000);
  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- Sleep\n");
#  else
#    error Could not find way to implement usleep
#  endif
}
#endif

INLINE static void
mat_init_arg_verbose ( char *input )
{
  char *name[] = { "default", "net", "mmp", "mmploud", "netin", "objpos", "interface", "connections", "xml", "scm-internal", "scm-user" };

  int   i;

  if (strcmp("all", input) == 0)
    {
      mat_arg_verbose = 0xffff;
      return;
    }

  for (i = 0; i < sizeof(name) / sizeof(name[0]); i ++)
    if (strcmp(name[i], input) == 0)
      {
        mat_arg_verbose |= 1 << i;
        return;
      }

  fprintf(stderr, "%s: Valid arguments for verbose are: all", program_name);

  for (i = 0; i < sizeof(name) / sizeof(name[0]); i ++)
    fprintf(stderr, " %s", name[i]);

  fprintf(stderr, "\n");
  exit(EXIT_FAILURE);
}
