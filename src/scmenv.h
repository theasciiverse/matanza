#ifndef __MATANZA_SCM_ENV_H__
#define __MATANZA_SCM_ENV_H__

MatScm                   *mat_scm_env_find                 ( MatScm             *env,
                                                             char               *name );

MatScm                   *mat_scm_env_extend               ( MatScm             *env,
                                                             MatScm             *names,
                                                             MatScm             *values );

void                      mat_scm_env_define               ( MatScm             *env,
                                                             char               *name,
                                                             MatScm             *value );

MatScm                   *mat_scm_env_malloc               ( void );

void                      mat_scm_env_add                  ( MatScm             *env );

void                      mat_scm_env_init                 ( void );

MatScm                   *mat_scm_env_current;

MatScm                   *mat_scm_env_new;
MatScm                   *mat_scm_env_set;
MatScm                   *mat_scm_env_envdef;
MatScm                   *mat_scm_env_envset;
MatScm                   *mat_scm_env_envget;

#endif
