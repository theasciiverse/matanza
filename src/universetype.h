#ifndef __MATANZA_UNIVERSETYPE_H__
#define __MATANZA_UNIVERSETYPE_H__

/* A structure with the information of a universe type */
typedef struct MatUniverseType MatUniverseType;

#include <scm.h>

INLINE MatScm            *mat_universetype_new             ( MatScm             *name,
                                                             unsigned int        ang,
                                                             unsigned int        mapsize_x,
                                                             unsigned int        mapsize_y,
                                                             MatScm             *banner );

INLINE void               mat_universetype_free            ( MatScm             *ut );

INLINE MatScm            *mat_universetype_name_get        ( MatScm             *type );

INLINE MatScm            *mat_universetype_banner_get      ( MatScm             *un );

INLINE MatScm            *mat_universetype_head_get        ( void );

INLINE MatScm            *mat_universetype_tail_get        ( void );

INLINE unsigned int       mat_universetype_ang_get         ( MatScm             *type );

INLINE MatAngleCache     *mat_universetype_ang_cache_get   ( MatScm             *type );

INLINE unsigned int       mat_universetype_mapsize_x_get   ( MatScm             *type );

INLINE unsigned int       mat_universetype_mapsize_y_get   ( MatScm             *type );

INLINE MatScm            *mat_universetype_shiptype_add    ( MatScm             *un,
                                                             MatScm             *st );

INLINE MatScm            *mat_universetype_ship_head_get   ( MatScm             *type );

INLINE MatScm            *mat_universetype_ship_tail_get   ( MatScm             *type );

/* Add an image to a universe and return its id */
unsigned int              mat_universetype_image_add       ( MatScm             *un,
                                                             MatScm             *img );

/* Return the number of images registered in one universetype */
INLINE unsigned int       mat_universetype_image_count_get ( MatScm             *un );

/* Return the image specified by id */
MatScm                   *mat_universetype_image_get       ( MatScm             *type, 
                                                             unsigned int        id );

/* Apply func to all the images in the universetype */
void                      mat_universetype_image_forall    ( MatScm             *type,
                                                             void (*func) (void *, MatScm *, unsigned int ),
                                                             void               *data );

INLINE void               mat_universetype_mask_sp         ( MatScm             *type,
                                                             MatScm             *obj );

INLINE int                mat_universetype_mask_crash      ( MatScm             *type,
                                                             MatScm             *obj );

#if 0
INLINE unsigned int       mat_universetype_image_asteroid_get( MatScm             *type,
                                                             MatAsteroidSize     size );
#endif

INLINE unsigned int       mat_universetype_image_missile_get( MatScm             *type,
                                                             MatMissileSize      size );

#if 0
INLINE void               mat_universetype_image_asteroid_set( MatScm             *type,
                                                             unsigned int       *id );
#endif

INLINE void               mat_universetype_image_missile_set( MatScm             *type,
                                                             unsigned int       *id );

INLINE void               mat_universetype_event_join_set  ( MatScm              *type,
                                                             MatScm              *proc );

INLINE void               mat_universetype_event_join_run  ( MatScm              *type, 
                                                             MatScm              *obj );

INLINE void               mat_universetype_event_start_set ( MatScm              *type,
                                                             MatScm              *proc );

INLINE void               mat_universetype_event_start_run ( MatScm              *type,
                                                             MatScm              *un );

INLINE void               mat_universetype_event_talk_set  ( MatScm              *type,
                                                             MatScm              *proc );

INLINE void               mat_universetype_event_talk_run  ( MatScm              *type,
                                                             MatScm              *ship,
                                                             MatScm              *msg );

INLINE void               mat_universetype_mark            ( MatScm              *type );

#endif
