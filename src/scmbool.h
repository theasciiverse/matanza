#ifndef __MATANZA_SCM_BOOL_H__
#define __MATANZA_SCM_BOOL_H__

void                      mat_scm_bool_init                ( void );

void                      mat_scm_bool_define              ( MatScm             *env );

MatScmType                mat_scm_bool_type;

MatScm                   *mat_scm_bool_true;

#endif
