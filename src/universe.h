#ifndef __MATANZA_UNIVERSE_H__
#define __MATANZA_UNIVERSE_H__

typedef struct MatUniverse MatUniverse;

#include <imagevector.h>
#include <movingobj.h>
#include <team.h>
#include <sub.h>
#include <shiptype.h>
#include <universetype.h>

#define MAT_UNIVERSE_OBJS_RM_END ((void *)&mat_universe_mapsize_x_get)

/* Return the width of the map for one universe. */
INLINE unsigned int       mat_universe_mapsize_x_get       ( MatScm             *un );

/* Return the height of the map for one universe. */
INLINE unsigned int       mat_universe_mapsize_y_get       ( MatScm             *un );

/* Iterate through all the images in the universe. */
INLINE void               mat_universe_image_forall        ( MatScm             *un,
                                                             void (*func) ( void *, MatScm *, unsigned int ),
                                                             void               *data );

INLINE MatScm            *mat_universe_shiptype_head_get   ( MatScm             *un);

INLINE MatScm            *mat_universe_shiptype_tail_get   ( MatScm             *un);

#if 0
INLINE MatScm            *mat_universe_shiptype_find       ( MatUniverse        *un,
                                                             int                 id );
#endif

INLINE void               mat_universe_movingobj_forall    ( MatScm             *un,
                                                             void (*func) ( void *, MatScm * ),
                                                             void               *data );

INLINE unsigned int       mat_universe_ship_dmg_get        ( MatScm             *un );

INLINE unsigned int       mat_universe_asteroid_dmg_get    ( MatScm             *un );

/* Iterate through all the machines in the universe. */
INLINE void               mat_universe_machine_forall      ( MatScm             *un,
                                                             void (*func) ( void *, MatMachine * ),
                                                             void               *data );

INLINE MatScm            *mat_universe_name_get            ( MatScm             *un );

INLINE MatScm            *mat_universe_banner_get          ( MatScm             *un );

INLINE MatScm            *mat_universe_new                 ( MatScm             *type,
                                                             MatScm             *name );

void         mat_universe_free ( MatScm      *un );

void                      mat_universe_update              ( MatScm             *un );

MatScm                   *mat_universe_image_get           ( MatScm             *un,
                                                             unsigned int        id );

unsigned int              mat_universe_image_count_get     ( MatScm             *un );

#if 0
INLINE MatUniverse       *mat_universe_hash_next_get       ( MatUniverse        *un );
#endif

unsigned int              mat_universe_angles_get          ( MatScm             *un );

INLINE MatAngleCache     *mat_universe_angles_cache_get    ( MatScm             *un );

INLINE int                mat_universe_pause_get           ( MatScm             *un );

INLINE void               mat_universe_pause_set           ( MatScm             *un,
                                                             MatScm             *src,
                                                             int                 pause );

#if 0
INLINE int               *mat_universe_obj_real_get        ( MatUniverse        *un,
                                                             MatMovingObjType    type );
#endif

INLINE void               mat_universe_ang_set             ( MatScm             *un,
                                                             unsigned int        ang );

#if 0
MatScm **mat_universe_obj_head_get ( MatScm *un, MatMovingObjType type );
MatScm **mat_universe_obj_tail_get ( MatScm *un, MatMovingObjType type );
#endif

#if 0
INLINE void               mat_universe_obj_head_set        ( MatScm             *un,
                                                             MatScm             *obj );

INLINE void               mat_universe_obj_tail_set        ( MatScm             *un,
                                                             MatScm             *obj );
#endif

INLINE double             mat_universe_sin_get             ( MatScm             *un,
                                                             unsigned int        ang );

INLINE double             mat_universe_cos_get             ( MatScm             *un,
                                                             unsigned int        ang );

#if 0
INLINE MatMovingObj  **mat_universe_objs_rm_get  ( MatUniverse *un );
#endif

INLINE MatSub        **mat_universe_subs_get     ( MatScm      *un );

INLINE unsigned int       mat_universe_id_get              ( MatScm             *un );

INLINE void mat_universe_message_printf ( MatScm      *u, char *fmt, ... );

INLINE unsigned int       mat_universe_maxdead_get         ( MatScm             *un );

INLINE MatScm            *mat_universe_team_add            ( MatScm             *un,
                                                             MatScm             *t );

INLINE MatScm            *mat_universe_team_get            ( MatScm             *un,
                                                             MatScm             *name );

MatScm                   *mat_universe_team_head_get       ( MatScm             *un );

MatScm                   *mat_universe_team_tail_get       ( MatScm             *un );

MatScm                   *mat_universe_head_get            ( void );

MatScm                   *mat_universe_tail_get            ( void );

#if 0
MatUniverse              *mat_universe_prev_get            ( MatUniverse        *un );

MatUniverse              *mat_universe_next_get            ( MatUniverse        *un );
#endif

#if 0
INLINE void               mat_universe_movingobj_remove    ( MatUniverse        *un,
                                                             MatMovingObj       *obj );
#endif

#if 0
INLINE unsigned int       mat_universe_image_asteroid_get  ( MatScm             *un, 
                                                             MatAsteroidSize     size );

INLINE unsigned int       mat_universe_image_missile_get   ( MatScm             *un, 
                                                             MatMissileSize      size );
#endif

INLINE void               mat_universe_event_join_run      ( MatScm             *un,
                                                             MatScm             *obj );

INLINE void               mat_universe_talk                ( MatScm             *un,
                                                             MatScm             *ship,
                                                             MatScm             *msg );

INLINE MatScm            *mat_universe_universetype_get    ( MatScm             *un );

INLINE MatScm            *mat_universe_movingobj_head_get  ( MatScm             *un );

INLINE MatScm            *mat_universe_movingobj_tail_get  ( MatScm             *un );

INLINE MatScm            *mat_universe_movingobj_add       ( MatScm             *un,
                                                             MatScm             *obj );

INLINE void               mat_universe_movingobj_remove    ( MatScm             *un,
                                                             MatScm             *obj );

INLINE void               mat_universe_mark                ( void               *funcdata );

MatScm           *mat_universe_head;
MatScm           *mat_universe_tail;

#endif
