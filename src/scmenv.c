#include <matanza.h>

static void env_set ( MatScm *env, char *name, MatScm *val );

MatScm                   *mat_scm_env_current = NULL;
MatScm                   *mat_scm_env_new = NULL;
MatScm                   *mat_scm_env_set = NULL;
MatScm                   *mat_scm_env_envdef = NULL;
MatScm                   *mat_scm_env_envset = NULL;
MatScm                   *mat_scm_env_envget = NULL;

static MatScmCallback     env_set_scm;
static MatScmCallback     mat_scm_env_op_current;
static MatScmCallback     mat_scm_env_op_new;
static MatScmCallback     envdef;
static MatScmCallback     envget;

MatScm *
mat_scm_env_find ( MatScm *env, char *name )
{
  ASSERT(name);

  MAT_VERBOSE(MAT_VERBOSE_SCM_INTERNAL)("Look up symbol: %s\n", name);

  while (env)
    {
      MatScm *names, *values;

      names  = mat_scm_cons_car_get(mat_scm_cons_car_get(env));
      values = mat_scm_cons_cdr_get(mat_scm_cons_car_get(env));

      while (names)
        {
          ASSERT(values);

          if (!strcmp(name, mat_scm_symbol_data_get(mat_scm_cons_car_get(names))))
            {
              MAT_VERBOSE(MAT_VERBOSE_SCM_INTERNAL)("Found value: %p (%s)\n",
                                                   mat_scm_cons_car_get(values),
                                                   mat_scm_cons_car_get(values) ? mat_scm_obj_type_get(mat_scm_cons_car_get(values))->name : "cons");
              return mat_scm_cons_car_get(values);
            }

          names  = mat_scm_cons_cdr_get(names);
          values = mat_scm_cons_cdr_get(values);
        }

      env = mat_scm_cons_cdr_get(env);
    }

  return NULL;
}

static void
env_set ( MatScm *env, char *name, MatScm *val )
{
  while (env)
    {
      MatScm *names, *values;

      names  = mat_scm_cons_car_get(mat_scm_cons_car_get(env));
      values = mat_scm_cons_cdr_get(mat_scm_cons_car_get(env));

      while (names)
        {
          ASSERT(values);

          if (!strcmp(name, mat_scm_symbol_data_get(mat_scm_cons_car_get(names))))
            return mat_scm_cons_car_set(values, val);

          names  = mat_scm_cons_cdr_get(names);
          values = mat_scm_cons_cdr_get(values);
        }

      env = mat_scm_cons_cdr_get(env);
    }
}

MatScm *
mat_scm_env_extend ( MatScm *env, MatScm *names, MatScm *values )
{
  if (!names)
    return env;

  return mat_scm_cons_new(mat_scm_cons_new(names, values), env);
}

void
mat_scm_env_define ( MatScm *env, char *name, MatScm *value )
{
  MatScm *frame;
  
  ASSERT(env);
  ASSERT(mat_scm_obj_type_get(env) == &mat_scm_cons_type);

  frame = mat_scm_cons_car_get(env);

  ASSERT(frame);
  ASSERT(mat_scm_obj_type_get(frame) == &mat_scm_cons_type);

  mat_scm_cons_car_set(frame, mat_scm_cons_new(mat_scm_symbol_new(name, strlen(name)), mat_scm_cons_car_get(frame)));
  mat_scm_cons_cdr_set(frame, mat_scm_cons_new(value, mat_scm_cons_cdr_get(frame)));
}

MatScm *
mat_scm_env_malloc ( void )
{
  return mat_scm_cons_new(mat_scm_cons_new(NULL, NULL), NULL);
}

void
mat_scm_env_add ( MatScm *env )
{
  mat_scm_bool_define(env);
  mat_scm_env_define(env, "if", mat_scm_if);
  mat_scm_env_define(env, "cond", mat_scm_cond);
  mat_scm_env_define(env, "case", mat_scm_case);
  mat_scm_env_define(env, "define", mat_scm_define);
  mat_scm_env_define(env, "let", mat_scm_let);
  mat_scm_env_define(env, "lambda", mat_scm_closure_lambda);
  mat_scm_env_define(env, "begin", mat_scm_begin);
  mat_scm_env_define(env, "+", mat_scm_number_add);
  mat_scm_env_define(env, "*", mat_scm_number_mul);
  mat_scm_env_define(env, "-", mat_scm_number_sub);
  mat_scm_env_define(env, "/", mat_scm_number_div);
  mat_scm_env_define(env, "random", mat_scm_number_random);
  mat_scm_env_define(env, "<", mat_scm_number_cmp_lt);
  mat_scm_env_define(env, "<=", mat_scm_number_cmp_lte);
  mat_scm_env_define(env, ">", mat_scm_number_cmp_gt);
  mat_scm_env_define(env, ">=", mat_scm_number_cmp_gte);
  mat_scm_env_define(env, "zero?", mat_scm_number_zero);
  mat_scm_env_define(env, "environment-current", mat_scm_env_current);
  mat_scm_env_define(env, "environment-new", mat_scm_env_new);
  mat_scm_env_define(env, "environment-define", mat_scm_env_envdef);
  mat_scm_env_define(env, "environment-set!", mat_scm_env_envset);
  mat_scm_env_define(env, "environment-get", mat_scm_env_envget);
  mat_scm_env_define(env, "set!", mat_scm_env_set);
  mat_scm_env_define(env, "string->list", mat_scm_string_list);
  mat_scm_env_define(env, "string-length", mat_scm_string_length);
  mat_scm_env_define(env, "string-append", mat_scm_string_append);
  mat_scm_env_define(env, "substring", mat_scm_string_substring);
  mat_scm_env_define(env, "symbol->string", mat_scm_symbol_string);
  mat_scm_env_define(env, "eq?", mat_scm_eq_eq);
  mat_scm_env_define(env, "eqv?", mat_scm_eq_eqv);
  mat_scm_env_define(env, "equal?", mat_scm_eq_equal);
  mat_scm_env_define(env, "list", mat_scm_list_list);
  mat_scm_env_define(env, "map", mat_scm_list_map);
  mat_scm_env_define(env, "apply", mat_scm_list_apply);
  mat_scm_env_define(env, "list->string", mat_scm_list_string);
  mat_scm_env_define(env, "car", mat_scm_cons_car);
  mat_scm_env_define(env, "cdr", mat_scm_cons_cdr);
  mat_scm_env_define(env, "set-car!", mat_scm_cons_setcar);
  mat_scm_env_define(env, "set-cdr!", mat_scm_cons_setcdr);
  mat_scm_env_define(env, "cons", mat_scm_cons_cons);
  mat_scm_env_define(env, "null?", mat_scm_cons_null);
  mat_scm_env_define(env, "get-type", mat_scm_obj_type);
  mat_scm_env_define(env, "load", mat_scm_load_load);
  mat_scm_env_define(env, "matanza-universe-type", mat_scm_universetype_new);
  mat_scm_env_define(env, "matanza-universe-type-banner-add", mat_scm_universetype_banner_add);
  mat_scm_env_define(env, "matanza-universe-type-image-add", mat_scm_universetype_image_add);
  mat_scm_env_define(env, "matanza-universe-type-ship-add", mat_scm_universetype_ship_add);
  //mat_scm_env_define(env, "matanza-universe-type-image-asteroid-set", mat_scm_universetype_image_asteroid_set);
  mat_scm_env_define(env, "matanza-universe-type-image-missile-set", mat_scm_universetype_image_missile_set);
  mat_scm_env_define(env, "matanza-universe-type-event-join-set", mat_scm_universetype_event_join_set);
  mat_scm_env_define(env, "matanza-universe-type-event-start-set", mat_scm_universetype_event_start_set);
  mat_scm_env_define(env, "matanza-universe-type-event-talk-set", mat_scm_universetype_event_talk_set);
  mat_scm_env_define(env, "matanza-ship-type", mat_scm_shiptype_new);
  mat_scm_ship_define(env);
  mat_scm_env_define(env, "matanza-image-load", mat_scm_image_load);
  mat_scm_env_define(env, "matanza-image-vector", mat_scm_imagevector_new);
  mat_scm_env_define(env, "matanza-verbose", mat_scm_verbose);
  mat_scm_movingobj_define(env);
  mat_scm_universe_define(env);
  mat_scm_env_define(env, "matanza-team-name-get", mat_scm_team_name_get);
  mat_scm_env_define(env, "matanza-team-ship-head-get", mat_scm_team_ship_head_get);
  mat_scm_env_define(env, "matanza-team-ship-tail-get", mat_scm_team_ship_tail_get);
}

void
mat_scm_env_init ( void )
{
  mat_scm_obj_protect(&mat_scm_env_current);
  mat_scm_obj_protect(&mat_scm_env_new);
  mat_scm_obj_protect(&mat_scm_env_set);
  mat_scm_obj_protect(&mat_scm_env_envdef);
  mat_scm_obj_protect(&mat_scm_env_envset);
  mat_scm_obj_protect(&mat_scm_env_envget);

  mat_scm_env_current = mat_scm_proc_new(mat_scm_env_op_current, NULL, 1);
  mat_scm_env_new     = mat_scm_proc_new(mat_scm_env_op_new, NULL, 1);
  mat_scm_env_set     = mat_scm_proc_new(env_set_scm, NULL, 0);
  mat_scm_env_envdef  = mat_scm_proc_new(envdef, NULL, 0);
  mat_scm_env_envset  = mat_scm_proc_new(envdef, NULL, 0);
  mat_scm_env_envget  = mat_scm_proc_new(envget, NULL, 0);
}

static MatScm *
mat_scm_env_op_current ( void *data, MatScm *env, MatScm *args )
{
  ASSERT(!args);
  return env;
}

static MatScm *
mat_scm_env_op_new ( void *data, MatScm *env, MatScm *args )
{
  ASSERT(!args);
  return mat_scm_env_malloc();
}

static MatScm *
env_set_scm ( void *data, MatScm *env, MatScm *args )
{
  MatScm *sym, *val;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  sym = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  val = mat_scm_obj_eval(mat_scm_cons_car_get(args), env);

  ASSERT(!mat_scm_cons_cdr_get(args));

  ASSERT(sym);
  ASSERT(mat_scm_obj_type_get(sym) == &mat_scm_symbol_type);

  env_set(env, mat_scm_symbol_data_get(sym), val);

  return NULL;
}

static MatScm *
envdef ( void *data, MatScm *parenv, MatScm *args )
{
  MatScm *env, *sym, *val;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  env = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  sym = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  val = mat_scm_obj_eval(mat_scm_cons_car_get(args), env);

  ASSERT(!mat_scm_cons_cdr_get(args));

  ASSERT(sym);
  ASSERT(mat_scm_obj_type_get(sym) == &mat_scm_symbol_type);

  ((void (*) (MatScm *, char *, MatScm *)) data)(env, mat_scm_symbol_data_get(sym), val);

  return NULL;
}

static MatScm *
envget ( void *data, MatScm *parenv, MatScm *args )
{
  MatScm *env, *sym;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  env = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  sym = mat_scm_cons_car_get(args);

  ASSERT(!mat_scm_cons_cdr_get(args));

  ASSERT(sym);
  ASSERT(mat_scm_obj_type_get(sym) == &mat_scm_symbol_type);

  return mat_scm_env_find(env, mat_scm_symbol_data_get(sym));
}
