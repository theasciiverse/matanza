#include <matanza.h>

#if MAT_BUILD_MMP

struct MatMachine
{
  MatSub          *subs[MAT_SUB_MACHINE_HASH_SIZE];
  MatConnection   *con;

  /* Hash with all the ships connected through this machine. */
  MatShip             *ship_hash[MAT_CONNECTION_SHIP_HASH_SIZE];

  /* Points to the first ship in the list. */
  MatShip             *ship_list;

  /* The number of ships connected through this machine. */
  int                  ship_count;

  unsigned int         ship_id_next;

  /* Used to parse the keys. */
  void               (*key_handler) ( MatMachine *, void *data, char );
  void                *key_data;
};


INLINE static void        mat_machine_update_universe      ( void               *m,
                                                             MatUniverse        *un );

INLINE static void        mat_machine_update_obj           ( void               *funcdata,
                                                             MatMovingObj       *obj );

INLINE static void        mat_machine_update_ship          ( void               *funcdata,
                                                             MatMovingObj       *obj );

INLINE MatSub **
mat_machine_subs_get ( MatMachine *m )
{
  return m->subs;
}

void *
mat_machine_new ( MatConnection *con )
{
  MatMachine *data;
  int i;

  data = malloc(sizeof(MatMachine));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->con = con;

  data->ship_count = 0;
  for (i = 0; i < MAT_CONNECTION_SHIP_HASH_SIZE; i++)
    data->ship_hash[i] = NULL;
  data->ship_list = NULL;
  data->ship_id_next = 0;

  mat_machine_init_set(data);

  mat_sub_machine_init(data);

  mat_connection_input_handler_set(con, mat_machine_input, data);

  mat_connection_state_set(con, MAT_STATE_MACHINE);

  return data;
}

INLINE void
mat_machine_input ( void *funcdata, unsigned char key )
{
  MatMachine *m = (MatMachine *) funcdata;

  ASSERT(m);
  ASSERT(m->key_handler);

  m->key_handler(m, m->key_data, key);
}

INLINE void
mat_machine_key_handler_set ( MatMachine *m, void (*func) ( MatMachine *, void *, char ), void *data )
{
  m->key_handler = func;
  m->key_data    = data;
}

/* If the client sends a message we can't understand, we shut down its
 * connection.  We are very strict and only talk to those we can understand. */

INLINE void
mat_machine_unknown_command ( MatMachine *m, int key )
{
  char buffer[sizeof("Client sent unknown command: ?") + 1];

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Unknown command from machine: %d (%c)\n", key, key);

  sprintf(buffer, "Client sent unknown command: %c", key);

  mat_connection_free(m->con, MMP_SERVER_CLOSE_MMPERROR, buffer);
}

INLINE MatConnection *
mat_machine_connection_get ( MatMachine *m )
{
  ASSERT(m);
  ASSERT(m->con);
  return m->con;
}

INLINE MatUniverse *
mat_machine_universe_get ( MatMachine *m, unsigned int id )
{
  return mat_sub_universe_find_get(m->subs[id % MAT_SUB_MACHINE_HASH_SIZE], id);
}

INLINE MatShip *
mat_machine_ship_get ( MatMachine *m, unsigned int id )
{
  MatShip *ship = m->ship_hash[id % MAT_CONNECTION_SHIP_HASH_SIZE];

  while (ship)
    if (mat_ship_machine_id_get(ship) == id)
      return ship;
    else
      ship = mat_ship_machine_hash_next_get(ship);

  return NULL;
}

INLINE unsigned int
mat_machine_ship_id_get ( MatMachine *m )
{
  unsigned int id = m->ship_id_next;

  while (mat_machine_ship_get(m, id))
    id ++;

  return id;
}

INLINE void
mat_machine_ship_add ( MatMachine *m, MatShip *ship )
{
  unsigned int pos;

  ASSERT(m);
  ASSERT(ship);

  mat_ship_machine_id_set(ship, mat_machine_ship_id_get(m));

  m->ship_count ++;

  pos = mat_ship_machine_id_get(ship) % MAT_CONNECTION_SHIP_HASH_SIZE;

  mat_ship_machine_hash_next_set(ship, m->ship_hash[pos]);
  mat_ship_machine_hash_prev_set(ship, NULL);

  if (m->ship_hash[pos])
    mat_ship_machine_hash_prev_set(m->ship_hash[pos], ship);

  m->ship_hash[pos] = ship;

  mat_ship_machine_list_next_set(ship, m->ship_list);
  mat_ship_machine_list_prev_set(ship, NULL);

  if (m->ship_list)
    mat_ship_machine_list_prev_set(m->ship_list, ship);

  m->ship_list = ship;
}

INLINE void
mat_machine_universe_forall ( MatMachine *m, void (*func) ( void *, MatUniverse * ), void *data )
{
  unsigned int      i;
  MatSub           *s;

  ASSERT(m);
  ASSERT(func);

  for (i = 0; i < MAT_SUB_MACHINE_HASH_SIZE; i ++)
    for (s = m->subs[i]; s; s = mat_sub_machine_next_get(s))
      {
        ASSERT(mat_sub_machine_get(s) == m);
        func(data, mat_sub_universe_get(s));
      }
}

INLINE void
mat_machine_update ( MatMachine *m )
{
  ASSERT(m);

  mat_machine_universe_forall(m, mat_machine_update_universe, m);
  mat_connection_flush(m->con);
}

INLINE static void
mat_machine_update_universe ( void *funcdata, MatUniverse *un )
{
  MatMachine *m = (MatMachine *) funcdata;

  unsigned int id;
  MatMovingObjType type;

  ASSERT(m);
  ASSERT(un);

  id = mat_universe_id_get(un);

  mat_connection_printf(m->con, "%c%c%c", MMP_SERVER_DEFAULT_FRAME_SHIPS, id >> 8, id & 0x00ff);

  mat_universe_movingobj_forall(un, MAT_MOVINGOBJ_TYPE_SHIP, mat_machine_update_ship, m);

  for (type = 0; type < MAT_MOVINGOBJ_TYPE_COUNT; type ++)
    if (type != MAT_MOVINGOBJ_TYPE_SHIP)
      mat_universe_movingobj_forall(un, type, mat_machine_update_obj, m);

  mat_connection_printf(m->con, "%c", MMP_SERVER_FRAME_DONE);
}

INLINE static void
mat_machine_update_obj ( void *funcdata, MatMovingObj *obj )
{
  MatMachine *m = (MatMachine *) funcdata;

  unsigned int img;

  ASSERT(m);
  ASSERT(obj);

  /* TODO: Enable */
  //img = mat_movingobj_image_id_get(obj);

  mat_connection_printf(m->con, "%c" "%c%c" "%c%c", MMP_SERVER_FRAME_OBJS,
                        1   >> 8, 1   & 0x00ff,
                        img >> 8, img & 0x00ff);


  mat_machine_update_ship(m, obj);
}

INLINE static void
mat_machine_update_ship ( void *funcdata, MatMovingObj *obj )
{
  MatMachine *m = (MatMachine *) funcdata;
  unsigned int pos_x, pos_y, ang;

  pos_x = mat_movingobj_pos_x_center_get(obj);
  pos_y = mat_movingobj_pos_y_center_get(obj);
  ang   = mat_movingobj_ang_get(obj);

  mat_connection_printf(m->con, "%c%c" "%c%c" "%c",
                        pos_x >> 8, pos_x & 0x00ff,
                        pos_y >> 8, pos_y & 0x00ff,
                        ang);
}

INLINE void
mat_machine_message_vprintf ( MatMachine *m, unsigned int id, char *fmt, va_list args )
{
  char buffer[2048];
  ASSERT(m);

  /* TODO: Arg, find out some way to make sure they never vfprintf something
   * longer than our buffer.  Or use some other function. */

  vsprintf(buffer, fmt, args);

  mat_connection_printf(m->con, "%c%c%c%s%c",
                        MMP_SERVER_DEFAULT_MESSAGE,
                        id >> 8, id & 0x00ff,
                        buffer, 0);
}

#endif
