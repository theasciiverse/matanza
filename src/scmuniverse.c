#include <matanza.h>

static MatScmCallback     u_type_get;
static MatScmCallback     u_msg_add;
//static MatScmCallback     u_event_set;

MatScm                   *su_type_get = NULL;
MatScm                   *su_message_add = NULL;
//MatScm                   *su_event_set = NULL;

MatScmType mat_scm_universe_type =
{
  "universe",
  free,
  mat_universe_mark,
  NULL,
  NULL,
  NULL,
};

void
mat_scm_universe_init ( void )
{
  mat_scm_obj_protect(&su_type_get);
  mat_scm_obj_protect(&su_message_add);
  //mat_scm_obj_protect(&su_event_set);

  su_type_get = mat_scm_proc_new(u_type_get, NULL, 1);
  su_message_add = mat_scm_proc_new(u_msg_add, NULL, 1);
  //su_message_print = mat_scm_proc_new(u_event_set, mat_universe_event_shot_set, 1);
}

void
mat_scm_universe_define ( MatScm *env )
{
  mat_scm_env_define(env, "matanza-universe-type-get", su_type_get);
  mat_scm_env_define(env, "matanza-universe-message-add", su_message_add);
}

static MatScm *
u_type_get ( void *data, MatScm *env, MatScm *args )
{
  MatScm *un;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  un = mat_scm_cons_car_get(args);

  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universe_type);
  ASSERT(mat_universe_universetype_get(un));

  ASSERT(!mat_scm_cons_cdr_get(args));

  return mat_universe_universetype_get(un);
}

static MatScm *
u_msg_add ( void *data, MatScm *env, MatScm *args )
{
  MatScm *un;
  char *msg;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  un = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universe_type);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_string_type);

  msg = mat_scm_string_data_get(mat_scm_cons_car_get(args));

  ASSERT(!mat_scm_cons_cdr_get(args));

  ASSERT(un);
  ASSERT(msg);

  mat_universe_message_printf(un, "%s", msg);

  return NULL;
}

#if 0
static MatScm *
u_event_set ( void *data, MatScm *env, MatScm *args )
{
  MatScm *un;
  MatScm *proc;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  un = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universe_type);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));

  proc = mat_scm_cons_car_get(args);

  ASSERT(!mat_scm_cons_cdr_get(args));

  ((void (*) ( MatScm *, MatScm * ))data)(obj, proc);

  return NULL;
}
#endif
