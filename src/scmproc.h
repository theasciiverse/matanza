#ifndef __MATANZA_SCM_PROC_H__
#define __MATANZA_SCM_PROC_H__

typedef MatScm *(MatScmCallback) ( void *data, MatScm *env, MatScm *args );

MatScm                   *mat_scm_proc_new                 ( MatScmCallback     *func,
                                                             void               *funcdata,
                                                             unsigned int        evalargs );

MatScm                   *mat_scm_proc_call                ( void               *data,
                                                             MatScm             *env,
                                                             MatScm             *args );

unsigned int              mat_scm_proc_evalargs_get        ( MatScm             *proc );

MatScmType                mat_scm_proc_type;

#endif
