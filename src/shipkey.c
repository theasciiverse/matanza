#include <matanza.h>

void
mat_ship_key_pause ( MatScm *ship )
{
  MatScm *un;

  un = mat_movingobj_universe_get(ship);

  mat_universe_pause_set(un, ship, !mat_universe_pause_get(un));
}

void
mat_ship_key_up ( MatScm *ship )
{
  ASSERT(ship);

  if (!mat_ship_alive_get(ship))
    {
      double mod_y, mod_t;

      if (mat_movingobj_sp_y_get(ship) >= 0)
        mod_y = mat_movingobj_sp_y_get(ship) + 1.0 / arg_universe_update_steps;
      else
        mod_y = 0;

      mod_t = sqrt(mat_movingobj_sp_x_get(ship) * mat_movingobj_sp_x_get(ship) + mod_y * mod_y);
      mat_movingobj_sp_linear_real_set(ship, mat_movingobj_sp_x_get(ship), mod_y, mod_t);
    }
  else if (mat_ship_speed_inc_get(ship) == 0)
    {
      unsigned int     ang = mat_movingobj_ang_get(ship);
      MatScm          *un  = mat_movingobj_universe_get(ship);

      mat_ship_speed_inc_set(ship, 3);

      if (arg_space)
        {
          double mod_x, mod_y, mod_t;

          mod_x  = mat_movingobj_sp_x_get(ship);
          mod_x += mat_universe_cos_get(un, ang) / arg_universe_update_steps;

          mod_y  = mat_movingobj_sp_y_get(ship);
          mod_y += mat_universe_sin_get(un, ang) / arg_universe_update_steps;

          mod_t = sqrt(mod_x * mod_x + mod_y * mod_y);

          if (arg_speed_max == 0 || (-arg_speed_max < mod_t && mod_t < arg_speed_max))
            mat_movingobj_sp_linear_real_set(ship, mod_x, mod_y, mod_t);
        }
      else if (arg_speed_max == 0 || mat_movingobj_sp_linear_get(ship) + 1.0 / arg_universe_update_steps < arg_speed_max)
        {
          double sp = mat_movingobj_sp_linear_get(ship) + 1.0 / arg_universe_update_steps;

          mat_movingobj_sp_linear_real_set(ship,
                                           sp * mat_universe_cos_get(un, ang),
                                           sp * mat_universe_sin_get(un, ang),
                                           sp);
        }
    }
}

void
mat_ship_key_down ( MatScm *ship )
{
  ASSERT(ship);
  if (!mat_ship_alive_get(ship))
    {
      double mod_y, mod_t;

      if (mat_movingobj_sp_y_get(ship) <= 0)
        mod_y = mat_movingobj_sp_y_get(ship) - 1.0 / arg_universe_update_steps;
      else
        mod_y = 0;

      mod_t = sqrt(mat_movingobj_sp_x_get(ship) * mat_movingobj_sp_x_get(ship) + mod_y * mod_y);
      mat_movingobj_sp_linear_real_set(ship, mat_movingobj_sp_x_get(ship), mod_y, mod_t);
    }
  else if (mat_ship_speed_inc_get(ship) == 0 && arg_brake)
    {
      unsigned int     ang = mat_movingobj_ang_get(ship);
      MatScm          *un  = mat_movingobj_universe_get(ship);

      mat_ship_speed_inc_set(ship, 3);

      if (arg_space)
        {
          double mod_x, mod_y, mod_t;

          mod_x  = mat_movingobj_sp_x_get(ship);
          mod_x -= mat_universe_cos_get(un, ang) / arg_universe_update_steps;

          mod_y  = mat_movingobj_sp_y_get(ship);
          mod_y -= mat_universe_sin_get(un, ang) / arg_universe_update_steps;

          mod_t = sqrt(mod_x * mod_x + mod_y * mod_y);

          if (arg_speed_max == 0 || (-arg_speed_max < mod_t && mod_t < arg_speed_max))
            mat_movingobj_sp_linear_real_set(ship, mod_x, mod_y, mod_t);
        }
      else if (arg_speed_max == 0 || -arg_speed_max < mat_movingobj_sp_linear_get(ship) - 1.0 / arg_universe_update_steps)
        {
          double sp = mat_movingobj_sp_linear_get(ship) - 1.0 / arg_universe_update_steps;

          if (!arg_fly_back && 0.0 < sp)
            sp = 0;

          mat_movingobj_sp_linear_real_set(ship,
                                           sp * mat_universe_cos_get(un, ang),
                                           sp * mat_universe_sin_get(un, ang),
                                           sp);
        }
    }
}

void
mat_ship_key_right ( MatScm *ship )
{
  if (!mat_ship_alive_get(ship))
    {
      double mod_x, mod_t;

      if (mat_movingobj_sp_x_get(ship) >= 0)
        mod_x = mat_movingobj_sp_x_get(ship) + 1.0 / arg_universe_update_steps;
      else
        mod_x = 0;

      mod_t = sqrt(mod_x * mod_x + mat_movingobj_sp_y_get(ship) * mat_movingobj_sp_y_get(ship));
      mat_movingobj_sp_linear_real_set(ship, mod_x, mat_movingobj_sp_y_get(ship), mod_t);
    }
  else if (0 < arg_ang_speed_max)
    {
      double sp = mat_movingobj_sp_ang_get(ship);

      if (0 < sp)
        mat_movingobj_sp_ang_set(ship, 0);
      else if (-arg_ang_speed_max < sp)
        mat_movingobj_sp_ang_set(ship, sp - 1.0 / arg_universe_update_steps);
    }
  else
   {
      MatScm            *un;
      unsigned int      ang;

      un = mat_movingobj_universe_get(ship);
      ASSERT(un);

      if (ang == 0)
        ang = mat_universe_angles_get(un);
      else
        ang = ang - 1;

      mat_movingobj_ang_set(ship, ang);

      if (!arg_space)
        {
          double sp, ang;

          sp = mat_movingobj_sp_linear_get(ship);

          mat_movingobj_sp_linear_set(ship,
                                      sp * mat_universe_cos_get(un, ang),
                                      sp * mat_universe_sin_get(un, ang));
        }
    }
}

void
mat_ship_key_left ( MatScm *ship )
{
  if (!mat_ship_alive_get(ship))
    {
      double mod_x, mod_t;

      if (mat_movingobj_sp_x_get(ship) <= 0)
        mod_x = mat_movingobj_sp_x_get(ship) - 1.0 / arg_universe_update_steps;
      else
        mod_x = 0;

      mod_t = sqrt(mod_x * mod_x + mat_movingobj_sp_y_get(ship) * mat_movingobj_sp_y_get(ship));
      mat_movingobj_sp_linear_real_set(ship, mod_x, mat_movingobj_sp_y_get(ship), mod_t);
    }
  else if (0 < arg_ang_speed_max)
    {
      double sp = mat_movingobj_sp_ang_get(ship);

      if (sp < 0)
        mat_movingobj_sp_ang_set(ship, 0);
      else if (sp < arg_ang_speed_max)
        mat_movingobj_sp_ang_set(ship, sp + 1.0 / arg_universe_update_steps);
    }
  else
    {
      MatScm            *un;
      unsigned int      ang;

      un = mat_movingobj_universe_get(ship);
      ASSERT(un);

      if (ang == mat_universe_angles_get(un))
        ang = 0;
      else
        ang = ang + 1;

      mat_movingobj_ang_set(ship, ang);

      if (!arg_space)
        {
          double sp;

          sp = mat_movingobj_sp_linear_get(ship);

          mat_movingobj_sp_linear_set(ship,
                                      sp * mat_universe_cos_get(un, ang),
                                      sp * mat_universe_sin_get(un, ang));
        }
    }
}
