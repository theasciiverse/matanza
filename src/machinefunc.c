#include <matanza.h>

#if MAT_BUILD_MMP

void                      mat_machine_input_shipadd_report ( void               *funcship,
                                                             MatMachine         *m );

void
mat_machine_input_shipadd_report ( void *funcship, MatMachine *m )
{
  MatShip *ship = (MatShip *) funcship;
  MatConnection *con;
  MatScm *un;
  unsigned int id;

  ASSERT(ship);
  ASSERT(m);

  if (mat_ship_machine_get(ship) == m)
    return;

  con = mat_machine_connection_get(m);
  un  = mat_ship_universe_get(ship);

  ASSERT(con);
  ASSERT(un);

  id = mat_universe_id_get(un);

  mat_connection_printf(con, "%c%c%c%s%c", MMP_SERVER_DEFAULT_SHIPADD_OTHER,
                        id >> 8, id & 0x00ff, mat_movingobj_name_get(ship), 0);

  mat_connection_flush(con);
}

#endif
