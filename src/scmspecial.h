#ifndef __MATANZA_SCM_SPECIAL_H__
#define __MATANZA_SCM_SPECIAL_H__

void                      mat_scm_special_init             ( void );

MatScmType                mat_scm_special_type;

MatScm                   *mat_scm_special_close;
MatScm                   *mat_scm_special_eof;
MatScm                   *mat_scm_special_begin;

#endif
