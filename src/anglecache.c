#include <matanza.h>

#define PI 3.141592

struct MatAngleCache
{
  double          *cos;
  double          *sin;
  unsigned int     ang;
};

MatAngleCache *
mat_anglecache_new ( unsigned int arg_ang )
{
  unsigned int i;
  MatAngleCache *c;

  c = malloc(sizeof(MatAngleCache));
  if (!c)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  c->ang = arg_ang;

  c->cos = malloc(sizeof(double) * arg_ang);
  c->sin = malloc(sizeof(double) * arg_ang);
  if (!c->cos || !c->sin)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  for (i = 0; i < arg_ang; i ++)
    {
      double ang;

      ang = PI * 2.0 * (double) i / (double) arg_ang;

      c->cos[i] = cos(ang);
      c->sin[i] = sin(ang);
    }

  return c;
}

void
mat_anglecache_free ( MatAngleCache *c )
{
  ASSERT(c);
  free(c->cos);
  free(c->sin);
  free(c);
}

INLINE double
mat_anglecache_sin ( MatAngleCache *m, unsigned int ang )
{
  ASSERT(m);
  ASSERT(ang < m->ang);
  return m->sin[ang];
}

INLINE double
mat_anglecache_cos ( MatAngleCache *m, unsigned int ang )
{
  ASSERT(m);
  ASSERT(ang < m->ang);
  return m->cos[ang];
}

unsigned int
mat_anglecache_best ( MatAngleCache *m, double dx, double dy, double dist )
{
  double ang;
 
  ASSERT(m);
  ASSERT(0 < dx * dx + dy * dy);
  ASSERT(0 < dist);

  dist = sqrt(dist);

  dx /= dist;
  dy /= dist;


  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("-=> mat_anglecache_best\n");
 
  ang = acos(dx);

  if (dy < 0.0)
    ang = 2 * PI - ang;

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("best = %f (%d)\n", ang, (int) ang);

  ang = ((double) m->ang * ang) / (2.0 * PI);

  ASSERT(0 <= (int) ang);
  ASSERT((int) ang < m->ang);

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("<=- mat_anglecache_best\n");

  return (int) ang;
}
