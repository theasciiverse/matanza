#include <matanza.h>

static MatScmCallback     scm_eq;

MatScm                   *mat_scm_eq_eq    = NULL;
MatScm                   *mat_scm_eq_eqv   = NULL;
MatScm                   *mat_scm_eq_equal = NULL;

void
mat_scm_eq_init ( void )
{
  mat_scm_obj_protect(&mat_scm_eq_eq);
  mat_scm_obj_protect(&mat_scm_eq_eqv);
  mat_scm_obj_protect(&mat_scm_eq_equal);

  mat_scm_eq_eq    = mat_scm_proc_new(scm_eq, mat_scm_obj_eq,    1);
  mat_scm_eq_eqv   = mat_scm_proc_new(scm_eq, mat_scm_obj_eqv,   1);
  mat_scm_eq_equal = mat_scm_proc_new(scm_eq, mat_scm_obj_equal, 1);
}

static MatScm *
scm_eq ( void *data, MatScm *env, MatScm *args )
{
  MatScm *obj0;

  if (!args)
    return mat_scm_bool_true;

  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  obj0 = mat_scm_cons_car_get(args);

  while (mat_scm_cons_cdr_get(args))
    {
      args = mat_scm_cons_cdr_get(args);

      ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

      if (!((int (*)(MatScm *, MatScm *))data)(obj0, mat_scm_cons_car_get(args)))
        return NULL;
    }
  return mat_scm_bool_true;
}
