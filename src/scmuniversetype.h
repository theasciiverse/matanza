#ifndef __MATANZA_SCM_UNIVERSE_TYPE_H__
#define __MATANZA_SCM_UNIVERSE_TYPE_H__

void                      mat_scm_universetype_init        ( void );

MatScmType                mat_scm_universetype_type;

MatScm                   *mat_scm_universetype_new;
MatScm                   *mat_scm_universetype_banner_add;
MatScm                   *mat_scm_universetype_image_add;
MatScm                   *mat_scm_universetype_ship_add;
//MatScm                   *mat_scm_universetype_image_asteroid_set;
MatScm                   *mat_scm_universetype_image_missile_set;
MatScm                   *mat_scm_universetype_event_join_set;
MatScm                   *mat_scm_universetype_event_start_set;
MatScm                   *mat_scm_universetype_event_talk_set;

#endif
