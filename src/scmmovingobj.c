#include <matanza.h>

typedef struct MatClosest MatClosest;

struct MatClosest
{
  MatScm       *src;
  MatScm       *dst;
  MatScm       *proc;
  double        dx;
  double        dy;
  double        d;
};

static void               mo_closest_check                 ( void               *funcdata,
                                                             MatScm             *dst );

static MatScm            *mo_build                         ( MatScm             *obj );

static MatScmCallback     mo_pos;
static MatScmCallback     mo_sp_linear;
static MatScmCallback     mo_sp_ang;
static MatScmCallback     mo_event_set;
static MatScmCallback     mo_closest;
static MatScmCallback     mo_ang_chase;
static MatScmCallback     mo_ang_set;
static MatScmCallback     mo_distance;
static MatScmCallback     mo_prop_get;
static MatScmCallback     mo_health_get;
static MatScmCallback     mo_health_set;
static MatScmCallback     mo_ttl_set;

MatScmType mat_scm_movingobj_type =
{
  "movingobj",
  free,
  mat_movingobj_mark,
  NULL,
  NULL,
  NULL
};

static MatScm            *smo_pos_set = NULL;
static MatScm            *smo_speed_linear_set = NULL;
static MatScm            *smo_speed_ang_set = NULL;
static MatScm            *smo_event_update_set = NULL;
static MatScm            *smo_event_crash_set = NULL;
static MatScm            *smo_event_shot_set = NULL;
static MatScm            *smo_closest_get = NULL;
static MatScm            *smo_angle_chase = NULL;
static MatScm            *smo_angle_set = NULL;
static MatScm            *smo_distance = NULL;
static MatScm            *smo_env_get = NULL;
static MatScm            *smo_ship_get = NULL;
static MatScm            *smo_universe_get = NULL;
static MatScm            *smo_health_get = NULL;
static MatScm            *smo_health_set = NULL;
static MatScm            *smo_ttl_set = NULL;

void
mat_scm_movingobj_init ( void )
{
  mat_scm_obj_protect(&smo_pos_set);
  mat_scm_obj_protect(&smo_speed_linear_set);
  mat_scm_obj_protect(&smo_speed_ang_set);
  mat_scm_obj_protect(&smo_event_update_set);
  mat_scm_obj_protect(&smo_event_crash_set);
  mat_scm_obj_protect(&smo_event_shot_set);
  mat_scm_obj_protect(&smo_closest_get);
  mat_scm_obj_protect(&smo_angle_chase);
  mat_scm_obj_protect(&smo_angle_set);
  mat_scm_obj_protect(&smo_distance);
  mat_scm_obj_protect(&smo_env_get);
  mat_scm_obj_protect(&smo_ship_get);
  mat_scm_obj_protect(&smo_universe_get);
  mat_scm_obj_protect(&smo_health_get);
  mat_scm_obj_protect(&smo_health_set);
  mat_scm_obj_protect(&smo_ttl_set);

  smo_pos_set = mat_scm_proc_new(mo_pos, NULL, 1);
  smo_speed_linear_set = mat_scm_proc_new(mo_sp_linear, NULL, 1);
  smo_speed_ang_set = mat_scm_proc_new(mo_sp_ang, NULL, 1);
  smo_event_update_set = mat_scm_proc_new(mo_event_set, mat_movingobj_event_update_set, 1);
  smo_event_crash_set = mat_scm_proc_new(mo_event_set, mat_movingobj_event_crash_set, 1);
  smo_event_shot_set = mat_scm_proc_new(mo_event_set, mat_movingobj_event_shot_set, 1);
  smo_closest_get = mat_scm_proc_new(mo_closest, NULL, 1);
  smo_angle_chase = mat_scm_proc_new(mo_ang_chase, NULL, 1);
  smo_angle_set = mat_scm_proc_new(mo_ang_set, NULL, 1);
  smo_distance = mat_scm_proc_new(mo_distance, NULL, 1);
  smo_env_get = mat_scm_proc_new(mo_prop_get, mat_movingobj_env_get, 1);
  smo_ship_get = mat_scm_proc_new(mo_prop_get, mat_movingobj_ship_get, 1);
  smo_universe_get = mat_scm_proc_new(mo_prop_get, mat_movingobj_universe_get, 1);
  smo_health_get = mat_scm_proc_new(mo_health_get, NULL, 1);
  smo_health_set = mat_scm_proc_new(mo_health_set, NULL, 1);
  smo_ttl_set = mat_scm_proc_new(mo_ttl_set, NULL, 1);
}

void
mat_scm_movingobj_define ( MatScm *env )
{
  mat_scm_env_define(env, "matanza-movingobj-pos-set", smo_pos_set);
  mat_scm_env_define(env, "matanza-movingobj-speed-linear-set", smo_speed_linear_set);
  mat_scm_env_define(env, "matanza-movingobj-speed-ang-set", smo_speed_ang_set);
  mat_scm_env_define(env, "matanza-movingobj-event-update-set", smo_event_update_set);
  mat_scm_env_define(env, "matanza-movingobj-event-crash-set", smo_event_crash_set);
  mat_scm_env_define(env, "matanza-movingobj-event-shot-set", smo_event_shot_set);
  mat_scm_env_define(env, "matanza-movingobj-closest-get", smo_closest_get);
  mat_scm_env_define(env, "matanza-movingobj-angle-chase", smo_angle_chase);
  mat_scm_env_define(env, "matanza-movingobj-angle-set", smo_angle_set);
  mat_scm_env_define(env, "matanza-movingobj-distance", smo_distance);
  mat_scm_env_define(env, "matanza-movingobj-environment-get", smo_env_get);
  mat_scm_env_define(env, "matanza-movingobj-ship-get", smo_ship_get);
  mat_scm_env_define(env, "matanza-movingobj-universe-get", smo_universe_get);
  mat_scm_env_define(env, "matanza-movingobj-health-get", smo_health_get);
  mat_scm_env_define(env, "matanza-movingobj-health-set", smo_health_set);
  mat_scm_env_define(env, "matanza-movingobj-ttl-set", smo_ttl_set);
}

static MatScm *
mo_prop_get ( void *data, MatScm *env, MatScm *args )
{
  MatScm *obj;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  obj = mo_build(mat_scm_cons_car_get(args));

  return ((MatScm *(*) (MatScm *)) data)(obj);
}

static MatScm *
mo_health_get ( void *data, MatScm *env, MatScm *args )
{
  MatScm *obj;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  obj = mo_build(mat_scm_cons_car_get(args));

  return mat_scm_number_new(mat_movingobj_health_get(obj));
}

#if 0
static MatScm *
mo_real ( void *data, MatScm *env, MatScm *args )
{
  MatScm *obj;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  if (mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == (MatScmType *) data)
    return mat_scm_cons_car_get(args);

  obj = mo_build(mat_scm_cons_car_get(args));

  ASSERT(!mat_scm_cons_cdr_get(args));

  ASSERT(data != &mat_scm_ship_type || mat_movingobj_type_get(obj) == MAT_MOVINGOBJ_TYPE_SHIP);
  ASSERT(data != &mat_scm_asteroid_type || mat_movingobj_type_get(obj) == MAT_MOVINGOBJ_TYPE_ASTEROID);
  ASSERT(data != &mat_scm_missile_type || mat_movingobj_type_get(obj) == MAT_MOVINGOBJ_TYPE_MISSILE);

  return mat_scm_obj_malloc((MatScmType *) data, mat_movingobj_data_get(obj));
}
#endif

#if 0
static MatScm *
mo_type ( void *data, MatScm *env, MatScm *args )
{
  char *type[] = { "ship", "asteroid", "missile" };
  MatScm *obj;

  /* TODO: Don't build a new symbol every time we are called! */
  /* TODO: Don't call strlen every time we are called! */

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  obj = mo_build(mat_scm_cons_car_get(args));

  ASSERT(!mat_scm_cons_cdr_get(args));

  return mat_scm_symbol_new(type[mat_movingobj_type_get(obj)], strlen(type[mat_movingobj_type_get(obj)]));
}
#endif

static MatScm *
mo_build ( MatScm *obj )
{
  MatScm *o;

  if (mat_scm_obj_type_get(obj) == &mat_scm_movingobj_type)
    return obj;

  ASSERT(mat_scm_obj_type_get(obj) == &mat_scm_ship_type);

  return mat_ship_movingobj_get(obj);
}

static MatScm *
mo_pos ( void *data, MatScm *env, MatScm *args )
{
  MatScm *obj;
  double pos_x, pos_y;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  obj = mo_build(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  pos_x = mat_scm_number_get(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  pos_y = mat_scm_number_get(mat_scm_cons_car_get(args));

  ASSERT(!mat_scm_cons_cdr_get(args));

  mat_movingobj_pos_set(obj, pos_x, pos_y);

  return NULL;
}

static MatScm *
mo_sp_linear ( void *data, MatScm *env, MatScm *args )
{
  MatScm *obj;
  MatAngleCache *ac;
  double sp;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  obj = mo_build(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  sp = mat_scm_number_get(mat_scm_cons_car_get(args)) / arg_universe_update_steps;

  ASSERT(!mat_scm_cons_cdr_get(args));

  ASSERT(obj);
  ASSERT(mat_movingobj_universe_get(obj));

  ac = mat_universe_angles_cache_get(mat_movingobj_universe_get(obj));

  ASSERT(ac);

  mat_movingobj_sp_linear_set(obj, sp * mat_anglecache_cos(ac, mat_movingobj_ang_get(obj)), sp * mat_anglecache_sin(ac, mat_movingobj_ang_get(obj)));

  return NULL;
}

static MatScm *
mo_sp_ang ( void *data, MatScm *env, MatScm *args )
{
  MatScm *obj;
  double sp;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  obj = mo_build(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  sp = mat_scm_number_get(mat_scm_cons_car_get(args))/ arg_universe_update_steps;

  ASSERT(!mat_scm_cons_cdr_get(args));

  mat_movingobj_sp_ang_set(obj, sp);

  return NULL;
}

static MatScm *
mo_event_set ( void *data, MatScm *env, MatScm *args )
{
  MatScm *obj;
  MatScm *proc;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  obj = mo_build(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  proc = mat_scm_cons_car_get(args);

  ASSERT(!mat_scm_cons_cdr_get(args));

  ((void (*) ( MatScm *, MatScm * ))data)(obj, proc);

  return NULL;
}

MatScm *
mo_closest ( void *funcdata, MatScm *env, MatScm *args )
{
  MatClosest data;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  data.src = mo_build(mat_scm_cons_car_get(args));
  data.dst = NULL;

  args = mat_scm_cons_cdr_get(args);

  if (args)
    {
      ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
      ASSERT(!mat_scm_cons_cdr_get(args));

      data.proc = mat_scm_cons_new(mat_scm_cons_car_get(args), mat_scm_cons_new(NULL, NULL));
      args = mat_scm_cons_cdr_get(args);
    }
  else
    data.proc = NULL;

  ASSERT(!args);

  mat_universe_movingobj_forall(mat_movingobj_universe_get(data.src), mo_closest_check, &data);

#if 0
  *dst = data.dst;
  *dx  = data.dx;
  *dy  = data.dy;
  *d   = data.d;
#endif

  return data.dst;
}

static void
mo_closest_check ( void *funcdata, MatScm *dst )
{
  MatClosest *data = (MatClosest *) funcdata;
  double dx, dy, d;

  ASSERT(data);
  ASSERT(data->src);

  if (dst == data->src)
    return;

  if (data->proc)
    {
      MatScm *result;
      mat_scm_cons_car_set(mat_scm_cons_cdr_get(data->proc), dst);
      if (!mat_scm_obj_eval(data->proc, NULL))
        return;
    }

  mat_distance_get(data->src, dst, &dx, &dy);

  d = dx * dx + dy * dy;

  if (!data->dst || d < data->d)
    {
      data->dst = dst;
      data->d   = d;
      data->dx  = dx;
      data->dy  = dy;
    }
}

static MatScm *
mo_ang_chase ( void *funcdata, MatScm *env, MatScm *args )
{
  MatScm *obj, *target;
  unsigned int ang;
  double dx, dy;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  obj = mo_build(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  target = mo_build(mat_scm_cons_car_get(args));

  ASSERT(!mat_scm_cons_cdr_get(args));

  ASSERT(obj);
  ASSERT(target);
  ASSERT(mat_movingobj_universe_get(obj));
  ASSERT(mat_movingobj_universe_get(target));
  ASSERT(mat_movingobj_universe_get(obj) == mat_movingobj_universe_get(target));

  mat_distance_get(obj, target, &dx, &dy);

  ang = mat_anglecache_best(mat_universe_angles_cache_get(mat_movingobj_universe_get(obj)), dx, dy, dx * dx + dy * dy);

  ASSERT(ang < mat_universe_angles_get(mat_movingobj_universe_get(obj)));

  return mat_scm_number_new((double) ang);
}

static MatScm *
mo_ang_set ( void *funcdata, MatScm *env, MatScm *args )
{
  MatScm *obj;
  double ang;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  obj = mo_build(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  ang = mat_scm_number_get(mat_scm_cons_car_get(args));

  ASSERT(!mat_scm_cons_cdr_get(args));

  ASSERT(obj);
  ASSERT(mat_movingobj_universe_get(obj));
  ASSERT(ang < mat_universe_angles_get(mat_movingobj_universe_get(obj)));

  mat_movingobj_ang_set(obj, ang);

  return NULL;
}

static MatScm *
mo_distance ( void *funcdata, MatScm *env, MatScm *args )
{
  MatScm *obj0, *obj1;
  double dx, dy;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  obj0 = mo_build(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  obj1 = mo_build(mat_scm_cons_car_get(args));

  ASSERT(!mat_scm_cons_cdr_get(args));

  ASSERT(obj0);
  ASSERT(mat_scm_obj_type_get(obj0) == &mat_scm_movingobj_type);
  ASSERT(obj1);
  ASSERT(mat_scm_obj_type_get(obj1) == &mat_scm_movingobj_type);
  ASSERT(mat_movingobj_universe_get(obj0));
  ASSERT(mat_movingobj_universe_get(obj1));
  ASSERT(mat_movingobj_universe_get(obj0) == mat_movingobj_universe_get(obj1));

  mat_distance_get(obj0, obj1, &dx, &dy);

  return mat_scm_number_new(dx * dx + dy * dy);
}

static MatScm *
mo_health_set ( void *funcdata, MatScm *env, MatScm *args )
{
  MatScm *obj;
  double health;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  obj = mo_build(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  health = mat_scm_number_get(mat_scm_cons_car_get(args));

  ASSERT(!mat_scm_cons_cdr_get(args));

  ASSERT(obj);
  ASSERT(mat_movingobj_universe_get(obj));

  mat_movingobj_health_set(obj, health);

  return NULL;
}

static MatScm *
mo_ttl_set ( void *funcdata, MatScm *env, MatScm *args )
{
  MatScm *obj;
  unsigned int ttl;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  obj = mo_build(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  ttl = mat_scm_number_get(mat_scm_cons_car_get(args));

  ASSERT(!mat_scm_cons_cdr_get(args));

  ASSERT(obj);
  ASSERT(mat_movingobj_universe_get(obj));

  mat_movingobj_ttl_set(obj, ttl);

  return NULL;
}
