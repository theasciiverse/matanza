#include <matanza.h>

typedef struct MatScmCons MatScmCons;

struct MatScmCons
{
  MatScm *car;
  MatScm *cdr;
};

static void               mat_scm_cons_mark                ( void               *funcdata );

static int                mat_scm_cons_equal               ( MatScm             *obj0,
                                                             MatScm             *obj1 );

static MatScmCallback     cons_get;
static MatScmCallback     cons_set;
static MatScmCallback     cons_cons;
static MatScmCallback     cons_null;

MatScmType mat_scm_cons_type =
{
  "cons",
  NULL,
  mat_scm_cons_mark,
  NULL,
  NULL,
  mat_scm_cons_equal
};

MatScm                   *mat_scm_cons_car = NULL;
MatScm                   *mat_scm_cons_cdr = NULL;
MatScm                   *mat_scm_cons_cons = NULL;
MatScm                   *mat_scm_cons_null = NULL;

void
mat_scm_cons_init ( void )
{
  mat_scm_obj_protect(&mat_scm_cons_car);
  mat_scm_obj_protect(&mat_scm_cons_cdr);
  mat_scm_obj_protect(&mat_scm_cons_cons);
  mat_scm_obj_protect(&mat_scm_cons_null);

  mat_scm_cons_car = mat_scm_proc_new(cons_get, mat_scm_cons_car_get, 1);
  mat_scm_cons_cdr = mat_scm_proc_new(cons_get, mat_scm_cons_cdr_get, 1);
  mat_scm_cons_setcar = mat_scm_proc_new(cons_set, mat_scm_cons_car_set, 1);
  mat_scm_cons_setcdr = mat_scm_proc_new(cons_set, mat_scm_cons_cdr_set, 1);
  mat_scm_cons_cons = mat_scm_proc_new(cons_cons, NULL, 1);
  mat_scm_cons_null = mat_scm_proc_new(cons_null, NULL, 1);
}

MatScm *
mat_scm_cons_new ( MatScm *car, MatScm *cdr )
{
  MatScmCons *data;

  data = malloc(sizeof(MatScmCons));

  data->car = car;
  data->cdr = cdr;

  return mat_scm_obj_malloc(&mat_scm_cons_type, data);
}

static void
mat_scm_cons_mark ( void *funcdata )
{
  MatScmCons *data = (MatScmCons *) funcdata;

  mat_scm_obj_mark(data->car);
  mat_scm_obj_mark(data->cdr);
}

MatScm *
mat_scm_cons_car_get ( MatScm *cons )
{
  MatScmCons *data;

  ASSERT(cons);
  ASSERT(mat_scm_obj_type_get(cons) == &mat_scm_cons_type);

  data = (MatScmCons *) mat_scm_obj_data_get(cons);

  ASSERT(data);

  return data->car;
}

MatScm *
mat_scm_cons_cdr_get ( MatScm *cons )
{
  MatScmCons *data;

  ASSERT(cons);
  ASSERT(mat_scm_obj_type_get(cons) == &mat_scm_cons_type);

  data = (MatScmCons *) mat_scm_obj_data_get(cons);

  ASSERT(data);

  return data->cdr;
}

static int
mat_scm_cons_equal ( MatScm *obj0, MatScm *obj1 )
{
  ASSERT(mat_scm_obj_type_get(obj0) == &mat_scm_cons_type);
  ASSERT(mat_scm_obj_type_get(obj1) == &mat_scm_cons_type);

  return mat_scm_obj_equal(mat_scm_cons_car_get(obj0), mat_scm_cons_car_get(obj1))
    && mat_scm_obj_equal(mat_scm_cons_cdr_get(obj0), mat_scm_cons_cdr_get(obj1));
}

static MatScm *
cons_get ( void *data, MatScm *env, MatScm *args )
{
  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_cons_type);
  ASSERT(!mat_scm_cons_cdr_get(args));

  return ((MatScm *(*) (MatScm *)) data)(mat_scm_cons_car_get(args));
}

static MatScm *
cons_set ( void *data, MatScm *env, MatScm *args )
{
  MatScm *cons;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  cons = mat_scm_cons_car_get(args);
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  ASSERT(!mat_scm_cons_cdr_get(args));

  ASSERT(mat_scm_obj_type_get(cons) == &mat_scm_cons_type);

  return ((MatScm *(*) (MatScm *, MatScm *)) data)(cons, mat_scm_cons_car_get(args));
}

static MatScm *
cons_cons ( void *data, MatScm *env, MatScm *args )
{
  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_cdr_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_cdr_get(args)) == &mat_scm_cons_type);
  ASSERT(!mat_scm_cons_cdr_get(mat_scm_cons_cdr_get(args)));

  return mat_scm_cons_new(mat_scm_cons_car_get(args), mat_scm_cons_car_get(mat_scm_cons_cdr_get(args)));
}

static MatScm *
cons_null ( void *data, MatScm *env, MatScm *args )
{
  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(!mat_scm_cons_cdr_get(args));

  return mat_scm_cons_car_get(args) ? NULL : mat_scm_bool_true;
}

void
mat_scm_cons_car_set ( MatScm *cons, MatScm *value )
{
  MatScmCons *data;

  ASSERT(cons);
  ASSERT(mat_scm_obj_type_get(cons) == &mat_scm_cons_type);

  data = (MatScmCons *) mat_scm_obj_data_get(cons);

  ASSERT(data);

  data->car = value;
}

void
mat_scm_cons_cdr_set ( MatScm *cons, MatScm *value )
{
  MatScmCons *data;

  ASSERT(cons);
  ASSERT(mat_scm_obj_type_get(cons) == &mat_scm_cons_type);

  data = (MatScmCons *) mat_scm_obj_data_get(cons);

  ASSERT(data);

  data->cdr = value;
}
