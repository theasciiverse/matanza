#include <matanza.h>

static MatScmCallback     l_eval;
static MatScmCallback     l_print;
static MatScmCallback     l_load;

MatScm                   *mat_scm_load_load = NULL;

void
mat_scm_load ( char *path, MatScm *env )
{
  MatScm *read, *eval, *print, *file;

  file = mat_scm_file_open(path, "r");
  if (!file)
    {
      fprintf(stderr, "%s: %s: %s\n", program_name, path, strerror(errno));
      exit(EXIT_FAILURE);
    }

  read  = mat_scm_read(file);
  eval  = mat_scm_proc_new(l_eval, NULL, 0);
  print = mat_scm_proc_new(l_print, NULL, 0);

  mat_scm_repl(read, eval, print, env);
}

void
mat_scm_load_init ( void )
{
  mat_scm_obj_protect(&mat_scm_load_load);

  mat_scm_load_load = mat_scm_proc_new(l_load, NULL, 1);
}

static MatScm *
l_eval ( void *data, MatScm *env, MatScm *args )
{
  return mat_scm_obj_eval(mat_scm_cons_car_get(args), env);
}

static MatScm *
l_print ( void *data, MatScm *env, MatScm *args )
{
  return NULL;
}

static MatScm *
l_load ( void *data, MatScm *env, MatScm *args )
{
  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_string_type);
  ASSERT(!mat_scm_cons_cdr_get(args));

  mat_scm_load(mat_scm_string_data_get(mat_scm_cons_car_get(args)), env);

  return NULL;
}
