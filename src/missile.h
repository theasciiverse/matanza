#ifndef __MATANZA_MISSILE_H__
#define __MATANZA_MISSILE_H__

typedef enum MatMissileSize MatMissileSize;

enum MatMissileSize
{
  MAT_MISSILE_SIZE_TINY,
  MAT_MISSILE_SIZE_SMALL,
  MAT_MISSILE_SIZE_BIG,
  MAT_MISSILE_SIZE_HUGE
};

#define MAT_MISSILE_SIZE_COUNT 4

#include <universe.h>
#include <machine.h>

MatScm                   *mat_missile_new                  ( MatScm             *source,
                                                             MatMissileSize      size,
                                                             int                 dir );

MatScm                   *mat_missile_new_ang              ( MatScm             *obj,
                                                             MatMissileSize      size,
                                                             int                 dir,
                                                             int                 ang );

#if 0
INLINE MatMissileSize     mat_missile_size_get             ( MatScm             *m );

INLINE unsigned int       mat_missile_dmg_get              ( MatScm             *m );

INLINE MatScm            *mat_missile_source_get           ( MatScm             *m );

INLINE char              *mat_missile_hit_ship_get         ( MatScm             *m );

INLINE char              *mat_missile_hit_asteroid_get     ( MatScm             *m );
#endif

#endif
