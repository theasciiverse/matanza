#ifndef __MATANZA_SCM_LIST_H__
#define __MATANZA_SCM_LIST_H__

void                      mat_scm_list_init                ( void );

unsigned int              mat_scm_list_length_get          ( MatScm             *list );

MatScm                   *mat_scm_list_list;
MatScm                   *mat_scm_list_map;
MatScm                   *mat_scm_list_apply;
MatScm                   *mat_scm_list_string;

#endif
