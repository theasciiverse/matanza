#include <matanza.h>

static void               team_free                        ( void               *data );

static MatScmCallback     t_prop;

MatScmType mat_scm_team_type =
{
  "team",
  team_free,
  mat_team_mark,
  NULL,
  NULL,
  NULL
};

MatScm                   *mat_scm_team_name_get = NULL;
MatScm                   *mat_scm_team_ship_head_get = NULL;
MatScm                   *mat_scm_team_ship_tail_get = NULL;

static void
team_free ( void *data)
{
  free(data);
}

void
mat_scm_team_init ( void )
{
  mat_scm_obj_protect(&mat_scm_team_name_get);
  mat_scm_obj_protect(&mat_scm_team_ship_head_get);
  mat_scm_obj_protect(&mat_scm_team_ship_tail_get);

  mat_scm_team_name_get = mat_scm_proc_new(t_prop, mat_team_name_get, 1);
  mat_scm_team_ship_head_get = mat_scm_proc_new(t_prop, mat_team_ship_head_get, 1);
  mat_scm_team_ship_tail_get = mat_scm_proc_new(t_prop, mat_team_ship_tail_get, 1);
}

static MatScm *
t_prop ( void *data, MatScm *env, MatScm *args )
{
  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_team_type);
  ASSERT(!mat_scm_cons_cdr_get(args));

  return ((MatScm *(*) (MatScm *))data)(mat_scm_cons_car_get(args));
}
