#include <matanza.h>

typedef struct MatScmString MatScmString;

struct MatScmString
{
  char          *buffer;
  unsigned int   buflen;
};

static void               mat_scm_string_free              ( void               *funcdata );

static int                mat_scm_string_equal             ( MatScm             *obj0,
                                                             MatScm             *obj1 );

static MatScmCallback     str_list;
static MatScmCallback     str_len;
static MatScmCallback     str_append;
static MatScmCallback     str_subs;

MatScm                   *mat_scm_string_list = NULL;
MatScm                   *mat_scm_string_length = NULL;
MatScm                   *mat_scm_string_append = NULL;
MatScm                   *mat_scm_string_substring = NULL;

MatScmType mat_scm_string_type =
{
  "string",
  mat_scm_string_free,
  NULL,
  NULL,
  NULL,
  mat_scm_string_equal
};

MatScm *
mat_scm_string_new ( char *buffer, unsigned int buflen )
{
  MatScmString *data;

  data = malloc(sizeof(MatScmString));

  data->buffer = buffer;
  data->buflen = buflen;

  return mat_scm_obj_malloc(&mat_scm_string_type, data);
}

INLINE MatScm *
mat_scm_string_data_new ( char *buffer )
{
  return mat_scm_string_new(buffer, strlen(buffer));
}

static void
mat_scm_string_free ( void *funcdata )
{
  MatScmString *data = (MatScmString *) funcdata;

  free(data->buffer);
  free(data);
}

char *
mat_scm_string_data_get ( MatScm *obj )
{
  MatScmString *data;

  ASSERT(obj);
  ASSERT(mat_scm_obj_type_get(obj) == &mat_scm_string_type);

  data = (MatScmString *) mat_scm_obj_data_get(obj);

  ASSERT(data);

  return data->buffer;
}

unsigned int
mat_scm_string_length_get ( MatScm *obj )
{
  MatScmString *data;

  ASSERT(obj);
  ASSERT(mat_scm_obj_type_get(obj) == &mat_scm_string_type);

  data = (MatScmString *) mat_scm_obj_data_get(obj);

  ASSERT(data);

  return data->buflen;
}

void
mat_scm_string_init ( void )
{
  mat_scm_obj_protect(&mat_scm_string_list);
  mat_scm_obj_protect(&mat_scm_string_length);
  mat_scm_obj_protect(&mat_scm_string_append);
  mat_scm_obj_protect(&mat_scm_string_substring);

  mat_scm_string_list = mat_scm_proc_new(str_list,   NULL, 1);
  mat_scm_string_length = mat_scm_proc_new(str_len, NULL, 1);
  mat_scm_string_append = mat_scm_proc_new(str_append, NULL, 1);
  mat_scm_string_substring = mat_scm_proc_new(str_subs, NULL, 1);
}

static MatScm *
str_list ( void *data, MatScm *env, MatScm *args )
{
  MatScm *str, *head = NULL, *tail = NULL;
  unsigned char *buf;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  str = mat_scm_cons_car_get(args);

  ASSERT(str);
  ASSERT(mat_scm_obj_type_get(str) == &mat_scm_string_type);

  ASSERT(!mat_scm_cons_cdr_get(args));

  for (buf = mat_scm_string_data_get(str); *buf; buf ++)
    {
      MatScm *obj = mat_scm_cons_new(mat_scm_char_new(*buf), NULL);
      if (!head)
        head = obj;
      else
        mat_scm_cons_cdr_set(tail, obj);
      tail = obj;
    }

  return head;
}

static MatScm *
str_len ( void *data, MatScm *env, MatScm *args )
{
  MatScm *str;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

  str = mat_scm_cons_car_get(args);

  ASSERT(str);
  ASSERT(mat_scm_obj_type_get(str) == &mat_scm_string_type);

  ASSERT(!mat_scm_cons_cdr_get(args));

  return mat_scm_number_new((double) mat_scm_string_length_get(str));
}

static MatScm *
str_append ( void *data, MatScm *env, MatScm *args )
{
  MatScm *tmp;
  unsigned int len;
  char *buffer;

  for (len = 0, tmp = args; tmp; tmp = mat_scm_cons_cdr_get(tmp))
    {
      ASSERT(mat_scm_obj_type_get(tmp) == &mat_scm_cons_type);
      ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(tmp)) == &mat_scm_string_type);

      len += mat_scm_string_length_get(mat_scm_cons_car_get(tmp));
    }

  buffer = malloc(len + 1);

  for (len = 0, tmp = args; tmp; tmp = mat_scm_cons_cdr_get(tmp))
    {
      ASSERT(mat_scm_obj_type_get(tmp) == &mat_scm_cons_type);
      ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(tmp)) == &mat_scm_string_type);

      strcpy(buffer + len, mat_scm_string_data_get(mat_scm_cons_car_get(tmp)));
      len += mat_scm_string_length_get(mat_scm_cons_car_get(tmp));
    }

  buffer[len] = 0;

  return mat_scm_string_new(buffer, len);
}

static int
mat_scm_string_equal ( MatScm *obj0, MatScm *obj1 )
{
  ASSERT(mat_scm_obj_type_get(obj0) == &mat_scm_string_type);
  ASSERT(mat_scm_obj_type_get(obj1) == &mat_scm_string_type);

  return mat_scm_string_length_get(obj0) == mat_scm_string_length_get(obj1)
    && !strncmp(mat_scm_string_data_get(obj0), mat_scm_string_data_get(obj1), mat_scm_string_length_get(obj0));
}

static MatScm *
str_subs ( void *data, MatScm *env, MatScm *args )
{
  char *buf;
  unsigned int p0, p1;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_string_type);

  buf = mat_scm_string_data_get(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  p0 = (unsigned int) mat_scm_number_get(mat_scm_cons_car_get(args));
  args = mat_scm_cons_cdr_get(args);

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_number_type);

  p1 = (unsigned int) mat_scm_number_get(mat_scm_cons_car_get(args));

  ASSERT(!mat_scm_cons_cdr_get(args));

  ASSERT(buf);

  return mat_scm_string_new(buf + p0, p1 - p0);
}
