#ifndef __MATANZA_MACHINE_H__
#define __MATANZA_MACHINE_H__

typedef struct MatMachine MatMachine;

#include <ship.h>
#include <movingobj.h>
#include <sub.h>
#include <connection.h>
#include <universe.h>

INLINE MatSub **mat_machine_subs_get ( MatMachine *m );

INLINE MatUniverse       *mat_machine_universe_get         ( MatScm             *m,
                                                             unsigned int        id );

INLINE void               mat_machine_ship_add             ( MatScm             *m,
                                                             MatScm             *ship );

INLINE unsigned int       mat_machine_ship_id_get          ( MatScm             *m );

INLINE MatShip           *mat_machine_ship_get             ( MatScm             *m,
                                                             unsigned int        id );

INLINE void               mat_machine_input                ( void               *m,
                                                             unsigned char       key );

MatScm                   *mat_machine_new                  ( MatScm             *con );

INLINE void               mat_machine_key_handler_set      ( MatScm             *m,
                                                             void (*func) ( MatScm *, void *, char ),
                                                             void               *data );

INLINE void               mat_machine_unknown_command      ( MatScm             *m,
                                                             int                 key );

INLINE MatConnection     *mat_machine_connection_get       ( MatScm             *m );

INLINE void               mat_machine_update               ( MatScm             *m );

INLINE void               mat_machine_message_vprintf      ( MatScm             *m,
                                                             unsigned int        id,
                                                             char               *fmt,
                                                             va_list             args );

#endif
