#ifndef __MATANZA_TEAM_H__
#define __MATANZA_TEAM_H__

typedef struct MatTeam MatTeam;

#include <universe.h>

MatScm                   *mat_team_new                     ( MatScm             *name,
                                                             MatScm             *un );

MatScm                   *mat_team_ship_add                ( MatScm             *team,
                                                             MatScm             *ship );

void                      mat_team_ship_rm                 ( MatScm             *team,
                                                             MatScm             *ship );

#if 0
void                      mat_team_prev_set                ( MatScm             *team,
                                                             MatScm             *prev );

void                      mat_team_next_set                ( MatScm             *team,
                                                             MatScm             *next );

MatTeam                  *mat_team_prev_get                ( MatTeam            *team );

MatTeam                  *mat_team_next_get                ( MatTeam            *team );
#endif

INLINE MatScm            *mat_team_name_get                ( MatScm             *team );

INLINE void               mat_team_ship_forall             ( MatScm             *team,
                                                             void              (*func) ( void *, MatScm  * ),
                                                             void               *data );

INLINE void               mat_team_times_dead_set          ( MatScm             *team,
                                                             unsigned int        times );

INLINE unsigned int       mat_team_times_dead_get          ( MatScm             *team );

INLINE MatScm            *mat_team_universe_get            ( MatScm             *team );

INLINE MatScm            *mat_team_ship_head_get           ( MatScm             *team );

INLINE MatScm            *mat_team_ship_tail_get           ( MatScm             *team );

INLINE void               mat_team_mark                    ( void               *team );

#endif
