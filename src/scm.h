#ifndef __MATANZA_SCM_H__
#define __MATANZA_SCM_H__

typedef struct MatScmType MatScmType;
typedef struct MatScm MatScm;

struct MatScmType
{
  char     *name;
  void    (*free)  (void *);
  void    (*mark)  (void *);
  int     (*eq)    (MatScm *obj0, MatScm *obj1);
  int     (*eqv)   (MatScm *obj0, MatScm *obj1);
  int     (*equal) (MatScm *obj0, MatScm *obj1);
};

struct MatScm
{
  MatScmType *type;
  void       *data;

  int         gc_mark;

  MatScm     *prev;
  MatScm     *next;
};

void                      mat_scm_obj_set                  ( MatScm             *obj,
                                                             MatScmType         *type,
                                                             void               *data );

MatScmType               *mat_scm_obj_type_get             ( MatScm             *obj );

void                     *mat_scm_obj_data_get             ( MatScm             *obj );

MatScm                   *mat_scm_obj_malloc               ( MatScmType         *type,
                                                             void               *data );

void                      mat_scm_obj_protect              ( MatScm            **var );

void                      mat_scm_obj_mark                 ( MatScm             *obj );

INLINE MatScm            *mat_scm_obj_eval                 ( MatScm             *obj,
                                                             MatScm             *env );

INLINE MatScm            *mat_scm_obj_call                 ( MatScm             *obj,
                                                             MatScm             *args,
                                                             MatScm             *env,
                                                             int                 evalargs );

void                      mat_scm_init                     ( void );

int                       mat_scm_obj_eq                   ( MatScm             *obj0,
                                                             MatScm             *obj1 );

int                       mat_scm_obj_eqv                  ( MatScm             *obj0,
                                                             MatScm             *obj1 );

int                       mat_scm_obj_equal                ( MatScm             *obj0,
                                                             MatScm             *obj1 );

MatScm                   *mat_scm_obj_type;

#endif
