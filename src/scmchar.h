#ifndef __MATANZA_SCM_CHAR_H__
#define __MATANZA_SCM_CHAR_H__

void                      mat_scm_char_init                ( void );

unsigned char             mat_scm_char_extract             ( MatScm             *obj );

MatScm                   *mat_scm_char_new                 ( unsigned char       c );

MatScmType                mat_scm_char_type;

#endif
