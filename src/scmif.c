#include <matanza.h>

MatScm                   *mat_scm_if = NULL;

static MatScmCallback     mat_scm_if_op;

void
mat_scm_if_init ( void )
{
  mat_scm_obj_protect(&mat_scm_if);

  mat_scm_if = mat_scm_proc_new(mat_scm_if_op, NULL, 0);
}

static MatScm *
mat_scm_if_op ( void *data, MatScm *env, MatScm *args )
{
  MatScm *rest = mat_scm_cons_cdr_get(args);
  MatScm *expr;

  expr = mat_scm_obj_eval(mat_scm_cons_car_get(args), env);
  if (expr)
    return mat_scm_obj_eval(mat_scm_cons_car_get(rest), env);

  rest = mat_scm_cons_cdr_get(rest);

  if (!rest)
    return NULL;

  return mat_scm_obj_eval(mat_scm_cons_car_get(rest), env);
}
