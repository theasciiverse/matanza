#include <matanza.h>

MatScm *
mat_missile_new ( MatScm *obj, MatMissileSize size, int dir )
{
  ASSERT(dir == 1 || dir == -1);
  ASSERT(obj);

  return mat_missile_new_ang(obj, size, dir, mat_movingobj_ang_get(obj));
}

MatScm *
mat_missile_new_ang ( MatScm *obj, MatMissileSize size, int dir, int ang )
{
  MatScm *data;
  double sp_x, sp_y, pos_x, pos_y;
  MatScm *un;
  MatScm *new;

  static int ttl[4] = { 20, 40, 200, 3000 };

  static double speed[4] = {
    12.0,
    15.0,
    18.5,
    12.0
  };

  unsigned int img;

  ASSERT(dir == 1 || dir == -1);
  ASSERT(obj);

  un = mat_movingobj_universe_get(obj);

  ASSERT(un);
  ASSERT(mat_scm_obj_type_get(un) == &mat_scm_universe_type);

  img = mat_universe_image_missile_get(un, size);

  //data->size   = size;
  //data->source = obj;
  new =  mat_movingobj_new(un, NULL, img, mat_palette_id, ttl[size], 100);

  mat_movingobj_ang_set(new, ang);

  pos_x = mat_movingobj_pos_x_center_get(obj);
  pos_y = mat_movingobj_pos_y_center_get(obj);

  ASSERT(0 <= (int) pos_x);
  ASSERT((int) pos_x < mat_universe_mapsize_x_get(mat_movingobj_universe_get(obj)));

  ASSERT(0 <= (int) pos_y);
  ASSERT((int) pos_y < mat_universe_mapsize_y_get(mat_movingobj_universe_get(obj)));

  mat_movingobj_pos_set(new, pos_x, pos_y);

  sp_x = mat_movingobj_sp_x_get(obj) + dir * speed[size] * mat_universe_cos_get(un, ang) / arg_universe_update_steps;
  sp_y = mat_movingobj_sp_y_get(obj) + dir * speed[size] * mat_universe_sin_get(un, ang) / arg_universe_update_steps;

  mat_movingobj_sp_linear_set(new, sp_x, sp_y);
  mat_movingobj_sp_ang_set(new, 0.0);

  return new;
}

#if 0
struct MatMissile
{
  MatMissileSize            size;
  MatMovingObj             *source;
  MatMovingObj             *obj;
};

static void               mat_missile_update               ( void               *m );

static void               mat_missile_free                 ( MatMovingObj       *m );

MatMovingObjTypeD mat_missile_type =
{
  "missile",
  NULL,
  NULL,
  mat_missile_free
};

static void
mat_missile_free ( MatMovingObj *obj )
{
  MatMissile *m;

  ASSERT(obj);

  m = mat_movingobj_data_get(obj);

  ASSERT(m);
  ASSERT(m->source);

  mat_movingobj_refs_dec(m->source);

  free(m);
}

INLINE unsigned int
mat_missile_dmg_get ( MatMissile *m )
{
  static int dmg[4] = { 15, 100, 400, 3000 };
  return mat_scm_number_get(mat_scm_env_find(mat_movingobj_env_get(m), "matanza-missile-damage"));
}

INLINE MatScm *
mat_missile_source_get ( MatScm *m )
{
  return mat_scm_number_get(mat_scm_env_find(mat_movingobj_env_get(m), "matanza-missile-source"));
}

INLINE char *
mat_missile_hit_ship_get ( MatScm *m )
{
  static char *msg[4] = {
    "-=> You got %s !",
    "-=> Your small missile got %s !",
    "-=> Bang, your missile got %s !",
    "-=> BOOM, your torpedo got %s !" };

  return msg[mat_missile_size_get(m)];
  return mat_scm_string_data_get(mat_scm_env_find(mat_movingobj_env_get(m), "matanza-missile-hit-ship"));
}

INLINE char *
mat_missile_hit_asteroid_get ( MatScm *m )
{
  static char *msg[4] = {
    "-=> You reached the asteroid !",
    "-=> Your small missile reached the asteroid !",
    "-=> Bang, your missile hit the asteroid !",
    "-=> BOOM, your torpedo got the asteroid !" };

  return mat_scm_string_data_get(mat_scm_env_find(mat_movingobj_env_get(m), "matanza-missile-hit-asteroid"));
}
#endif
