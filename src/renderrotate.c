#include <matanza.h>

struct MatRenderRotate
{
  MatScm               *(*func) (void *data);
  void                   *userdata;
  unsigned int            ang;
  unsigned int            ang_total;
};

static MatScm            *redraw                           ( void               *rot );

static int                update                           ( void               *rot );


static int mat_scm_renderrotate_equal ( MatScm *img0, MatScm *img1 );

static void mat_scm_renderrotate_free ( void *funcdata );

MatScmType mat_scm_renderrotate_type =
{
  "renderrotate",
  free,
  NULL,
  NULL,
  NULL,
  NULL
};

MatScm *
mat_render_rotate_new ( MatScm *(*func) (void *), void *userdata, unsigned int ang )
{
  MatRenderRotate *data;
  MatScm *scmdata;

  data = malloc(sizeof(MatRenderRotate));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  ASSERT(func);

  data->func      = func;
  data->userdata  = userdata;
  data->ang       = 0;
  data->ang_total = ang;

  scmdata = mat_scm_obj_malloc(&mat_scm_renderrotate_type, data);

  return scmdata;
}

MatScm *
mat_render_rotate_widget_new ( MatScm *rotate, MatScm *window, int x, int y )
{
  ASSERT(rotate);
  ASSERT(rotate->type == &mat_scm_renderrotate_type);
  ASSERT(window);
  ASSERT(window->type == &mat_scm_window_type);
  return mat_widget_render_new(window, x, y, redraw, update, rotate);
}

static MatScm *
redraw ( void *funcdata )
{
  MatRenderRotate *rot;
  MatScm *img;

  ASSERT(mat_scm_obj_type_get(funcdata) == &mat_scm_renderrotate_type);

  rot = mat_scm_obj_data_get(funcdata);

  ASSERT(rot);
  ASSERT(rot->ang < rot->ang_total);
  ASSERT(rot->func);

  img = rot->func(rot->userdata);
  if (!img)
    return NULL;

  ASSERT(img);
  ASSERT(img->type == &mat_scm_imagevector_type);

  return mat_imagevector_get(img, rot->ang);
}

static int
update ( void *funcdata )
{
  MatRenderRotate *rot;

  ASSERT(mat_scm_obj_type_get(funcdata) == &mat_scm_renderrotate_type);

  rot = mat_scm_obj_data_get(funcdata);

  ASSERT(rot);
  ASSERT(rot->ang < rot->ang_total);

  rot->ang = (rot->ang + 1) % rot->ang_total;

  return 1;
}
