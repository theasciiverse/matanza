#include <matanza.h>

#if MAT_BUILD_MMP

/* Here we will use a structure to efficently keep track of what machines are
 * subscribed to what universes.
 *
 * It has to perform the following operations as fast as possible:
 *
 * - Return a list of all the machines in a given universe.
 * - Return a list of all the universes for a given machine.
 * - Get rid of all the subscriptions for a given machine.
 * - Get rid off a given subscription for one universe for one machine.
 *
 * The natural solution for this is a doubly linked list that can be traversed
 * by the universe (to get all the machines in a universe) or by the machine
 * (to get all the machines in a universe).
 *
 * m_prev and m_next point to the node for the previous and next
 * subscription for the current machine.  Actually, not the next node but the
 * next node in the current list in the hash of subscriptions for the machine.
 *
 * u_prev and u_next point to the node for the previous and next subscription
 * for the current universe.
 *
 * m and u point to the current machine and universe. */

struct MatSub
{
  MatScm              *m;

  MatSub              *m_prev;
  MatSub              *m_next;

  MatScm              *u;

  MatSub              *u_prev;
  MatSub              *u_next;
};

/* Lets make a function to get rid of one given subscription.
 *
 * This function is only called by code inside this section.
 *
 * s is the subscription you want to get rid off. */

static void
mat_sub_rm ( MatSub *s )
{
  int hf;

  hf = mat_universe_id_get(s->u) % MAT_SUB_MACHINE_HASH_SIZE;

  if (s->m_prev)
    s->m_prev->m_next = s->m_next;
  else
    mat_machine_subs_get(s->m)[hf] = s->m_next;

  if (s->m_next)
    s->m_next = s->m_prev;

  if (s->u_prev)
    s->u_prev->u_next = s->u_next;
  else
    *(mat_universe_subs_get(s->u)) = s->u_next;

  if (s->u_next)
    s->u_next->u_prev = s->u_prev;

  free(s);
}

void
mat_sub_machine_rm ( MatScm *m )
{
  MatSub *c;
  MatSub **msubs;
  int i;

  msubs = mat_machine_subs_get(m);

  for (i = 0; i < MAT_SUB_MACHINE_HASH_SIZE; i ++)
    {
      c = msubs[i];
      while (c)
        {
          MatSub *tmp;

          tmp = c->m_next;

          mat_sub_rm(c);

          c = tmp;
        }
    }
}

/* And now lets make a function that subscribes a machine to a universe
 * (depending on its id). */

int
mat_sub_add ( MatScm *m, MatScm *un )
{
  MatSub  *c;
  int      pos;
  MatSub **msubs;

  c = malloc(sizeof(MatSub));
  if (!c)
    return 0;

  c->u = un;
  c->m = m;

  c->u_prev = NULL;
  c->u_next = *mat_universe_subs_get(un);

  if (c->u_next)
    c->u_next->u_prev = c;

  *mat_universe_subs_get(un) = c;

  pos = mat_universe_id_get(un) % MAT_SUB_MACHINE_HASH_SIZE;

  msubs = mat_machine_subs_get(m);

  c->m_prev = NULL;
  c->m_next = msubs[pos];

  if (msubs[pos])
    msubs[pos]->m_prev = c;

  msubs[pos] = c;

  return 1;
}

INLINE void
mat_sub_machine_init ( MatScm *m )
{
  MatSub **msubs;
  int i;

  msubs = mat_machine_subs_get(m);

  for (i = 0; i < MAT_SUB_MACHINE_HASH_SIZE; i ++)
    msubs[i] = NULL;
}

INLINE MatScm *
mat_sub_universe_find_get ( MatSub *s, unsigned int id )
{
  while (s)
    if (mat_universe_id_get(s->u) == id)
      return s->u;
    else
      s = s->m_next;

  return NULL;
}

INLINE MatScm *
mat_sub_universe_get ( MatSub *s )
{
  ASSERT(s);
  ASSERT(s->u);
  return s->u;
}

INLINE MatScm *
mat_sub_machine_get ( MatSub *s )
{
  ASSERT(s);
  ASSERT(s->m);
  return s->m;
}

INLINE MatSub *
mat_sub_universe_next_get ( MatSub *s )
{
  return s->u_next;
}

INLINE MatSub *
mat_sub_machine_next_get ( MatSub *s )
{
  return s->m_next;
}

#endif
