#include <matanza.h>

/* players_head and players_tail hold a list with all the players in this
 * universe.
 *
 * players_real is the number of players currently playing (in
 * MAT_STATE_PLAYING state) in this universe. */

struct MatUniverse
{
  unsigned int         id;

  MatScm              *type;

  /* The list with all the objects */
  MatScm              *obj_head;
  MatScm              *obj_tail;

#if 0
  MatMovingObj        *obj_head[MAT_MOVINGOBJ_TYPE_COUNT];
  MatMovingObj        *obj_tail[MAT_MOVINGOBJ_TYPE_COUNT];
#endif

  /* The list of teams */
  MatScm              *team_head;
  MatScm              *team_tail;

#if 0
  MatMovingObj        *obj_head_all;
  MatMovingObj        *obj_tail_all;
#endif

  /* Counts the number of registered objects for every given type. */
#if 0
  int                  obj_real[MAT_MOVINGOBJ_TYPE_COUNT];
#endif

  MatSub              *subs;

  unsigned int         ship_dmg;
  unsigned int         asteroid_dmg;

  /* String with the universe's name */
  MatScm              *name;

  int                  pause;

  /* Should we draw dots for this universe (background)? */
  int                  dots;

#if 0
  /* Single linked list of all the objects (other than ships) in this universe
   * that should be removed as soon as possible.  To traverse the list, follow
   * the |rm_next| field of every node.  The last node in the list has
   * |rm_next| set to MAT_UNIVERSE_OBJS_RM_END, not to NULL!
   *
   * Consult the documentation next ro |rm_next| in the |MatMovingObj| struct.
   *
   * Also, keep in mind that ships (objects of type |MAT_MOVINGOBJ_TYPE_SHIP|)
   * should not be added to this list... WHY NOT?
   */

  MatMovingObj        *objs_rm;
#endif
};

static INLINE MatUniverse *data_get ( MatScm *un );
static void mat_universe_update_objs ( MatScm *un );
static void mat_universe_update_objs_move ( MatScm *un );
static void mat_universe_update_objs_crash ( MatScm *un, MatScm *obj );
#if 0
static void mat_universe_update_objs_rm ( MatUniverse *un );
#endif

MatScm           *mat_universe_head = NULL;
MatScm           *mat_universe_tail = NULL;

static MatScm    *mat_universe_hash[MAT_UNIVERSES_HASH_SIZE];

int               mat_universe_count = 0;

static INLINE MatUniverse *
data_get ( MatScm *un )
{
  MatUniverse *data;

  ASSERT(un);

  if (mat_scm_obj_type_get(un) == &mat_scm_ship_type)
    un = mat_ship_movingobj_get(un);
  if (mat_scm_obj_type_get(un) == &mat_scm_movingobj_type)
    un = mat_movingobj_universe_get(un);

  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universe_type);

  data = mat_scm_obj_data_get(un);

  ASSERT(data);
  ASSERT(data->type);
  ASSERT(mat_scm_obj_type_get(data->type) == &mat_scm_universetype_type);
  ASSERT(data->name);
  ASSERT(mat_scm_obj_type_get(data->name) == &mat_scm_string_type);
  ASSERT((!data->obj_head) == (!data->obj_tail));

  return data;
}

INLINE unsigned int
mat_universe_mapsize_x_get ( MatScm *un )
{
  return mat_universetype_mapsize_y_get(mat_universe_universetype_get(un));
}

INLINE unsigned int
mat_universe_mapsize_y_get ( MatScm *un )
{
  return mat_universetype_mapsize_x_get(mat_universe_universetype_get(un));
}

MatScm *
mat_universe_new ( MatScm *type, MatScm *name )
{
  MatUniverse *data;
  MatScm *un;
  MatScm *node;
  int i;

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> mat_universe_new\n");

  ASSERT(type);
  ASSERT(mat_scm_obj_type_get(type) == &mat_scm_universetype_type);

  ASSERT(name);
  ASSERT(mat_scm_obj_type_get(name) == &mat_scm_string_type);

  data = malloc(sizeof(MatUniverse));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->id = mat_universe_count ++;

  data->type = type;
  data->name = name;

#if 0
  data->next_hash = mat_universe[data->id % MAT_UNIVERSES_HASH_SIZE];
  mat_universe[data->id % MAT_UNIVERSES_HASH_SIZE] = data;
#endif

#if 0
  for (i = 0; i < MAT_MOVINGOBJ_TYPE_COUNT; i ++ )
    {
      data->obj_head[i] = data->obj_tail[i] = NULL;
      data->obj_real[i] = 0;
    }
#endif

  data->obj_head = data->obj_tail = NULL;

#if 0
  data->objs_rm = MAT_UNIVERSE_OBJS_RM_END;
#endif

  data->pause = 0;

  data->subs = NULL;

  data->dots   = arg_bg_dots;

  data->ship_dmg      = 200;
  data->asteroid_dmg  = 200;

  data->team_head = data->team_tail = NULL;

#if 0
  data->obj_head_all = data->obj_tail_all = NULL;
#endif

  un = mat_scm_obj_malloc(&mat_scm_universe_type, data);

  /* Add the universe to the hash of universes */

  mat_universe_hash[data->id % MAT_UNIVERSES_HASH_SIZE] = mat_scm_cons_new(un, mat_universe_hash[data->id]);

  /* Add the universe to the list of universes */

  node = mat_scm_cons_new(un, mat_scm_cons_new(mat_universe_tail, NULL));

  if (mat_universe_head)
    {
      ASSERT(mat_universe_tail);
      ASSERT(mat_scm_obj_type_get(mat_universe_tail) == &mat_scm_cons_type);
      ASSERT(mat_scm_cons_cdr_get(mat_universe_tail));
      ASSERT(mat_scm_obj_type_get(mat_scm_cons_cdr_get(mat_universe_tail)) == &mat_scm_cons_type);
      ASSERT(!mat_scm_cons_cdr_get(mat_scm_cons_cdr_get(mat_universe_tail)));

      mat_scm_cons_cdr_set(mat_scm_cons_cdr_get(mat_universe_tail), node);
    }
  else
    {
      ASSERT(!mat_universe_tail);
      mat_universe_head = node;
    }

  mat_universe_tail = node;

  mat_universetype_event_start_run(type, un);

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- mat_universe_new\n");

  return un;
}

#if 0
INLINE void
mat_universe_load ( char *path )
{
  FILE           *in;
  MatUniverse *un;
  int i;

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> mat_universe_load (%s)\n", path);

  un = malloc(sizeof(MatUniverse));
  if (!un)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  un->id = mat_universes_count ++;

  un->next_hash = mat_universes[un->id % MAT_UNIVERSES_HASH_SIZE];
  mat_universes[un->id % MAT_UNIVERSES_HASH_SIZE] = un;

  un->path = path;

  un->obj_head_all = un->obj_tail_all = NULL;

  for (i = 0; i < MAT_MOVINGOBJ_TYPE_COUNT; i ++)
    {
      un->obj_head[i] = un->obj_tail[i] = NULL;
      un->obj_real[i] = 0;
    }

  un->objs_rm = MAT_UNIVERSE_OBJS_RM_END;

  un->pause = 0;

  un->subs = NULL;

  un->shiptype_head = NULL;
  un->shiptype_tail = NULL;

  ASSERT(sizeof("Unnamed Universe") + 1 < MAT_UNIVERSE_NAME_MAX);
  strcpy(un->name, "Unnamed Universe");

  for (i = 0; i < 256; i ++)
    un->location[i] = NULL;

  un->images_count = 0;
  for (i = 0; i < MAT_UNIVERSE_IMAGE_HASH_SIZE; i ++)
    un->images[i] = NULL;

  un->mapsize_x = arg_mapsize_x;
  un->mapsize_y = arg_mapsize_y;

  un->dots      = arg_bg_dots;

  un->un_prev = mat_universe_tail;
  un->un_next = NULL;

  if (mat_universe_tail)
    mat_universe_tail->un_next = un;
  else
    mat_universe_head = un;

  mat_universe_tail = un;

  in = fopen(un->path, "r");
  if (!in)
    {
      fprintf(stderr, "%s: %s: %s\n", program_name, un->path, strerror(errno));
      exit(EXIT_FAILURE);
    }

  mat_usl_load(un, in);

  fclose(in);

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- mat_universe_load\n");
}
#endif

#if 0
TODO: Make sure the images are big enough.

@<Load background images@>=
{
  MatImgWorld *tmp;

  for (tmp = mat_bg_head; tmp; tmp = tmp->next)
    {
      tmp->img = mat_png_load(tmp->path);
      if (arg_bg_color_max != -1)
        {
          int col_min = 0;
          int col_max = arg_bg_color_max;

          MatImage *img = tmp->img;

          @<Adjust the colors in image |img|@>;
        }
    }

  for (tmp = mat_fg_head; tmp; tmp = tmp->next)
    tmp->img = mat_png_load(tmp->path);

  if (arg_bg_size && (mat_bg_head || mat_fg_tail))
    arg_mapsize_x = arg_mapsize_y = 0;

  for (tmp = mat_bg_head; tmp; tmp = tmp->next)
    {
      arg_mapsize_x = MAX(arg_mapsize_x, tmp->img->w);
      arg_mapsize_y = MAX(arg_mapsize_y, tmp->img->h);
    }

  for (tmp = mat_fg_head; tmp; tmp = tmp->next)
    {
      arg_mapsize_x = MAX(arg_mapsize_x, tmp->img->w);
      arg_mapsize_y = MAX(arg_mapsize_y, tmp->img->h);
    }
}
#endif


void
mat_universe_free ( MatScm *un )
{
  MatUniverse *data = data_get(un);
  free(data);
}

void
mat_universe_update ( MatScm *un )
{
  if (mat_universe_pause_get(un) || !mat_universe_movingobj_head_get(un))
    return;

  mat_universe_update_objs(un);
  mat_universe_update_objs_move(un);
  //mat_universe_update_objs_rm(un);
}

static void
mat_universe_update_objs_move ( MatScm *un )
{
  MatScm *node;
  int i;

  ASSERT(un);

  for (i = 0; i < arg_universe_update_steps; i ++)
    for (node = mat_universe_movingobj_head_get(un); node; node = mat_scm_cons_cdr_get(mat_scm_cons_cdr_get(node)))
      {
        MatScm *obj;

        ASSERT(mat_scm_obj_type_get(node) == &mat_scm_cons_type);
        ASSERT(mat_scm_cons_car_get(node));
        ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(node)) == &mat_scm_movingobj_type);
        ASSERT(mat_scm_cons_cdr_get(node));
        ASSERT(mat_scm_obj_type_get(mat_scm_cons_cdr_get(node)) == &mat_scm_cons_type);

        obj = mat_scm_cons_car_get(node);

        if (mat_universetype_mask_crash(mat_universe_universetype_get(un), obj))
          mat_movingobj_crash_wall(obj);
        else
          mat_universe_update_objs_crash(un, node);

        mat_universetype_mask_sp(mat_universe_universetype_get(un), obj);
        mat_movingobj_move(obj);
      }
}

static void
mat_universe_update_objs ( MatScm *un )
{
  MatScm *node;

  for (node = mat_universe_movingobj_head_get(un); node; node = mat_scm_cons_cdr_get(mat_scm_cons_cdr_get(node)))
    {
      ASSERT(mat_scm_obj_type_get(node) == &mat_scm_cons_type);
      ASSERT(mat_scm_cons_car_get(node));
      ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(node)) == &mat_scm_movingobj_type);
      ASSERT(mat_scm_cons_cdr_get(node));
      ASSERT(mat_scm_obj_type_get(mat_scm_cons_cdr_get(node)) == &mat_scm_cons_type);

      mat_movingobj_update(mat_scm_cons_car_get(node));
    }
}

static void
mat_universe_update_objs_crash ( MatScm *un, MatScm *node )
{
  MatScm *i;

  ASSERT(node);
  ASSERT(mat_scm_obj_type_get(node) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(node));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(node)) == &mat_scm_movingobj_type);
  ASSERT(mat_scm_cons_cdr_get(node));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_cdr_get(node)) == &mat_scm_cons_type);

  ASSERT(mat_movingobj_alive_get(mat_scm_cons_car_get(node)));

#if 0
  ASSERT(!mat_movingobj_rm_queue_get(obj));
#endif

  ASSERT(0 <= (int) mat_movingobj_pos_x_ul_get(mat_scm_cons_car_get(node)));
  ASSERT((int) mat_movingobj_pos_x_ul_get(mat_scm_cons_car_get(node)) < mat_universe_mapsize_x_get(un));

  ASSERT(0 <= (int) mat_movingobj_pos_y_ul_get(mat_scm_cons_car_get(node)));
  ASSERT((int) mat_movingobj_pos_y_ul_get(mat_scm_cons_car_get(node)) < mat_universe_mapsize_y_get(un));

  for (i = mat_dlist_next_get(node); i; i = mat_dlist_next_get(i))
    {
      ASSERT(mat_movingobj_alive_get(mat_dlist_obj_get(node)));

#if 0
      ASSERT(!mat_movingobj_rm_queue_get(obj));
#endif

      ASSERT(i);
      ASSERT(mat_scm_obj_type_get(i) == &mat_scm_cons_type);
      ASSERT(mat_scm_cons_car_get(i));
      ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(i)) == &mat_scm_movingobj_type);
      ASSERT(mat_scm_cons_cdr_get(node));
      ASSERT(mat_scm_obj_type_get(mat_scm_cons_cdr_get(node)) == &mat_scm_cons_type);

      if (mat_movingobj_ttl_get(mat_scm_cons_car_get(i)) != 1 && mat_movingobj_ttl_get(mat_scm_cons_car_get(node)) != 1)
        if (mat_movingobj_event_crash_get(mat_scm_cons_car_get(i)) || mat_movingobj_event_crash_get(mat_scm_cons_car_get(node)))
          if (mat_movingobj_crash_check_get(mat_scm_cons_car_get(node), mat_scm_cons_car_get(i)))
            {
              mat_movingobj_crash_handle(mat_scm_cons_car_get(node), mat_scm_cons_car_get(i));
              if (!mat_movingobj_event_crash_get(mat_scm_cons_car_get(node)))
                return;
            }
    }
}

#if 0
/* Remove objects queued for removal. */
static void
mat_universe_update_objs_rm ( MatUniverse *un )
{
  ASSERT(un);

  while (un->objs_rm != MAT_UNIVERSE_OBJS_RM_END)
    {
      MatMovingObj *obj;

      obj = un->objs_rm;

      ASSERT(obj);
      ASSERT(mat_movingobj_universe_get(obj) == un);

      un->objs_rm = mat_movingobj_rm_queue_next_get(obj);

      mat_movingobj_free(obj);
    }
}
#endif

INLINE MatScm *
mat_universe_image_get ( MatScm *un, unsigned int id )
{
  return mat_universetype_image_get(mat_universe_universetype_get(un), id);
}

INLINE unsigned int
mat_universe_image_count_get ( MatScm *un )
{
  ASSERT(un);
  ASSERT(un->type);
  return mat_universetype_image_count_get(mat_universe_universetype_get(un));
}

#if 0
MatUniverse *
mat_universe_hash_next_get ( MatUniverse *un )
{
  return un->next_hash;
}

INLINE int *
mat_universe_obj_real_get ( MatUniverse *un, MatMovingObjType type )
{
  ASSERT(un);
  return &un->obj_real[type];
}

MatMovingObj **
mat_universe_obj_head_get ( MatUniverse *un, MatMovingObjType type )
{
  return &(un->obj_head[type]);
}

MatMovingObj **
mat_universe_obj_tail_get ( MatUniverse *un, MatMovingObjType type )
{
  ASSERT(un);
  return &(un->obj_tail[type]);
}

INLINE MatMovingObj **
mat_universe_objs_rm_get ( MatUniverse *un )
{
  return &un->objs_rm;
}
#endif

INLINE MatSub **
mat_universe_subs_get ( MatScm *un )
{
  MatUniverse *data = data_get(un);
  return &data->subs;
}

INLINE unsigned int
mat_universe_id_get ( MatScm *un )
{
  MatUniverse *data = data_get(un);
  return data->id;
}

INLINE void
mat_universe_message_printf ( MatScm *un, char *fmt, ... )
{
  MatScm *node;
  va_list arg;

  for (node = mat_universe_movingobj_head_get(un); node; node = mat_scm_cons_cdr_get(mat_scm_cons_cdr_get(node)))
    {
      ASSERT(mat_scm_obj_type_get(node) == &mat_scm_cons_type);
      ASSERT(mat_scm_cons_car_get(node));
      ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(node)) == &mat_scm_movingobj_type);

      va_start(arg, fmt);
      mat_movingobj_message_vprintf(mat_scm_cons_car_get(node), fmt, arg);
      va_end(arg);

      ASSERT(mat_scm_cons_cdr_get(node));
      ASSERT(mat_scm_obj_type_get(mat_scm_cons_cdr_get(node)) == &mat_scm_cons_type);
    }
}

unsigned int
mat_universe_angles_get ( MatScm *un )
{
  return mat_universetype_ang_get(mat_universe_universetype_get(un));
}

INLINE double
mat_universe_sin_get ( MatScm *un, unsigned int ang )
{
  return mat_anglecache_sin(mat_universetype_ang_cache_get(mat_universe_universetype_get(un)), ang);
}

INLINE double
mat_universe_cos_get ( MatScm *un, unsigned int ang )
{
  return mat_anglecache_cos(mat_universetype_ang_cache_get(mat_universe_universetype_get(un)), ang);
}

INLINE void
mat_universe_machine_forall ( MatScm *un, void (*func) ( void *, MatMachine * ), void *userdata )
{
#if MAT_BUILD_MMP
  MatUniverse *data = data_get(un);
  MatSub *s;

  for (s = data->subs; s; s = mat_sub_universe_next_get(s))
    {
      ASSERT(mat_sub_universe_get(s) == un);
      func(userdata, mat_sub_machine_get(s));
    }
#endif
}

INLINE void
mat_universe_movingobj_forall ( MatScm *un, void (*func) ( void *, MatScm * ), void *data )
{
  MatScm *node;

  ASSERT(un);

  for (node = mat_universe_movingobj_head_get(un); node; node= mat_scm_cons_cdr_get(mat_scm_cons_cdr_get(node)))
    {
      ASSERT(mat_scm_obj_type_get(node) == &mat_scm_cons_type);
      ASSERT(mat_scm_cons_car_get(node));
      ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(node)) == &mat_scm_movingobj_type);

      func(data, mat_scm_cons_car_get(node));

      ASSERT(mat_scm_cons_cdr_get(node));
      ASSERT(mat_scm_obj_type_get(mat_scm_cons_cdr_get(node)) == &mat_scm_cons_type);
    }
}

INLINE MatScm *
mat_universe_shiptype_head_get ( MatScm *un)
{
  return mat_universetype_ship_head_get(mat_universe_universetype_get(un));
}

INLINE MatScm *
mat_universe_shiptype_tail_get ( MatScm *un)
{
  return mat_universetype_ship_tail_get(mat_universe_universetype_get(un));
}

#if 0
INLINE MatShipType *
mat_universe_shiptype_find ( MatUniverse *un, int id )
{
  MatShipType *type;

  ASSERT(un);

  for (type = mat_universe_shiptype_head_get(un); type; type = mat_shiptype_next_get(type))
    if (mat_shiptype_id_get(type) == id)
      return type;

  return NULL;
}
#endif

INLINE void
mat_universe_image_forall ( MatScm *un, void (*func) ( void *, MatScm *, unsigned int ), void *data )
{
  ASSERT(un);
  ASSERT(un->type);
  mat_universetype_image_forall(mat_universe_universetype_get(un), func, data);
}

INLINE int
mat_universe_pause_get ( MatScm *un )
{
  MatUniverse *data = data_get(un);
  return data->pause;
}

INLINE void
mat_universe_pause_set ( MatScm *un, MatScm *ship, int pause )
{
  MatUniverse *data = data_get(un);

  data->pause = pause;

  if (pause)
    mat_universe_message_printf(un, "-=> %s freezes time . . .", mat_scm_string_data_get(mat_ship_name_get(ship)));
  else
    mat_universe_message_printf(un, "-=> %s makes time continue . . .", mat_scm_string_data_get(mat_ship_name_get(ship)));
}

#if 0
INLINE MatMovingObj *
mat_universe_obj_head_all_get ( MatUniverse *un )
{
  ASSERT(un);
  return un->obj_head_all;
}

INLINE MatMovingObj *
mat_universe_obj_tail_all_get ( MatUniverse *un )
{
  ASSERT(un);
  return un->obj_tail_all;
}

INLINE void 
mat_universe_obj_head_all_set ( MatUniverse *un, MatMovingObj *obj )
{
  ASSERT(un);
  un->obj_head_all = obj;
}

INLINE void 
mat_universe_obj_tail_all_set ( MatUniverse *un, MatMovingObj *obj )
{
  ASSERT(un);
  un->obj_tail_all = obj;
}
#endif

INLINE MatScm *
mat_universe_name_get ( MatScm *un )
{
  MatUniverse *data = data_get(un);
  return data->name;
}

INLINE MatAngleCache *
mat_universe_angles_cache_get ( MatScm *un )
{
  return mat_universetype_ang_cache_get(mat_universe_universetype_get(un));
}

INLINE unsigned int
mat_universe_ship_dmg_get ( MatScm *un )
{
  MatUniverse *data = data_get(un);
  return data->ship_dmg;
}

INLINE unsigned int
mat_universe_asteroid_dmg_get ( MatScm *un )
{
  MatUniverse *data = data_get(un);
  return data->asteroid_dmg;
}

INLINE unsigned int
mat_universe_maxdead_get ( MatScm *un )
{
  /* TODO: Fix this.  Return a field from un. */
  return arg_lives;
}

INLINE MatScm *
mat_universe_team_add ( MatScm *un, MatScm *obj )
{
  MatUniverse *data = data_get(un);
  return mat_dlist_add_tail(&data->team_head, &data->team_tail, obj);
}

INLINE void
mat_universe_team_rm ( MatScm *un, MatScm *node )
{
  MatUniverse *data = data_get(un);
  mat_dlist_rm(&data->team_head, &data->team_tail, node);
}

MatScm *
mat_universe_head_get ( void )
{
  return mat_universe_head;
}

MatScm *
mat_universe_tail_get ( void )
{
  return mat_universe_tail;
}

#if 0
MatUniverse *
mat_universe_prev_get ( MatUniverse *un )
{
  ASSERT(un);

  return un->un_prev;
}

MatUniverse *
mat_universe_next_get ( MatUniverse *un )
{
  ASSERT(un);

  return un->un_next;
}
#endif

INLINE MatScm *
mat_universe_team_head_get ( MatScm *un )
{
  MatUniverse *data = data_get(un);
  return data->team_head;
}

INLINE MatScm *
mat_universe_team_tail_get ( MatScm *un )
{
  MatUniverse *data = data_get(un);
  return data->team_tail;
}

INLINE MatScm *
mat_universe_team_get ( MatScm *un, MatScm *name )
{
  MatUniverse *data = data_get(un);
  MatScm *team;

  ASSERT(un);
  ASSERT(name);

  for (team = mat_universe_team_head_get(un); team; team = mat_dlist_next_get(team))
    if (!mat_scm_obj_equal(mat_team_name_get(team), name))
      return team;

  return NULL;
}

#if 0
INLINE void
mat_universe_movingobj_remove ( MatUniverse *un, MatMovingObj *obj )
{
  ASSERT(un);
  ASSERT(obj);
  ASSERT(mat_movingobj_universe_get(obj) == un);
  ASSERT(!mat_movingobj_alive_get(obj));

  mat_movingobj_rm_type(obj);
  mat_movingobj_rm_all(obj);

  mat_movingobj_refs_dec(obj);
}
#endif

#if 0
INLINE unsigned int
mat_universe_image_asteroid_get ( MatScm *un, MatAsteroidSize size )
{
  return mat_universetype_image_asteroid_get(mat_universe_universetype_get(un), size);
}
#endif

INLINE unsigned int
mat_universe_image_missile_get ( MatScm *un, MatMissileSize size )
{
  return mat_universetype_image_missile_get(mat_universe_universetype_get(un), size);
}

INLINE MatScm *
mat_universe_banner_get ( MatScm *un )
{
  return mat_universetype_banner_get(mat_universe_universetype_get(un));
}

INLINE void
mat_universe_event_join_run ( MatScm *un, MatScm *obj )
{
  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universe_type);
  ASSERT(obj);
  ASSERT(mat_scm_obj_type_get(obj) == &mat_scm_movingobj_type);
  ASSERT(mat_movingobj_universe_get(obj) == un);

  mat_universetype_event_join_run(mat_universe_universetype_get(un), obj);
}

INLINE void
mat_universe_talk ( MatScm *un, MatScm *ship, MatScm *msg )
{
  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universe_type);
  ASSERT(ship);
  ASSERT(mat_scm_obj_type_get(ship) == &mat_scm_ship_type);
  ASSERT(mat_movingobj_universe_get(mat_ship_movingobj_get(ship)) == un);
  ASSERT(msg);
  ASSERT(mat_scm_obj_type_get(msg) == &mat_scm_string_type);

  mat_universetype_event_talk_run(mat_universe_universetype_get(un), ship, msg);
}

INLINE MatScm *
mat_universe_universetype_get ( MatScm *un )
{
  MatUniverse *data = data_get(un);
  return data->type;
}

INLINE MatScm *
mat_universe_movingobj_add ( MatScm *un, MatScm *obj )
{
  MatUniverse *data = data_get(un);
  MatScm *node;

  ASSERT((!data->obj_head) == (!data->obj_tail));
  node = mat_dlist_add_tail(&data->obj_head, &data->obj_tail, obj);
  ASSERT((!data->obj_head) == (!data->obj_tail));

  return node;
}

INLINE void
mat_universe_movingobj_rm ( MatScm *un, MatScm *node )
{
  MatUniverse *data = data_get(un);
  mat_dlist_rm(&data->obj_head, &data->obj_tail, node);
  ASSERT((!data->obj_head) == (!data->obj_tail));
}

INLINE MatScm *
mat_universe_movingobj_head_get ( MatScm *un )
{
  MatUniverse *data = data_get(un);
  return data->obj_head;
}

INLINE MatScm *
mat_universe_movingobj_tail_get ( MatScm *un )
{
  MatUniverse *data = data_get(un);
  return data->obj_tail;
}

INLINE void
mat_universe_mark ( void *funcdata )
{
  MatUniverse *data = (MatUniverse *) funcdata;
  mat_scm_obj_mark(data->type);
  mat_scm_obj_mark(data->obj_head);
  mat_scm_obj_mark(data->obj_tail);
  mat_scm_obj_mark(data->team_head);
  mat_scm_obj_mark(data->team_tail);
  mat_scm_obj_mark(data->name);
}
