#include <matanza.h>

typedef struct MatShipCrashShip MatShipCrashShip;

struct MatShipCrashShip
{
  /* The ship that died. */
  char victim[MAT_USER_MAX + sizeof(" of ") - 1 + MAT_TEAM_NAME_MAX + 1];

  /* The ship that killed it. */
  char source[MAT_USER_MAX + sizeof(" of ") - 1 + MAT_TEAM_NAME_MAX + 1];

  /* Did the victim lost the game? */
  int                  out;
};

typedef struct MatShipCrashAsteroid MatShipCrashAsteroid;

struct MatShipCrashAsteroid
{
  /* The ship that died. */
  char victim[MAT_USER_MAX + sizeof(" of ") - 1 + MAT_TEAM_NAME_MAX + 1];

  /* Did the victim lost the game? */
  int                  out;
};

typedef struct MatShipCrashMissile MatShipCrashMissile;

struct MatShipCrashMissile
{
  /* The ship that died. */
  char victim[MAT_USER_MAX + sizeof(" of ") - 1 + MAT_TEAM_NAME_MAX + 1];

  /* The ship that killed it. */
  char source[MAT_USER_MAX + sizeof(" of ") - 1 + MAT_TEAM_NAME_MAX + 1];

  /* Did the victim lost the game? */
  int                  out;
};

static int                cr_kill                          ( MatScm             *ship );

INLINE static void        cr_ship                          ( MatScm             *ship,
                                                             MatScm             *other );

INLINE static void        cr_dead_team                     ( void               *dummy,
                                                             MatScm             *ship );

INLINE static void        cr_ship_dead                     ( void               *funcdata,
                                                             MatScm             *obj );

INLINE static void        cr_asteroid                      ( MatScm             *ship,
                                                             MatScm             *ast );

INLINE static void        cr_asteroid_dead                 ( void                 *d,
                                                             MatScm             *obj );

INLINE static void        cr_missile                       ( MatScm             *ship,
                                                             MatScm             *missile );

INLINE static void        cr_missile_dead                  ( void               *d,
                                                             MatScm             *obj );

static void               ship_name                        ( char               *buffer,
                                                             MatScm             *obj );

void
mat_ship_crash ( MatScm *ship, MatScm *other )
{
  ASSERT(ship);
  //ASSERT(mat_movingobj_type_get(ship) == MAT_MOVINGOBJ_TYPE_SHIP);
  ASSERT(mat_movingobj_event_crash_get(ship));

  ASSERT(other);
  ASSERT(mat_movingobj_event_crash_get(other));

  ASSERT(mat_movingobj_universe_get(ship) == mat_movingobj_universe_get(other));

  //switch (mat_movingobj_type_get(other))
    //{
    //case MAT_MOVINGOBJ_TYPE_SHIP:
      cr_ship(ship, other);
      //break;
#if 0
    case MAT_MOVINGOBJ_TYPE_ASTEROID:
      cr_asteroid(ship, other);
      break;
    case MAT_MOVINGOBJ_TYPE_MISSILE:
      cr_missile(ship, other);
      break;
#endif
    //}
}

static int
cr_kill ( MatScm *ship )
{
  MatScm *team;
  int sent_out;
  unsigned int dead;

  team = mat_ship_team_get(ship);

  if (team)
    mat_team_times_dead_set(team, mat_team_times_dead_get(team) + 1);

  dead = mat_ship_times_dead_get(ship) + 1;
  mat_ship_times_dead_set(ship, dead);

  if (team && arg_teams_share)
    sent_out = mat_team_times_dead_get(team) >= mat_universe_maxdead_get(mat_movingobj_universe_get(mat_ship_movingobj_get(ship)));
  else
    sent_out = dead                          >= mat_universe_maxdead_get(mat_movingobj_universe_get(mat_ship_movingobj_get(ship)));

  if (!sent_out)
    {
      if (mat_ship_team_get(ship) && arg_teams_share)
        mat_team_ship_forall(mat_ship_team_get(ship), cr_dead_team, NULL);
      else
        mat_movingobj_message_printf(ship, "-=> You have lost %d ships now !", mat_ship_times_dead_get(ship));

      mat_ship_start(ship);
    }
  else
    {
      mat_movingobj_message_printf(ship, "-=> Game over !");
      //mat_ship_remove(ship);
    }

  return sent_out;
}

INLINE static void
cr_ship ( MatScm *ship, MatScm *other )
{
  unsigned int dmg;

  ASSERT(ship);
  ASSERT(other);

  ASSERT(mat_movingobj_event_crash_get(ship));
  ASSERT(mat_movingobj_event_crash_get(other));

  ASSERT(mat_movingobj_universe_get(ship) == mat_movingobj_universe_get(other));

  if (!arg_space)
    return;

  dmg = mat_universe_ship_dmg_get(mat_movingobj_universe_get(ship));

  if (mat_movingobj_health_get(ship) > dmg)
    {
      mat_movingobj_health_set(ship, mat_movingobj_health_get(ship) - dmg);

      if (dmg)
        mat_movingobj_message_printf(ship, "-=> You crashed against player %s ! [Health:%d%%]",
                                mat_scm_string_data_get(mat_ship_name_get(other)),
                                mat_movingobj_health_get(ship));

      mat_movingobj_sp_x_set(ship, -mat_movingobj_sp_x_get(ship));
      mat_movingobj_sp_y_set(ship, -mat_movingobj_sp_y_get(ship));
    }
  else
    {
      MatShipCrashShip info;

      MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> player killed: crash with ship\n");

      mat_movingobj_health_set(ship, 0);

      ship_name(info.victim, ship);
      ship_name(info.source, other);

      info.out = cr_kill(ship);

      mat_universe_movingobj_forall(mat_movingobj_universe_get(ship), cr_ship_dead, &info);

      MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- player killed: crash with ship\n");
    }
}

INLINE static void
cr_dead_team ( void *dummy, MatScm *ship )
{
  mat_movingobj_message_printf(ship, "-=> [%s] Arg, we have lost %d ships now !",
                          mat_team_name_get(mat_ship_team_get(ship)),
                          mat_team_times_dead_get(mat_ship_team_get(ship)));
}

INLINE static void
cr_ship_dead ( void *funcdata, MatScm *obj )
{
  MatShipCrashShip *d = (MatShipCrashShip *) funcdata;

  ASSERT(d);
  //ASSERT(mat_movingobj_type_get(obj) == MAT_MOVINGOBJ_TYPE_SHIP);

  mat_movingobj_message_printf(obj, "-=> BOOM ! %s died in crash against %s.", d->victim, d->source);

  if (d->out)
    mat_movingobj_message_printf(obj, "-=> %s is sent out of the game.", d->victim);
}

#if 0
INLINE static void
cr_asteroid ( MatScm *ship, MatScm *ast )
{
  unsigned int dmg;

  ASSERT(ship);
  ASSERT(ast);

  ASSERT(mat_movingobj_crash_get(ship));
  ASSERT(mat_movingobj_crash_get(ast));

  ASSERT(mat_movingobj_universe_get(ship) == mat_movingobj_universe_get(ast));

  if (!arg_space)
    return;

  /* Make damage. */

  dmg = mat_universe_asteroid_dmg_get(mat_movingobj_universe_get(ship));

  if (mat_movingobj_health_get(ship) > dmg)
    {
      mat_movingobj_health_set(ship, mat_movingobj_health_get(ship) - dmg);

      mat_movingobj_message_printf(ship, "-=> You crashed against an asteroid ! [Health:%d%%]", mat_movingobj_health_get(ship));

      mat_movingobj_sp_x_set(ship, -mat_movingobj_sp_x_get(ship));
      mat_movingobj_sp_y_set(ship, -mat_movingobj_sp_y_get(ship));
    }
  else
    {
      MatShipCrashAsteroid info;

      MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> player killed: crash with asteroid\n");

      mat_movingobj_health_set(ship, 0);

      ship_name(info.victim, ship);
      info.out = cr_kill(ship);

      mat_universe_movingobj_forall(mat_movingobj_universe_get(ship), MAT_MOVINGOBJ_TYPE_SHIP, cr_asteroid_dead, &info);

      MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- player killed: crash with asteroid\n");
    }
}

INLINE static void
cr_asteroid_dead ( void *funcdata, MatScm *obj )
{
  MatShipCrashAsteroid *d = (MatShipCrashAsteroid *) funcdata;

  ASSERT(d);
  ASSERT(mat_movingobj_type_get(obj) == MAT_MOVINGOBJ_TYPE_SHIP);

  mat_movingobj_message_printf(obj, "-=> KABOOM ! %s is destroyed by an asteroid . . .", d->victim);

  if (d->out)
    mat_movingobj_message_printf(obj, "-=> %s is sent out of the game.", d->victim);
}
#endif

#if 0
INLINE static void
cr_missile ( MatScm *ship, MatScm *missile )
{
  unsigned int dmg;
  MatScm *source;

  ASSERT(ship);
  ASSERT(missile);

  ASSERT(mat_movingobj_crash_get(ship));
  ASSERT(mat_movingobj_crash_get(missile));

  ASSERT(mat_movingobj_universe_get(ship) == mat_movingobj_universe_get(missile));

  if (!mat_missile_ttl_get(missile))
    return;

  if (mat_ship_movingobj_get(mat_missile_source_get(missile)) == mat_ship_movingobj_get(ship))
    return;

  /* Remove the bullet. */
  mat_missile_ttl_set(missile, 0);

  source = mat_missile_source_get(missile);
  ASSERT(source);

  dmg = mat_missile_dmg_get(missile);

  if (mat_movingobj_health_get(ship) > dmg)
    {
      mat_movingobj_health_set(ship, mat_movingobj_health_get(ship) - dmg);

      switch (mat_movingobj_type_get(source))
        {
        case MAT_MOVINGOBJ_TYPE_SHIP:

          mat_movingobj_message_printf(ship, "-=> Argh! %s got you! [Health:%u%%]",
                                  mat_scm_string_data_get(mat_ship_name_get(mat_movingobj_data_get(source))),
                                  mat_movingobj_health_get(ship));

          mat_movingobj_message_printf(mat_movingobj_data_get(source),
                                  mat_missile_hit_ship_get(missile),
                                  mat_scm_string_data_get(mat_ship_name_get(ship)));
          break;

        case MAT_MOVINGOBJ_TYPE_ASTEROID:
          mat_movingobj_message_printf(ship, "-=> Argh! Some asteroid got you! [Health:%u%%]",
                                  mat_movingobj_health_get(ship));
          break;

        default:
          ASSERT(0);
        }
    }
  else
    {
      MatScm *source;
      MatShipCrashMissile info;

      MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> player killed: missile\n");

      mat_movingobj_health_set(ship, 0);

      ship_name(info.victim, ship);
      switch (mat_movingobj_type_get(source))
        {
        case MAT_MOVINGOBJ_TYPE_SHIP:
          ship_name(info.source, source);
          break;

        case MAT_MOVINGOBJ_TYPE_ASTEROID:
          sprintf(info.source, "an asteroid");
          break;

        default:
          ASSERT(0);
        }

      info.out = cr_kill(ship);

      mat_universe_movingobj_forall(mat_movingobj_universe_get(ship), MAT_MOVINGOBJ_TYPE_SHIP, cr_missile_dead, &info);

      MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- player killed: missile\n");
    }
}

static void
cr_missile_dead ( void *funcdata, MatScm *obj )
{
  MatShipCrashMissile *d = (MatShipCrashMissile *) funcdata;

  ASSERT(d);
  ASSERT(mat_movingobj_type_get(obj) == MAT_MOVINGOBJ_TYPE_SHIP);

  mat_movingobj_message_printf(obj, "-=> KABOOM ! %s is destroyed by %s !", d->victim, d->source);

  if (d->out)
    mat_movingobj_message_printf(obj, "-=> %s is sent out of the game.", d->victim);
}
#endif

static void
ship_name ( char *buffer, MatScm *obj )
{
  int pos;

  ASSERT(buffer);

  pos = sprintf(buffer, "%s", mat_scm_string_data_get(mat_ship_name_get(obj)));

  if (mat_ship_team_get(obj))
    sprintf(buffer + pos, " of %s", mat_team_name_get(mat_ship_team_get(obj)));
}
