#ifndef __MATANZA_SHIP_H__
#define __MATANZA_SHIP_H__

typedef struct MatShip MatShip;

#include <universe.h>
#include <machine.h>
#include <team.h>
#include <widgetplay.h>
#include <machine.h>
#include <missile.h>

/* Return the movingobj associated to the ship */
INLINE MatScm            *mat_ship_movingobj_get           ( MatScm             *sh );

/* Send a message to the player controling the ship */
INLINE void               mat_ship_message_vprintf         ( MatScm             *ship,
                                                             char               *fmt,
                                                             va_list             args );

INLINE void               mat_ship_message_printf          ( MatScm             *obj,
                                                             char               *fmt,
                                                             ... );

void                      mat_ship_update                  ( MatScm             *ship );

/* Create a new ship in the given team and universe */
MatScm                   *mat_ship_new                     ( MatScm             *un,
                                                             MatScm             *name,
                                                             MatScm             *team,
                                                             MatScm             *mmp,
                                                             MatScm             *widget,
                                                             MatScm             *type );

unsigned int              mat_ship_player_get              ( MatScm             *ship );

INLINE MatScm            *mat_ship_universe_get            ( MatScm             *ship );

void                      mat_ship_start                   ( MatScm             *ship );

INLINE MatScm            *mat_ship_team_get                ( MatScm             *ship );

INLINE void               mat_ship_team_set                ( MatScm             *ship,
                                                             MatScm             *team );

INLINE unsigned int       mat_ship_times_dead_get          ( MatScm             *ship );

INLINE void               mat_ship_times_dead_set          ( MatScm             *ship,
                                                             unsigned int        times );

INLINE void               mat_ship_health_reset            ( MatScm             *ship );

unsigned int              mat_ship_game_over_get           ( MatScm             *ship );

INLINE MatScm            *mat_ship_machine_get             ( MatScm             *ship );

INLINE MatScm            *mat_ship_widget_play_get         ( MatScm             *ship );

INLINE void               mat_ship_widget_play_set         ( MatScm             *ship,
                                                             MatScm             *widget );

INLINE MatScm            *mat_ship_machine_hash_next_get   ( MatScm             *ship );

INLINE void               mat_ship_machine_hash_next_set   ( MatScm             *ship,
                                                             MatScm             *next );

INLINE MatScm            *mat_ship_machine_hash_prev_get   ( MatScm             *ship );

INLINE void               mat_ship_machine_hash_prev_set   ( MatScm             *ship,
                                                             MatScm             *prev );

INLINE MatScm            *mat_ship_machine_list_next_get   ( MatScm             *ship );

INLINE void               mat_ship_machine_list_next_set   ( MatScm             *ship,
                                                             MatScm             *next );

INLINE MatScm            *mat_ship_machine_list_prev_get   ( MatScm             *ship );

INLINE void               mat_ship_machine_list_prev_set   ( MatScm             *ship,
                                                             MatScm             *prev );

void                      mat_ship_shot                    ( MatScm             *ship,
                                                             MatMissileSize      size,
                                                             int                 dir );

/* Register that the connection for this ship is gone */
void                      mat_ship_remove                  ( MatScm             *ship );

/* Is the ship still inside the lists of the universe? */
INLINE int                mat_ship_alive_get               ( MatScm             *ship );

INLINE void               mat_ship_mark                    ( MatScm             *ship );

INLINE unsigned int       mat_ship_speed_inc_get           ( MatScm             *ship );

INLINE void               mat_ship_speed_inc_set           ( MatScm             *ship,
                                                             unsigned int        inc );

INLINE MatScm            *mat_ship_shiptype_get            ( MatScm             *ship );

/* Return the name of the player controling the ship */
INLINE MatScm            *mat_ship_name_get                ( MatScm             *sh );


INLINE MatScm            *mat_ship_mmp_get                 ( MatScm             *obj );

INLINE int                mat_ship_mmp_id_get              ( MatScm             *ship );

INLINE void               mat_ship_mmp_id_set              ( MatScm             *ship,
                                                             unsigned int        id );

INLINE MatScm            *mat_ship_widgetplay_get          ( MatScm             *obj );

INLINE void               mat_ship_widgetplay_set          ( MatScm             *obj,
                                                             MatScm             *widget );


#if 0
MatMovingObjTypeD         mat_ship_type;
#endif

#endif
