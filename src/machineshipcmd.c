#include <matanza.h>

#if MAT_BUILD_MMP

typedef struct MatMachineShipcmd MatMachineShipcmd;

struct MatMachineShipcmd
{
  unsigned int  id;
  unsigned int  id_byte;
};

INLINE static void        mat_machine_input_shipcmd        ( MatMachine         *m,
                                                             void               *data,
                                                             char                key );

INLINE void
mat_machine_shipcmd_set ( MatMachine *m )
{
  MatMachineShipcmd *data;

  data = malloc(sizeof(MatMachineShipcmd));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->id       = 0;
  data->id_byte  = 0;

  mat_machine_key_handler_set(m, &mat_machine_input_shipcmd, data);
}

INLINE static void
mat_machine_input_shipcmd ( MatMachine *m, void *funcdata, char key )
{
  MatMachineShipcmd *data = (MatMachineShipcmd *) funcdata;

  ASSERT(data);

  if (data->id_byte < 2)
    {
      data->id = (data->id << 8) + key;
      data->id_byte ++;
    }
  else
    {
      MatShip *ship;

      ASSERT(data->id_byte == 2);

      ship = mat_machine_ship_get(m, data->id);
      free(data);

      mat_machine_default_set(m);

      if (!ship)
        {
          mat_connection_free(mat_machine_connection_get(m), MMP_SERVER_CLOSE_MMPERROR, "Ship not found");
          return;
        }

      switch (key)
        {
        case MMP_CLIENT_DEFAULT_SHIPCMD_PAUSE:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Client: PAUSE\n");
          mat_ship_key_pause(ship);
          break;

        case MMP_CLIENT_DEFAULT_SHIPCMD_BRAKE:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Client: BRAKE\n");
          mat_ship_key_down(ship);
          break;

        case MMP_CLIENT_DEFAULT_SHIPCMD_ACCEL:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Client: ACCEL\n");
          mat_ship_key_up(ship);
          break;

        case MMP_CLIENT_DEFAULT_SHIPCMD_LEFT:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Client: LEFT\n");
          mat_ship_key_left(ship);
          break;

        case MMP_CLIENT_DEFAULT_SHIPCMD_RIGHT:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Client: RIGHT\n");
          mat_ship_key_right(ship);
          break;

        case MMP_CLIENT_DEFAULT_SHIPCMD_SHOT_BIG:
          MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Client: SHOT BIG\n");
          mat_missile_new(mat_ship_movingobj_get(ship), MAT_MISSILE_SIZE_BIG, 1);
        }
    }
}

#endif
