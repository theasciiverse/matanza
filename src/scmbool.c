#include <matanza.h>

static MatScmCallback     bool;
static MatScmCallback     bool_not;

static MatScm            *sb_and = NULL;
static MatScm            *sb_or = NULL;
static MatScm            *sb_not = NULL;

MatScmType mat_scm_bool_type =
{
  "bool",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};

void
mat_scm_bool_init ( void )
{
  mat_scm_obj_protect(&sb_and);
  mat_scm_obj_protect(&sb_or);
  mat_scm_obj_protect(&sb_not);
  mat_scm_obj_protect(&mat_scm_bool_true);

  sb_and = mat_scm_proc_new(bool, &mat_scm_bool_init, 0);
  sb_or  = mat_scm_proc_new(bool, NULL, 0);
  sb_not = mat_scm_proc_new(bool_not, NULL, 1);

  mat_scm_bool_true  = mat_scm_obj_malloc(&mat_scm_bool_type, NULL);
}

void
mat_scm_bool_define ( MatScm *env )
{
  mat_scm_env_define(env, "and", sb_and);
  mat_scm_env_define(env, "or", sb_or);
  mat_scm_env_define(env, "not", sb_not);
}

static MatScm *
bool_not ( void *data, MatScm *env, MatScm *args )
{
  MatScm *obj;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(!mat_scm_cons_cdr_get(args));

  obj = mat_scm_cons_car_get(args);

  return obj ? NULL : mat_scm_bool_true;
}

static MatScm *
bool ( void *and, MatScm *env, MatScm *args )
{
  while (args)
    {
      ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

      if (mat_scm_obj_eval(mat_scm_cons_car_get(args), env) == (and ? NULL : mat_scm_bool_true))
        return and ? NULL : mat_scm_bool_true;

      args = mat_scm_cons_cdr_get(args);
    }

  return and ? mat_scm_bool_true : NULL;
}
