#ifndef __MATANZA_SCM_LOAD_H__
#define __MATANZA_SCM_LOAD_H__

INLINE void               mat_scm_load                     ( char               *file,
                                                             MatScm             *env );

void                      mat_scm_load_init                ( void );

MatScm                   *mat_scm_load_load;

#endif
