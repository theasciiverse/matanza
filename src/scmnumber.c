#include <matanza.h>

typedef struct MatScmNumber MatScmNumber;

typedef double (MatScmNumberOp) (double, double);
typedef int (MatScmNumberCmp) (double, double);

struct MatScmNumber
{
  double number;
};

static void               mat_scm_number_free              ( void               *data );

static int                mat_scm_number_eqv               ( MatScm             *obj0,
                                                             MatScm             *obj1 );

static MatScmCallback     num_calc;
static MatScmCallback     num_random;
static MatScmCallback     num_cmp;
static MatScmCallback     num_zero;

static MatScmNumberOp     mat_scm_number_op_add;
static MatScmNumberOp     mat_scm_number_op_mul;
static MatScmNumberOp     mat_scm_number_op_sub;
static MatScmNumberOp     mat_scm_number_op_div;

static MatScmNumberCmp    num_cmp_lt;
static MatScmNumberCmp    num_cmp_lte;
static MatScmNumberCmp    num_cmp_gt;
static MatScmNumberCmp    num_cmp_gte;

MatScmType mat_scm_number_type =
{
  "number",
  mat_scm_number_free,
  NULL,
  NULL,
  mat_scm_number_eqv,
  mat_scm_number_eqv
};

MatScm                   *mat_scm_number_add = NULL;
MatScm                   *mat_scm_number_mul = NULL;
MatScm                   *mat_scm_number_sub = NULL;
MatScm                   *mat_scm_number_div = NULL;
MatScm                   *mat_scm_number_random = NULL;
MatScm                   *mat_scm_number_cmp_lt = NULL;
MatScm                   *mat_scm_number_cmp_lte = NULL;
MatScm                   *mat_scm_number_cmp_gt = NULL;
MatScm                   *mat_scm_number_cmp_gte = NULL;
MatScm                   *mat_scm_number_zero = NULL;

INLINE MatScm *
mat_scm_number_new ( double n )
{
  MatScmNumber *data;

  data = malloc(sizeof(MatScmNumber));

  data->number = n;

  return mat_scm_obj_malloc(&mat_scm_number_type, data);
}

MatScm *
mat_scm_number_new_string ( char *buffer )
{
  return mat_scm_number_new(atof(buffer));
}

static void
mat_scm_number_free ( void *data )
{
  free(data);
}

INLINE double
mat_scm_number_get ( MatScm *n )
{
  ASSERT(mat_scm_obj_type_get(n) == &mat_scm_number_type);
  return ((MatScmNumber *) mat_scm_obj_data_get(n))->number;
}

static double
mat_scm_number_op_add ( double n1, double n2 )
{
  return n1 + n2;
}

static double
mat_scm_number_op_mul ( double n1, double n2 )
{
  return n1 * n2;
}

static double
mat_scm_number_op_sub ( double n1, double n2 )
{
  return n1 - n2;
}

static double
mat_scm_number_op_div ( double n1, double n2 )
{
  return n1 / n2;
}

void
mat_scm_number_init ( void )
{
  mat_scm_obj_protect(&mat_scm_number_add);
  mat_scm_obj_protect(&mat_scm_number_mul);
  mat_scm_obj_protect(&mat_scm_number_sub);
  mat_scm_obj_protect(&mat_scm_number_div);
  mat_scm_obj_protect(&mat_scm_number_random);
  mat_scm_obj_protect(&mat_scm_number_cmp_lt);
  mat_scm_obj_protect(&mat_scm_number_cmp_lte);
  mat_scm_obj_protect(&mat_scm_number_cmp_gt);
  mat_scm_obj_protect(&mat_scm_number_cmp_gte);
  mat_scm_obj_protect(&mat_scm_number_zero);

  mat_scm_number_add = mat_scm_proc_new(num_calc, mat_scm_number_op_add, 1);
  mat_scm_number_mul = mat_scm_proc_new(num_calc, mat_scm_number_op_mul, 1);
  mat_scm_number_sub = mat_scm_proc_new(num_calc, mat_scm_number_op_sub, 1);
  mat_scm_number_div = mat_scm_proc_new(num_calc, mat_scm_number_op_div, 1);
  mat_scm_number_random = mat_scm_proc_new(num_random, NULL, 1);
  mat_scm_number_cmp_lt = mat_scm_proc_new(num_cmp, num_cmp_lt, 1);
  mat_scm_number_cmp_lte = mat_scm_proc_new(num_cmp, num_cmp_lte, 1);
  mat_scm_number_cmp_gt = mat_scm_proc_new(num_cmp, num_cmp_gt, 1);
  mat_scm_number_cmp_gte = mat_scm_proc_new(num_cmp, num_cmp_gte, 1);
  mat_scm_number_zero = mat_scm_proc_new(num_zero, NULL, 1);
}

static MatScm *
num_calc ( void *funcdata, MatScm *env, MatScm *args )
{
  MatScmNumberOp *data = (MatScmNumberOp *) funcdata;
  double n1;

  n1 = mat_scm_number_get(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  while (args)
    {
      n1 = data(n1, mat_scm_number_get(mat_scm_cons_car_get(args)));
      args = mat_scm_cons_cdr_get(args);
    }

  return mat_scm_number_new(n1);
}

static int
mat_scm_number_eqv ( MatScm *obj0, MatScm *obj1 )
{
  ASSERT(mat_scm_obj_type_get(obj0) == &mat_scm_number_type);
  ASSERT(mat_scm_obj_type_get(obj1) == &mat_scm_number_type);

  return fabs(mat_scm_number_get(obj0) - mat_scm_number_get(obj1)) < 0.000001;
}

static MatScm *
num_random ( void *data, MatScm *env, MatScm *args )
{
  double top;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  top = mat_scm_number_get(mat_scm_cons_car_get(args));

  return mat_scm_number_new(top * (double) rand() / (double) RAND_MAX);
}

static MatScm *
num_cmp ( void *data, MatScm *env, MatScm *args )
{
  double n0;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  n0 = mat_scm_number_get(mat_scm_cons_car_get(args));

  for (args = mat_scm_cons_cdr_get(args); args; args = mat_scm_cons_cdr_get(args))
    {
      double n1;

      ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
      ASSERT(mat_scm_cons_car_get(args));
      ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

      n1 = mat_scm_number_get(mat_scm_cons_car_get(args));

      if (!((int (*) (double, double)) data)(n0, n1))
        return NULL;

      n0 = n1;
    }

  return mat_scm_bool_true;
}

static int
num_cmp_lt ( double n0, double n1 )
{
  return n0 < n1;
}

static int
num_cmp_lte ( double n0, double n1 )
{
  return n0 <= n1;
}

static int
num_cmp_gt ( double n0, double n1 )
{
  return n0 > n1;
}

static int
num_cmp_gte ( double n0, double n1 )
{
  return n0 >= n1;
}

static MatScm *
num_zero ( void *data, MatScm *env, MatScm *args )
{
  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  return fabs(mat_scm_number_get(mat_scm_cons_car_get(args))) < 0.000001 ? mat_scm_bool_true : NULL;
}
