#include <matanza.h>

static void               mat_scm_ship_free                ( void               *funcdata );

int                       mat_scm_ship_equal               ( MatScm             *img0,
                                                             MatScm             *img1 );

static MatScmCallback     prop_get;
static MatScmCallback     msg_add;
static MatScmCallback     ship_new;
static MatScmCallback     player;

MatScmType mat_scm_ship_type =
{
  "ship",
  mat_scm_ship_free,
  NULL,
  NULL,
  NULL,
  mat_scm_ship_equal
};

MatScm                   *mss_message_add = NULL;
MatScm                   *mss_new = NULL;
MatScm                   *mss_player = NULL;
MatScm                   *mss_name_get = NULL;
MatScm                   *mss_movingobj_get = NULL;

int
mat_scm_ship_equal ( MatScm *img0, MatScm *img1 )
{
  /* TODO: Write */
  return 0;
}

static void
mat_scm_ship_free ( void *funcdata )
{
  // mat_ship_free(funcdata);
}

void
mat_scm_ship_init ( void )
{
  mat_scm_obj_protect(&mss_message_add);
  mat_scm_obj_protect(&mss_new);
  mat_scm_obj_protect(&mss_player);
  mat_scm_obj_protect(&mss_name_get);
  mat_scm_obj_protect(&mss_movingobj_get);

  mss_message_add = mat_scm_proc_new(msg_add, NULL, 1);
  mss_new = mat_scm_proc_new(ship_new, NULL, 1);
  mss_player = mat_scm_proc_new(player, NULL, 1);
  mss_name_get = mat_scm_proc_new(prop_get, mat_ship_name_get, 1);
  mss_movingobj_get = mat_scm_proc_new(prop_get, mat_ship_movingobj_get, 1);
}

void
mat_scm_ship_define ( MatScm *env )
{
  mat_scm_env_define(env, "matanza-ship-message-add", mss_message_add);
  mat_scm_env_define(env, "matanza-ship-new", mss_new);
  mat_scm_env_define(env, "matanza-ship-player?", mss_player);
  mat_scm_env_define(env, "matanza-ship-name-get", mss_name_get);
  mat_scm_env_define(env, "matanza-ship-movingobj-get", mss_movingobj_get);
}

static MatScm *
msg_add ( void *data, MatScm *env, MatScm *args )
{
  MatScm *ship;
  char *msg;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  ship = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(ship);

  if (mat_scm_obj_type_get(ship) == &mat_scm_ship_type)
    ship = mat_ship_movingobj_get(ship);

  ASSERT(ship);
  ASSERT(mat_scm_obj_type_get(ship) == &mat_scm_movingobj_type);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_string_type);

  msg = mat_scm_string_data_get(mat_scm_cons_car_get(args));

  ASSERT(!mat_scm_cons_cdr_get(args));

  mat_movingobj_message_printf(ship, "%s", msg);

  return NULL;
}

static MatScm *
ship_new ( void *data, MatScm *env, MatScm *args )
{
  MatScm *un;
  MatScm *name;
  MatScm *type;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  un = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(un);
  if (mat_scm_obj_type_get(un) == &mat_scm_ship_type)
    un = mat_ship_movingobj_get(un);

  ASSERT(un);
  if (mat_scm_obj_type_get(un) == &mat_scm_movingobj_type)
    un = mat_movingobj_universe_get(un);

  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universe_type);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  name = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  type = mat_scm_cons_car_get(args);

  ASSERT(!mat_scm_cons_cdr_get(args));

  ASSERT(mat_scm_obj_type_get(name) == &mat_scm_string_type);
  ASSERT(mat_scm_obj_type_get(type) == &mat_scm_shiptype_type);

  return mat_ship_new(un, name, NULL, NULL, NULL, type);
}

static MatScm *
player ( void *data, MatScm *env, MatScm *args )
{
  MatScm *obj;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  if (mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_movingobj_type)
    obj = mat_movingobj_ship_get(mat_scm_cons_car_get(args));

  ASSERT(obj->type == &mat_scm_ship_type);

  ASSERT(!mat_scm_cons_cdr_get(args));

  return mat_ship_player_get(obj) ? mat_scm_bool_true : NULL;
}

static MatScm *
prop_get ( void *data, MatScm *env, MatScm *args )
{
  MatScm *obj;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  obj = mat_scm_cons_car_get(args);

  ASSERT(!mat_scm_cons_cdr_get(args));

  if (mat_scm_obj_type_get(obj) == &mat_scm_movingobj_type)
    obj = mat_movingobj_ship_get(mat_scm_cons_car_get(args));

  ASSERT(obj);
  ASSERT(obj->type == &mat_scm_ship_type);

  return ((MatScm *(*) (MatScm *)) data)(obj);
}
