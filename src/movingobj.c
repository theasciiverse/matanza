/* All the physical objects in a given universe are represented by the
 * MatMovingObj structure.  The functions that act on this structure do all the
 * repetitive stuff (such as detect collisions among objects or periodically
 * add their speed to their position).
 *
 * Each instance of the MatMovingObj structure has a type of object that
 * describes the specific details of how the object behaves.  The type is
 * described in instances of the MatMovingObjType structure.  All the types are
 * registered in the mat_movingobj_table below.
 *
 * To create a new type you must add an entry to the mat_movingobj_table array
 * below.
 */

#if 0
@


@
@<Function prototypes@>=
void              mat_alpha_mod_ship_appear                ( MatMovingObj         *s );
void              mat_alpha_mod_ship_disappear             ( MatMovingObj         *s );

@
@<Functions@>=
void
mat_alpha_mod_ship_appear ( MatMovingObj *s )
{
  s->alpha += 0.01;
  if (s->alpha >= 1.0)
    {
      s->alpha_mod = NULL;
      s->crash     = 1;
    }
}

@
@<Functions@>=
void
mat_alpha_mod_ship_disappear ( MatMovingObj *s )
{
  s->alpha -= 0.01;
  if (s->alpha <= 0)
    {
      s->alpha_mod = NULL;
    }
}
#endif

/* --- Code above this line needs a cleanup. ------------------ */

#include <matanza.h>

#define MAT_MOVING_OBJ_MOVE_NORMAL 1
#define MAT_MOVING_OBJ_MOVE_DEAD   2

typedef struct MatMovingObj MatMovingObj;

struct MatMovingObj
{
  /* The universe this object belongs to. */
  MatScm                *un;

  /* An environment that the Scheme code can use to store properties
   * for this ship. */

  MatScm                *env;

  /* The health this obj still has.  The obj dies when it reaches 0. */
  unsigned int           health;

  /* Time to live.  Object will be removed when it gets to 0.  You can
   * set it to 0 directly to prevent the object from being removed. */
  unsigned int           ttl;

#if 0
  /* Is there a player associated to this movingobj? */
  unsigned int           player : 1;
#endif

  /* Is the object still in the list of objects for the universe? */
  unsigned int           alive : 1;

  /* pos_x and pos_y specify the position of the object, relative to the center
   * of the object */
  double                 pos_x;
  double                 pos_y;

  /* sp_x and sp_y hold the speed in the x and y coordinate.  sp holds the
   * composite speed (that is, sp * sp = sp_x * sp_x + sp_y * sp_y). */
  double                 sp_x;
  double                 sp_y;
  double                 sp;

  unsigned char         *pal;

  unsigned int           img;

  /* ang is the current angle of this object, such that 0 <= ang < angles for
   * the universe.  sp_ang is the speed of rotation. */
  double                 ang;
  double                 sp_ang;

  /* The next/previous object of the same type in this universe. */
  MatMovingObj          *nexttype;
  MatMovingObj          *prevtype;

  /* The procedure executed once every turn.  It receives the object as
   * its only argument. */
  MatScm                *update;
  unsigned int           update_wait;

  /* The handler for a crash between this and other objects or NULL if
   * crashes should be ignored */
  MatScm                *crash;

  /* The handler called when this object attempts to shot or NULL if
   * the object is not allowed to shot */
  MatScm                *shot;

  /* This contains the entry associated to this object in the doubly
   * linked list of all the objects in the universe. */

  MatScm                *un_list;

#if 0
  /* The list of behaviours for this object (most of the time empty) */
  MatBehaviour          *behaviour_head;
  MatBehaviour          *behaviour_tail;
#endif

  /* rm_next is used to maintain a single linked list of objects that should be
   * removed.  The last node of the object has rm_next == &main.  This is so we
   * can make assertions on all objects that should not be removed having
   * rm_next == NULL.  That is, to know if an object should be removed,
   * evaluate rm_next != NULL.
   *
   * Also, it is worth noticing that ships are not kept in this list.  Other
   * objects are kept in the objs_rm list for their universe (see the
   * definition of MatUniverse). */

  MatMovingObj          *rm_next;

  /* There are times when it is impossible to free an object because there are
   * references to it in other structs.  Consider, for example, that a player
   * might shot a missile and disconnect.  The missile has a reference to the
   * ship so removing the ship before the missile will probably cause a crash
   * (when the missile hits another player, the reference is followed to find
   * out the name of the player who shot it).  To solve that problem, we use
   * reference counting on the moving objects, only freeing one when it is no
   * longer referenced.
   *
   * The only allowed dependencies of this sort are:
   *
   *   Missile: depends the object that shot it
   *   Universe: the object is on its list of objects
   *   Connection: depends on the object to view things
   *
   * References to the objects in the universe they are in are not counted,
   * since there is no way for an object to exist outside of a universe.
   *
   * On the other hand, references to a ship from a connection should be
   * counted.  This is because a ship may exist after its connection has been
   * gone.
   *
   * Don't forget to set this field to 0 everytime you create a new object. */

  unsigned int           refs;

  /* We want to make it possible to draw objects transparently.  This is used
   * mostly for the times when they are appearing.
   *
   * alpha goes from 0 to 1, 0 meaning the object will be invisible and 1
   * meaning it will be solid.
   *
   * If alpha_mod is not NULL, it will be called once for every turn and is
   * supposed to modify the alpha parameter. */

  double         alpha;
  void         (*alpha_mod) (MatMovingObj *);

#if 0
  /* The subtype of moving object. */
  MatMovingObjType       type;
#endif

  /* The data depends on the type. */
  void                  *data;

  /* We need a way to keep track of what other players are seeing the world as
   * it looks like around any given object.  This is important cause when the
   * object disappears, we need a fast way to know what other players
   * where looking to the world through it and reset their views. */

  MatScm                *view_head;
  MatScm                *view_tail;

  MatScm                *ship;
#if 0
  union
  {
    MatShip        player;
    MatBullet      bullet;
    MatAst         ast;
    MatSmartObj    smart;
  } info;
#endif
};

#if 0
MatMovingObjTypeD *mat_movingobj_table[] =
{
  &mat_ship_type
  /*&mat_ship_type,
  &mat_ship_type,
  &mat_ship_type*/
};
#endif

INLINE static MatMovingObj *data_get ( MatScm *obj );

#if 0
INLINE static double      mat_movingobj_new_spec_sp        ( const char         *sp );

INLINE static double      mat_movingobj_new_spec_sp_ang    ( const char         *sp );

INLINE static double      mat_movingobj_new_spec_pos       ( const char         *pos,
                                                             int                 max );

static void               mat_movingobj_rm_queue_add       ( MatMovingObj       *obj );

static void mat_movingobj_crash_wall_ship ( MatMovingObj *obj );
#endif

static int mat_movingobj_crash_check_bullet_get ( int bullet_pos, int obj_pos, int obj_size, int mapsize );

INLINE MatScm *
mat_movingobj_event_crash_get ( MatScm *obj )
{
  return data_get(obj)->crash;
}

INLINE unsigned int
mat_movingobj_health_get ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);
  return data->health;
}

INLINE void
mat_movingobj_health_set ( MatScm *obj, unsigned int health )
{
  MatMovingObj *data = data_get(obj);
  MatScm *team;

  data->health = health;

  if (health)
    return;

  if (data->ship)
    mat_ship_die(data->ship);
}

#if 0
/* Return the first in the list of behaviours for this object */
INLINE MatBehaviour *
mat_movingobj_behaviour_head_get ( MatMovingObj *obj )
{
  ASSERT(obj);
  return obj->behaviour_head;
}

/* Return the last in the list of behaviours for this object */
INLINE MatBehaviour *
mat_movingobj_behaviour_tail_get ( MatMovingObj *obj )
{
  ASSERT(obj);
  return obj->behaviour_tail;
}
/* Add a behaviour at the end of the list for this object */
INLINE void
mat_movingobj_behaviour_add ( MatMovingObj *obj, MatBehaviour *b )
{
  ASSERT(obj);
  ASSERT(b);

  mat_behaviour_prev_set(b, mat_movingobj_behaviour_tail_get(obj));
  mat_behaviour_next_set(b, NULL);

  if (!mat_movingobj_behaviour_tail_get(obj))
    mat_movingobj_behaviour_head_set(obj, b);
  else
    mat_behaviour_next_set(mat_movingobj_behaviour_tail_get(obj), b);

  mat_movingobj_behaviour_tail_set(obj, b);
}

INLINE void
mat_movingobj_behaviour_head_set ( MatMovingObj *obj, MatBehaviour *b )
{
  ASSERT(obj);
  obj->behaviour_head = b;
}

INLINE void
mat_movingobj_behaviour_tail_set ( MatMovingObj *obj, MatBehaviour *b )
{
  ASSERT(obj);
  obj->behaviour_tail = b;
}

/* Execute all the behaviours for this object */
INLINE void
mat_movingobj_behaviour_exec ( MatMovingObj *obj )
{
  MatBehaviour *b;
  ASSERT(obj);

  for (b = mat_movingobj_behaviour_head_get(obj); b; b = mat_behaviour_next_get(b))
    mat_behaviour_exec(b, obj);
}
#endif

INLINE static MatMovingObj *
data_get ( MatScm *obj )
{
  MatMovingObj *data;

  ASSERT(obj);

  if (mat_scm_obj_type_get(obj) == &mat_scm_ship_type)
    obj = mat_ship_movingobj_get(obj);

  ASSERT(obj);
  ASSERT(mat_scm_obj_type_get(obj) == &mat_scm_movingobj_type);

  data = mat_scm_obj_data_get(obj);

  ASSERT(data);
  ASSERT(!data->ship || mat_scm_obj_type_get(data->ship) == &mat_scm_ship_type);

  return data;
}

INLINE double
mat_movingobj_sp_x_get ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);
  return data->sp_x;
}

INLINE double
mat_movingobj_sp_y_get ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);
  return data->sp_y;
}

INLINE double
mat_movingobj_sp_linear_get ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);
  return data->sp;
}

INLINE double
mat_movingobj_sp_ang_get ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);
  return data->sp_ang;
}

INLINE double
mat_movingobj_pos_x_center_get ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);
  return data->pos_x;
}

INLINE double
mat_movingobj_pos_y_center_get ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);
  return data->pos_y;
}

INLINE double
mat_movingobj_pos_x_ul_get ( MatScm *obj )
{
  double pos;
  double size;

  ASSERT(obj);
  ASSERT(mat_movingobj_universe_get(obj));

  pos = mat_movingobj_pos_x_center_get(obj);

  if (!mat_movingobj_image_get(obj))
    return pos;

  ASSERT(0 <= pos);
  ASSERT(pos < mat_universe_mapsize_x_get(obj));

  size = (double) mat_image_size_x_get(mat_movingobj_image_get(obj)) / 2.0;

  ASSERT(0 <= size);
  ASSERT(size < mat_universe_mapsize_x_get(obj));

  if (pos < size)
    pos += mat_universe_mapsize_x_get(obj);

  pos -= size;

  ASSERT(0 <= pos);
  ASSERT(pos < mat_universe_mapsize_x_get(obj));

  return pos;
}

INLINE double
mat_movingobj_pos_y_ul_get ( MatScm *obj )
{
  double pos;
  double size;

  ASSERT(obj);
  ASSERT(mat_movingobj_universe_get(obj));

  pos  = mat_movingobj_pos_y_center_get(obj);

  if (!mat_movingobj_image_get(obj))
    return pos;

  ASSERT(0 <= pos);
  ASSERT(pos < mat_universe_mapsize_y_get(obj));

  size = (double) mat_image_size_y_get(mat_movingobj_image_get(obj)) / 2.0;

  ASSERT(0 <= size);
  ASSERT(size < mat_universe_mapsize_y_get(obj));

  if (pos < size)
    pos += mat_universe_mapsize_y_get(obj);

  pos -= size;

  ASSERT(0 <= pos);
  ASSERT(pos < mat_universe_mapsize_y_get(obj));

  return pos;
}

MatScm *
mat_movingobj_new ( MatScm *un, MatScm *ship, unsigned int img, unsigned char *pal, unsigned int ttl, unsigned int health )
{
  MatMovingObj       *obj;
  MatScm             *objscm;

  MatMovingObj      **list_head;
  MatMovingObj      **list_tail;

  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universe_type);
  ASSERT(pal);

  obj = malloc(sizeof(MatMovingObj));
  if (!obj)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  ASSERT(!ship || mat_scm_obj_type_get(ship) == &mat_scm_ship_type);

  obj->env                  = mat_scm_env_malloc();
  obj->ship                 = ship;

  obj->rm_next              = NULL;

  obj->refs                 = 0;

  obj->alive                = 1;

  obj->view_head            = NULL;
  obj->view_tail            = NULL;

  obj->health               = health;

  obj->alpha                = 1.0;
  obj->alpha_mod            = NULL;

  obj->un                   = un;

  obj->img                  = img;

  obj->update               = NULL;
  obj->crash                = NULL;
  obj->shot                 = NULL;

#if 0
  obj->next                 = NULL;
  obj->prev                 = mat_universe_obj_tail_all_get(un);

  if (obj->prev)
    obj->prev->next = obj;
  else
    mat_universe_obj_head_all_set(un, obj);

  mat_universe_obj_tail_all_set(un, obj);
#endif

#if 0
  list_head = mat_universe_obj_head_get(un, type);
  list_tail = mat_universe_obj_tail_get(un, type);

  ASSERT(list_head);
  ASSERT(list_tail);

  obj->nexttype             = *list_head;
  obj->prevtype             = NULL;
#endif

#if 0
  obj->behaviour_head       = NULL;
  obj->behaviour_tail       = NULL;
#endif

#if 0
  if (*list_head)
    (*list_head)->prevtype = obj;
  else
    *list_tail = obj;

  *list_head = obj;
#endif

#if 0
  (*mat_universe_obj_real_get(un, type)) ++;
#endif

#if 0
  obj->ang                  = 0;
  obj->sp_ang               = 0;

  obj->sp_x = sp_x;
  obj->sp_y = sp_y;
  obj->sp   = sqrt(obj->sp_x * obj->sp_y);

  obj->pos_x = pos_x;
  obj->pos_y = pos_y;
#endif

  //obj->data = data;

  objscm = mat_scm_obj_malloc(&mat_scm_movingobj_type, obj);

  mat_movingobj_pal_set(objscm, pal);

  obj->un_list = mat_universe_movingobj_add(un, objscm);

  ASSERT(obj->un);
  ASSERT(obj->pal);

  return objscm;
}

void
mat_movingobj_move ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);

  ASSERT(data->un);

  ASSERT(- (double) mat_universe_mapsize_x_get(data->un) < data->sp_x);
  ASSERT(data->sp_x < (double) mat_universe_mapsize_x_get(data->un));
  ASSERT(data->pos_x >= 0.0);
  ASSERT(data->pos_x < (double) mat_universe_mapsize_x_get(data->un));

  ASSERT(- (double) mat_universe_mapsize_y_get(data->un) < data->sp_y);
  ASSERT(data->sp_y < (double) mat_universe_mapsize_y_get(data->un));
  ASSERT(data->pos_y >= 0.0);
  ASSERT(data->pos_y < (double) mat_universe_mapsize_y_get(data->un));

  data->pos_x += data->sp_x;
  data->pos_y += data->sp_y;

  ASSERT(- (double) mat_universe_mapsize_x_get(data->un) <= data->pos_x);
  ASSERT(data->pos_x < 2.0 * (double) mat_universe_mapsize_x_get(data->un));

  ASSERT(- (double) mat_universe_mapsize_y_get(data->un) <= data->pos_y);
  ASSERT(data->pos_y < 2.0 * (double) mat_universe_mapsize_y_get(data->un));

  if (data->pos_x < 0.0)
    {
      if (!arg_limits || mat_movingobj_ship_get(obj))
        {
          data->pos_x += (double) mat_universe_mapsize_x_get(data->un);
          ASSERT(0.0 <= data->pos_x);
        }
      else
        {
          data->pos_x = 0.0;
          data->sp_x  = 0.0;
        }
    }
  else if (data->pos_x >= (double) mat_universe_mapsize_x_get(data->un))
    {
      if (!arg_limits || mat_movingobj_ship_get(obj))
        {
          data->pos_x -= (double) mat_universe_mapsize_x_get(data->un);
          ASSERT(data->pos_x < (double) mat_universe_mapsize_x_get(data->un));
        }
      else
        {
          data->pos_x = (double) mat_universe_mapsize_x_get(data->un) - 1;
          data->sp_x  = 0.0;
        }
    }

  if (data->pos_y < 0.0)
    {
      if (!arg_limits || mat_movingobj_ship_get(obj))
        {
          data->pos_y += (double) mat_universe_mapsize_y_get(data->un);
          ASSERT(0.0 <= data->pos_y);
        }
      else
        {
          data->pos_y = 0.0;
          data->sp_y  = 0.0;
        }
    }
  else if (data->pos_y >= (double) mat_universe_mapsize_y_get(data->un))
    {
      if (!arg_limits || mat_movingobj_ship_get(obj))
        {
          data->pos_y -= (double) mat_universe_mapsize_y_get(data->un);
          ASSERT(data->pos_y < (double) mat_universe_mapsize_y_get(data->un));
        }
      else
        {
          data->pos_y = (double) mat_universe_mapsize_y_get(data->un);
          data->sp_y  = 0.0;
        }
    }

  ASSERT(0.0 <= data->pos_x);
  ASSERT(data->pos_x < (double) mat_universe_mapsize_x_get(data->un));

  ASSERT(0.0 <= data->pos_y);
  ASSERT(data->pos_y < (double) mat_universe_mapsize_y_get(data->un));

  ASSERT(0 <= (int) data->ang);
  ASSERT((int) data->ang < mat_universe_angles_get(data->un));

  ASSERT(- (int) mat_universe_angles_get(data->un) < (int) data->sp_ang);
  ASSERT((int) data->sp_ang < (int) mat_universe_angles_get(data->un));

  data->ang += data->sp_ang;
  if ((int) mat_universe_angles_get(data->un) <= (int) data->ang)
    data->ang -= (double) mat_universe_angles_get(data->un);
  else if ((int) data->ang < 0)
    data->ang += (double) mat_universe_angles_get(data->un);

  ASSERT(0 <= (int) data->ang);
  ASSERT((int) data->ang < (int) mat_universe_angles_get(data->un));

  if (!arg_space && data->sp_ang != 0.0 && mat_movingobj_ship_get(obj))
    {
      data->sp_x = (double) data->sp * mat_universe_cos_get(data->un, (int) data->ang);
      data->sp_y = (double) data->sp * mat_universe_sin_get(data->un, (int) data->ang);
    }

  ASSERT((int) data->ang >= 0);
  ASSERT((int) data->ang < (int) mat_universe_angles_get(data->un));
}

INLINE void
mat_movingobj_sp_calc ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);
  data->sp = sqrt(data->sp_x * data->sp_x + data->sp_y * data->sp_y);
}

INLINE void
mat_movingobj_sp_x_set ( MatScm *obj, double sp )
{
  MatMovingObj *data = data_get(obj);
  data->sp_x = sp;
}

INLINE void
mat_movingobj_sp_y_set ( MatScm *obj, double sp )
{
  MatMovingObj *data = data_get(obj);
  data->sp_y = sp;
}

INLINE void
mat_movingobj_sp_linear_set ( MatScm *obj, double spx, double spy )
{
  mat_movingobj_sp_x_set(obj, spx);
  mat_movingobj_sp_y_set(obj, spy);
  mat_movingobj_sp_calc(obj);
}

INLINE void
mat_movingobj_sp_linear_real_set ( MatScm *obj, double spx, double spy, double sp )
{
  MatMovingObj *data = data_get(obj);
  mat_movingobj_sp_x_set(obj, spx);
  mat_movingobj_sp_y_set(obj, spy);
  data->sp = sp;
}

INLINE void
mat_movingobj_pos_x_set ( MatScm *obj, double pos_x )
{
  MatMovingObj *data = data_get(obj);

  ASSERT(0 <= (int) pos_x);
  ASSERT((int) pos_x < mat_universe_mapsize_x_get(obj));

  data->pos_x = pos_x;
}

INLINE void
mat_movingobj_pos_y_set ( MatScm *obj, double pos_y )
{
  MatMovingObj *data = data_get(obj);

  ASSERT(0 <= (int) pos_y);
  ASSERT((int) pos_y < mat_universe_mapsize_y_get(obj));

  data->pos_y = pos_y;
}

INLINE void
mat_movingobj_pos_set ( MatScm *obj, double pos_x, double pos_y )
{
  mat_movingobj_pos_x_set(obj, pos_x);
  mat_movingobj_pos_y_set(obj, pos_y);
}

INLINE void
mat_movingobj_ang_set ( MatScm *obj, unsigned int ang )
{
  MatMovingObj *data = data_get(obj);
  ASSERT(ang < mat_universe_angles_get(obj));
  data->ang = ang;
}

INLINE void
mat_movingobj_sp_ang_set ( MatScm *obj, double sp )
{
  MatMovingObj *data = data_get(obj);
  data->sp_ang = sp;
}

void
mat_movingobj_crash_wall ( MatScm *obj )
{
#if 0
  switch (obj->type)
    {
    case MAT_MOVINGOBJ_TYPE_BULLET:
      /* Remove the bullet and go on. */
      obj->info.bullet.expires = 0;
      break;

    case MAT_MOVINGOBJ_TYPE_SHIP:
      mat_movingobj_crash_wall_ship(obj);
      break;

#if 0
    case MAT_MOVINGOBJ_TYPE_AST:
      obj->sp_x = -obj->sp_x;
      obj->sp_y = -obj->sp_y;
      /* TODO: If !arg_space, fix the angle. */
      break;
#endif

    case MAT_MOVINGOBJ_TYPE_SMART:
      break;
    }
#endif
}

#if 0
static void
mat_movingobj_crash_wall_ship ( MatMovingObj *obj )
{
  /* TODO: Finish this algorithm. */

  /*
   * +-+-+
   * |0|1|
   * +-+-+
   * |*|2|
   * +-+-+
   *
   * * is where the crash took place.
   */


  int x0, y0, x1, y1, x2, y2;
  double spx, spy;

  spx = obj->sp_x;
  spy = obj->sp_y;

  if (spx > 0)
    {
      if (spy > 0)
        {
          x0 = crash_x - 1;
          y0 = crash_y;
          x1 = crash_x - 1;
          y1 = crash_y - 1;
          x2 = crash_x;
          y2 = crash_y - 1;
        }
      else
        {
          x0 = crash_x;
          y0 = crash_y + 1;
          x1 = crash_x - 1;
          y1 = crash_y + 1;
          x2 = crash_x - 1;
          y2 = crash_y;
        }
    }
  else
    {
      if (spy > 0)
        {
          x0 = crash_x;
          y0 = crash_y - 1;
          x1 = crash_x + 1;
          y1 = crash_y - 1;
          x2 = crash_x + 1;
          y2 = crash_y;
        }
      else
        {
          x0 = crash_x + 1;
          y0 = crash_y;
          x1 = crash_x + 1;
          y1 = crash_y + 1;
          x2 = crash_x;
          y2 = crash_y + 1;
        }
    }
}
#endif

#if 0
static INLINE void
mat_movingobj_rm_queue_add ( MatMovingObj *obj )
{
  MatMovingObj **objs_rm;

  objs_rm = mat_universe_objs_rm_get(obj->un);

  obj->rm_next = *objs_rm;
  *objs_rm = obj;
}

INLINE int
mat_movingobj_rm_queue_get ( MatMovingObj *obj )
{
  return obj->rm_next != NULL;
}

INLINE MatMovingObj *
mat_movingobj_rm_queue_next_get ( MatMovingObj *obj )
{
  return obj->rm_next;
}
#endif

#if 0
MatMovingObj *
mat_movingobj_prev_type_get ( MatMovingObj *obj )
{
  return obj->prevtype;
}

INLINE MatMovingObj *
mat_movingobj_prev_get ( MatMovingObj *obj )
{
  return obj->prev;
}
MatMovingObj *
mat_movingobj_next_type_get ( MatMovingObj *obj )
{
  return obj->nexttype;
}

INLINE MatMovingObj *
mat_movingobj_next_get ( MatMovingObj *obj )
{
  return obj->next;
}

INLINE MatMovingObjType
mat_movingobj_type_get ( MatMovingObj *obj )
{
  return obj->type;
}
#endif

int
mat_movingobj_crash_check_get ( MatScm *a, MatScm *b )
{
  MatScm *aimg, *bimg;

  ASSERT(mat_movingobj_universe_get(a) == mat_movingobj_universe_get(b));
  ASSERT(mat_movingobj_event_crash_get(a) || mat_movingobj_event_crash_get(b));

  aimg = mat_movingobj_image_get(a);
  bimg = mat_movingobj_image_get(b);

  ASSERT(!aimg || aimg->type == &mat_scm_image_type);
  ASSERT(!bimg || bimg->type == &mat_scm_image_type);

  if (aimg && bimg)
    {
      int aw, ah, bw, bh, mx, my, ax, ay, bx, by;

      ax = mat_movingobj_pos_x_ul_get(a);
      ay = mat_movingobj_pos_y_ul_get(a);
                                      
      bx = mat_movingobj_pos_x_ul_get(b);
      by = mat_movingobj_pos_y_ul_get(b);

      ASSERT((int)ax >= 0);
      ASSERT((int)ax < mat_universe_mapsize_x_get(a));

      ASSERT((int)ay >= 0);
      ASSERT((int)ay < mat_universe_mapsize_y_get(a));

      ASSERT((int)bx >= 0);
      ASSERT((int)bx < mat_universe_mapsize_x_get(b));

      ASSERT((int)by >= 0);
      ASSERT((int)by < mat_universe_mapsize_y_get(b));

      aw = mat_image_size_x_get(aimg);
      ah = mat_image_size_y_get(aimg);

      bw = mat_image_size_x_get(bimg);
      bh = mat_image_size_y_get(bimg);

      mx = mat_universe_mapsize_x_get(a);
      my = mat_universe_mapsize_y_get(a);

      if ((   (   (ax >= bx && ax < bx + bw) || (bx >= ax && bx < ax + aw)
               || (       mx + ax < bx + bw) || (       mx + bx < ax + aw) )
           && (   (ay >= by && ay < by + bh) || (by >= ay && by < ay + ah)
               || (       my + ay < by + ah) || (       my + by < ay + bh) ) ))
        {
          int xmod, ymod, xbeg, xend, ybeg, yend, xtmp, ytmp;

          xmod = bx - ax;
          if      (xmod > 0 && xmod >  mx / 2)
            xmod =  mx - xmod;
          else if (xmod < 0 && xmod < -mx / 2)
            xmod = -mx - xmod;

          ASSERT(ABS(xmod) < aw || ABS(xmod) < bw);

          ymod = by - ay;
          if      (ymod > 0 && ymod >  my / 2)
            ymod =  my - ymod;
          else if (ymod < 0 && ymod < -my / 2)
            ymod = -my - ymod;

          ASSERT(ABS(ymod) < ah || ABS(ymod) < bh);

          xbeg = MAX(0, xmod);
          xend = MIN(aw, xmod + bw);

          ybeg = MAX(0, ymod);
          yend = MIN(ah, ymod + bh);

          for (ytmp = ybeg; ytmp < yend; ytmp ++)
            for (xtmp = xbeg; xtmp < xend; xtmp ++)
              {
                ASSERT(xtmp < aw);
                ASSERT(ytmp < ah);
                ASSERT(xtmp - xmod < bw);
                ASSERT(ytmp - ymod < bh);

                if (   mat_image_core_get(aimg, xtmp       , ytmp       ) > 0
                    && mat_image_core_get(bimg, xtmp - xmod, ytmp - ymod) > 0)
                  {
                    return 1;
                    xtmp = xend;
                    ytmp = yend;
                  }
              }
        }
    }
  else if (aimg || bimg)
    {
      MatScm *src, *bul;
      int posx, posy;
      MatScm *srcimg;

      if (aimg)
        {
          src = a;
          bul = b;
          srcimg = aimg;
        }
      else
        {
          src = b;
          bul = a;
          srcimg = bimg;
        }

      posx = mat_movingobj_crash_check_bullet_get(mat_movingobj_pos_x_ul_get(bul), mat_movingobj_pos_x_ul_get(src), mat_image_size_x_get(srcimg), mat_universe_mapsize_x_get(src));

      if (posx != -1)
        {
          posy = mat_movingobj_crash_check_bullet_get(mat_movingobj_pos_y_ul_get(bul), mat_movingobj_pos_y_ul_get(src), mat_image_size_y_get(srcimg), mat_universe_mapsize_y_get(src));

          if (posy != -1)
            return mat_image_core_get(srcimg, posx, posy) > 0;
        }
    }

  return 0;
}

/* mat_movingobj_crash_check_bullet takes as arguments the bullet's position
 * (bullet_pos), the object's position (obj_pos), the object's size (obj_size)
 * and the map's size (mapsize) and checks to see if the bullet is close to the
 * object.  If it isn't, -1 is returned.  If it is, the coordinate the bullet
 * is on inside the object's image is returned. */

static int
mat_movingobj_crash_check_bullet_get ( int bullet_pos, int obj_pos, int obj_size, int mapsize )
{
  int c;

  c = bullet_pos - obj_pos;  

  if      (0 <= c &&         c < obj_size)
    ;
  else if (        mapsize + c < obj_size)
    c = c + mapsize;
  else
    c = -1;

  ASSERT(c == -1 || (c >= 0 && c < obj_size));

  return c;
}

INLINE void
mat_movingobj_event_update_set ( MatScm *obj, MatScm *proc )
{
  MatMovingObj *data = data_get(obj);

  data->update = mat_scm_cons_new(proc, NULL);

  /* We use a random value or else lots of objects would end up being updated
   * in the same tick. */

  data->update_wait = (10 * (double) rand() / (double) RAND_MAX) ;
}

INLINE void
mat_movingobj_event_crash_set ( MatScm *obj, MatScm *proc )
{
  data_get(obj)->crash = proc;
}

INLINE void
mat_movingobj_event_shot_set ( MatScm *obj, MatScm *proc )
{
  data_get(obj)->shot = proc;
}

INLINE void
mat_movingobj_update ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);

  if (data->ttl == 1)
    mat_movingobj_rm(obj);
  else if (data->ttl)
    data->ttl --;

  if (data->update)
    {
      if (data->update_wait > mat_arg_update_delay)
        {
          data->update_wait = 0;
          mat_scm_obj_eval(data->update, NULL);
        }
      else
        data->update_wait ++;
    }

  if (data->ship)
    mat_ship_update(data->ship);

#if 0
  if (!mat_movingobj_table[mat_movingobj_type_get(obj)]->update)
    return;

  mat_movingobj_table[mat_movingobj_type_get(obj)]->update(mat_movingobj_data_get(obj));
#endif

#if 0
  switch (obj->type)
    {
    case MAT_MOVING_OBJ_PLAYER:
      if (obj->info.player.bullets_expire > 0)
        obj->info.player.bullets_expire--;

      if (obj->info.player.ship_speed_inc > 0)
        obj->info.player.ship_speed_inc--;

      if (obj->alpha_mod)
        obj->alpha_mod(obj);

      if (!obj->info.player.visible && obj->info.player.visible_exp -- <= 0)
        {
          obj->info.player.visible = 1;
          VISIBILITY_BROADCAST(obj);
        }
      break;
    case MAT_MOVING_OBJ_BULLET:
      ASSERT(b->info.bullet.expires >= 0);
      if (b->info.bullet.expires -- == 0)
        {
          mat_movingobj_rm_queue(b);

          ASSERT(b->info.bullet.src);
          ASSERT(b->info.bullet.src->refs > 0);

          mat_movingobj_unref(b->info.bullet.src);
        }
      break;
    case MAT_MOVING_OBJ_SMART:
      ASSERT(obj->info.smart.type >= 0);
      ASSERT(obj->info.smart.type < sizeof(mat_objects) / sizeof(MatSmartObjType));

      if (mat_objects[obj->info.smart.type].move)
        mat_objects[obj->info.smart.type].move(obj);
    }
#endif
}

INLINE MatScm *
mat_movingobj_image_get ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);

  ASSERT(obj);
  ASSERT(mat_movingobj_universe_get(obj));
  ASSERT(mat_universe_image_get(obj, data->img));
  ASSERT(mat_universe_image_get(obj, data->img)->type == &mat_scm_imagevector_type);

  return mat_imagevector_get(mat_universe_image_get(obj, data->img), mat_movingobj_ang_get(obj));
}

#if 0
void
mat_movingobj_rm_type ( MatMovingObj *obj )
{
  MatMovingObj **list_head, **list_tail;

  list_head = mat_universe_obj_head_get(obj->un, obj->type);
  list_tail = mat_universe_obj_tail_get(obj->un, obj->type);

  if (obj->nexttype)
    obj->nexttype->prevtype = obj->prevtype;
  else
    *list_tail = obj->prevtype;

  if (obj->prevtype)
    obj->prevtype->nexttype = obj->nexttype;
  else
    *list_head = obj->nexttype;
}
#endif

#if 0
INLINE void
mat_movingobj_rm_all ( MatMovingObj *obj )
{
  MatUniverse *un;

  ASSERT(obj);
  ASSERT(mat_scm_obj_type_get(obj) == &mat_scm_universe_type);

  un = mat_scm_obj_data_get(obj);

  ASSERT(un);

  if (obj->prev)
    {
      ASSERT(obj != mat_universe_obj_head_all_get(un));
      obj->prev->next = obj->next;
    }
  else
    {
      ASSERT(obj == mat_universe_obj_head_all_get(un));
      mat_universe_obj_head_all_set(un, obj->next);
    }

  if (obj->next)
    {
      ASSERT(obj != mat_universe_obj_tail_all_get(un));
      obj->next->prev = obj->prev;
    }
  else
    {
      ASSERT(obj == mat_universe_obj_tail_all_get(un));
      mat_universe_obj_tail_all_set(un, obj->prev);
    }
}
#endif

void
mat_movingobj_crash_handle ( MatScm *a, MatScm *b )
{
  MatMovingObj *data;
  MatScm *args;

  ASSERT(mat_movingobj_universe_get(a) == mat_movingobj_universe_get(b));

  ASSERT(mat_movingobj_event_crash_get(a) || mat_movingobj_event_crash_get(b));

  args = mat_scm_cons_new(NULL, NULL);

  data = data_get(a);
  if (data->crash)
    {
      mat_scm_cons_car_set(args, b);
      mat_scm_obj_call(data->crash, args, NULL, 0);
    }

  data = data_get(b);
  if (data->crash)
    {
      mat_scm_cons_car_set(args, a);
      mat_scm_obj_call(data->crash, args, NULL, 0);
    }
}

INLINE MatScm *
mat_movingobj_universe_get ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);

  ASSERT(data->un);
  ASSERT(mat_scm_obj_type_get(data->un) == &mat_scm_universe_type);

  return data->un;
}

INLINE void *
mat_movingobj_data_get ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);
  ASSERT(data->data);
  return data->data;
}

INLINE double
mat_movingobj_ang_get ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);
  ASSERT(data);
  ASSERT((int) data->ang < mat_universe_angles_get(obj));
  return data->ang;
}

#if 0
INLINE MatMovingObj *
mat_movingobj_new_spec ( MatUniverse *un, char **atts )
{
  char *type_spec = NULL;
  double pos_x = 0, pos_y = 0, sp_x = 0, sp_y = 0, sp_ang = 0;
  unsigned int i;
  MatMovingObjType type;
  MatMovingObj *obj;

  for (i = 0; atts[i]; i += 2)
    if (!strcmp(atts[i], "type"))
      type_spec = atts[i + 1];
    else if (!strcmp(atts[i], "posx"))
      pos_x = mat_movingobj_new_spec_pos(atts[i + 1], mat_universe_mapsize_x_get(un));
    else if (!strcmp(atts[i], "posy"))
      pos_y = mat_movingobj_new_spec_pos(atts[i + 1], mat_universe_mapsize_y_get(un));
    else if (!strcmp(atts[i], "spx"))
      sp_x = mat_movingobj_new_spec_sp(atts[i + 1]);
    else if (!strcmp(atts[i], "spy"))
      sp_y = mat_movingobj_new_spec_sp(atts[i + 1]);
    else if (!strcmp(atts[i], "spang"))
      sp_ang = mat_movingobj_new_spec_sp_ang(atts[i + 1]);
#if 0
    else if (!strcmp(atts[i], "health"))
      obj->info.smart.health = atoi(atts[i + 1]);
#endif
    else
      {
        fprintf(stderr, "%s: %s: Unknown attribute for <Object>\n", program_name, atts[i]);
        exit(EXIT_FAILURE);
      }

  if (!type_spec)
    {
      fprintf(stderr, "%s: Missing type of object\n", program_name);
      exit(EXIT_FAILURE);
    }

  for (type = 0; strcmp(type_spec, mat_movingobj_table[type]->name); type ++)
    if (type == sizeof(mat_movingobj_table) / sizeof(mat_movingobj_table[0]) - 1)
      {
        fprintf(stderr, "%s: %s: Unknown object type\n", program_name, type_spec);
        exit(EXIT_FAILURE);
      }

  ASSERT(mat_movingobj_table[type]);
  ASSERT(mat_movingobj_table[type]->create);

  obj = mat_movingobj_table[type]->create(un, atts);

  mat_movingobj_pos_set(obj, pos_x, pos_y);
  mat_movingobj_sp_ang_set(obj, sp_ang);
  mat_movingobj_sp_linear_set(obj, sp_x, sp_y);

  return obj;
}

INLINE static double
mat_movingobj_new_spec_pos ( const char *pos, int max )
{
  ASSERT(max > 0);

  if (!strcmp(pos, "random"))
    return ((double) max * rand() / (double) RAND_MAX);
  else
    return (double) atof(pos);
}

INLINE static double
mat_movingobj_new_spec_sp ( const char *sp )
{
  if (!strcmp(sp, "random"))
    return 0.5 - (1.0 * (double) rand() / (double) RAND_MAX);
  else
    return (double) atof(sp);
}

INLINE static double
mat_movingobj_new_spec_sp_ang ( const char *sp )
{
  if (!strcmp(sp, "random"))
    return (0.5 - (1.0 * (double) rand() / (double) RAND_MAX)) / arg_universe_update_steps;
  else
    return (double) atof(sp);
}
#endif

#if 0
INLINE MatMovingObjTypeD *
mat_movingobj_type_desc_get ( MatMovingObjType type )
{
  ASSERT(type < sizeof(mat_movingobj_table) / sizeof(MatMovingObjTypeD *));
  return mat_movingobj_table[type];
}
#endif

#if 0
INLINE void
mat_movingobj_image_set ( MatMovingObj *obj, unsigned int id )
{
  ASSERT(obj);

  obj->img_id = id;
  obj->img = mat_universe_image_get(obj, id);
}

INLINE unsigned int
mat_movingobj_image_id_get ( MatMovingObj *obj )
{
  ASSERT(obj);
  return obj->img_id;
}
#endif

INLINE double
mat_movingobj_alpha_get ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);

  ASSERT(0.0 <= data->alpha);
  ASSERT(data->alpha <= 1.0);

  return data->alpha;
}

INLINE void
mat_movingobj_alpha_set ( MatScm *obj, double alpha )
{
  MatMovingObj *data = data_get(obj);

  ASSERT(0.0 <= alpha);
  ASSERT(alpha <= 1.0);

  data->alpha = alpha;
}

INLINE unsigned char *
mat_movingobj_pal_get ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);

  ASSERT(data);
  ASSERT(data->pal);

  return data->pal;
}

INLINE void
mat_movingobj_pal_set ( MatScm *obj, unsigned char *pal )
{
  MatMovingObj *data = data_get(obj);

  ASSERT(pal);

  data->pal = pal;
}

#if 0
INLINE void
mat_movingobj_refs_inc ( MatMovingObj *obj )
{
  ASSERT(obj);
  obj->refs ++;
}

INLINE void
mat_movingobj_refs_dec ( MatMovingObj *obj )
{
  ASSERT(obj);

  if (!obj->refs --)
    mat_movingobj_rm_queue_add(obj);
}
#endif

INLINE void
mat_movingobj_rm ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);
  MatScm *view;

  ASSERT(data->un_list);

  mat_universe_movingobj_rm(obj, data->un_list);

  data->un_list = NULL;

  for (view = data->view_head; view; view = mat_dlist_next_get(view))
    mat_widget_play_view_reset(mat_dlist_obj_get(view));

  ASSERT(data->alive);

  data->crash = NULL;
  data->alive = 0;
}

INLINE int
mat_movingobj_alive_get ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);
  return data->alive;
}

INLINE MatScm *
mat_movingobj_env_get ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);
  return data->env;
}

INLINE MatScm *
mat_movingobj_universe_entry_get ( MatScm *obj )
{
  MatMovingObj *data = data_get(obj);
  return data->un_list;
}

MatScm *
mat_movingobj_view_add ( MatScm *obj, MatScm *view )
{
  MatMovingObj *data = data_get(obj);
  return mat_dlist_add_head(&data->view_head, &data->view_tail, view);
}

void
mat_movingobj_view_rm ( MatScm *obj, MatScm *entry )
{
  MatMovingObj *data = data_get(obj);
  return mat_dlist_rm(&data->view_head, &data->view_tail, entry);
}

#if 0
INLINE MatScm *
mat_ship_machine_hash_next_get ( MatShip *ship )
{
  ASSERT(ship);
  return ship->conn_hash_next;
}

INLINE void
mat_ship_machine_hash_next_set ( MatShip *ship, MatShip *next )
{
  ASSERT(ship);
  ship->conn_hash_next = next;
}

INLINE MatShip *
mat_ship_machine_hash_prev_get ( MatShip *ship )
{
  ASSERT(ship);
  return ship->conn_hash_prev;
}

INLINE void
mat_ship_machine_hash_prev_set ( MatShip *ship, MatShip *prev )
{
  ASSERT(ship);
  ship->conn_hash_prev = prev;
}

INLINE MatShip *
mat_ship_machine_list_next_get ( MatShip *ship )
{
  ASSERT(ship);
  return ship->conn_list_next;
}

INLINE void
mat_ship_machine_list_next_set ( MatShip *ship, MatShip *next )
{
  ASSERT(ship);
  ship->conn_list_next = next;
}

INLINE MatShip *
mat_ship_machine_list_prev_get ( MatShip *ship )
{
  ASSERT(ship);
  return ship->conn_list_prev;
}

INLINE void
mat_ship_machine_list_prev_set ( MatShip *ship, MatShip *prev )
{
  ASSERT(ship);
  ship->conn_list_prev = prev;
}
#endif

INLINE void
mat_movingobj_mark ( void *funcdata )
{
  MatMovingObj *data = (MatMovingObj *) funcdata;

  mat_scm_obj_mark(data->un);
  mat_scm_obj_mark(data->env);
  mat_scm_obj_mark(data->update);
  mat_scm_obj_mark(data->crash);
  mat_scm_obj_mark(data->un_list);
  mat_scm_obj_mark(data->view_head);
  mat_scm_obj_mark(data->view_tail);
  mat_scm_obj_mark(data->ship);
}

INLINE MatScm *
mat_movingobj_ship_get ( MatScm *obj )
{
  return data_get(obj)->ship;
}

INLINE void
mat_movingobj_message_vprintf ( MatScm *obj, char *fmt, va_list args )
{
  if (!mat_movingobj_ship_get(obj))
    return;

  mat_ship_message_vprintf(mat_movingobj_ship_get(obj), fmt, args);
}

INLINE void
mat_movingobj_message_printf ( MatScm *obj, char *fmt, ... )
{
  va_list args;

  if (!mat_movingobj_ship_get(obj))
    return;

  va_start(args, fmt);
  mat_ship_message_vprintf(mat_movingobj_ship_get(obj), fmt, args);
  va_end(args);
}

INLINE void
mat_movingobj_shot ( MatScm *src, MatMissileSize size, MatScm *missile )
{
  MatScm *args;
  if (!data_get(src)->shot)
    return;
  args = mat_scm_cons_new(mat_scm_number_new(size), mat_scm_cons_new(missile, NULL));
  mat_scm_obj_call(data_get(src)->shot, args, NULL, 0);
}

INLINE unsigned int
mat_movingobj_ttl_get ( MatScm *obj )
{
  return data_get(obj)->ttl;
}

INLINE void
mat_movingobj_ttl_set ( MatScm *obj, unsigned int ttl )
{
  data_get(obj)->ttl = ttl;
}
