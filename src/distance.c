#include <matanza.h>

INLINE static double      dist_one                         ( double              src,
                                                             double              src_img,
                                                             double              dst,
                                                             double              dst_img,
                                                             double              un );

void
mat_distance_get ( MatScm *src, MatScm *dst, double *dx, double *dy )
{
  ASSERT(src);
  ASSERT(dst);

  ASSERT(mat_movingobj_universe_get(src));
  ASSERT(mat_movingobj_universe_get(dst));

  ASSERT(mat_movingobj_universe_get(src) == mat_movingobj_universe_get(dst));

  ASSERT(mat_movingobj_image_get(src));
  ASSERT(mat_movingobj_image_get(dst));

  *dx = dist_one(mat_movingobj_pos_x_center_get(src), mat_image_size_x_get(mat_movingobj_image_get(src)), mat_movingobj_pos_x_center_get(dst), mat_image_size_x_get(mat_movingobj_image_get(dst)), mat_universe_mapsize_x_get(src));

  *dy = dist_one(mat_movingobj_pos_y_center_get(src), mat_image_size_y_get(mat_movingobj_image_get(src)), mat_movingobj_pos_y_center_get(dst), mat_image_size_y_get(mat_movingobj_image_get(dst)), mat_universe_mapsize_y_get(src));

  ASSERT(- (int) mat_universe_mapsize_x_get(src) <= *dx * 2);
  ASSERT(- (int) mat_universe_mapsize_y_get(src) <= *dy * 2);
  ASSERT(*dx * 2 <= mat_universe_mapsize_x_get(src));
  ASSERT(*dy * 2 <= mat_universe_mapsize_y_get(src));
}

static INLINE double
dist_one ( double src, double src_img, double dst, double dst_img, double un )
{
  double d0, d1;

  ASSERT(0 <= src);
  ASSERT(0 <= dst);
  ASSERT(src < un);
  ASSERT(dst < un);

  src += src_img / 2;
  dst += dst_img / 2;

  if (un <= src)
    src -= un;

  if (un <= dst)
    dst -= un;

  ASSERT(0 <= src);
  ASSERT(0 <= dst);
  ASSERT(src < un);
  ASSERT(dst < un);

  d0 = (src < dst ? un : 0) + src - dst;
  d1 = (dst < src ? un : 0) + dst - src;

  ASSERT(0 <= d0);
  ASSERT(0 <= d1);
  ASSERT(d0 < un);
  ASSERT(d1 < un);

  ASSERT(2 * d0 <= un || 2 * d1 <= un);

  return d0 < d1 ? -d0 : d1;
}
