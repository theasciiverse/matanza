#include <matanza.h>

typedef struct MatImage MatImage;

struct MatImage
{
  unsigned int h, w;

  /* img is the matrix with the colors in the image.  If no palette is applied,
   * a value of 0 is taken to be black and a value of 255 is taken to be white.
   * Otherwise, the values are interpreted as entries in the palette. */
  unsigned char *img;
};

MatImage                 *data_get                         ( MatScm             *img );

static void               i_free                           ( void               *funcdata );

static MatScmCallback     i_load_calc;

MatScmType mat_scm_image_type =
{
  "image",
  i_free,
  NULL,
  NULL,
  NULL,
  NULL
};

MatScm                   *mat_scm_image_load = NULL;

static void
i_free ( void *funcdata )
{
  MatImage *img = (MatImage *) funcdata;
  ASSERT(img);
  free(img->img);
  free(img);
}

void
mat_scm_image_init ( void )
{
  mat_scm_obj_protect(&mat_scm_image_load);

  mat_scm_image_load = mat_scm_proc_new(i_load_calc, NULL, 1);
}

static MatScm *
i_load_calc ( void *data, MatScm *env, MatScm *args )
{
  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_string_type);
  ASSERT(!mat_scm_cons_cdr_get(args));

  return mat_image_load(mat_scm_string_data_get(mat_scm_cons_car_get(args)));
}

MatImage *
data_get ( MatScm *img )
{
  ASSERT(img);
  ASSERT(mat_scm_obj_type_get(img) == &mat_scm_image_type);

  return mat_scm_obj_data_get(img);
}

unsigned char
mat_image_core_get ( MatScm *img, unsigned int x, unsigned int y )
{
  MatImage *data = data_get(img);
  ASSERT(x < data->w);
  ASSERT(y < data->h);
  return data->img[y * data->w + x];
}

unsigned int
mat_image_size_x_get ( MatScm *img )
{
  MatImage *data = data_get(img);
  return data->w;
}

unsigned int
mat_image_size_y_get ( MatScm *img )
{
  MatImage *data = data_get(img);
  return data->h;
}

unsigned char *
mat_image_core_buffer_get ( MatScm *img )
{
  MatImage *data = data_get(img);
  return data->img;
}

#if 0
INLINE MatScm *
mat_image_array_get ( MatScm *img, int pos )
{
  MatImage *data = data_get(img);
  return &data[pos];
}
#endif

MatScm *
mat_image_new ( unsigned int width, unsigned int height )
{
  MatImage *i;

  i = malloc(sizeof(MatImage));
  if (!i)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  i->img = malloc(width * height);
  if (!i->img)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  i->w = width;
  i->h = height;

  return mat_scm_obj_malloc(&mat_scm_image_type, i);
}

INLINE void
mat_image_core_set_all ( MatScm *img, unsigned char val )
{
  MatImage *data = data_get(img);
  memset(data->img, val, data->w * data->h);
}

INLINE void
mat_image_core_set ( MatScm *img, unsigned int x, unsigned int y, unsigned char val )
{
  MatImage *data = data_get(img);
  ASSERT(x < data->w);
  ASSERT(y < data->h);
  mat_image_core_set_pos(img, y * data->w + x, val);
}

INLINE void
mat_image_sub ( MatScm *img, unsigned int xbeg, unsigned int ybeg, unsigned int xend, unsigned int yend )
{
  MatImage *data = data_get(img);
  unsigned int x, y, pos = 0, width, height;

  width  = xend - xbeg;
  height = yend - ybeg;

  for (y = ybeg; y < yend; y ++)
    for (x = xbeg; x < xend; x ++)
      data->img[pos ++] = mat_image_core_get(img, x, y);

  ASSERT(pos == width * height);

  /* realloc never fails when the allocated size is reduced. */
  data->img = realloc(data->img, pos);
  ASSERT(data->img);

  data->w = width;
  data->h = height;
}

INLINE void
mat_image_core_set_pos ( MatScm *img, unsigned int pos, unsigned char val )
{
  MatImage *data = data_get(img);
  ASSERT(pos < data->w * data->h);
  data->img[pos] = val;
}

/* scale_x: src->width / dst->width */
MatScm *
mat_image_resize ( MatScm *src, double rate_x, double rate_y )
{
  MatScm *dst;
  unsigned int dst_h, dst_w;
  unsigned int i, j;

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> mat_image_resize [rate_x:%f][rate_y:%f]\n", rate_x, rate_y);

  /* I think this algorithm doesn't work if the scaling enlarges the image. */
  ASSERT(rate_x > 1.0);
  ASSERT(rate_y > 1.0);

  dst_w = (double) mat_image_size_x_get (src) / rate_x;
  dst_h = (double) mat_image_size_y_get(src) / rate_y;

  dst = mat_image_new(dst_w, dst_h);

  for (i = 0; i < mat_image_size_y_get(dst); i ++)
    for (j = 0; j < mat_image_size_x_get (dst); j ++)
      {
        unsigned int col, col_y, col_x;

        col = 0;

        for (col_y = 0; col_y < rate_y && (int) (i * rate_y) + col_y < mat_image_size_y_get(src); col_y ++)
          for (col_x = 0; col_x < rate_x && (int) (j * rate_x) + col_x < mat_image_size_x_get (src); col_x ++)
            col += mat_image_core_get(src, (int) (j * rate_x) + col_x, (int) (i * rate_y) + col_y);

        /* Make sure that we got at least one dot. */
        ASSERT(0 < col_x);
        ASSERT(0 < col_y);

        if (col > 0)
          mat_image_core_set(dst, j, i, col / (col_y * col_x));
      }

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- mat_image_resize\n");

  return dst;
}

void
mat_image_palette_apply ( MatScm *img, unsigned char *pal )
{
  MatImage *data = data_get(img);
  unsigned int y, x, pos = 0;

  for (y = 0; y < mat_image_size_y_get(img); y ++)
    for (x = 0; x < mat_image_size_x_get(img); x ++)
      data->img[pos] = pal[data->img[pos ++]];
}

#if HAVE_LIBPNG
MatScm *
mat_image_load ( const char *path )
{
  png_structp  png_ptr;
  png_infop   info_ptr;
  FILE *fp;
  int y, dst;
  png_bytep row_pointers;

  MatScm *nimg;

  png_color_16 background;
  int bd, color;
  png_uint_32 w, h;
  int dummy;

  fp = fopen(path, "rb");
  if (!fp)
    {
      fprintf(stderr, "%s: %s: %s\n", program_name, path, strerror(errno));
      exit(EXIT_FAILURE);
    }

  /* Create and initialize the png_struct with the desired error handler
   * functions.  If you want to use the default stderr and longjump method,
   * you can supply NULL for the last three parameters.  We also supply the
   * the compiler header file version, so that we know if the application
   * was compiled with a compatible version of the library.
   */

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!png_ptr)
    return 0;

  /* Allocate/initialize the memory for image information.  REQUIRED. */
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      exit(EXIT_FAILURE);
    }

  if (setjmp(png_ptr->jmpbuf))
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      exit(EXIT_FAILURE);
    }

  png_init_io(png_ptr, fp);

  png_read_info(png_ptr, info_ptr);
  png_get_IHDR(png_ptr, info_ptr, &w, &h, &bd, &color, &dummy, NULL, NULL);

  nimg = mat_image_new(w, h);

  /* We only support grayscale PNG files. */
  if (color != PNG_COLOR_TYPE_GRAY && color != PNG_COLOR_TYPE_GRAY_ALPHA)
    {
      fprintf(stderr, "%s: %s: Only grayscale PNG files are supported\n", program_name, path);
      exit(EXIT_FAILURE);
    }

  /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
   * byte into separate bytes. */
  png_set_packing(png_ptr);

  /* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
  if ((color == PNG_COLOR_TYPE_GRAY && bd < 8) || png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_expand(png_ptr);

  /* Set the background color to draw transparent and alpha images over.
   * It is possible to set the red, green, and blue components directly
   * for paletted images instead of supplying a palette index.  Note that
   * even if the PNG file supplies a background, you are not required to
   * use it - you should use the (solid) application background if it has one.
   */

  background.red = background.green = background.blue = 0;

  png_set_background(png_ptr, &background, PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);

  if (png_set_interlace_handling(png_ptr) != 1)
    {
      fprintf(stderr, "%s: %s: Interlaced PNGs are not supported\n", program_name, path);
      exit(EXIT_FAILURE);
    }

  /* Optional call to gamma correct and add the background to the palette
   * and update info structure.  REQUIRED if you are expecting libpng to
   * update the palette for you (ie you selected such a transform above).
   */
  png_read_update_info(png_ptr, info_ptr);

  row_pointers = malloc(png_get_rowbytes(png_ptr, info_ptr));

  ASSERT(color == PNG_COLOR_TYPE_GRAY || color == PNG_COLOR_TYPE_GRAY_ALPHA);

  for (y = 0; y < mat_image_size_y_get(nimg); y++)
    {
      int x;
      png_read_rows(png_ptr, &row_pointers, NULL, 1);
      for (x = 0; x < mat_image_size_x_get(nimg); x ++)
        mat_image_core_set(nimg, x, y, row_pointers[x]);
    }

  free(row_pointers);

  /* read rest of file, and get additional chunks in info_ptr */
  png_read_end(png_ptr, info_ptr);

  /* clean up after the read, and free any memory allocated */
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

  return nimg;
}
#endif /* HAVE_LIBPNG */
