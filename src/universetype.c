#include <matanza.h>

typedef struct MatLocation MatLocation;

struct MatUniverseType
{
  /* A string with the universe's name. */

  MatScm              *name;

  /* The entry in the list of universe types */

  MatScm              *un_list;

  /* The number of valid angles for the objects in this universe. */

  unsigned int         ang;

  /* A cache with the sin and cos of every available angle. */

  MatAngleCache       *ang_cache;

  /* The size of the map for this universe. */

  unsigned int         mapsize_x;
  unsigned int         mapsize_y;

  /* List of ship types available in this universe type */

  MatScm              *shiptype_head;
  MatScm              *shiptype_tail;

  MatScm              *bg;
  MatScm              *mask;

  /* Array with all the available images in this universe. */

  MatScm             **images;
  int                  images_count;
  int                  images_size;

  //unsigned int         images_asteroid[MAT_ASTEROID_SIZE_MAX];
  unsigned int         images_missile[MAT_MISSILE_SIZE_COUNT];

  /* Information for the different zones */

  MatLocation         *location[256];

  /* A list of strings that are shown in the banner (in the
   * menu shown once the player selects this universe) */

  MatScm              *banner;

  /* Procedure executed whenever a ship joins this universe.
   * Receives the universe and the ship as its arguments. */

  MatScm              *join;

  /* Procedure executed whenever a new instance of this
   * universe is created.  Receives the instance as its only
   * argument. */

  MatScm              *start;

  /* Procedure executed whenever a player says something in
   * this universe.  This can be used to implement an interface
   * for controlling certain aspects of the universe.  The
   * procedure receives the ship and the message as its
   * arguments. */

  MatScm              *talk;
};

struct MatLocation
{
  /* Should the objects crash against this object? */
  int              crash[3];

  /* How much damage does this object deal to the player on each turn?  Usually
   * 0.  Can be positive or negative. */
  int              health;

  /* Does being on this area slow the objects down? The movement of the objects
   * is multiplied with this value once for every turn. */
  double           move[3];
};

static MatScm *mat_universetype_head = NULL;
static MatScm *mat_universetype_tail = NULL;

static INLINE MatUniverseType *data_get ( MatScm *type );

static INLINE MatUniverseType *
data_get ( MatScm *type )
{
  MatUniverseType *data;

  ASSERT(type);
  ASSERT(mat_scm_obj_type_get(type) == &mat_scm_universetype_type);

  data = mat_scm_obj_data_get(type);

  ASSERT(data);
  ASSERT(data->name);
  ASSERT(mat_scm_obj_type_get(data->name) == &mat_scm_string_type);
  ASSERT(data->ang_cache);
  //ASSERT(data->images_asteroid);
  //ASSERT(data->images_missile);
  ASSERT(data->banner);
  ASSERT(mat_scm_obj_type_get(data->banner) == &mat_scm_cons_type);

  return data;
}

MatScm *
mat_universetype_new ( MatScm *name, unsigned int ang, unsigned int mapsize_x, unsigned int mapsize_y, MatScm *banner )
{
  MatUniverseType *data;
  MatScm *type;
  int i;

  ASSERT(name);
  ASSERT(mat_scm_obj_type_get(name) == &mat_scm_string_type);

  data = malloc(sizeof(MatUniverseType));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->name = name;
  data->banner = banner;

  data->ang = ang;
  data->ang_cache = mat_anglecache_new(ang);
  if (!data->ang_cache)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->mapsize_x = mapsize_x;
  data->mapsize_y = mapsize_y;

  data->shiptype_head = data->shiptype_tail = NULL;
  data->bg = data->mask = NULL;

  if (data->bg)
    {
      if (mat_image_size_x_get(data->bg) > data->mapsize_x)
        data->mapsize_x = mat_image_size_x_get(data->bg);

      if (mat_image_size_y_get(data->bg) > data->mapsize_y)
        data->mapsize_y = mat_image_size_y_get(data->bg);
    }

  ASSERT(!data->bg || mat_image_size_x_get(data->bg) <= data->mapsize_x);
  ASSERT(!data->bg || mat_image_size_y_get(data->bg) <= data->mapsize_y);

  for (i = 0; i < 256; i ++)
    data->location[i] = NULL;

  data->images_count = 0;
  data->images_size  = 16;
  data->images = malloc(sizeof(MatScm *) * data->images_size);
  if (!data->images)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->join = NULL;

  type = mat_scm_obj_malloc(&mat_scm_universetype_type, data);

  data->un_list = mat_dlist_add_head(&mat_universetype_head, &mat_universetype_tail, type);

  return type;
}

INLINE MatScm *
mat_universetype_name_get ( MatScm *type )
{
  MatUniverseType *data = data_get(type);;
  return data->name;
}

INLINE void
mat_universetype_free ( MatScm *type )
{
  MatUniverseType *data = data_get(type);;
  mat_anglecache_free(data->ang_cache);
  free(data);
}

INLINE MatAngleCache *
mat_universetype_ang_cache_get ( MatScm *type )
{
  MatUniverseType *data = data_get(type);;
  return data->ang_cache;
}

INLINE unsigned int
mat_universetype_mapsize_x_get ( MatScm *type )
{
  MatUniverseType *data = data_get(type);
  return data->mapsize_x;
}

INLINE unsigned int
mat_universetype_mapsize_y_get ( MatScm *type )
{
  MatUniverseType *data = data_get(type);
  return data->mapsize_y;
}

INLINE MatScm *
mat_universetype_shiptype_add ( MatScm *type, MatScm *shiptype )
{
  MatUniverseType *data = data_get(type);
  return mat_dlist_add_tail(&data->shiptype_head, &data->shiptype_tail, shiptype);
}

INLINE MatScm *
mat_universetype_ship_head_get ( MatScm *type )
{
  MatUniverseType *data = data_get(type);
  return data->shiptype_head;
}

INLINE MatScm *
mat_universetype_ship_tail_get ( MatScm *type )
{
  MatUniverseType *data = data_get(type);
  return data->shiptype_tail;
}

unsigned int
mat_universetype_image_add ( MatScm *type, MatScm *img )
{
  MatUniverseType *data = data_get(type);

  ASSERT(img);
  ASSERT(img->type == &mat_scm_imagevector_type);

  if (data->images_count == data->images_size)
    {
      data->images_size = data->images_size ? data->images_size * 2 : 16;
      data->images = realloc(data->images, sizeof(MatScm *) * data->images_size);
      if (!data->images)
        {
          fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
          exit(EXIT_FAILURE);
        }
    }

  data->images[data->images_count] = img;

  return data->images_count ++;
}

MatScm *
mat_universetype_image_get ( MatScm *type, unsigned int id )
{
  MatUniverseType *data = data_get(type);
  if (data->images_count < id)
    return NULL;
  return data->images[id];
}

INLINE unsigned int
mat_universetype_image_count_get ( MatScm *type )
{
  MatUniverseType *data = data_get(type);
  return data->images_count;
}

void
mat_universetype_image_forall ( MatScm *type, void (*func) (void *, MatScm *, unsigned int ), void *userdata )
{
  MatUniverseType *data = data_get(type);
  int pos;

  for (pos = 0; pos < data->images_count; pos ++)
    {
      ASSERT(data->images[pos]);
      ASSERT(data->images[pos]->type == &mat_scm_imagevector_type);
      func(userdata, data->images[pos], pos);
    }
}

INLINE void
mat_universetype_mask_sp ( MatScm *type, MatScm *obj )
{
  MatUniverseType *data = data_get(type);

  int x, y;

  ASSERT(type);
  ASSERT(obj);

  if (!data->mask)
    return;

  x = (int) mat_movingobj_pos_x_center_get(obj);
  y = (int) mat_movingobj_pos_y_center_get(obj);

  if (x < mat_image_size_x_get(data->mask) && y < mat_image_size_y_get(data->mask))
    {
      double index;

      ASSERT(data->location[mat_image_core_get(data->mask, x, y)]);

      index = 1.0; //data->location[mat_image_core_get(data->mask, x, y)]->move[mat_movingobj_type_get(obj)];

      if (index != 1.0)
        {
          mat_movingobj_sp_x_set(obj, mat_movingobj_sp_x_get(obj) * index);
          mat_movingobj_sp_y_set(obj, mat_movingobj_sp_y_get(obj) * index);
          mat_movingobj_sp_calc(obj);
        }
    }
}

INLINE int
mat_universetype_mask_crash ( MatScm *type, MatScm *obj )
{
  MatUniverseType *data = data_get(type);

  int crash_x, crash_y;

  if (!data->mask)
    return 0;

  crash_x = (int) mat_movingobj_pos_x_center_get(obj);
  crash_y = (int) mat_movingobj_pos_y_center_get(obj);

  ASSERT(0 <= crash_x);
  ASSERT(0 <= crash_y);
  ASSERT(crash_x < mat_universetype_mapsize_x_get(type));
  ASSERT(crash_y < mat_universetype_mapsize_y_get(type));

  if (crash_x >= mat_image_size_x_get(data->mask) || crash_y >= mat_image_size_y_get(data->mask))
    return 0;

  ASSERT(data->location[mat_image_core_get(data->mask, crash_x, crash_y)]);

  //return data->location[mat_image_core_get(data->mask, crash_x, crash_y)]->crash[mat_movingobj_type_get(obj)];
  return 0;
}

INLINE MatScm *
mat_universetype_head_get ( void )
{
  return mat_universetype_head;
}

INLINE MatScm *
mat_universetype_tail_get ( void )
{
  return mat_universetype_tail;
}

#if 0
INLINE unsigned int
mat_universetype_image_asteroid_get ( MatScm *type, MatAsteroidSize size )
{
  MatUniverseType *data = data_get(type);
  ASSERT(size < MAT_ASTEROID_SIZE_MAX + 1);
  return data->images_asteroid[size];
}
#endif

INLINE unsigned int
mat_universetype_image_missile_get ( MatScm *type, MatMissileSize size )
{
  MatUniverseType *data = data_get(type);
  ASSERT(size < MAT_MISSILE_SIZE_COUNT);
  return data->images_missile[size];
}

unsigned int
mat_universetype_ang_get ( MatScm *type )
{
  MatUniverseType *data = data_get(type);
  return data->ang;
}

INLINE MatScm *
mat_universetype_banner_get ( MatScm *type )
{
  MatUniverseType *data = data_get(type);
  return data->banner;
}

#if 0
INLINE void
mat_universetype_image_asteroid_set ( MatUniverseType *type, unsigned int *id )
{
  int i;
  ASSERT(type);

  for (i = 0; i < sizeof(type->images_asteroid) / sizeof(type->images_asteroid[0]); i++)
    type->images_asteroid[i] = id[i];
}
#endif

INLINE void
mat_universetype_image_missile_set ( MatScm *type, unsigned int *id )
{
  MatUniverseType *data = data_get(type);
  unsigned int i;

  for (i = 0; i < sizeof(data->images_missile) / sizeof(data->images_missile[0]); i++)
    data->images_missile[i] = id[i];
}

INLINE void
mat_universetype_event_join_set ( MatScm *type, MatScm *proc )
{
  MatUniverseType *data = data_get(type);
  data->join = proc;
}

INLINE void
mat_universetype_event_join_run ( MatScm *type, MatScm *obj )
{
  MatUniverseType *data = data_get(type);

  ASSERT(mat_scm_obj_type_get(obj) == &mat_scm_movingobj_type);

  if (!data->join)
    return;

  mat_scm_obj_call(data->join, mat_scm_cons_new(obj, NULL), NULL, 0);
}

INLINE void
mat_universetype_event_start_set ( MatScm *type, MatScm *proc )
{
  MatUniverseType *data = data_get(type);
  data->start = proc;
}

INLINE void
mat_universetype_event_start_run ( MatScm *type, MatScm *un )
{
  MatUniverseType *data = data_get(type);

  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universe_type);

  if (!data->start)
    return;

  mat_scm_obj_call(data->start, mat_scm_cons_new(un, NULL), NULL, 0);
}

INLINE void
mat_universetype_event_talk_set ( MatScm *type, MatScm *proc )
{
  MatUniverseType *data = data_get(type);
  data->talk = proc;
}

INLINE void
mat_universetype_event_talk_run ( MatScm *type, MatScm *ship, MatScm *msg )
{
  MatUniverseType *data = data_get(type);
  ASSERT(ship);
  ASSERT(mat_scm_obj_type_get(ship) == &mat_scm_ship_type);
  ASSERT(msg);
  ASSERT(mat_scm_obj_type_get(msg) == &mat_scm_string_type);

  if (!data->talk)
    mat_universe_message_printf(mat_movingobj_universe_get(ship),
                                "%s> %s",
                                mat_ship_name_get(ship),
                                mat_scm_string_data_get(msg));
  else
    mat_scm_obj_call(data->talk, mat_scm_cons_new(ship, mat_scm_cons_new(msg, NULL)), NULL, 0);
}
