#include <matanza.h>

MatScm                   *mat_scm_begin = NULL;

static MatScmCallback     mat_scm_begin_op;

void
mat_scm_begin_init ( void )
{
  mat_scm_obj_protect(&mat_scm_begin);

  mat_scm_begin = mat_scm_proc_new(mat_scm_begin_op, NULL, 0);
}

static MatScm *
mat_scm_begin_op ( void *data, MatScm *env, MatScm *args )
{
  MatScm *obj = NULL;

  for (obj = NULL; args; args = mat_scm_cons_cdr_get(args))
    obj = mat_scm_obj_eval(mat_scm_cons_car_get(args), env);

  return obj;
}
