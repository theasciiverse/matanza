#include <matanza.h>

MatScm                   *mat_scm_let = NULL;

static MatScmCallback     mat_scm_let_op;

void
mat_scm_let_init ( void )
{
  mat_scm_obj_protect(&mat_scm_let);

  mat_scm_let = mat_scm_proc_new(mat_scm_let_op, NULL, 0);
}

static MatScm *
mat_scm_let_op ( void *data, MatScm *env, MatScm *args )
{
  MatScm *names = NULL, *values = NULL;
  MatScm *tmp;

  for (tmp = mat_scm_cons_car_get(args); tmp; tmp = mat_scm_cons_cdr_get(tmp))
    {
      ASSERT(mat_scm_obj_type_get(tmp) == &mat_scm_cons_type);
      names = mat_scm_cons_new(mat_scm_cons_car_get(mat_scm_cons_car_get(tmp)), names);
      values = mat_scm_cons_new(mat_scm_obj_eval(mat_scm_cons_car_get(mat_scm_cons_cdr_get(mat_scm_cons_car_get(tmp))), env), values);
    }

  return mat_scm_obj_eval(mat_scm_cons_new(mat_scm_begin, mat_scm_cons_cdr_get(args)), mat_scm_env_extend(env, names, values));
}
