#include <matanza.h>

typedef struct MatScmProc MatScmProc;

struct MatScmProc
{
  MatScmCallback *func;
  void           *data;
  unsigned int    evalargs : 1;
};

MatScmType mat_scm_proc_type =
{
  "primitive-procedure",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

MatScm *
mat_scm_proc_new ( MatScmCallback *func, void *funcdata, unsigned int evalargs )
{
  MatScmProc *data;

  data = malloc(sizeof(MatScmProc));

  data->func     = func;
  data->data     = funcdata;
  data->evalargs = evalargs;

  return mat_scm_obj_malloc(&mat_scm_proc_type, data);
}

MatScm *
mat_scm_proc_call ( void *funcdata, MatScm *env, MatScm *args )
{
  MatScmProc *data = (MatScmProc *) funcdata;

  ASSERT(data);
  ASSERT(data->func);

  return data->func(data->data, env, args);
}

unsigned int
mat_scm_proc_evalargs_get ( MatScm *proc )
{
  MatScmProc *data;

  ASSERT(proc);
  ASSERT(mat_scm_obj_type_get(proc) == &mat_scm_proc_type);

  data = mat_scm_obj_data_get(proc);

  return data->evalargs;
}
