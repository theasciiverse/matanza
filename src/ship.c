#include <matanza.h>

struct MatShip
{
  /* Points to the MatMovingObj for this ship. */
  MatScm              *obj;

  /* The type of this ship */
  MatScm              *shiptype;

  /* The team this ship is in and the next ship in the same team. */
  MatScm              *team;

  /* The entry in the list of ships in this team. */
  MatScm              *team_list;

  unsigned int         speed_inc;

  unsigned int         times_dead;

  int                  bullets_expire[4];
  int                  bullets[4];

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

  /* Now fields used to identify the ship in the connection.
   *
   * conn_id is the id for this ship among the ships for this connection.
   *
   * conn points to the connection or, if it was lost, its set to NULL
   * (this can happen if the connection is gone).
   *
   * conn_hash_next and conn_hash_prev point to the previous and next nodes
   * in the list in the hash of ships for the connection, while
   * conn_list_next and conn_list_prev are used to traverse the list with
   * all the ships for the connection. */

  MatScm              *conn;
  unsigned int         conn_id;
  MatScm              *conn_hash_list;
  MatScm              *conn_list_list;

  MatScm              *widget;

  /* The name of the controler of this ship. */
  MatScm              *name;


};

INLINE static MatShip    *data_get                         ( MatScm             *obj );

static void               mat_ship_free                    ( MatScm             *obj );

#if 0
MatMovingObjTypeD mat_ship_type =
{
  "ship",
  mat_ship_update,
  mat_ship_crash,
  mat_ship_free
};
#endif

INLINE static MatShip *
data_get ( MatScm *obj )
{
  MatShip *data;

  ASSERT(obj);

  if (mat_scm_obj_type_get(obj) == &mat_scm_movingobj_type)
    obj = mat_movingobj_ship_get(obj);

  ASSERT(obj);
  ASSERT(mat_scm_obj_type_get(obj) == &mat_scm_ship_type);

  data = mat_scm_obj_data_get(obj);

  ASSERT(data);
  ASSERT(!data->obj || mat_scm_obj_type_get(data->obj) == &mat_scm_movingobj_type);
  ASSERT(data->shiptype);
  ASSERT(mat_scm_obj_type_get(data->shiptype) == &mat_scm_shiptype_type);
  ASSERT(data->name);
  ASSERT(mat_scm_obj_type_get(data->name) == &mat_scm_string_type);
  ASSERT(!data->conn || !data->widget);

  return data;
}

INLINE MatScm *
mat_ship_movingobj_get ( MatScm *sh )
{
  return data_get(sh)->obj;
}

MatScm *
mat_ship_new ( MatScm *un, MatScm *name, MatScm *team, MatScm *m, MatScm *w, MatScm *type )
{
  MatShip *data;
  MatScm  *ship;
  char    *addr;

  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universe_type);
  ASSERT(name);
  ASSERT(mat_scm_obj_type_get(name) == &mat_scm_string_type);
  ASSERT(type);
  ASSERT(mat_scm_obj_type_get(type) == &mat_scm_shiptype_type);

  data = malloc(sizeof(MatShip));
  if (!data)
    return NULL;

  data->conn     = m;
  data->widget   = w;

  data->shiptype = type;

  data->times_dead      = 0;

  //ship->view      = ship->obj;
  //ship->viewers   = ship;
  //ship->view_next = NULL;
  //ship->view_prev = NULL;

  //ship->refs      = 1;

  //ship->img_id    = 0;
  //ship->img       = mat_ship;

  //ship->draw      = 1;

  //ship->un = un;

  //MAT_CONNECTION_SHIP_ADD(m, s);

  /* TODO: Get it working with the right palette!!! */

  ship = mat_scm_obj_malloc(&mat_scm_ship_type, data);

  data->name = name;

  addr = "Unknown";

#if 0
  if (data->conn)
    {
      mat_machine_ship_add(m, ship);

      mat_connection_printf(mat_machine_connection_get(m), "%c%c%c",
                            MMP_SERVER_DEFAULT_SHIPADD_CLIENT,
                            data->conn_id >> 8, data->conn_id & 0x00ff);

#if 0
      mat_connection_printf(mat_machine_connection_get(m), "%c%c%c%s%c",
                            MMP_SERVER_DEFAULT_MESSAGE,
                            ship->conn_id >> 8, ship->conn_id & 0x00ff,
                            mat_universe_start_message_get(un), 0);
#endif

      addr = mat_connection_addr_get(mat_machine_connection_get(m));
    }
  else
    {
      /* TODO: Get the address, silly. */
      addr = "Unknown";
    }
#endif

  data->obj = NULL;
  data->team = NULL;
  data->team_list = NULL;

  data->obj = mat_movingobj_new(un, ship, mat_shiptype_image_get(data->shiptype), /*mat_palette_ship[0]*/ mat_palette_id, 0, mat_shiptype_health_get(type));
  if (!data->obj)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  mat_ship_team_set(ship, team);

  mat_universe_event_join_run(un, data->obj);
  mat_ship_start(ship);

  mat_universe_message_printf(un, "-=> %s (%s) joins the game", mat_scm_string_data_get(mat_ship_name_get(ship)), addr);

#if MAT_BUILD_MMP
  mat_universe_machine_forall(un, mat_machine_input_shipadd_report, ship);
#endif

  data_get(ship);

  return ship;
}

#if 0
@d GET_WORLD_HE(xpl, xwi)
  ((int) ((double) (xwi) * CORRECTION * (double) (xpl).imghe / (double) (xpl).imgwi))

@d GET_WORLD_WI(xpl, xhe)
  ((int) ((double) (xhe) * (double) (xpl).imgwi / ((double) (xpl).imghe * CORRECTION)))
#endif

void
mat_ship_start ( MatScm *ship )
{
  MatShip *data = data_get(ship);
  MatScm *un;
  double x, y;
  unsigned int i;

  ASSERT(ship);

  un = mat_movingobj_universe_get(mat_ship_movingobj_get(ship));

  //ship->crash     = 0;
  //ship->alpha     = 0.0;
  //ship->alpha_mod = mat_alpha_mod_ship_appear;

  /* For the moment, a random location.  In the future we might pick up one
   * from a list of locations or it might depend on the player's team or
   * something. */

  x = (double) (rand() % mat_universe_mapsize_x_get(un));
  y = (double) (rand() % mat_universe_mapsize_y_get(un));

  mat_movingobj_pos_set(ship, x, y);
  mat_movingobj_ang_set(ship, rand() % mat_universe_angles_get(un));

  mat_movingobj_sp_ang_set(ship, arg_ang_speed_max == 0 ? 0 : (double) (rand() % 5 - 2) / 10.0);
  mat_movingobj_sp_linear_set(ship, 0.0, 0.0);

  for (i = 0; i < MAT_MISSILE_SIZE_COUNT; i ++)
    {
      data->bullets[i] = mat_shiptype_missiles_get(data->shiptype, i);
      data->bullets_expire[i] = 0;
    }

  //ship->visible          = 1;
  //ship->visible_exp      = arg_visible_init;

  data->speed_inc = 0;

  mat_ship_health_reset(ship);

  //ship->pal                        = mat_ship_palette[2];

  //sprintf(message_get(ship), "%s", ship->un->start_message);
}

INLINE void
mat_ship_health_reset ( MatScm *ship )
{
  mat_movingobj_health_set(ship, mat_shiptype_health_get(mat_ship_shiptype_get(ship)));
}

void
mat_ship_update ( MatScm *ship )
{
  MatShip *data = data_get(ship);
  int i;

  for (i = 0; i < 4; i ++)
    if (data->bullets_expire[i])
      data->bullets_expire[i]--;

  if (data->speed_inc > 0)
    data->speed_inc--;

#if 0
  if (pl->alpha_mod)
    pl->alpha_mod(pl);

  if (!pl->info.player.visible && pl->info.player.visible_exp -- <= 0)
    {
      pl->info.player.visible = 1;
      VISIBILITY_BROADCAST(pl);
    }
#endif
}

INLINE unsigned int
mat_ship_times_dead_get ( MatScm *ship )
{
  MatShip *data = data_get(ship);
  return data->times_dead;
}

INLINE void
mat_ship_times_dead_set ( MatScm *ship, unsigned int times )
{
  MatShip *data = data_get(ship);
  data->times_dead = times;
}

INLINE MatScm *
mat_ship_team_get ( MatScm *ship )
{
  MatShip *data = data_get(ship);
  return data->team;
}

INLINE void
mat_ship_team_set ( MatScm *ship, MatScm *team )
{
  MatShip *data = data_get(ship);

  if (data->team)
    mat_team_ship_rm(data->team, data->team_list);

  data->team      = team;
  data->team_list = team ? mat_team_ship_add(team, ship) : NULL;
}

#if 0
INLINE MatShip *
mat_ship_team_prev_get ( MatShip *ship )
{
  ASSERT(ship);
  return ship->team_prev;
}

INLINE MatShip *
mat_ship_team_next_get ( MatShip *ship )
{
  ASSERT(ship);
  return ship->team_next;
}

INLINE void
mat_ship_team_prev_set ( MatShip *ship, MatShip *prev )
{
  ASSERT(ship);
  ship->team_prev = prev;
}

INLINE void
mat_ship_team_next_set ( MatShip *ship, MatShip *next )
{
  ASSERT(ship);
  ship->team_next = next;
}
#endif

void
mat_ship_shot ( MatScm *obj, MatMissileSize size, int dir )
{
  MatShip *data = data_get(obj);
  static int exp[] = { 3, 5, 15, 100 };

  if (!data->bullets[size])
    {
      mat_movingobj_message_printf(obj, "-=> Ooops, you've run out of those bullets !");
      return;
    }

  if (data->bullets_expire[size])
    return;

  data->bullets_expire[size] = exp[size];
  data->bullets[size]--;

  mat_movingobj_shot(obj, size, mat_missile_new(mat_ship_movingobj_get(obj), size, dir));
}

#if 0
static MatMovingObj *
mat_ship_create ( MatUniverse *un, char **atts )
{
  char          *name      = "Default";
  MatShipType   *type      = NULL;

  int i;

  ASSERT(un);
  ASSERT(atts);

  for (i = 0; atts[i]; i += 2)
    if (!strcmp(atts[i], "name"))
      name = atts[i + 1];
    else if (!strcmp(atts[i], "shiptype"))
      type = mat_universe_shiptype_find(un, atoi(atts[i + 1]));

  if (!type)
    type = mat_universe_shiptype_head_get(un);

  return mat_ship_movingobj_get(mat_ship_new(un, NULL, NULL, NULL, name, type));
}
#endif

void
mat_ship_rm ( MatScm *ship )
{
  MatShip *data = data_get(ship);
  //MatWidgetPlay *view;

  ASSERT(ship);
  ASSERT(mat_ship_alive_get(ship));

  if (data->team)
    mat_team_ship_rm(data->team, data->team_list);

  //mat_movingobj_alpha_set(ship->obj, 0);
  //mat_movingobj_crash_set(ship->obj, 0);

  mat_movingobj_sp_linear_real_set(ship, 0, 0, 0);
  mat_movingobj_sp_ang_set(ship, 0);

  mat_movingobj_rm(ship);
}

static void
mat_ship_free ( MatScm *obj )
{
  MatShip *data = data_get(obj);
  free(data);
}

INLINE int
mat_ship_alive_get ( MatScm *obj )
{
  return mat_movingobj_alive_get(obj);
}

INLINE void
mat_ship_mark ( MatScm *obj )
{
  MatShip *data = data_get(obj);
  mat_scm_obj_mark(data->shiptype);
  mat_scm_obj_mark(data->obj);
  mat_scm_obj_mark(data->name);
  mat_scm_obj_mark(data->conn);
  mat_scm_obj_mark(data->conn_hash_list);
  mat_scm_obj_mark(data->conn_list_list);
  mat_scm_obj_mark(data->widget);
}

INLINE unsigned int
mat_ship_speed_inc_get ( MatScm *ship )
{
  MatShip *data = data_get(ship);
  return data->speed_inc;
}

INLINE void
mat_ship_speed_inc_set ( MatScm *ship, unsigned int inc )
{
  MatShip *data = data_get(ship);
  data->speed_inc = inc;
}

INLINE MatScm *
mat_ship_shiptype_get ( MatScm *ship )
{
  MatShip *data = data_get(ship);
  return data->shiptype;
}

INLINE int
mat_ship_machine_id_get ( MatScm *obj )
{
  MatShip *data = data_get(obj);
  return data->conn_id;
}

INLINE void
mat_ship_machine_id_set ( MatScm *obj, unsigned int id )
{
  MatShip *data = data_get(obj);
  data->conn_id = id;
}

INLINE MatScm *
mat_ship_mmp_get ( MatScm *obj )
{
  MatShip *data = data_get(obj);
  return data->conn;
}

INLINE MatScm *
mat_ship_widgetplay_get ( MatScm *obj )
{
  MatShip *data = data_get(obj);
  return data->widget;
}

INLINE void
mat_ship_widgetplay_set ( MatScm *obj, MatScm *widget )
{
  MatShip *data = data_get(obj);
  data->widget = widget;
}

INLINE MatScm *
mat_ship_name_get ( MatScm *sh )
{
  MatShip *data = data_get(sh);
  return data->name;
}

INLINE unsigned int
mat_ship_player_get ( MatScm *obj )
{
  return mat_ship_mmp_get(obj) || mat_ship_widgetplay_get(obj);
}

INLINE void
mat_ship_die ( MatScm *ship )
{
  MatShip *data = data_get(ship);
  MatScm *team;
  int sent_out;
  unsigned int dead, cmp;

  team = mat_ship_team_get(ship);

  if (team)
    mat_team_times_dead_set(team, mat_team_times_dead_get(team) + 1);

  dead = mat_ship_times_dead_get(ship) + 1;
  mat_ship_times_dead_set(ship, dead);

  cmp = team && arg_teams_share ? mat_team_times_dead_get(team) : dead;
  sent_out = cmp >= mat_universe_maxdead_get(mat_movingobj_universe_get(mat_ship_movingobj_get(ship)));

  if (!sent_out)
    {
      //if (mat_ship_team_get(ship) && arg_teams_share)
      //mat_team_ship_forall(mat_ship_team_get(ship), cr_dead_team, NULL);
      //else
      mat_movingobj_message_printf(ship, "-=> You have lost %d ships now !", mat_ship_times_dead_get(ship));
      mat_ship_start(ship);
    }
  else
    {
      mat_movingobj_message_printf(ship, "-=> Game over !");
      mat_movingobj_rm(ship);
    }
}
