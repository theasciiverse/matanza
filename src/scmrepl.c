#include <matanza.h>

void
mat_scm_repl ( MatScm *read, MatScm *eval, MatScm *print, MatScm *env )
{
  /* TODO: Keep it from creating new conses on each iteration */

  read = mat_scm_cons_new(read, NULL);

  for (;;)
    {
      MatScm *obj = mat_scm_obj_eval(read, env);
      if (obj == mat_scm_special_eof)
        return;
      obj = mat_scm_obj_eval(mat_scm_cons_new(eval, mat_scm_cons_new(obj, NULL)), env);
      mat_scm_obj_eval(mat_scm_cons_new(print, mat_scm_cons_new(obj, NULL)), env);
    }
}
