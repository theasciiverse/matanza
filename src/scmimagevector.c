#include <matanza.h>

static MatScmCallback     mat_scm_imagevector_new_calc;

void
mat_scm_imagevector_init ( void )
{
  mat_scm_obj_protect(&mat_scm_imagevector_new);

  mat_scm_imagevector_new = mat_scm_proc_new(mat_scm_imagevector_new_calc, NULL, 1);
}

static MatScm *
mat_scm_imagevector_new_calc ( void *data, MatScm *env, MatScm *args )
{
  MatScm *img;
  MatScm *v;
  MatScm *un;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  img = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(img);
  ASSERT(mat_scm_obj_type_get(img) == &mat_scm_image_type);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  un = mat_scm_cons_car_get(args);

  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universetype_type);

  ASSERT(!mat_scm_cons_cdr_get(args));

  ASSERT(img);

  v = mat_imagevector_new(img, mat_universetype_ang_get(un), mat_universetype_ang_cache_get(un));

  ASSERT(v);

  return mat_scm_obj_malloc(&mat_scm_imagevector_type, v);
}
