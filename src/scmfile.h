#ifndef __MATANZA_SCM_FILE_H__
#define __MATANZA_SCM_FILE_H__

MatScm                   *mat_scm_file_open                ( char               *path,
                                                             char               *mode );

char                      mat_scm_file_getc                ( MatScm             *file );

char                      mat_scm_file_peekc               ( MatScm             *file );

MatScmType                mat_scm_file_type;

#endif
