#include <matanza.h>

static MatScmCallback     cond;

MatScm                   *mat_scm_cond = NULL;

INLINE void
mat_scm_cond_init ( void )
{
  mat_scm_obj_protect(&mat_scm_cond);

  mat_scm_cond = mat_scm_proc_new(cond, NULL, 0);
}

static MatScm *
cond ( void *data, MatScm *env, MatScm *args )
{
  while (args)
    {
      MatScm *obj;

      ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

      obj = mat_scm_cons_car_get(args);

      ASSERT(mat_scm_obj_type_get(obj) == &mat_scm_cons_type);

      if (mat_scm_obj_eval(mat_scm_cons_car_get(obj), env))
        return mat_scm_obj_call(mat_scm_begin, mat_scm_cons_cdr_get(obj), env, 1);

      args = mat_scm_cons_cdr_get(args);
    }

  return NULL;
}
