@* Matanza - Artificial Inteligence System

@c
@<Portability@>
@<Typedefs@>
@<Struct definitions@>
@<Function prototypes@>
@<Global variables@>
@<Functions@>
@<The main program@>

@

@d AI_SCREEN_HEIGHT 24
@d AI_SCREEN_WIDTH  80

@<Global variables@>=

char         screen_buffer[AI_SCREEN_HEIGHT * AI_SCREEN_WIDTH];

char        *program_name;

FILE        *mat_server_in;
FILE        *mat_server_out;

void (*input_process) ( unsigned int ) = input_process_initial;

void (*input_process_done) ( void );

char        *input_string;
int          input_string_siz;
int          input_string_len;

int          input_number;
int          input_number_bytes;

@ Our standard hashing function.

@<Functions@>=
unsigned long
fhash (char *name)
{
  unsigned long h = 0;
  unsigned long g;

  ASSERT(name);

  /* Our hashing function. */

  while ( *name )
    {
      h = ( h << 4 ) + tolower(*name++);
      if ( (g = h & 0xF0000000) )
	h ^= g >> 24;
      h &= ~g;
    }

  return h;
}

@
@<Function prototypes@>=
unsigned long fhash (char *name);

@
@<The main program@>=
int
main ( int argc, char **argv )
{
  @<Initialize environment@>;

  while (1)
    {
      @<Process incomming events@>;
      usleep(10000);
    }
}

@* Network Input

In this big section we are placing all the code that is somehow related to
reading input from the network.

@ The following functions handle input from the network.

@<Function prototypes@>=
void           input_process_initial                     ( unsigned int c );
void           input_process_close                       ( unsigned int c );
void           input_process_string                      ( unsigned int c );
void           input_process_number                      ( unsigned int c );
void           input_process_default                     ( unsigned int c );
void           input_process_joinfail                    ( unsigned int c );
void           input_process_frame                       ( unsigned int c );

@ The following functions can be called after a string is read (by
|input_process_string).  

For example, if you get the code for an error, you'll set |input_process| to
|input_process_string| and |input_process_done| to |input_process_syserror|.
That way, all characters received are passed to |input_process_string|, which
stores them on |input_string|, and once it's done, it will call
|input_process_done|, which will do something useful with the string.

@<Function prototypes@>=

void           input_process_shiprm_un                   ( void );
void           input_process_shiprm_name                 ( void );
void           input_process_frame_ships                 ( void );
void           input_process_shipname                    ( void );
void           input_process_mmperror                    ( void );
void           input_process_syserror                    ( void );
void           input_process_connect_server_id           ( void );
void           input_process_connect_server_version      ( void );
void           input_process_frame_ships_name            ( void );
void           input_process_frame_ships_x               ( void );
void           input_process_frame_ships_y               ( void );
void           input_process_frame_ships_ang             ( void );
void           input_process_shipadd_other_un            ( void );
void           input_process_shipadd_other_name          ( void );

@ When we are in the Close state, we must wait for a code to report the reason
why the server is closing the connection and act accordingly.

In the case of an error, we wait for the description before termination.
Otherwise, we show a message about the problem and terminate.

@<Functions@>=
void
input_process_close ( unsigned int c )
{
  switch (c)
    {
    case MMP_SERVER_CLOSE_SYSERROR:
      input_process = input_process_string;
      input_process_done = input_process_syserror;
      break;

    case MMP_SERVER_CLOSE_MMPERROR:
      input_process = input_process_string;
      input_process_done = input_process_mmperror;
      break;

    case MMP_SERVER_CLOSE_SHUTDOWN:
      fprintf(stderr, "%s: Server is being shutdown.\n", program_name);
      exit(EXIT_FAILURE);
      break;

    case MMP_SERVER_CLOSE_CLIENT:
      fprintf(stderr, "%s: Server is closing the connection.\n", program_name);
      exit(EXIT_FAILURE);
      break;
    }
}

@ When the server had an error, we simply print its description and terminate.

@<Functions@>=
void
input_process_syserror ( void )
{
  fprintf(stderr, "%s: Server error: %s\n", program_name, input_string);
  exit(EXIT_FAILURE);
}

@ When the server reported a violation of MMP, we just terminate.

@<Functions@>=
void
input_process_mmperror ( void )
{
  fprintf(stderr, "%s: Server reported violation of MMP: %s\n", program_name, input_string);
  exit(EXIT_FAILURE);
}

@ When we are in the initial state, we wait for the three valid messages
described in the protocol.h file.

@<Functions@>=
void
input_process_initial ( unsigned int c )
{
  switch(c)
    {
    case MMP_SERVER_INITIAL_FULL:
      @<Server is full@>;
      break;
    
    case MMP_SERVER_CLOSE:
      input_process = input_process_close;
      break;

    case MMP_SERVER_INITIAL_CONNECTED:
      @<Read server id and version@>;
      break;

    default:
      fprintf(stderr, "Unknown message: [msg:%d][state:Initial]\n", c);
      break;
    }
}

@ If the server is full, there is no use in going on.  We show a message and
terminate.

@<Server is full@>=
{
  fprintf(stderr, "%s: Server full\n", program_name);
  exit(EXIT_FAILURE);
}

@ When we receive the |MMP_SERVER_INITIAL_CONNECTED| message, we read the
server's id and version and report it to the user.

@<Read server id and version@>=
{
  input_string       = NULL;
  input_string_len   = 0;
  input_string_siz   = 0;

  input_process      = input_process_string;
  input_process_done = input_process_connect_server_id;
}

@
@<Functions@>=
void
input_process_string ( unsigned int c )
{
  if (input_string_len == input_string_siz)
    @<Resize |input_string|@>@;

  input_string[input_string_len ++] = c;

  if (c == 0)
    input_process_done();
}

@
@<Resize |input_string|@>=
{
  input_string_siz = input_string_siz ? input_string_siz * 2 : 256;
  input_string = realloc(input_string, input_string_siz + 1);
  if (!input_string)
    {
      fprintf(stderr, "%s: realloc: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }
}

@ When this function gets called, the server's id is left in |input_string|.

@<Functions@>=
void
input_process_connect_server_id ()
{
  printf("Server: %s\n", input_string ? input_string : "");

  free(input_string);

  input_string       = NULL;
  input_string_len   = 0;
  input_string_siz   = 0;

  input_process      = input_process_string;
  input_process_done = input_process_connect_server_version;
}

@ When this function gets called, the server's version is at |input_string|.

@<Functions@>=
void
input_process_connect_server_version ()
{
  input_process = input_process_default;

  printf("Server version: %s\n", input_string ? input_string : "Unknown");

  free(input_string);

  fprintf(mat_server_out, "%c%c%c", MMP_CLIENT_DEFAULT_JOIN, 0, 0);
  fflush(mat_server_out);
}

@

@<Functions@>=
void
input_process_default ( unsigned int c )
{
  /*VERBOSE("Command: %d\n", c);*/
  switch (c)
    {
    case MMP_SERVER_DEFAULT_JOIN_OK:
      @<Get ships in the default universe@>;
      break;

    case MMP_SERVER_DEFAULT_JOIN_FAIL:
      input_process = input_process_joinfail;
      break;

    case MMP_SERVER_CLOSE:
      input_process = input_process_close;
      break;

    case MMP_SERVER_DEFAULT_SHIPADD_CLIENT:
      @<Register ship added by ourselves@>;
      break;

    case MMP_SERVER_DEFAULT_SHIPADD_OTHER:
      @<Register ship added by another client@>;
      break;

    case MMP_SERVER_DEFAULT_FRAME_SHIPS:
      input_process      = input_process_number;
      input_process_done = input_process_frame_ships;

      input_number       = 0;
      input_number_bytes = 2;
      break;

    case MMP_SERVER_DEFAULT_SHIPRM:
      @<Remove the ship specified@>;
      break;

    default:
      fprintf(stderr, "Unknown message: [msg:%d][state:Default]\n", c);
      exit(EXIT_FAILURE);
    }
}

@
@<Remove the ship specified@>=
{
  input_process      = input_process_number;
  input_process_done = input_process_shiprm_un;

  input_number       = 0;
  input_number_bytes = 2;
}

@
@<Functions@>=
void
input_process_shiprm_un ( void )
{
  input_process          = input_process_string;
  input_process_done     = input_process_shiprm_name;

  input_string           = NULL;
  input_string_len       = 0;
  input_string_siz       = 0;
}

@
@<Functions@>=
void
input_process_shiprm_name ( void )
{
  input_process          = input_process_default;
  VERBOSE("Remove ship: %s\n", input_string);
  REMOVE_SHIP(input_string);
  free(input_string);
}

@
@<Register ship added by another client@>=
{
  input_number       = 0;
  input_number_bytes = 2;

  input_process      = input_process_number;
  input_process_done = input_process_shipadd_other_un;
}

@
@<Functions@>=
void
input_process_shipadd_other_un ( void )
{
  ASSERT(input_number == 0);

  input_string       = NULL;
  input_string_len   = 0;
  input_string_siz   = 0;

  input_process      = input_process_string;
  input_process_done = input_process_shipadd_other_name;
}

@
@<Functions@>=
void
input_process_shipadd_other_name ( void )
{
  ASSERT(input_string);

  REGISTER_SHIP_HEAD(input_string);

  VERBOSE("Adding ship: %s\n", input_string);

  input_process = input_process_default;
}

@
@<Register ship added by ourselves@>=
{
  static int current = 0;
  char *buffer;

  buffer = malloc(9);
  if (!buffer)
    {
      fprintf(stderr, "%s: malloc: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }
    
  sprintf(buffer, "AI%d", current ++);

  printf("Added a ship to the universe.\n");
  REGISTER_SHIP_HEAD(strdup(buffer));
}

@ Now a function to read numbers.

@<Functions@>=
void
input_process_number ( unsigned int c )
{
  ASSERT(input_number_bytes > 0);

  ASSERT(0 <= c);
  ASSERT(     c <= 255);

  input_number = input_number * 255 + c;

  if (input_number_bytes == 1)
    input_process_done();
  else
    input_number_bytes --;
}

@ A function to read information about a ship's state.
@<Functions@>=
void
input_process_frame_ships ( void )
{
  if (!mat_universe->ship_list_head)
    input_process = input_process_frame;
  else
    {
      input_process      = input_process_number;
      input_process_done = input_process_frame_ships_x;

      input_number       = 0;
      input_number_bytes = 2;

      mat_universe->ship_read = mat_universe->ship_list_head;
    }
}

@
@<Functions@>=
void
input_process_frame_ships_x ( void )
{
  mat_universe->ship_read->pos_x[mat_universe->read] = input_number;

  input_process      = input_process_number;
  input_process_done = input_process_frame_ships_y;

  input_number       = 0;
  input_number_bytes = 2;
}

@
@<Functions@>=
void
input_process_frame_ships_y ( void )
{
  mat_universe->ship_read->pos_y[mat_universe->read] = input_number;

  input_process      = input_process_number;
  input_process_done = input_process_frame_ships_ang;

  input_number       = 0;
  input_number_bytes = 2;
}

@
@<Functions@>=
void
input_process_frame_ships_ang ( void )
{
  mat_universe->ship_read->ang[mat_universe->read] = input_number;

  VERBOSE("[x:%d][y:%d][ang:%d]\n",
          mat_universe->ship_read->pos_x[mat_universe->read],
          mat_universe->ship_read->pos_y[mat_universe->read],
          mat_universe->ship_read->ang[mat_universe->read]);

  if (!mat_universe->ship_read->next)
    {
      VERBOSE("Done\n");
      input_process = input_process_frame;
    }
  else
    {
      input_process      = input_process_number;
      input_process_done = input_process_frame_ships_x;

      input_number       = 0;
      input_number_bytes = 2;

      mat_universe->ship_read = mat_universe->ship_read->next;
    }
}

@
@<Functions@>=
void
input_process_frame ( unsigned int c )
{
  switch (c)
    {
    case MMP_SERVER_FRAME_DONE:
      input_process = input_process_default;
      break;
    default:
      fprintf(stderr, "Unknown message: [msg:%d][state:Frame]\n", c);
      exit(EXIT_FAILURE);
    }
}

@ This gets executed once the client gets to join a universe.  It adds as many
ships as specified by |ai_arg_concurrent|.

@<Get ships in the default universe@>=
{
  input_string       = NULL;
  input_string_len   = 0;
  input_string_siz   = 0;

  @<Initialize |mat_universe|@>;

  input_process      = input_process_string;
  input_process_done = input_process_shipname;
}

@
@<Initialize |mat_universe|@>=
{
  int i;

  mat_universe = malloc(sizeof(MatUniverse));
  if (!mat_universe)
    {
      fprintf(stderr, "%s: malloc: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  for (i = 0; i < MAT_SHIP_HASH_SIZE; i ++)
    mat_universe->ship_hash[i] = NULL;

  mat_universe->read = 0;

  mat_universe->ship_list_head = NULL;
  mat_universe->ship_list_tail = NULL;

  mat_universe->size_x = 1200;
  mat_universe->size_y = 1000;
}

@

@<Functions@>=
void
input_process_shipname ( void )
{
  if (!input_string || !*input_string)
    @<Report successful join to the universe@>@;
  else
    {
      REGISTER_SHIP_TAIL(input_string);

      input_string       = NULL;
      input_string_len   = 0;
      input_string_siz   = 0;

      input_process      = input_process_string;
      input_process_done = input_process_shipname;
    }
}

@ When this code gets executed, the name of a ship in the universe is in
input_string.  We register it.

@d REGISTER_SHIP_TAIL(xname)
{
  MatShip *ship;

  int pos;

  REGISTER_SHIP_STANDARD(xname);

  ship->ship->next = NULL;
  ship->ship->prev = mat_universe->ship_list_tail;

  if (mat_universe->ship_list_tail)
    mat_universe->ship_list_tail->next = ship->ship;
  else
    mat_universe->ship_list_head = ship->ship;

  mat_universe->ship_list_tail = ship->ship;
}

@d REGISTER_SHIP_HEAD(xname)
{
  MatShip *ship;

  int pos;

  REGISTER_SHIP_STANDARD(xname);

  ship->ship->next = mat_universe->ship_list_head;
  ship->ship->prev = NULL;

  if (mat_universe->ship_list_head)
    mat_universe->ship_list_head->prev = ship->ship;
  else
    mat_universe->ship_list_tail = ship->ship;

  mat_universe->ship_list_head = ship->ship;
}

@d REGISTER_SHIP_STANDARD(xname)
{
  ship = malloc(sizeof(MatShip));
  if (!ship)
    {
      fprintf(stderr, "%s: malloc: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  ship->name = xname;

  ship->ship = malloc(sizeof(MatMovingObj));
  if (!ship->ship)
    {
      fprintf(stderr, "%s: malloc: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  pos = fhash(ship->name) % MAT_SHIP_HASH_SIZE;

  ship->next = mat_universe->ship_hash[pos];
  mat_universe->ship_hash[pos] = ship;
}

@d REMOVE_SHIP(xname)
{
  MatShip      *ship;
  int           pos;

  pos = fhash(xname) % MAT_SHIP_HASH_SIZE;

  ship = mat_universe->ship_hash[pos];

  ASSERT(ship);

  if (!strcmp(ship->name, xname))
    mat_universe->ship_hash[pos] = ship->next;
  else
    {
      MatShip *tmp;

      ASSERT(ship->next);
      while (strcmp(ship->next->name, xname))
        {
          ship = ship->next;

          ASSERT(ship->next);
          ASSERT(ship->next->name);
        }

      tmp = ship->next;
      ship->next = ship->next->next;
      ship = tmp;
    }

  if (ship->ship->prev)
    ship->ship->prev->next = ship->ship->next;
  else
    mat_universe->ship_list_head = ship->ship->next;

  if (ship->ship->next)
    ship->ship->next->prev = ship->ship->prev;
  else
    mat_universe->ship_list_tail = ship->ship->prev;

  free(ship->ship);
  free(ship->name);
  free(ship);
}

@
@<Report successful join to the universe@>=
{
  input_process = input_process_default;

  printf("Successfully joined default universe.\n");

  fprintf(mat_server_out, "%c%c%c%s%c", MMP_CLIENT_DEFAULT_SHIPADD, 0, 0, "AICode", 0);
  fflush(mat_server_out);
}

@

@<Functions@>=
void
input_process_joinfail ( unsigned int c )
{
  switch (c)
    {
    case MMP_SERVER_JOINFAIL_UNEXISTANT:
      fprintf(stderr, "%s: Default universe not available\n", program_name);
      break;

    default:
      fprintf(stderr, "Unknown message: [msg:%d][state:JoinFail]\n", c);
      break;
    }
  exit(EXIT_FAILURE);
}

@ Here we read all the information available on how the world has changed.

@<Process incomming events@>=
{
  int read;

  while ((read = getc(mat_server_in)) != EOF)
    {
      /*VERBOSE("Got: %d (%c)\n", read, read);*/
      input_process((unsigned int) read);
    }
}

@* Program Initialization

In this section we will write all the code that initializes the Matanza
Aritificial Inteligence system.

@<Initialize environment@>=
{
  @<Set the program name@>;
  @<Parse command line arguments@>;
  @<Load the artificial inteligence@>;
  @<Connect to the server@>;
}

@ Here we set the global variable |program_name|.  It is used across the
program at the beginning of all the error messages.

If the program name contains slashes, we discard everything before the last of
them.

@<Set the program name@>=
{
  program_name = strrchr(argv[0], '/');
  if (program_name)
    program_name++;
  else
    program_name = argv[0];
}

@
@<Load the artificial inteligence@>=
{
}

@
@<Global variables@>=

char *arg_host = "localhost";
int   arg_port = 7793;

char *arg_src  = NULL;

@
@<Parse command line arguments@>=
{
  int iterate         = 1;
  int show_help       = 0;
  int show_version    = 0;

  while (iterate)
    {
      int oi = 0;
      static struct option lopt[] =
      {
        { "help",               no_argument,       NULL, 'h' },
        { "version",            no_argument,       NULL, 'v' },
        { "host",               required_argument, NULL,  1  },
        { "port",               required_argument, NULL,  2  },
	{ 0, 0, 0, 0 }
      };

      switch (getopt_long(argc, argv, "hvm:bW:H:x:y:l:A:t:p::", lopt, &oi))
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
        case 1:
          arg_host = optarg;
          break;
        case 2:
          arg_port = atoi(optarg);
          break;
	default:
	  printf("Try '%s --help' for more information.\n", program_name);
	  exit(EXIT_FAILURE);
	}
    }

  if (show_help)
    {
      printf("Usage: %s [OPTION]... [FILE]\n"
             "Execute a Matanza Artificial Inteligence from FILE\n\n"
             "  -h, --help                  Show this information and exit\n"
             "  -v, --version               Show version number and exit\n"
             "  --host=HOST                 Connect to HOST\n"
             "  --port=NUM                  Use TCP port NUM\n"
             "\n"
             "Report bugs to <bachue@@bachue.com>\n", program_name);
      exit(EXIT_SUCCESS);
      /* TODO: List parameter ai!!! */
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
}

@ Here we perform the DNS and port lookup and attempt to connect to all known
addresses for the server specified.

@<Connect to the server@>=
{
  struct sockaddr_in   addr;
  struct hostent      *host = NULL;
  int                  i = 0;
  int                  fd;

  @<Lookup the server@>;

  /* Get a socket file descriptor. */
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
    {
      fprintf(stderr, "%s: socket: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  /* Connect to the server. */
  while (connect(fd, &addr, sizeof(struct sockaddr_in)) == -1)
    if (errno != EINTR)
      {
        if (!host || !host->h_addr_list[++i])
          {
            fprintf(stderr, "%s: %s: %s\n", program_name, arg_host, strerror(errno));
            exit(EXIT_FAILURE);
          }

        memmove((caddr_t)&addr.sin_addr, host->h_addr_list[i], host->h_length);
      }

  @<Make socket non blocking@>;
  @<Open |mat_server_in| and |mat_server_out|@>;
  @<Send initialization string to the server@>;
}

@
@<Send initialization string to the server@>=
{
  fprintf(mat_server_out, "%c%s%c%s%c", MMP_CLIENT_INITIAL_ID,
          PACKAGE " - Artificial Inteligence System", 0, VERSION, 0);
  fflush(mat_server_out);
}

@ Here we make it so reads and writes to the socket won't block the process.

@<Make socket non blocking@>=
{
  int opts;

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
}

@ This one calls |fdopen| on |fd| to open |mat_server_in| and |mat_server_out|.

@<Open |mat_server_in| and |mat_server_out|@>=
{
  mat_server_in  = fdopen(fd, "r");
  if (!mat_server_in)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  mat_server_out = fdopen(fd, "w");
  if (!mat_server_out)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }
}

@
@<Lookup the server@>=
{
  /* Initialize the sockaddr_in structure. */
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_addr.s_addr = inet_addr(arg_host);
  if (addr.sin_addr.s_addr != -1)
    addr.sin_family = AF_INET;
  else
    {
      host = gethostbyname(arg_host);
      if (!host)
        {
#if HAVE_HSTRERROR
          fprintf(stderr, "%s: %s: %s\n", program_name, arg_host, hstrerror(errno));
#else
          fprintf(stderr, "%s: %s: Host not found\n", program_name, arg_host);
#endif
          exit(EXIT_FAILURE);
        }

      addr.sin_family = host->h_addrtype;
      memmove((caddr_t)&addr.sin_addr, host->h_addr_list[0], host->h_length);
    }

  addr.sin_port = htons(arg_port);
}

@* State of the Program

In this section we will add information used to represent the state of the
world in memory.

@ First, lets define a structure we can use to keep a record of the ships in a
universe.

@<Typedefs@>=
typedef struct MatShip            MatShip;

@ Lets see the fields for every |MatShip| object.

We will need to keep the name of the ship in the |name field|.

In MMP, we receive information about the state of every ship individually, and
they are identified by their name.  That means we need to have fast access to
every ship identified by the name.  We will keep a hash with every player.
|next| will hold a pointer to the next node in the current list in the hash.

Finally, rather than keep the whole information about the ship here, we use the
struct |MatMovingObj|.

@<Struct definitions@>=
struct MatShip
{
  char                    *name;
  MatMovingObj            *ship;
  MatShip                 *next;
};

@ We spoke about |MatMovingObj| so we better define it.  It will keep all the
information sent through MMP about all the moving objects.

@<Typedefs@>=
typedef struct MatMovingObj       MatMovingObj;

@ In every MatMovingObj we keep the information about a single object in the
universe.

The fields |pos_x| and |pos_y| contain the object's coordinates.  The current
angle of the object is kept in |ang|.

As you can see, for both |pos_x|, |pos_y| and |ang|, we use an array with two
possible values.  The field |read| in this objects' universe indicates on which
field we are reading from the network, and the other is the one that gets used
during AI's execution.

@<Struct definitions@>=
struct MatMovingObj
{
  int                  pos_x[2];
  int                  pos_y[2];
  int                  ang[2];

  MatMovingObj        *next;
  MatMovingObj        *prev;
};

@ Lets now define the information we keep for every universe.

@<Typedefs@>=
typedef struct MatUniverse MatUniverse;

@ Fields |size_x| and |size_y| hold the size of the universe.

In |ship_hash| we keep a hash with all the players so they can be easily
accessed by their name.  |ship_list| is the list of all the players, so it's
fast to paint them all.

|ship_read| points to the ship currently being read from the network.

We store a count of the number of frames received in |frames|.

@d MAT_SHIP_HASH_SIZE 47

@<Struct definitions@>=

struct MatUniverse
{
  int             size_x, size_y;

  int             read;

  unsigned long   frames;

  MatMovingObj   *ship_read;

  MatShip        *ship_hash[MAT_SHIP_HASH_SIZE];

  MatMovingObj   *ship_list_head;
  MatMovingObj   *ship_list_tail;
};

@
@<Global variables@>=
MatUniverse *mat_universe = NULL;

@* AI Related stuff

@<Typedefs@>=
typedef struct AIInstruction AIInstruction;

@
@<Struct definitions@>=

struct AIInstruction
{
  int            code;
  int            arg1;
  int            arg2;
  int            arg3;
  AIInstruction *next;
};

@

@d AI_INST_STORE 0

@<Execute one instruction@>=
{
  AIInstruction *in;

  in = ai_program[ai_current];

  switch (in->code)
    {
    /* mem[arg2] = arg1 */
    case 1:
      ai_hash_set(in->arg2, in->arg1);
      break;

    /* mem[arg2] = mem[arg1] */
    case 2:
      ai_hash_set(in->arg2, ai_hash_get(in->arg1));
      break;

    /* mem[mem[arg2]] = arg1 */
    case 4:
      ai_hash_set(ai_hash_get(in->arg2), in->arg1);
      break;

    /* mem[mem[arg2]] = mem[arg1] */
    case 3:
      ai_hash_set(ai_hash_get(in->arg2), ai_hash_get(in->arg1));
      break;

    /* mem[arg2] = screen[arg1 x arg2] */
    case 4:
      if (in->arg1 > 
      ai_hash_set(in->arg2, ai_hash_get(in
    }
}

@
@<Execute |AI_INST_SAVE_IM|@>=
{
}

@
@<Execute |AI_INST_SAVE_REF|@>=
{
}

@ Lets include all the files we can.  We should check this and make sure it
really is what we want.

@<Include files@>=
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <math.h>
#include <limits.h>

#include <shipreal.c>

#include <mmp.h>

@ Now some mechanisms for assertions.

@d ASSERT(x) if(!(x)) fprintf(stderr, "%s:%d: ASSERT failed\n", __FILE__, __LINE__), assert_exit()

@d ASSERT_FOR(x) for (x)

@<Function prototypes@>=
void assert_exit ( void );

@ The function assert_exit is used so you can have your debugger stop execution
when an assertion fails.

@<Functions@>=
void assert_exit ( void )
{
  exit(-1);
}

@ Define |VERBOSE| to empty to get rid of the messages.

@d VERBOSE printf

@
@<Portability@>=
#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#define PACKAGE "matanza"
#define VERSION "{Unknown-Version}"
#endif

@<Include files@>

#if ! HAVE_HSTRERROR
#  define hsterror(x) "Host not found"
#endif
