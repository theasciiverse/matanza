#include <matanza.h>

static MatScmCallback     scmcase;

MatScm                   *mat_scm_case = NULL;

INLINE void
mat_scm_case_init ( void )
{
  mat_scm_obj_protect(&mat_scm_cond);

  mat_scm_case = mat_scm_proc_new(scmcase, NULL, 0);
}

static MatScm *
scmcase ( void *data, MatScm *env, MatScm *args )
{
  MatScm *key;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  key = mat_scm_obj_eval(mat_scm_cons_car_get(args), env);
  args = mat_scm_cons_cdr_get(args);

  while (args)
    {
      MatScm *obj, *tmp;

      ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

      obj = mat_scm_cons_car_get(args);

      if (mat_scm_obj_type_get(obj) != &mat_scm_cons_type)
        return mat_scm_obj_call(mat_scm_begin, mat_scm_cons_cdr_get(obj), env, 1);

      for (tmp = mat_scm_cons_car_get(obj); tmp; tmp = mat_scm_cons_cdr_get(tmp))
        {
          ASSERT(mat_scm_obj_type_get(obj) == &mat_scm_cons_type);
          if (mat_scm_obj_eqv(key, mat_scm_cons_car_get(tmp)))
            return mat_scm_obj_call(mat_scm_begin, mat_scm_cons_cdr_get(obj), env, 1);
        }

      args = mat_scm_cons_cdr_get(args);
    }

  return NULL;
}
