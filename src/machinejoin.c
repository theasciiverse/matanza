#include <matanza.h>

#if MAT_BUILD_MMP

/* TODO: Add a maximum per-machine limit to the number of subscriptions so a
 * single mean machine won't be able to eat all our bandwidth. */

typedef struct MatMachineJoin MatMachineJoin;

struct MatMachineJoin
{
  unsigned int  universe_id;
  unsigned int  universe_id_byte;
};

static void               mat_machine_input_join           ( MatMachine         *m,
                                                             void               *data,
                                                             char                key );

INLINE static void        mat_machine_input_join_reply     ( MatMachine         *m,
                                                             MatUniverse        *un );

INLINE static void        mat_machine_input_join_image     ( void               *m,
                                                             MatImageVector     *img,
                                                             unsigned int        id );


INLINE void
mat_machine_join_set ( MatMachine *m )
{
  MatMachineJoin *data;

  data = malloc(sizeof(MatMachineJoin));
  if (!data)
    {
      fprintf(stderr, "%s:%s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->universe_id      = 0;
  data->universe_id_byte = 0;

  mat_machine_key_handler_set(m, &mat_machine_input_join, data);
}

static void
mat_machine_input_join ( MatMachine *m, void *funcdata, char key )
{
  MatMachineJoin *data = (MatMachineJoin *) funcdata;
  MatUniverse    *un;
  MatConnection  *c;

  ASSERT(data);
  ASSERT(data->universe_id_byte < 2);

  data->universe_id = (data->universe_id << 8) + key;

  if (data->universe_id_byte ++ < 1)
    return;

  c = mat_machine_connection_get(m);

  un = mat_universes[data->universe_id % MAT_UNIVERSES_HASH_SIZE];
  while (un && mat_universe_id_get(un) != data->universe_id)
    un = mat_universe_hash_next_get(un);

  if (!un)
    {
      free(data);
      mat_connection_printf(c, "%c%c", MMP_SERVER_DEFAULT_JOIN_FAIL, MMP_SERVER_JOINFAIL_UNEXISTANT);
      mat_connection_flush(c);
      return;
    }

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("MMP: Machine attempts to join universe: [%d]\n", mat_universe_id_get(un));

  if (!mat_sub_add(m, un))
    {
      free(data);
      mat_connection_free(c, MMP_SERVER_CLOSE_SYSERROR, strerror(errno));
      return;
    }

  mat_machine_input_join_reply(m, un);

  free(data);

  mat_machine_default_set(m);
}

INLINE static void
mat_machine_input_join_reply ( MatMachine *m, MatScm *un )
{
  MatConnection *c;
  MatMovingObj *obj;

  unsigned int   mx;
  unsigned int   my;
  unsigned int   ang;
  unsigned int   img;

  ASSERT(un);
  ASSERT(m);

  c = mat_machine_connection_get(m);

  mx  = mat_universe_mapsize_x_get(un);
  my  = mat_universe_mapsize_y_get(un);
  ang = mat_universe_angles_get(un);
  img = mat_universe_image_count_get(un);

  mat_connection_printf(c, "%c" "%c%c" "%c%c" "%c%c" "%c%c",
                        MMP_SERVER_DEFAULT_JOIN_OK,
                        mx  >> 8, mx  & 0x00ff,
                        my  >> 8, my  & 0x00ff,
                        ang >> 8, ang & 0x00ff,
                        img >> 8, img & 0x00ff);

  mat_universe_image_forall(un, mat_machine_input_join_image, c);

  obj = *mat_universe_obj_head_get(un, MAT_MOVINGOBJ_TYPE_SHIP);

  while (obj)
    {
      ASSERT(mat_movingobj_type_get(obj) == MAT_MOVINGOBJ_TYPE_SHIP);
      ASSERT(mat_movingobj_name_get(mat_movingobj_data_get(obj)));
      ASSERT(mat_scm_string_length_get(mat_movingobj_name_get(mat_movingobj_data_get(obj))) > 0);

      mat_connection_printf(c, "%s%c", mat_movingobj_name_get(mat_movingobj_data_get(obj)), 0);
      obj = mat_movingobj_next_type_get(obj);
    }

  mat_connection_printf(c, "%c", 0);

  mat_connection_flush(c);
}

static void
mat_machine_input_join_image ( void *c, MatImageVector *v, unsigned int id )
{
  mat_connection_printf((MatConnection *) c, "%c%c", id >> 8, id & 0x00ff);
}

#endif
