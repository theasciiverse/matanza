#include <matanza.h>

/* A structure for a team. */
struct MatTeam
{
  /* The maximum number of players allowed into this team. */
  int           max;

  /* The current number of players in this team. */
  int           cur;

  /* The universe this team belongs to. */
  MatScm       *un;

  /* The entry in the list of teams for the universe. */
  MatScm       *un_list;

  unsigned int  times_dead;

  /* The name of this team. */
  MatScm       *name;

  /* The list of ships in this team. */
  MatScm       *head;
  MatScm       *tail;
};

INLINE static MatTeam *data_get ( MatScm *obj );

INLINE static MatTeam *
data_get ( MatScm *obj )
{
  MatTeam *data;

  ASSERT(obj);
  ASSERT(mat_scm_obj_type_get(obj) == &mat_scm_team_type);

  data = mat_scm_obj_data_get(obj);

  ASSERT(data);
  ASSERT(data->name);
  ASSERT(mat_scm_obj_type_get(data->name) == &mat_scm_string_type);
  ASSERT(data->un);
  ASSERT(mat_scm_obj_type_get(data->un) == &mat_scm_universe_type);
  ASSERT(data->un_list);
  ASSERT(mat_scm_obj_type_get(data->un_list) == &mat_scm_cons_type);
  ASSERT(data->times_dead <= mat_universe_maxdead_get(data->un));

  return data;
}

MatScm *
mat_team_new ( MatScm *name, MatScm *un )
{
  MatTeam *t;
  MatScm  *team;

  t = malloc(sizeof(MatTeam));
  if (!t)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  team = mat_scm_obj_malloc(&mat_scm_team_type, t);

  t->name = name;

  t->max        = 0;
  t->cur        = 0;
  t->times_dead = 0;
  t->head       = NULL;
  t->tail       = NULL;

  t->un         = un;
  t->un_list    = mat_universe_team_add(un, team);

  return team;
}

INLINE MatScm *
mat_team_name_get ( MatScm *team )
{
  MatTeam *data = data_get(team);
  return data->name;
}

INLINE void
mat_team_ship_forall ( MatScm *team, void (*func) ( void *, MatScm * ), void *userdata )
{
  MatTeam *data = data_get(team);

  ASSERT(func);

  mat_dlist_forall(data->head, func, userdata);
}

INLINE void
mat_team_times_dead_set ( MatScm *team, unsigned int times )
{
  MatTeam *data = data_get(team);
  ASSERT(times <= mat_universe_maxdead_get(mat_team_universe_get(team)));
  data->times_dead = times;
}

INLINE unsigned int
mat_team_times_dead_get ( MatScm *team )
{
  MatTeam *data = data_get(team);
  return data->times_dead;
}

INLINE MatScm *
mat_team_ship_add ( MatScm *team, MatScm *ship )
{
  MatTeam *data = data_get(team);
  return mat_dlist_add_tail(&data->head, &data->tail, ship);
}

void
mat_team_ship_rm ( MatScm *team, MatScm *ship )
{
  MatTeam *data = data_get(team);
  mat_dlist_rm(&data->head, &data->tail, ship);
}

INLINE MatScm *
mat_team_universe_get ( MatScm *team )
{
  MatTeam *data = data_get(team);
  return data->un;
}

INLINE MatScm *
mat_team_ship_head_get ( MatScm *team )
{
  MatTeam *data = data_get(team);
  return data->head;
}

INLINE MatScm *
mat_team_ship_tail_get ( MatScm *team )
{
  MatTeam *data = data_get(team);
  return data->tail;
}

INLINE void
mat_team_mark ( void *funcdata )
{
  MatTeam *data = (MatTeam *) funcdata;
  mat_scm_obj_mark(data->un);
  mat_scm_obj_mark(data->un_list);
  mat_scm_obj_mark(data->name);
  mat_scm_obj_mark(data->head);
  mat_scm_obj_mark(data->tail);
}
