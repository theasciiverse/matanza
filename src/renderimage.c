#include <matanza.h>

static MatScm            *mat_render_image_redraw          ( void               *funcdata );

static int ri_equal ( MatScm *, MatScm * );

static void ri_mark ( void *funcdata );

static void ri_free ( void *funcdata );

MatScmType mat_scm_renderimage_type =
{
  "renderimage",
  ri_free,
  ri_mark,
  NULL,
  NULL,
  ri_equal
};

static int
ri_equal ( MatScm *ri0, MatScm *ri1 )
{
  return mat_scm_obj_equal(mat_scm_obj_data_get(ri0), mat_scm_obj_data_get(ri1));
}

static void
ri_mark ( void *funcdata )
{
  MatScm *data = (MatScm *) funcdata;

  ASSERT(data);
  ASSERT(data->type == &mat_scm_image_type);

  mat_scm_obj_mark(data);
}

static void
ri_free ( void *funcdata )
{
  free(funcdata);
}

MatScm *
mat_render_image_new ( MatScm *img )
{
  return mat_scm_obj_malloc(&mat_scm_renderimage_type, img);
}

MatScm *
mat_render_image_widget_new ( MatScm *img, MatScm *window, int x, int y )
{
  return mat_widget_render_new(window, x, y, mat_render_image_redraw, NULL, img);
}

static MatScm *
mat_render_image_redraw ( void *funcdata )
{
  MatScm *data = (MatScm *) funcdata;

  ASSERT(data);
  ASSERT(data->type == &mat_scm_image_type);

  return data;
}
