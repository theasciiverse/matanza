#ifndef __MATANZA_SCM_STRING_H__
#define __MATANZA_SCM_STRING_H__

MatScm                   *mat_scm_string_new               ( char               *buffer,
                                                             unsigned int        len );

INLINE MatScm            *mat_scm_string_data_new          ( char               *buffer );

char                     *mat_scm_string_data_get          ( MatScm             *obj );

void                      mat_scm_string_init              ( void );

MatScmType                mat_scm_string_type;

MatScm                   *mat_scm_string_list;
MatScm                   *mat_scm_string_length;
MatScm                   *mat_scm_string_append;
MatScm                   *mat_scm_string_substring;

#endif
