#ifndef __MATANZA_SHIPTYPE_H__
#define __MATANZA_SHIPTYPE_H__

/* A structure with the information of a ship type */
typedef struct MatShipType MatShipType;

#include <missile.h>

INLINE MatScm            *mat_shiptype_new                 ( MatScm             *name,
                                                             MatScm             *desc,
                                                             unsigned int        id,
                                                             unsigned int        sp,
                                                             unsigned int        health,
                                                             unsigned int        sp_ang,
                                                             unsigned int       *m,
                                                             unsigned int       *fr );

INLINE MatScm            *mat_shiptype_name_get            ( MatScm             *st );

INLINE MatScm            *mat_shiptype_desc_get            ( MatScm             *st );

INLINE unsigned int       mat_shiptype_id_get              ( MatScm             *st );

INLINE unsigned int       mat_shiptype_health_get          ( MatScm             *st );

INLINE unsigned int       mat_shiptype_sp_get              ( MatScm             *st );

INLINE unsigned int       mat_shiptype_sp_ang_get          ( MatScm             *st );

INLINE unsigned int       mat_shiptype_missiles_get        ( MatScm             *st,
                                                             MatMissileSize      size );

INLINE unsigned int       mat_shiptype_firerate_get        ( MatScm             *st,
                                                             MatMissileSize      size );

INLINE unsigned int       mat_shiptype_image_get           ( MatScm             *st );

#endif
