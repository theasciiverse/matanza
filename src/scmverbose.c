#include <matanza.h>

static MatScmCallback     verbose;

MatScm                   *mat_scm_verbose = NULL;

void
mat_scm_verbose_init ( void )
{
  mat_scm_obj_protect(&mat_scm_verbose);

  mat_scm_verbose = mat_scm_proc_new(verbose, NULL, 1);
}

static MatScm *
verbose ( void *data, MatScm *env, MatScm *args )
{
  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_string_type);
  ASSERT(!mat_scm_cons_cdr_get(args));

  MAT_VERBOSE(MAT_VERBOSE_SCM_USER)("%s\n", mat_scm_string_data_get(mat_scm_cons_car_get(args)));

  return NULL;
}
