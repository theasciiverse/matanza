#ifndef __MATANZA_H__
#  define __MATANZA_H__

#  ifdef WIN32
#    include <winconfig.h>
#  else /* !WIN32 */
#    ifdef HAVE_CONFIG_H
#      include <config.h>
#    else /* !HAVE_CONFIG_H */
#      define PACKAGE "matanza"
#      define VERSION "{Unknown-Version}"
#    endif /* !HAVE_CONFIG_H */
#  endif /* !WIN32 */

#  define INLINE inline

/* If you change MAT_USER_MAX, change MAT_USER_BLANK: It should hold
 * MAT_USER_MAX spaces. */

#define MAT_USER_MAX       8

/* The maximum length for the passwords */
#define MAT_PASSWORD_MAX   8

/* The maximum allowed length for a team's name. */
#define MAT_TEAM_NAME_MAX  8

/* The default image width for Telnet connections: how much of the world does he see? */
/* TODO: Experiment if increasing this a bit makes thing better, it is my
 * intuition that the first thing most players do is zoom out so perhaps we do
 * that for them. */
#  define MAT_IMGWIDTH_DEFAULT  80 * 4

/* The maximum number of characters in a universe's name. */
#  define MAT_UNIVERSE_NAME_MAX 32

/* TODO: What's this? */
#  define MAT_UNIVERSE_MAX    256

/* TODO: Rename to MAT_UNIVERSE_HASH_SIZE. */
/* The size of the array in the hash with universes. */
#  define MAT_UNIVERSES_HASH_SIZE 1

/* The size of the array in the hash of images in one universe. */
#  define MAT_UNIVERSE_IMAGE_HASH_SIZE 15

/* The size of the hash of ships in a given connection. */
#define MAT_CONNECTION_SHIP_HASH_SIZE 13

/* The size of the hash of universes a given connection is subscribed to.  We
 * use an incredibly small value for the hash size since, for the moment, we
 * only support one universe. */
/* TODO: Once support for multiple universes is added, increase the size of the
 * hash. */
#define MAT_SUB_MACHINE_HASH_SIZE 1

/* The maximum size for a name of a terminal (such as vt100). */
#  define MAT_TELNET_TERM_NAME_MAX 14

/* Make sure the following is required: */
/* TODO: Rename to MAT_SPEED_MAX */
/* TODO: What's this? */
#  define ANGLE_SPEED_MAX 1

/* This are the codes used by the Telnet handler to represent the arrows (these
 * codes get passed to the handker functions). */

#  define KEY_ARROW_UP    -10
#  define KEY_ARROW_DOWN  -11
#  define KEY_ARROW_RIGHT -12
#  define KEY_ARROW_LEFT  -13

/* C header files hell! */

/* Standard C header files. */

#  define __USE_ISOC99 1

#  include <stdarg.h>
#  include <signal.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
#  include <errno.h>
#  include <ctype.h>
#  include <fcntl.h>
#  include <stdarg.h>
#  include <math.h>
#  include <limits.h>
#  include <time.h>

/* Headers for Windows */

#  if HAVE_WINSOCK2_H
#    include <winsock2.h>
#  endif /* HAVE_WINSOCK2_H */

#  if HAVE_WINDOWS_H
#    include <windows.h>
#  endif /* HAVE_WINDOWS_H */

#  if HAVE_DIRECT_H
#    include <direct.h>
#  endif /* HAVE_DIRECT_H */

/* Headers for BeOS */

#  if HAVE_OS_H
#    include <OS.h>
#  endif /* HAVE_OS_H */

#  if HAVE_ERRORS_H
#    include <Errors.h>
#  endif /* HAVE_ERRORS_H */

/* Headers for Unix */

#  if HAVE_NETDB_H
#    include <netdb.h>
#  endif /* HAVE_NETDB_H */

#  if HAVE_UNISTD_H
#    include <unistd.h>
#  endif /* HAVE_UNISTD_H */

#  if HAVE_NETINET_IN_H
#    include <netinet/in.h>
#  endif /* HAVE_NETINET_IN_H */

#  if HAVE_ARPA_INET_H
#    include <arpa/inet.h>
#  endif /* HAVE_ARPA_INET_H */

#  if HAVE_SYS_IOCTL_H
#    include <sys/ioctl.h>
#  endif /* HAVE_SYS_IOCTL_H */

#  if HAVE_SYS_TIME_H
#    include <sys/time.h>
#  endif /* HAVE_SYS_TIME_H */

#  if HAVE_SYS_TYPES_H
#    include <sys/types.h>
#  endif /* HAVE_SYS_TYPES_H */

#  if HAVE_SYS_SOCKET_H
#    include <sys/socket.h>
#  endif /* HAVE_SYS_SOCKET_H */

#  ifdef HAVE_ARPA_TELNET_H
#    include <arpa/telnet.h>
#  else /* !HAVE_ARPA_TELNET_H */
#    define	SE	240		/* end sub negotiation */
#    define	BREAK	243		/* break */
#    define	IP	244		/* interrupt process--permanently */
#    define	AO	245		/* abort output--but let prog finish */
#    define	AYT	246		/* are you there */
#    define	SB	250		/* interpret as subnegotiation */
#    define	WILL	251		/* I will use option */
#    define	WONT	252		/* I won't use option */
#    define	DO	253		/* please, you use option */
#    define	DONT	254		/* you are not to use option */
#    define	IAC	255		/* interpret as command: */

#    define TELOPT_ECHO	1	/* echo */
#    define	TELOPT_SGA	3	/* suppress go ahead */
#    define	TELOPT_LOGOUT	18	/* force logout */
#    define	TELOPT_TTYPE	24	/* terminal type */
#    define	TELOPT_NAWS	31	/* window size */
#    define	TELOPT_TSPEED	32	/* terminal speed */
#    define TELOPT_LINEMODE	34	/* Linemode option */

#    define	LM_MODE		1
#    define	LM_SLC		3

#    define	TELQUAL_IS	0	/* option is... */
#    define	TELQUAL_SEND	1	/* send option */
#  endif /* !HAVE_ARPA_TELNET_H */

#  ifndef MAX
#    define MAX(x,y) ((x)>(y)?(x):(y))
#  endif

#  ifndef MIN
#    define MIN(x,y) ((x)<(y)?(x):(y))
#  endif

#  ifndef ABS
#    define ABS(x) ((x)<0?-(x):(x))
#  endif

/* The following files are bundled with Matanza.  If you get errors with
 * the following lines, make sure you have the directory where your Matanza
 * source code resides as part of your standard includes (for gcc, this is
 * specified using the parameter -I). */

#  include <getopt.h>

#  include <mmp.h>

#  include <machine.h>
#  include <machinedefault.h>
#  include <machineimage.h>
#  include <machineinit.h>
#  include <machinejoin.h>
#  include <machineshipadd.h>
#  include <machineshipcmd.h>
#  include <machinefunc.h>

#  include <distance.h>

#  include <network.h>
#  include <message.h>
#  include <connection.h>
#  include <asciirender.h>
#  include <image.h>
#  include <imagevector.h>
#  include <universe.h>
#  include <universetype.h>
#  include <movingobj.h>
#  include <shiptype.h>
#  include <team.h>
#  include <sub.h>
#  include <telnet.h>
#  include <anglecache.h>

#  include <palette.h>

#  include <ship.h>
#  include <shipcrash.h>
#  include <shipkey.h>

#  include <renderrotate.h>
#  include <renderimage.h>

#  include <dlist.h>

#  include <scm.h>
#  include <scmbegin.h>
#  include <scmbool.h>
#  include <scmcase.h>
#  include <scmchar.h>
#  include <scmclosure.h>
#  include <scmcond.h>
#  include <scmconnection.h>
#  include <scmcons.h>
#  include <scmdefine.h>
#  include <scmenv.h>
#  include <scmeq.h>
#  include <scmfile.h>
#  include <scmif.h>
#  include <scmimage.h>
#  include <scmimagevector.h>
#  include <scmlet.h>
#  include <scmlist.h>
#  include <scmload.h>
#  include <scmmovingobj.h>
#  include <scmnumber.h>
#  include <scmproc.h>
#  include <scmquote.h>
#  include <scmread.h>
#  include <scmrepl.h>
#  include <scmship.h>
#  include <scmshiptype.h>
#  include <scmspecial.h>
#  include <scmstring.h>
#  include <scmsymbol.h>
#  include <scmteam.h>
#  include <scmtelnet.h>
#  include <scmuniverse.h>
#  include <scmuniversetype.h>
#  include <scmverbose.h>
#  include <scmwindow.h>
#  include <scmwidget.h>
#  include <scmwidgetplay.h>

#  include <window.h>
#  include <windowbuffer.h>

#  include <widget.h>
#  include <widgetbanner.h>
#  include <widgetbutton.h>
#  include <widgetlabel.h>
#  include <widgetlist.h>
#  include <widgetplay.h>
#  include <widgetplaydraw.h>
#  include <widgetplaykey.h>
#  include <widgetrender.h>
#  include <widgettext.h>

#  include <menu0.h>
#  include <menu1.h>

#  include <animation000.h>
#  include <animation001.h>
#  include <animation002.h>
#  include <animationfire.h>

#if 0
/* TODO: Move this elsewhere, we don't want to have structures in matanza.h. */

typedef struct MatImgWorld MatImgWorld;

struct MatImgWorld
{
  MatImage      *img;
  char          *path;

  MatImgWorld   *next;
  MatImgWorld   *prev;
};
#endif

#include <common.h>

/* Now the prototypes for the global variables. */

/* Contains the default image for a ship. */
MatScm           *arg_ship_graphic;

/* TODO: Rename to mat_program_name. */
/* The program's name.  Used for error reports. */
char             *program_name;

/* Movement type: space, air. */
int               arg_space;

/* Can the players brake their ship. */
int               arg_brake;

/* Can the players fly backwards. */
/* TODO: Make sure this paremeter is respected. */
int               arg_fly_back;

/* The clients are expected to provide the indicated password. */
char             *arg_password;

/* By default, when asked to terminate, we won't free anything (to avoid
 * possibly causing page faults).  However, when running Matanza under a memory
 * leaks detector, freeing everything makes things easier.  We will use
 * parameter arg_free_at_exit to know whether we should free memory when
 * terminating the program. */
int               arg_free_at_exit;

/* Socket for incoming players. */
MatSocketT        mat_mainsock;

/* Socket for incoming machines. */
MatSocketT        mat_mainsock_machine;

/* TCP port where we listen for incoming Telnet connections. */
int               arg_port;

/* TCP port where we listen for MMP connections. */
int               arg_port_machine;

/* Seconds to wait between updates. */
unsigned long     arg_usleep;

/* The default number of angles per universe. */
int               arg_ang;

/* The default number of lives given to the players. */
int               arg_lives;

/* Are the players allowed to exchange messages by default? */
int               arg_chat;

/* TODO: Rename to mat_connections_count */
/* The number of connections. */
int               players_count;

/* The maximum number of connections that are allowed. */
/* TODO: Rename to mat_connections_max. */
int               arg_max_players;

/* TODO: Rename to mat_connections. */
/* The first connection in the list of connections. */
MatConnection    *connections;

/* The number of universes in mat_universes. */
int               mat_universes_count;

/* Should we use bold characters for Telnet connections. */
int               arg_bold;

/* Default ship friction. */
double            arg_ship_friction;

/* TODO: What's this? */
int               arg_imgwidth;

/* Default players' health. */
int               arg_health;

/* Default universes' width. */
int               arg_mapsize_x;

/* Default universes' height. */
int               arg_mapsize_y;

/* Number of players per Telnet connection. */
/* TODO: Remove this and let each individual connection specify how many
 * players it has. */
int               arg_ppc;

/* Should we use our compression algorithm for Telnet connections (involves
 * moving the cursor a lot). */
int               arg_compress;

/* If 1, the universes' will, by default, be given hard limits so the player
 * won't be able to go from one edge to the other but will rather crash against
 * it. */
int               arg_limits;

/* Should we draw the background dots. */
int               arg_bg_dots;

/* TODO: What's this?  The distance between the dots in the background? */
int               arg_bg_adj;

/* TODO: What's this? */
int               arg_bg_color_max;

/* If true  and a  background image  is specified  in the  command line
 * arguments, the arg_mapsize_x and arg_mapsize_y variables are ignored and the
 * universes' sizes are taken from their background images' sizes. */
int               arg_bg_size;

/* Is it safe for members of the same team to shot themselves?  If false, it is
 * possible for two ships to hit each other even if their players are on the
 * same team. */
int               arg_teams_safe;

/* Do the members of a team share their lives? */
int               arg_teams_share;

/* Maximum angular speed for a ship. */
double            arg_ang_speed_max;

/* Maximum linear speed for a ship. */
double            arg_speed_max;

/* The number of asteroids stored in the default universe. */
int               arg_ast_num;

/* If non-NULL, the default ships' image is loaded from the specified file. */
char             *arg_ship_graphic_path;

/* The default number of rounds that a player is allowed to become invisible. */
int               arg_visible_init;

/* If true, a good but slow algorithm will be used for scaling images for
 * Telnet players.  This is disabled by default as it tends to require lots
 * of procesing time as the number of players increases and they all zoom
 * out. */
int               arg_image_scale_good;

/* Do we allow the players to see the direction for other players? */
int               mat_arg_radar;

/* Only update objects every mat_arg_update_delay ticks.  The closer
 * to zero, the more often objects are updated. */
int               mat_arg_update_delay;

/* Should we use the mist effect? */
int               mat_arg_mist;

/* Should we apply a motion blur effect to the way we draw objects that
 * are moving fast with respect to whoever views them?  Leaving it on makes
 * the game better but requires more processing power. */
int               mat_arg_motion_blur;

/* When the universe is updated, it is updated in a series of steps (this is
 * used to detect collisions better, I guess).  How many steps should we divide
 * each update into? */
int               arg_universe_update_steps;


#if 0
/* The list of background images, drawn before all other objects. */
MatImgWorld      *mat_bg_head;
MatImgWorld      *mat_bg_tail;

/* The list of foreground images, drawn after all other objects. */
MatImgWorld      *mat_fg_head;
MatImgWorld      *mat_fg_tail;
#endif

#endif
