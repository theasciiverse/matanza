#include <matanza.h>

MatScm                   *mat_scm_define = NULL;

static MatScmCallback     mat_scm_define_op;

void
mat_scm_define_init ( void )
{
  mat_scm_obj_protect(&mat_scm_define);

  mat_scm_define = mat_scm_proc_new(mat_scm_define_op, NULL, 0);
}

static MatScm *
mat_scm_define_op ( void *data, MatScm *env, MatScm *args )
{
  MatScm *name, *value;

  name = mat_scm_cons_car_get(args);
  value = mat_scm_cons_new(mat_scm_begin, mat_scm_cons_cdr_get(args));

  while (mat_scm_obj_type_get(name) == &mat_scm_cons_type)
    {
      value = mat_scm_cons_new(mat_scm_closure_lambda, mat_scm_cons_new(mat_scm_cons_cdr_get(name), value));
      name = mat_scm_cons_car_get(name);
    }

  mat_scm_env_define(env, mat_scm_symbol_data_get(name), mat_scm_obj_eval(value, env));

  return NULL;
}
