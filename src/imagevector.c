#include <matanza.h>

/* We are constantly scaling images.  If we actually calculated all the new
 * images (for all the angles), it would introduce noticeable delays into our
 * program (for instance, when a Telnet player zooms in/out).
 *
 * Instead, we will make our image vectors dynamical structures in the sense
 * that the actual images are only calculated when they are needed the first
 * time (thus the actual computations are delayed until they are needed).
 *
 * So rather than calculate all the resizes for all the angles all at once,
 * they are divided over time.
 *
 * The only overhead added is one check for a NULL pointer when a given image
 * is requested.
 *
 * Actually, not only the scaling is delayed but also the applying of palettes
 * (what happens when you apply a palette to an image vector with all its
 * images delayed?  Either you calculate all the images and apply the palette
 * or you delay the applying of the palette). */

typedef struct MatImageVector MatImageVector;

struct MatImageVector
{
  /* The number of angles in the vector. */
  unsigned int     ang;

  /* The vector with the actual images for all the rotations. */
  MatScm         **core;

  /* For delayed resizes, src points to the original vector (NULL otherwise). */
  MatScm          *src;

  double           rate_x;
  double           rate_y;
  unsigned char   *palette;
};

static void               iv_free                          ( void               *funcdata );

static void               iv_mark                          ( void               *funcdata );

static int                iv_equal                         ( MatScm             *img0,
                                                             MatScm             *img1 );

INLINE static MatImageVector *data_get                     ( MatScm             *v );

INLINE static void        iv_optimize                      ( MatScm             *vector );

INLINE static void        iv_calc_resize                   ( MatScm             *v,
                                                             unsigned int        ang );

MatScmType mat_scm_imagevector_type =
{
  "imagevector",
  iv_free,
  iv_mark,
  NULL,
  NULL,
  iv_equal
};

int
iv_equal ( MatScm *img0, MatScm *img1 )
{
  /* TODO: Write */
  return 0;
}

static void
iv_mark ( void *funcdata )
{
  MatImageVector *v = (MatImageVector *) funcdata;
  unsigned int i;
  for (i = 0; i < v->ang; i ++)
    mat_scm_obj_mark(v->core[i]);
  if (v->src)
    mat_scm_obj_mark(v->src);
}

static void
iv_free ( void *funcdata )
{
  MatImageVector *v = (MatImageVector *) funcdata;
  free(v->core);
  free(v);
}

MatScm *
mat_imagevector_new ( MatScm *src, unsigned int ang_total, MatAngleCache *ang_cache )
{
  unsigned int w, h, ang, size;
  int i, j;
  MatImageVector *v;
  MatScm *iv;

  ASSERT(ang_total > 0);

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> mat_imagevector_new [ang:%d]\n", ang_total);

  v = malloc(sizeof(MatImageVector));
  if (!v)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  v->core = malloc(sizeof(MatImageVector *) * ang_total);
  if (!v->core)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  v->ang = ang_total;

  v->src = NULL;
  v->palette = NULL;
  v->rate_x = 1.0;
  v->rate_y = 1.0;

  w = mat_image_size_x_get(src);
  h = mat_image_size_y_get(src);

  size = (int) sqrt(w * w + h * h) + 1;

  for (ang = 0; ang < ang_total; ang ++)
    {
      MatScm *image;
      unsigned int pos = 0;
      double c, s;

      c = mat_anglecache_cos(ang_cache, ang);
      s = mat_anglecache_sin(ang_cache, ang);

      image = mat_image_new(size, size);

      for (j = - (int) size / 2; j < (int) size / 2; j ++)
        for (i = - (int) size / 2; i < (int) size / 2; i ++)
          {
            double x, y;

            x = i * c - j * s;
            y = i * s + j * c;

            ASSERT(0 <= pos);
            ASSERT(pos < size * size);

            if ((int) x < - (int) w / 2 || (int) x >= (int) w / 2 || (int) y < - (int) h / 2 || (int) y >= (int) h / 2 )
              {
                mat_image_core_set_pos(image, pos ++, 0);
              }
            else
              {
                ASSERT((int) x + w / 2 < w);
                ASSERT((int) y + h / 2 < h);

                ASSERT(0 <= (int) (y + h / 2) * w + (int) (x + w / 2));
                ASSERT((int) (y + h / 2) * w + (int) (x + w / 2) < w * h);

                mat_image_core_set_pos(image, pos ++, mat_image_core_get(src, x + w / 2, y + h / 2));
              }
          }

      v->core[ang] = image;
    }

  iv = mat_scm_obj_malloc(&mat_scm_imagevector_type, v);
  iv_optimize(iv);

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- mat_imagevector_new\n");

  return iv;
}

INLINE MatImageVector *
data_get ( MatScm *v )
{
  MatImageVector *data;

  ASSERT(v);
  ASSERT(v->type == &mat_scm_imagevector_type);

  data = mat_scm_obj_data_get(v);

  ASSERT(data);
  ASSERT(data->core);

  return data;
}

INLINE MatScm *
mat_imagevector_get ( MatScm *v, unsigned int pos )
{
  MatImageVector *data = data_get(v);

  ASSERT(pos < data->ang);

  if (!data->core[pos])
    iv_calc_resize(v, pos);

  ASSERT(data->core[pos]);

  return data->core[pos];
}

static void
iv_optimize ( MatScm *v )
{
  MatImageVector *data = data_get(v);
  unsigned int xbeg, xend, ybeg, yend, ang, blank, i, height, width;

  ASSERT(data);
  ASSERT(data->core);
  ASSERT(data->core[0]);

  width  = mat_image_size_x_get (data->core[0]);
  height = mat_image_size_y_get(data->core[0]);

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> mat_imagevector_optimize [%dx%d]\n", width, height);

  /* Calculate the maximum increase for the beginning in the Y coordinate. */
  blank = 1;

  for (ybeg = 0; blank; ybeg ++)
    for (ang = 0; ang < data->ang; ang ++)
      for (i = 0; i < width; i ++)
        if (mat_image_core_get(data->core[ang], i, ybeg) != 0)
          blank = 0, i = width, ang = data->ang;

  ASSERT(0 < ybeg);

  ybeg --;

  ASSERT(ybeg < height);

  /* Calculate the maximum decrease for the ending in the Y coordinate. */
  blank = 1;

  for (yend = height - 1; blank; yend --)
    for (ang = 0; ang < data->ang; ang ++)
      for (i = 0; i < width; i ++)
        if (mat_image_core_get(data->core[ang], i, yend) != 0)
          blank = 0, i = width, ang = data->ang;

  yend ++;

  ASSERT(ybeg < yend);
  ASSERT(yend < height);

  /* Calculate the maximum increase for the beginning of the X coordinate. */
  blank = 1;

  for (xbeg = 0; blank; xbeg ++)
    for (ang = 0; ang < data->ang; ang ++)
      for (i = ybeg; i < yend; i ++)
        if (mat_image_core_get(data->core[ang], xbeg, i) != 0)
          blank = 0, i = height, ang = data->ang;

  ASSERT(xbeg > 0);

  xbeg --;

  ASSERT(0 <= xbeg);
  ASSERT(xbeg < width);

  blank = 1;

  for (xend = width - 1; blank; xend --)
    for (ang = 0; ang < data->ang; ang ++)
      for (i = ybeg; i < yend; i ++)
        if (mat_image_core_get(data->core[ang], xend, i) != 0)
          blank = 0, i = height, ang = data->ang;

  xend --;

  ASSERT(xbeg < xend);
  ASSERT(xend < width);

  /* Now actually perform the reduction. */
  for (ang = 0; ang < data->ang; ang ++)
    mat_image_sub(data->core[ang], xbeg, ybeg, xend, yend);

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- mat_imagevector_optimize [%dx%d]\n",
          mat_image_size_x_get(data->core[0]),
          mat_image_size_y_get(data->core[0]));
}

INLINE MatScm *
mat_imagevector_resize ( MatScm *src, double rate_x, double rate_y )
{
  MatImageVector *dst, *srcdata = data_get(src);
  unsigned int i;

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> mat_imagevector_resize [ang:%d]\n", srcdata->ang);

  dst = malloc(sizeof(MatImageVector));
  if (!dst)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  dst->core = malloc(sizeof(MatImageVector *) * srcdata->ang);
  if (!dst->core)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  dst->ang = srcdata->ang;

  dst->src = src;
  dst->rate_x = rate_x;
  dst->rate_y = rate_y;

  dst->palette = NULL;

  for (i = 0; i < dst->ang; i ++)
    dst->core[i] = NULL;

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- mat_imagevector_resize\n");

  return mat_scm_obj_malloc(&mat_scm_imagevector_type, dst);
}

INLINE static void
iv_calc_resize ( MatScm *v, unsigned int ang )
{
  MatImageVector *data = data_get(v);

  ASSERT(data);
  ASSERT(data->src);
  ASSERT(ang < data->ang);

  data->core[ang] = mat_image_resize(mat_imagevector_get(data->src, ang), data->rate_x, data->rate_y);

  if (data->palette)
    mat_image_palette_apply(data->core[ang], data->palette);
}

INLINE void
mat_imagevector_palette_apply ( MatScm *img, unsigned char *pal )
{
  MatImageVector *data = data_get(img);
  unsigned int i;

  for (i = 0; i < data->ang; i ++)
    if (data->core[i])
      mat_image_palette_apply(data->core[i], pal);

  data->palette = pal;
}
