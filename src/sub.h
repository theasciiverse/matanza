#ifndef __MATANZA_SUB_H__
#define __MATANZA_SUB_H__

typedef struct MatSub MatSub;

#include <universe.h>
#include <connection.h>

int                       mat_sub_add                      ( MatScm                *m,
                                                             MatScm                *un );

void                      mat_sub_machine_rm               ( MatScm                *m );

INLINE void               mat_sub_machine_init             ( MatScm                *m );

INLINE MatScm            *mat_sub_universe_find_get        ( MatSub                *s,
                                                             unsigned int           id );

INLINE MatSub            *mat_sub_universe_next_get        ( MatSub                *s );

INLINE MatSub            *mat_sub_machine_next_get         ( MatSub                *s );

INLINE MatScm            *mat_sub_machine_get              ( MatSub                *s );

INLINE MatScm            *mat_sub_universe_get             ( MatSub                *s );

#endif
