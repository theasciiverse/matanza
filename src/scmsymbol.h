#ifndef __MATANZA_SCM_SYMBOL_H__
#define __MATANZA_SCM_SYMBOL_H__

void                      mat_scm_symbol_init              ( void );

MatScm                   *mat_scm_symbol_new               ( char               *buffer,
                                                             int                 len );

char                     *mat_scm_symbol_data_get          ( MatScm             *obj );

MatScmType                mat_scm_symbol_type;

MatScm                   *mat_scm_symbol_string;

#endif
