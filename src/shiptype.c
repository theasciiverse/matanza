#include <matanza.h>

struct MatShipType
{
  MatScm *name;
  MatScm *desc;
  unsigned int health;
  unsigned int sp;
  unsigned int sp_ang;
  unsigned int missiles[MAT_MISSILE_SIZE_COUNT];
  unsigned int firerate[MAT_MISSILE_SIZE_COUNT];

  unsigned int image;

  MatScm         *un_list;
};

static INLINE MatShipType *
data_get ( MatScm *shiptype )
{
  MatShipType *data;

  ASSERT(shiptype);

  if (mat_scm_obj_type_get(shiptype) == &mat_scm_ship_type)
    shiptype = mat_ship_shiptype_get(shiptype);

  ASSERT(mat_scm_obj_type_get(shiptype) == &mat_scm_shiptype_type);

  data = mat_scm_obj_data_get(shiptype);

  ASSERT(data->health);

  return data;
}

INLINE MatScm *
mat_shiptype_new ( MatScm *name, MatScm *desc, unsigned int img, unsigned int sp, unsigned int health, unsigned int sp_ang, unsigned int *m, unsigned int *fr )
{
  int i;
  MatShipType *data;

  data = (MatShipType *) malloc(sizeof(MatShipType));

  data->name = name;
  data->desc = desc;
  data->image = img;

  data->sp     = sp;
  data->health = health;
  data->sp_ang = sp_ang;

  for (i = 0; i < MAT_MISSILE_SIZE_COUNT; i ++)
    {
      data->missiles[i] = m[i];
      data->firerate[i] = fr[i];
    }

  return mat_scm_obj_malloc(&mat_scm_shiptype_type, data);
}

INLINE MatScm *
mat_shiptype_name_get ( MatScm *st )
{
  MatShipType *data = data_get(st);
  return data->name;
}

INLINE MatScm *
mat_shiptype_desc_get ( MatScm *st )
{
  MatShipType *data = data_get(st);
  return data->desc;
}

INLINE unsigned int
mat_shiptype_health_get ( MatScm *st )
{
  MatShipType *data = data_get(st);
  return data->health;
}

INLINE unsigned int
mat_shiptype_speed_get ( MatScm *st )
{
  MatShipType *data = data_get(st);
  return data->sp;
}

INLINE unsigned int
mat_shiptype_sp_ang_get ( MatScm *st )
{
  MatShipType *data = data_get(st);
  return data->sp_ang;
}

INLINE unsigned int
mat_shiptype_missiles_get ( MatScm *st, MatMissileSize type )
{
  MatShipType *data = data_get(st);
  ASSERT(0 <= type);
  ASSERT(type < MAT_MISSILE_SIZE_COUNT);
  return data->missiles[type];
}

INLINE unsigned int
mat_shiptype_firerate_get ( MatScm *st, MatMissileSize type )
{
  MatShipType *data = data_get(st);
  ASSERT(0 <= type);
  ASSERT(type < MAT_MISSILE_SIZE_COUNT);
  return data->firerate[type];
}

INLINE unsigned int
mat_shiptype_image_get ( MatScm *st )
{
  MatShipType *data = data_get(st);
  return data->image;
}
