#include <matanza.h>

MatScm                   *mat_scm_quote = NULL;

static MatScmCallback     quote;

void
mat_scm_quote_init ( void )
{
  mat_scm_obj_protect(&mat_scm_quote);

  mat_scm_quote = mat_scm_proc_new(quote, NULL, 0);
}

static MatScm *
quote ( void *data, MatScm *env, MatScm *args )
{
  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(!mat_scm_cons_cdr_get(args));
  return mat_scm_cons_car_get(args);
}
