#ifndef __MATANZA_SCM_NUMBER_H__
#define __MATANZA_SCM_NUMBER_H__

MatScm                   *mat_scm_number_new               ( double              n );

MatScm                   *mat_scm_number_new_string        ( char               *buffer );

double                    mat_scm_number_get               ( MatScm             *n );

void                      mat_scm_number_init              ( void );

MatScmType                mat_scm_number_type;

MatScm                   *mat_scm_number_add;
MatScm                   *mat_scm_number_mul;
MatScm                   *mat_scm_number_sub;
MatScm                   *mat_scm_number_div;
MatScm                   *mat_scm_number_random;
MatScm                   *mat_scm_number_cmp_lt;
MatScm                   *mat_scm_number_cmp_lte;
MatScm                   *mat_scm_number_cmp_gt;
MatScm                   *mat_scm_number_cmp_gte;
MatScm                   *mat_scm_number_zero;

#endif
