#include <matanza.h>

#if MAT_BUILD_MMP

typedef struct MatMachineShipadd MatMachineShipadd;

struct MatMachineShipadd
{
  unsigned int  id;
  unsigned int  id_byte;

  char          name[MAT_USER_MAX + 1];
  unsigned int  name_pos;
};

static void               mat_machine_input_shipadd        ( MatMachine         *m,
                                                             void               *funcdata,
                                                             char                key );

static void               mat_machine_input_shipadd_now    ( MatMachine         *m,
                                                             MatMachineShipadd  *data );

INLINE void
mat_machine_shipadd_set ( MatMachine *m )
{
  MatMachineShipadd *data;

  data = malloc(sizeof(MatMachineShipadd));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->id       = 0;
  data->id_byte  = 0;
  data->name_pos = 0;

  mat_machine_key_handler_set(m, &mat_machine_input_shipadd, data);
}

static void
mat_machine_input_shipadd ( MatMachine *m, void *funcdata, char key )
{
  MatMachineShipadd *data = (MatMachineShipadd *) funcdata;

  if (data->id_byte < 2)
    {
      data->id = (data->id << 8) + key;
      data->id_byte ++;
    }
  else if (key == 0)
    {
      data->name[data->name_pos] = 0;
      mat_machine_input_shipadd_now(m, data);
    }
  else if (isalnum(key) && data->name_pos < MAT_USER_MAX)
    data->name[data->name_pos++] = key;
}

static void
mat_machine_input_shipadd_now ( MatMachine *m, MatMachineShipadd *data )
{
  MatUniverse   *un;
  MatConnection *con;
  MatShip       *ship;

  ASSERT(m);
  ASSERT(data);

  mat_machine_default_set(m);

  un = mat_universes[data->id % MAT_UNIVERSES_HASH_SIZE];
  while (un && mat_universe_id_get(un) != data->id)
    un = mat_universe_hash_next_get(un);

  con = mat_machine_connection_get(m);

  ASSERT(con);

  if (!un)
    {
      mat_connection_printf(con, "%c%c", MMP_SERVER_DEFAULT_SHIPADD_FAIL, MMP_SERVER_SHIPADDFAIL_UNEXISTANT);
      mat_connection_flush(con);
      return;
    }

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("MMP: Adding ship: [universe:%d][ship:%s]\n", mat_universe_id_get(un), data->name);

  ship = mat_ship_new(un, mat_scm_string_new(data->name, strlen(data->name)), NULL, m, NULL, , mat_scm_cons_car_get(mat_universe_shiptype_head_get(un)));

  free(data->name);
  free(data);

  if (!ship)
    {
      mat_connection_free(con, MMP_SERVER_CLOSE_SYSERROR, strerror(errno));
      return;
    }

  mat_connection_flush(con);
}

#endif
