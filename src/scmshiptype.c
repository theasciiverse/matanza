#include <matanza.h>

static void               mat_scm_shiptype_free            ( void               *funcdata );

int                       mat_scm_shiptype_equal           ( MatScm             *img0,
                                                             MatScm             *img1 );

static MatScmCallback     mat_scm_shiptype_new_calc;

MatScmType mat_scm_shiptype_type =
{
  "shiptype",
  mat_scm_shiptype_free,
  NULL,
  NULL,
  NULL,
  mat_scm_shiptype_equal
};

MatScm                   *mat_scm_shiptype_new = NULL;

int
mat_scm_shiptype_equal ( MatScm *img0, MatScm *img1 )
{
  /* TODO: Write */
  return 0;
}

static void
mat_scm_shiptype_free ( void *funcdata )
{
  /* TODO: Enable */
  /* mat_shiptype_free(funcdata); */
}

void
mat_scm_shiptype_init ( void )
{
  mat_scm_obj_protect(&mat_scm_shiptype_new);

  mat_scm_shiptype_new = mat_scm_proc_new(mat_scm_shiptype_new_calc, NULL, 1);
}

static MatScm *
mat_scm_shiptype_new_calc ( void *data, MatScm *env, MatScm *args )
{
  MatScm *tmp, *name, *desc;
  unsigned int img;
  int sp, health, sp_ang;
  int m[MAT_MISSILE_SIZE_COUNT], fr[MAT_MISSILE_SIZE_COUNT];
  unsigned int i;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_string_type);

  name = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_string_type);

  desc = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  img = mat_scm_number_get(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  sp = mat_scm_number_get(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  health = mat_scm_number_get(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  sp_ang = mat_scm_number_get(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  for (i = 0, tmp = mat_scm_cons_car_get(args); i < MAT_MISSILE_SIZE_COUNT; i++, tmp = mat_scm_cons_cdr_get(tmp))
    {
      ASSERT(tmp);
      ASSERT(mat_scm_obj_type_get(tmp) == &mat_scm_cons_type);
      m[i] = mat_scm_number_get(mat_scm_cons_car_get(tmp));
    }

  ASSERT(!tmp);

  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  for (i = 0, tmp = mat_scm_cons_car_get(args); i < MAT_MISSILE_SIZE_COUNT; i++, tmp = mat_scm_cons_cdr_get(tmp))
    {
      ASSERT(tmp);
      ASSERT(mat_scm_obj_type_get(tmp) == &mat_scm_cons_type);
      fr[i] = mat_scm_number_get(mat_scm_cons_car_get(tmp));
    }

  ASSERT(!tmp);

  return mat_shiptype_new(name, desc, img, sp, health, sp_ang, m, fr);
}
