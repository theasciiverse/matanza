// MATANZA MACHINE MMP - MMP
//
// This is the master  file for the MMP.  It is formatted  as a C header
// file  to make  it possible  to  include it  into C  programs, but  it
// contains  the whole  documentation on  how to  implement a  compliant
// server/client.
//
// It should be noted that the MMP is currently in development stage and
// is  very likely  to change  in the  future.  Keep  that in  mind when
// developing your clients.
//
// MMP is  the protocol Matanza servers  and clients talk.  It  is being
// designed with the following goals:
//
// 1. Making it possible to use  Matanza proxies, reducing the bandwidth
// at the server.  That should help it scale very much.
//
// 2. Making  it possible  to  implement artificial  inteligence to  run
// outside of  the Matanza process.  This  way, multiple implementations
// of  artificial inteligence  might be  connected to  the same  server,
// regardless of the programming languages they were written on.
//
// 3. Help  others  implement different  clients  to  the Matanza  game.
// Specially, Sergio Garcia,  who said he would soon begin  to work on a
// graphical Java-based client.
//
// MMP  has not  been designed  to  be human-readable.   Doing so  would
// increase the bandwidth required by MMP-talking programs.
//
// On the other hand, one of the  design goals of MMP was that it should
// be fairly easy to implement  a MMP-speaking program.  This means that
// it doesn't use any complicated compression algorithms of any sort.
//
// As you will see, it is possible to control multiple ships in multiple
// universes (once  multiple universes gets supported  in Matanza) using
// only  one connection.   This  adds little  complexity,  but makes  it
// possible to have multiple clients connected to one proxy, having that
// proxy use only one connection to the central server.
//
// In  this  initial  version  of  MMP,  TCP/IP  connections  are  used.
// However,  it should  be noted  that  there is  many information  that
// should be  sent over UDP  rather than TCP  (while it is  important to
// send other using TCP).  In the  future, two programs talking MMP will
// probably use both  one TCP connection for important  messages and one
// UDP  connection for  the rest.   For example,  information about  the
// positions of the objects is better sent over UDP, while messages such
// as the fact  that a given ship  controled by the client  has been hit
// should  be  sent  over  TCP.   We are  aiming  at  making  this  work
// completely over TCP first, avoiding  the complexity of using both TCP
// and UDP, to eventually start sending messages through the appropriate
// channel.
//
// All the  messages in MMP begin  with a single character,  followed by
// any relevant  arguments.  Basically, both  the server and  the client
// are  at special  defined states  and  the messages  they may  receive
// depend on this information.
//
// In MMP, unless said otherwise, all numbers are sent as two bytes, the
// first being the  most important (for example, the  number 12345 would
// be sent  as the  numbers 48 and  57, since  48 * 256  + 57  = 12345).
// Strings are sent  as 0-terminated strings.  For example,  to send the
// string "ABC", one would send the numbers 65 (`A'), 66 (`B'), 67 (`C')
// and 0.
//
// Images  are sent  as two  two-bytes  numbers (the  width and  height)
// followed by width x height bytes for each position in the image.  For
// example, the image
//
//     ABCJ
//     DEFJ
//     GHIJ
//
// is  sent   as  the  numbers   4  and   3,  followed  by   the  stream
// "ABCJDEFJGHIJ".  In hexa that whould look like
//
//   +-------+--------+-------------------------------+
//   | width | height |  A B  C J  D E  F J  G H  I J |
//   +-------+--------+-------------------------------+
//   |  0004 |   0003 | 4142 434A 4445 464A 4748 494A |
//   +-------+--------+-------------------------------+
//
// The  messages   designed  to  be   sent  by  the  server   have  been
// prefixed with  `MMP_SERVER_'.  Those  sent by  the client  begin with
// `MMP_CLIENT_'.   They have  been grouped  depending on  the state  on
// which they are allowed.
//
// We will first describe the messages sent by the server and then those
// sent by the clients.
//
// The initial states for both servers  and clients once a connection is
// established is Initial.

// FUNCTIONALITY STILL MISSING IN MMP
//
// As said  above, MMP  is still undergoing  heavy development.   As you
// will see, there are some things that  are not yet possible to do with
// the current specification.
//
// The following is provided  as a list of things that  you will be able
// to do with MMP, but not yet.
//
// - Add support for teams
//
//    None  of the  messages has  the concept  of teams.   It should  be
//    possible to play with teams.
//
// - Add chat functionality
//
//    It should be possible to  send public and private messages through
//    MMP.
//
// - Complete this list
//
//    There are  still a  few things  missing, that we  must add  to the
//    list.  If you come up with any, email Alejandro so he adds it.

// MESSAGES SENT BY THE SERVER
//
// The following messages  can be sent by the server  when the client is
// in  the  Initial  state  (that  is, right  after  the  connection  is
// established).
//
// Actually,  the server  will wait  for a  MMP_CLIENT_INITIAL_ID before
// sending any of these replies.

#define MMP_SERVER_CLOSE                                                  0

//   MMP_SERVER_CLOSE
//
//       An error took  place at the server.  The client's  state is set
//       to Close.

#define MMP_SERVER_INITIAL_FULL                                           1

//   MMP_SERVER_INITIAL_FULL
//
//       Indicates that the server is currently full.  The client should
//       try to  connect latter.  The  server must close  the connection
//       after sending this message.

#define MMP_SERVER_INITIAL_CONNECTED                                      2

//   MMP_SERVER_INITIAL_CONNECTED
//
//       Indicates that the operation was successful and sets the client
//       on the Default  state.  This message contains  two strings used
//       to identify the  server.  The first is the  server's name (such
//       as  "MatanzaServer") and  the second  is its  version (such  as
//       "0.11").
//
//       A server can only send this message after MMP_CLIENT_INITIAL_ID
//       has been received.

// The following messages  can be sent by the server  when the client is
// in the Default state:

//   MMP_SERVER_CLOSE
//
//       This message  indicates that the  server is about to  close the
//       connection.  The client enters the Close state.

#define MMP_SERVER_DEFAULT_FRAME_SHIPS                                    1

//   MMP_SERVER_DEFAULT_FRAME_SHIPS
//
//       The server uses this message to begin sending information about
//       one of the universes that the client is subscribed to.
//
//       A simple frame  contains the whole information  about the state
//       of the world.
//
//       This message  has two arguments.   The first  is the id  of the
//       universe it  will begin  to update.  The  second argument  is a
//       vector with  the following information  for every ship  on that
//       universe:
//
//           pos_x - The position of the ship in the X coordinate.  This
//           is a two-bytes number.
//
//           pos_y - The position of the ship in the Y coordinate.  This
//           is a two-bytes number.
//
//           ang -  The current angle of  the ship.  This is  a one-byte
//           number.
//
//       The client must know how many  ships every universe has and how
//       they are ordered.
//
//       This message sets the client's state to Frame.

#define MMP_SERVER_DEFAULT_SHIPADD_CLIENT                                 3

//   MMP_SERVER_DEFAULT_SHIPADD_CLIENT
//
//       This message is sent by  the server when the client succesfully
//       adds a ship to a universe.  This message has no arguments.
//
//       For every  MMP_CLIENT_DEFAULT_SHIPADD request, the  server will
//       reply  with  either  a MMP_SERVER_DEFAULT_SHIPADD_CLIENT  or  a
//       MMP_SERVER_DEFAULT_SHIPADD_FAILURE message.   This replies will
//       be  sent in  the same  order as  the MMP_CLIENT_DEFAULT_SHIPADD
//       requests are received.
//
//       The new ship is added at the beginning of the list of ships for
//       the universe.  Note that the order  of this list is the same as
//       used  for the  arguments to  the MMP_SERVER_DEFAULT_FRAME_SHIPS
//       message.
//
//       The Id  through which  the client  may refer  to the  ship (two
//       bytes) is sent as the only argument of this message.
//
//       See MMP_CLIENT_DEFAULT_SHIPADD for more information.

#define MMP_SERVER_DEFAULT_SHIPADD_FAIL                                   4

//   MMP_SERVER_DEFAULT_SHIPADD_FAIL
//
//       If an  attempt to add  a ship to  a universe (indicated  by the
//       client  sending  a MMP_CLIENT_DEFAULT_SHIPADD  message)  fails,
//       the  server  will  send  this reply.   The  client  enters  the
//       ShipAddFailure state.

#define MMP_SERVER_DEFAULT_SHIPADD_OTHER                                  5

//   MMP_SERVER_DEFAULT_SHIPADD_OTHER
//
//       This message  is sent by the  server to notify the  client that
//       someone else  has added  a ship  to one  of the  universes' the
//       client is subscribed to.
//
//       The new ship is added at the beginning of the list of ships for
//       the universe.  Note that the order  of this list is the same as
//       used  for the  arguments to  the MMP_SERVER_DEFAULT_FRAME_SHIPS
//       message.
//
//       The message has the universe  id (number) and the player's name
//       (string) as its arguments.

#define MMP_SERVER_DEFAULT_DAMAGE                                         6

//   MMP_SERVER_DEFAULT_DAMAGE
//
//       Whenever one of  the ships controled by the  client is damaged,
//       the server  will send this  message.  The argument is  a number
//       with the ammount of damage.   It is the clients' responsability
//       to  keep track  of  its ships'  damage to  know  when they  are
//       finally destroyed.

#define MMP_SERVER_DEFAULT_JOIN_OK                                        7

//   MMP_SERVER_DEFAULT_JOIN_OK
//
//       This message does not modify the client's state.
//
//       It   is    sent   by   the    server   as   a   reply    to   a
//       MMP_CLIENT_DEFAULT_JOIN   that   resulted   in   a   successful
//       subscription to a universe.
//
//       The message sends the mapsize in  the x coordinate first and in
//       the y  coordinate second.  After  that, it sends the  number of
//       angles for all objects in this universe.
//
//       The next argument  is a vector with the Ids  for all the images
//       in  the  universe.   The  vector's  size is  sent  first  as  a
//       two-bytes  number  and then  the  Ids  are sent  (as  two-bytes
//       numbers too).
//
//       Finally, is a  list with the names for all  the ships currently
//       in the universe is sent.  When no more ships are left, an empty
//       string is sent.
//
//       Note  that  the  order  of  the ships  is  relevant  as  it  is
//       in  this  order   that  their  information  will   be  sent  in
//       MMP_SERVER_DEFAULT_FRAME_SHIPS.
//
//       See MMP_CLIENT_DEFAULT_JOIN for more information.

#define MMP_SERVER_DEFAULT_JOIN_FAIL                                      8

//   MMP_SERVER_DEFAULT_JOIN_FAIL
//
//       This  message  indicates that  an  attempt  to subscribe  to  a
//       universe (sent by the  client using the MMP_CLIENT_DEFAULT_JOIN
//       message) failed.   The client  is then  set on  the JoinFailure
//       state.

#define MMP_SERVER_DEFAULT_SHIPRM                                         9

//   MMP_SERVER_DEFAULT_SHIPRM
//
//       This  message  is used  by  the  server  to inform  the  client
//       that  a particular  ship in  a  given universe  has left.   The
//       client's  state  is not  modified,  but  it should  record  the
//       change  in the  list  of  ships for  the  universe, as  further
//       MMP_SERVER_DEFAULT_FRAME_SHIPS  messages will  not contain  any
//       information for the ship.
//
//       The message's arguments  are the universe's id  (two bytes) and
//       the ship's name.

#define MMP_SERVER_DEFAULT_PONG                                          10

//   MMP_SERVER_DEFAULT_PONG
//
//       Whenever   the   server  receives   a   MMP_CLIENT_DEFAULT_PING
//       message,  it   replies  with  this  one.    The  only  argument
//       this  message  carries  is  the   number  that  came  with  the
//       MMP_CLIENT_DEFAULT_PING message.

#define MMP_SERVER_DEFAULT_IMGSET                                        11

//   MMP_SERVER_DEFAULT_IMGSET
//
//       This message is used to  transfer information about images from
//       the server to the client.
//
//       The   server  sends   these  messages   whenever  it   receives
//       MMP_CLIENT_DEFAULT_IMGGET requests.
//
//       This message has as its argument the image requested in all the
//       angles.

#define MMP_SERVER_DEFAULT_MESSAGE                                       12

//   MMP_SERVER_DEFAULT_MESSAGE
//
//       The  server sends  this message  to  a machine  when a  message
//       should be sent to one of its ships.
//
//       This  message has  the  ship's ID  (two-bytes  number) and  the
//       message (0-terminated string) as its only arguments.

// The following messages are sent when the client is in the JoinFailure
// state.  They are  used by the server to indicate  the reasons why the
// request failed.  All of them set the client's state to the Default:

#define MMP_SERVER_JOINFAIL_UNEXISTANT                                    0

//   MMP_SERVER_JOINFAIL_UNEXISTANT
//
//       This message  is sent  when the  client attempts  to join  to a
//       universe that does not exist.

#define MMP_SERVER_JOINFAIL_AUTH                                          1

//   MMP_SERVER_JOINFAIL_AUTH
//
//       This message will be defined in the future.

// The  following  messages  are  valid   when  the  client  is  in  the
// ShipAddFailure state.  They all bring  the client back to the Default
// state.

#define MMP_SERVER_SHIPADDFAIL_UNEXISTANT                                 1

//   MMP_SERVER_SHIPADDFAIL_UNEXISTANT
//
//       The universe specified does not exist.

#define MMP_SERVER_SHIPADDFAIL_FULL                                       2

//   MMP_SERVER_SHIPADDFAIL_FULL

#define MMP_SERVER_SHIPADDFAIL_BADNAME                                    3

//   MMP_SERVER_SHIPADDFAIL_BADNAME
//
//       This error happens when the name for the ship has problems.  A string
//       describing the error with the name specified is sent.

// The following are the messages that the client can receive during the
// Close  state.  The  server  must always  close  the connection  after
// sending any of this messages.

#define MMP_SERVER_CLOSE_SHUTDOWN                                         0

//   MMP_SERVER_CLOSE_SHUTDOWN
//
//       Indicates that the server is  closing the connection because it
//       is being shutdown.

#define MMP_SERVER_CLOSE_SYSERROR                                         1

//   MMP_SERVER_CLOSE_SYSERROR
//
//       Indicates that the server is  closing the connection because it
//       came  across  some  error  during  a  system  call.   A  string
//       describing the error must be provided.

#define MMP_SERVER_CLOSE_MMPERROR                                         2

//   MMP_SERVER_CLOSE_MMPERROR
//
//       Indicates  that the  reason for  the server  to be  closing the
//       connection  is that  it violated  the rules  set forth  in this
//       documentation and  attempted to do something  ilegal.  A string
//       describing the error must be provided.

#define MMP_SERVER_CLOSE_CLIENT                                           3

//   MMP_SERVER_CLOSE_CLIENT
//
//       Indicates that the server is  closing the connection because it
//       was asked by the client to do so.

#define MMP_SERVER_FRAME_DONE                                             1

//   MMP_SERVER_FRAME_DONE
//
//       Sends the  client to  the default state.   This message  has no
//       arguments.  This is used by the server to indicate that no more
//       objects are available in a given universe.

#define MMP_SERVER_FRAME_OBJS                                             2

//   MMP_SERVER_FRAME_OBJS
//
//       This message  is used  to pass information  about objects  of a
//       given type.  The  first parameter passed is  a two-bytes number
//       specifing  the number  of objects  that are  going to  be sent.
//       Next comes  the id  of the image  for those  objects (two-bytes
//       number).
//
//       The  last argument  is a  vector with  the objects.   For every
//       object  we  send  the  x position  (two-bytes  number),  the  y
//       position (two-bytes number) and the angle (one-byte number).

// MESSAGES SENT BY THE CLIENT
//
// Right after the connection is established, the client must send the
// following message:

#define MMP_CLIENT_INITIAL_ID                                             1

//   MMP_CLIENT_INITIAL_ID
//
//       This is almost always the  first message sent once a connection
//       is established.
//
//       The  message carries  two arguments,  both strings.   The first
//       identifies  the   client  (for  example,   "Matanza  Artificial
//       Inteligence Client")  and the second provides  its version (for
//       example, "0.12").
//
//       None of these strings should be longer than 256 characters.
//
//       After  sending  this  message,  the  client  must  wait  for  a
//       reply  from  the  server.   This   reply  can  be  any  of  the
//       MMP_SERVER_INITIAL_* messages or MMP_SERVER_CLOSE.
//
//       The  clients must  wait  for  a reply  to  this message  before
//       sending any other messages.

// The following messages  can be sent by the client  when the server is
// in the Default state:

#define MMP_CLIENT_DEFAULT_JOIN                                           1

//   MMP_CLIENT_DEFAULT_JOIN
//
//       This message is sent by the  client to subscribe to a universe.
//       The only argument it takes is the universe's id.

#define MMP_CLIENT_DEFAULT_SHIPADD                                        2

//   MMP_CLIENT_DEFAULT_SHIPADD
//
//       This message indicates that the client wants to add a ship to a
//       universe.  The  message has  two arguments,  the first  being a
//       number identifying the  universe and the second  being the name
//       of the player.

#define MMP_CLIENT_DEFAULT_SHIPEVENT                                      3

//   MMP_CLIENT_DEFAULT_SHIPEVENT
//
//       This message is used by the  client to inform the server of one
//       event  for a  ship.  Examples  of events  are shoting  bullets,
//       turning, acelerating and so one.
//
//       The message  takes the ship's  id (number) as its  argument and
//       sets  the  server of  the  ShipEvent  state, on  which  further
//       information about the event is sent.

#define MMP_CLIENT_DEFAULT_PING                                           4

//   MMP_CLIENT_DEFAULT_PING
//
//       The  client can  use this  message to  check the  speed of  its
//       connection to the  server.  This request takes  one argument, a
//       number, which is returned  by the MMP_SERVER_DEFAULT_PONG reply
//       it generates.

#define MMP_CLIENT_DEFAULT_SHIPCMD                                        5

//   MMP_CLIENT_DEFAULT_SHIPCMD
//
//       Used by the client to send the  server a command for one of its
//       ships.  The  command has  as its arguments  the ship's  Id (two
//       bytes,  as  returned by  the  MMP_SERVER_DEFAULT_SHIPADD_CLIENT
//       message) and the specification of the command (one byte), which
//       can be any of the following:

#define MMP_CLIENT_DEFAULT_SHIPCMD_PAUSE                                  0
#define MMP_CLIENT_DEFAULT_SHIPCMD_LEFT                                   1
#define MMP_CLIENT_DEFAULT_SHIPCMD_RIGHT                                  2
#define MMP_CLIENT_DEFAULT_SHIPCMD_ACCEL                                  3
#define MMP_CLIENT_DEFAULT_SHIPCMD_BRAKE                                  4
#define MMP_CLIENT_DEFAULT_SHIPCMD_SHOT_BIG                               5

#define MMP_CLIENT_DEFAULT_IMGGET                                         6

//   MMP_CLIENT_DEFAULT_IMGGET
//
//       The client uses this command to query the server for a specific
//       image in one given universe.
//
//       This requests has two arguments:  the universe's Id (two bytes)
//       and the image's Id (two bytes).
//
//       Whenever the server receives this  request, it replies with one
//       MMP_SERVER_DEFAULT_IMGSET message.
