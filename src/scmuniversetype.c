#include <matanza.h>

static void               mat_scm_universetype_free        ( void               *funcdata );

int                       mat_scm_universetype_equal       ( MatScm             *img0,
                                                             MatScm             *img1 );

static MatScmCallback     un_new;
static MatScmCallback     un_img_add;
static MatScmCallback     un_ship_add;
//static MatScmCallback     un_img_ast_set;
static MatScmCallback     un_img_mis_set;
static MatScmCallback     un_evt_set;

MatScmType mat_scm_universetype_type =
{
  "universetype",
  mat_scm_universetype_free,
  NULL,
  NULL,
  NULL,
  mat_scm_universetype_equal
};

MatScm                   *mat_scm_universetype_new = NULL;
MatScm                   *mat_scm_universetype_image_add = NULL;
MatScm                   *mat_scm_universetype_ship_add = NULL;
//MatScm                   *mat_scm_universetype_image_asteroid_set = NULL;
MatScm                   *mat_scm_universetype_image_missile_set = NULL;
MatScm                   *mat_scm_universetype_event_join_set = NULL;
MatScm                   *mat_scm_universetype_event_start_set = NULL;
MatScm                   *mat_scm_universetype_event_talk_set = NULL;

int
mat_scm_universetype_equal ( MatScm *ut0, MatScm *ut1 )
{
  /* TODO: Write */
  return 0;
}

static void
mat_scm_universetype_free ( void *funcdata )
{
  mat_universetype_free(funcdata);
}

void
mat_scm_universetype_init ( void )
{
  mat_scm_obj_protect(&mat_scm_universetype_new);
  mat_scm_obj_protect(&mat_scm_universetype_image_add);
  mat_scm_obj_protect(&mat_scm_universetype_ship_add);
  //mat_scm_obj_protect(&mat_scm_universetype_image_asteroid_set);
  mat_scm_obj_protect(&mat_scm_universetype_image_missile_set);
  mat_scm_obj_protect(&mat_scm_universetype_event_join_set);
  mat_scm_obj_protect(&mat_scm_universetype_event_start_set);
  mat_scm_obj_protect(&mat_scm_universetype_event_talk_set);

  mat_scm_universetype_new = mat_scm_proc_new(un_new, NULL, 1);
  mat_scm_universetype_image_add = mat_scm_proc_new(un_img_add, NULL, 1);
  mat_scm_universetype_ship_add = mat_scm_proc_new(un_ship_add, NULL, 1);
  //mat_scm_universetype_image_asteroid_set = mat_scm_proc_new(un_img_ast_set, NULL, 1);
  mat_scm_universetype_image_missile_set = mat_scm_proc_new(un_img_mis_set, NULL, 1);
  mat_scm_universetype_event_join_set = mat_scm_proc_new(un_evt_set, mat_universetype_event_join_set, 1);
  mat_scm_universetype_event_start_set = mat_scm_proc_new(un_evt_set, mat_universetype_event_start_set, 1);
  mat_scm_universetype_event_talk_set = mat_scm_proc_new(un_evt_set, mat_universetype_event_talk_set, 1);
}

static MatScm *
un_new ( void *data, MatScm *env, MatScm *args )
{
  MatScm *name;
  unsigned int ang;
  unsigned int mx, my;
  MatScm *banner;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  name = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  ang = mat_scm_number_get(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  mx = mat_scm_number_get(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  my = mat_scm_number_get(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  banner = mat_scm_cons_car_get(args);

  ASSERT(!mat_scm_cons_cdr_get(args));

  ASSERT(name);
  ASSERT(mat_scm_obj_type_get(name) == &mat_scm_string_type);
  ASSERT(banner);
  ASSERT(mat_scm_obj_type_get(banner) == &mat_scm_cons_type);

  return mat_universetype_new(name, ang, mx, my, banner);
}

static MatScm *
un_img_add ( void *data, MatScm *env, MatScm *args )
{
  MatScm *un;
  MatScm *img;
  MatScm *vect;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  un = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universetype_type);
  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(!mat_scm_cons_cdr_get(args));

  img = mat_scm_cons_car_get(args);

  ASSERT(img);
  ASSERT(mat_scm_obj_type_get(img) == &mat_scm_image_type);

  vect = mat_imagevector_new(img, mat_universetype_ang_get(un), mat_universetype_ang_cache_get(un));

  return mat_scm_number_new(mat_universetype_image_add(un, vect));
}

static MatScm *
un_ship_add ( void *data, MatScm *env, MatScm *args )
{
  MatScm *un;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  un = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universetype_type);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_shiptype_type);

  ASSERT(!mat_scm_cons_cdr_get(args));

  mat_universetype_shiptype_add(un, mat_scm_cons_car_get(args));

  return NULL;
}

#if 0
static MatScm *
un_img_ast_set ( void *data, MatScm *env, MatScm *args )
{
  MatScm *un;
  unsigned int array[MAT_ASTEROID_SIZE_MAX + 1];
  unsigned int i;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  un = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universetype_type);

  for (i = 0; i < sizeof(array) / sizeof(array[0]); i ++)
    {
      ASSERT(args);
      ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
      ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);
      array[i] = mat_scm_number_get(mat_scm_cons_car_get(args));
      args = mat_scm_cons_cdr_get(args);
    }

  ASSERT(!args);

  mat_universetype_image_asteroid_set(un, array);

  return NULL;
}
#endif

static MatScm *
un_img_mis_set ( void *data, MatScm *env, MatScm *args )
{
  MatScm *un;
  unsigned int array[MAT_MISSILE_SIZE_COUNT];
  unsigned int i;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  un = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universetype_type);

  for (i = 0; i < sizeof(array) / sizeof(array[0]); i ++)
    {
      ASSERT(args);
      ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
      ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);
      array[i] = mat_scm_number_get(mat_scm_cons_car_get(args));
      args = mat_scm_cons_cdr_get(args);
    }

  ASSERT(!args);

  mat_universetype_image_missile_set(un, array);

  return NULL;
}

static MatScm *
un_evt_set ( void *data, MatScm *env, MatScm *args )
{
  MatScm *un;
  MatScm *proc;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  un = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universetype_type);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  proc = mat_scm_cons_car_get(args);

  ASSERT(proc);
  ASSERT(!mat_scm_cons_cdr_get(args));

  ((void (*) (MatScm *, MatScm *)) data)(un, proc);

  return NULL;
}
