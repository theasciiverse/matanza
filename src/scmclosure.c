#include <matanza.h>

typedef struct MatScmClosure MatScmClosure;

struct MatScmClosure
{
  MatScm *code;
  MatScm *args;
  MatScm *env;
};

static void               cl_mark                          ( void               *funcdata );

MatScmType mat_scm_closure_type =
{
  "closure",
  NULL,
  cl_mark,
  NULL,
  NULL,
  NULL,
};

static MatScmCallback     cl_lambda;

MatScm                   *mat_scm_closure_lambda = NULL;

INLINE void
mat_scm_closure_init ( void )
{
  mat_scm_closure_lambda = mat_scm_proc_new(cl_lambda, NULL, 0);
}

static MatScm *
cl_lambda ( void *funcdata, MatScm *env, MatScm *args )
{
  MatScmClosure *data;

  /* TODO: Only use mat_scm_special_begin when required. */

  data = malloc(sizeof(MatScmClosure));

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  data->args = mat_scm_cons_car_get(args);
  data->code = mat_scm_cons_new(mat_scm_begin, mat_scm_cons_cdr_get(args));
  data->env = env;

  return mat_scm_obj_malloc(&mat_scm_closure_type, data);
}

static void
cl_mark ( void *funcdata )
{
  MatScmClosure *data = (MatScmClosure *) funcdata;

  ASSERT(data);

  mat_scm_obj_mark(data->code);
  mat_scm_obj_mark(data->args);
  mat_scm_obj_mark(data->env);
}

MatScm *
mat_scm_closure_call ( void *funcdata, MatScm *env, MatScm *args )
{
  MatScmClosure *data = (MatScmClosure *) funcdata;

  ASSERT(data);
  ASSERT(mat_scm_list_length_get(args) == mat_scm_list_length_get(data->args));

  return mat_scm_obj_eval(data->code, mat_scm_env_extend(data->env, data->args, args));
}
