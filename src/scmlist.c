#include <matanza.h>

static MatScmCallback     list_list;
static MatScmCallback     list_map;
static MatScmCallback     list_apply;
static MatScmCallback     list_string;

MatScm                   *mat_scm_list_list = NULL;
MatScm                   *mat_scm_list_map = NULL;
MatScm                   *mat_scm_list_apply = NULL;
MatScm                   *mat_scm_list_string = NULL;

void
mat_scm_list_init ( void )
{
  mat_scm_obj_protect(&mat_scm_list_list);
  mat_scm_obj_protect(&mat_scm_list_map);
  mat_scm_obj_protect(&mat_scm_list_apply);
  mat_scm_obj_protect(&mat_scm_list_string);

  mat_scm_list_list   = mat_scm_proc_new(list_list, NULL, 1);
  mat_scm_list_map    = mat_scm_proc_new(list_map, NULL, 1);
  mat_scm_list_apply  = mat_scm_proc_new(list_apply, NULL, 1);
  mat_scm_list_string = mat_scm_proc_new(list_string, NULL, 1);
}

static MatScm *
list_list ( void *data, MatScm *env, MatScm *args )
{
  return args;
}

static MatScm *
list_map ( void *data, MatScm *env, MatScm *args )
{
  MatScm *f;
  MatScm *head = NULL, *tail = NULL;

  /* TODO: Support multiple lists (as Guile does) */

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_cdr_get(args));
  ASSERT(!mat_scm_cons_car_get(mat_scm_cons_cdr_get(args)) || mat_scm_obj_type_get(mat_scm_cons_car_get(mat_scm_cons_cdr_get(args))) == &mat_scm_cons_type);
  ASSERT(!mat_scm_cons_cdr_get(mat_scm_cons_cdr_get(args)));

  f = mat_scm_cons_car_get(args);
  args = mat_scm_cons_car_get(mat_scm_cons_cdr_get(args));

  while (args)
    {
      MatScm *val = mat_scm_obj_call(f, mat_scm_cons_new(mat_scm_cons_car_get(args), NULL), env, 0);
      MatScm *obj = mat_scm_cons_new(val, NULL);

      if (!head)
        head = tail = obj;
      else
        {
          mat_scm_cons_cdr_set(tail, obj);
          tail = obj;
        }
      args = mat_scm_cons_cdr_get(args);
    }

  return head;
}

static MatScm *
list_apply ( void *data, MatScm *env, MatScm *args )
{
  return mat_scm_obj_eval(args, env);
}

static MatScm *
list_string ( void *data, MatScm *env, MatScm *args )
{
  int len;
  MatScm *tmp;
  char *buffer;

  for (len = 0, tmp = mat_scm_cons_car_get(args); tmp; tmp = mat_scm_cons_cdr_get(tmp))
    {
      ASSERT(mat_scm_obj_type_get(tmp) == &mat_scm_cons_type);
      ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(tmp)) == &mat_scm_char_type);

      len ++;
    }

  buffer = malloc(len + 1);

  for (len = 0, tmp = mat_scm_cons_car_get(args); tmp; tmp = mat_scm_cons_cdr_get(tmp))
    {
      ASSERT(mat_scm_obj_type_get(tmp) == &mat_scm_cons_type);
      ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(tmp)) == &mat_scm_char_type);

      buffer[len ++] = mat_scm_char_extract(mat_scm_cons_car_get(tmp));
    }

  return mat_scm_string_new(buffer, len);
}

unsigned int
mat_scm_list_length_get ( MatScm *list )
{
  unsigned int i;

  for (i = 0; list; i ++)
    {
      ASSERT(mat_scm_obj_type_get(list) == &mat_scm_cons_type);
      list = mat_scm_cons_cdr_get(list);
    }

  return i;
}
