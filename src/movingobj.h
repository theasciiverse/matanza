#ifndef __MATANZA_MOVINGOBJ_H__
#define __MATANZA_MOVINGOBJ_H__

#if 0
typedef enum MatMovingObjType MatMovingObjType;

enum MatMovingObjType
{
  MAT_MOVINGOBJ_TYPE_SHIP,
  MAT_MOVINGOBJ_TYPE_ASTEROID,
  /*MAT_MOVINGOBJ_TYPE_MISSILE*/
  /*MAT_MOVINGOBJ_TYPE_BULLET,*/
};

/* The number of entries in the enumeration MatMovingObjType. */
#define MAT_MOVINGOBJ_TYPE_COUNT 2

typedef struct MatMovingObjTypeD MatMovingObjTypeD;
#endif

#include <scm.h>

/* Description for a type for a moving object. */
struct MatMovingObjTypeD
{
  /* The name of the type of moving object. */

  char                 *name;

  /* This function is called periodically to update the moving object. */

  void                (*update) ( MatScm *data );

  /* This function is called if the object a of this type crashes against
   * another object. */

  void                (*crash)  ( MatScm *a, MatScm *b );

  /* This function is called to get rid of the object. */

  void                (*free) ( MatScm *obj );
};

#include <universe.h>

int                       mat_movingobj_crash_check_get    ( MatScm             *a,
                                                             MatScm             *b );

INLINE double             mat_movingobj_sp_x_get           ( MatScm             *obj );
INLINE double             mat_movingobj_sp_y_get           ( MatScm             *obj );
INLINE double             mat_movingobj_sp_linear_get      ( MatScm             *obj );
INLINE double             mat_movingobj_sp_ang_get         ( MatScm             *obj );

/* Get the x position of the object relative to the center of its image */
INLINE double             mat_movingobj_pos_x_center_get   ( MatScm             *obj );

/* Get the y position of the object relative to the center of its image */
INLINE double             mat_movingobj_pos_y_center_get   ( MatScm             *obj );

/* Return the environment associated to this object */
INLINE MatScm            *mat_movingobj_env_get            ( MatScm             *obj );

/* Get the x position of the object relative to its upper left corner */
INLINE double             mat_movingobj_pos_x_ul_get       ( MatScm             *obj );

/* Get the y position of the object relative to its upper left corner */
INLINE double             mat_movingobj_pos_y_ul_get       ( MatScm             *obj );

#if 0
INLINE MatMovingObj      *mat_movingobj_next_type_get      ( MatMovingObj       *obj );

INLINE MatMovingObj      *mat_movingobj_prev_type_get      ( MatMovingObj       *obj );

INLINE MatMovingObj      *mat_movingobj_prev_get           ( MatMovingObj       *obj );

INLINE MatMovingObj      *mat_movingobj_next_get           ( MatMovingObj       *obj );
#endif

INLINE unsigned int       mat_movingobj_crash_get          ( MatScm             *obj );

INLINE void               mat_movingobj_crash_set          ( MatScm             *obj,
                                                             unsigned int        crash );

INLINE unsigned int       mat_movingobj_health_get         ( MatScm             *obj );

INLINE void               mat_movingobj_health_set         ( MatScm             *obj,
                                                             unsigned int        health );

#if 0
/* Return the first in the list of behaviours for this object */
INLINE MatBehaviour      *mat_movingobj_behaviour_head_get ( MatMovingObj       *obj );

/* Return the last in the list of behaviours for this object */
INLINE MatBehaviour      *mat_movingobj_behaviour_tail_get ( MatMovingObj       *obj );

/* Add a behaviour at the end of the list for this object */
INLINE void               mat_movingobj_behaviour_add      ( MatMovingObj       *obj,
                                                             MatBehaviour       *b );

/* Set the first pointer in the list of behaviours for this object */
INLINE void               mat_movingobj_behaviour_head_set ( MatMovingObj       *obj,
                                                             MatBehaviour       *b );

/* Set the last pointer in the list of behaviours for this object */
INLINE void               mat_movingobj_behaviour_tail_set ( MatMovingObj       *obj,
                                                             MatBehaviour       *b );

/* Execute all the behaviours for object obj */
INLINE void               mat_movingobj_behaviour_exec     ( MatMovingObj       *obj );
#endif

#if 0
INLINE int                mat_movingobj_rm_queue_get       ( MatMovingObj       *obj );
INLINE MatMovingObj      *mat_movingobj_rm_queue_next_get  ( MatMovingObj       *obj );
#endif

INLINE MatScm            *mat_movingobj_image_get          ( MatScm             *obj );

INLINE unsigned int       mat_movingobj_image_id_get       ( MatScm             *obj );
#if 0
INLINE MatMovingObjType   mat_movingobj_type_get           ( MatScm             *obj );
#endif

/* Return the universe this movingobj belongs to */
INLINE MatScm            *mat_movingobj_universe_get       ( MatScm             *obj );

INLINE void               mat_movingobj_mark               ( void               *obj );

INLINE MatScm            *mat_movingobj_ship_get           ( MatScm             *obj );

INLINE double             mat_movingobj_ang_get            ( MatScm             *obj );

#if 0
INLINE void               mat_movingobj_image_set          ( MatMovingObj       *obj,
                                                             unsigned int        id );
#endif

#if 0
INLINE MatMovingObj      *mat_movingobj_new_spec           ( MatUniverse        *un,
                                                             char              **atts );
#endif

/* Create a new moving object of the specified type. */
MatScm                   *mat_movingobj_new                ( MatScm             *un,
                                                             MatScm             *ship,
                                                             unsigned int        img,
                                                             unsigned char      *pal,
                                                             unsigned int        ttl,
                                                             unsigned int        health );

void                      mat_movingobj_move               ( MatScm             *obj );

INLINE void               mat_movingobj_sp_calc            ( MatScm             *obj );

INLINE void               mat_movingobj_sp_x_set           ( MatScm             *obj,
                                                             double              sp );

INLINE void               mat_movingobj_sp_y_set           ( MatScm             *obj,
                                                             double              sp );

INLINE void               mat_movingobj_sp_linear_set      ( MatScm             *obj,
                                                             double              spx,
                                                             double              spy );

INLINE void               mat_movingobj_sp_linear_real_set ( MatScm             *obj,
                                                             double              spx,
                                                             double              spy,
                                                             double              sp );

INLINE void               mat_movingobj_pos_x_set          ( MatScm             *obj,
                                                             double              pos_x );

INLINE void               mat_movingobj_pos_y_set          ( MatScm             *obj,
                                                             double              pos_y );

INLINE void               mat_movingobj_pos_set            ( MatScm             *obj,
                                                             double              pos_x,
                                                             double              pos_y );

INLINE void               mat_movingobj_ang_set            ( MatScm             *obj,
                                                             unsigned int        ang );

INLINE void               mat_movingobj_sp_ang_set         ( MatScm             *obj,
                                                             double              sp );

void                      mat_movingobj_crash_wall         ( MatScm             *obj );

INLINE void               mat_movingobj_event_update_set   ( MatScm             *obj,
                                                             MatScm             *proc );

INLINE void               mat_movingobj_event_crash_set    ( MatScm             *obj,
                                                             MatScm             *crash );

INLINE void               mat_movingobj_event_shot_set     ( MatScm             *obj,
                                                             MatScm             *crash );

INLINE void               mat_movingobj_update             ( MatScm             *obj );

void                      mat_movingobj_rm_type            ( MatScm             *obj );

/* Remove the object from the list that includes all objects in the universe */
INLINE void               mat_movingobj_rm_all             ( MatScm             *obj );

/* Handle a crash of object a with object b. */
void                      mat_movingobj_crash_handle       ( MatScm             *a,
                                                             MatScm             *b );

#if 0
INLINE MatMovingObjTypeD *mat_movingobj_type_desc_get      ( MatMovingObjType    type );
#endif

INLINE double             mat_movingobj_alpha_get          ( MatScm             *obj );

INLINE void               mat_movingobj_alpha_set          ( MatScm             *obj,
                                                             double              alpha );

INLINE unsigned char     *mat_movingobj_pal_get            ( MatScm             *obj );

INLINE void               mat_movingobj_pal_set            ( MatScm             *obj,
                                                             unsigned char      *pal );

#if 0
INLINE void               mat_movingobj_refs_inc           ( MatMovingObj       *obj );

INLINE void               mat_movingobj_refs_dec           ( MatMovingObj       *obj );
#endif

INLINE void               mat_movingobj_remove             ( MatScm             *obj );

INLINE void               mat_movingobj_free               ( MatScm             *obj );

/* Is the object still inside the lists of the universe? */
INLINE int                mat_movingobj_alive_get          ( MatScm             *obj );

INLINE MatScm            *mat_movingobj_universe_entry_get ( MatScm             *obj );

/* Register that view depends on ship */
MatScm                   *mat_movingobj_view_add           ( MatScm             *ship,
                                                             MatScm             *view );

/* Register that view no longer depends on ship */
void                      mat_movingobj_view_rm            ( MatScm             *ship,
                                                             MatScm             *entry );

INLINE void               mat_movingobj_rm                 ( MatScm             *obj );

INLINE void               mat_movingobj_message_vprintf    ( MatScm             *obj,
                                                             char               *fmt,
                                                             va_list             args );

INLINE void               mat_movingobj_message_printf     ( MatScm             *obj,
                                                             char               *fmt,
                                                             ... );

#endif
