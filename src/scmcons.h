#ifndef __MATANZA_SCM_CONS_H__
#define __MATANZA_SCM_CONS_H__

MatScm                   *mat_scm_cons_car_get             ( MatScm             *cons );

MatScm                   *mat_scm_cons_cdr_get             ( MatScm             *cons );

void                      mat_scm_cons_car_set             ( MatScm             *cons,
                                                             MatScm             *value );

void                      mat_scm_cons_cdr_set             ( MatScm             *cons,
                                                             MatScm             *value );

MatScm                   *mat_scm_cons_new                 ( MatScm             *car,
                                                             MatScm             *cdr );

void                      mat_scm_cons_init                ( void );

MatScmType                mat_scm_cons_type;

MatScm                   *mat_scm_cons_car;
MatScm                   *mat_scm_cons_cdr;
MatScm                   *mat_scm_cons_setcar;
MatScm                   *mat_scm_cons_setcdr;
MatScm                   *mat_scm_cons_cons;
MatScm                   *mat_scm_cons_null;

#endif
