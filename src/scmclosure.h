#ifndef __MATANZA_SCM_CLOSURE_H__
#define __MATANZA_SCM_CLOSURE_H__

MatScm                   *mat_scm_closure_call             ( void               *data,
                                                             MatScm             *env,
                                                             MatScm             *args );

INLINE void               mat_scm_closure_init             ( void );

MatScmType                mat_scm_closure_type;

MatScm                   *mat_scm_closure_lambda;

#endif
