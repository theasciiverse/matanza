#include <matanza.h>

static MatScm            *mat_scm_char_table = NULL;

MatScmType mat_scm_char_type =
{
  "char",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};

void
mat_scm_char_init ( void )
{
  int i;

  mat_scm_char_table = malloc(sizeof(MatScm) * 256);

  for (i = 0; i < 256; i ++)
    mat_scm_obj_set(mat_scm_char_table + i, &mat_scm_char_type, NULL);
}

unsigned char
mat_scm_char_extract ( MatScm *obj )
{
  ASSERT(obj);
  ASSERT(mat_scm_obj_type_get(obj) == &mat_scm_char_type);

  return obj - mat_scm_char_table;
}

MatScm *
mat_scm_char_new ( unsigned char c )
{
  return mat_scm_char_table + c;
}
