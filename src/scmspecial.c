#include <matanza.h>

MatScmType mat_scm_special_type =
{
  "special",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

MatScm                   *mat_scm_special_close  = NULL;
MatScm                   *mat_scm_special_eof    = NULL;
MatScm                   *mat_scm_special_begin  = NULL;

void
mat_scm_special_init ( void )
{
  mat_scm_obj_protect(&mat_scm_special_close);
  mat_scm_obj_protect(&mat_scm_special_eof);
  mat_scm_obj_protect(&mat_scm_special_begin);

  mat_scm_special_close  = mat_scm_obj_malloc(&mat_scm_special_type, NULL);
  mat_scm_special_eof    = mat_scm_obj_malloc(&mat_scm_special_type, NULL);
  mat_scm_special_begin  = mat_scm_obj_malloc(&mat_scm_special_type, NULL);
}
