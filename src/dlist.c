#include <matanza.h>

INLINE MatScm *
mat_dlist_add_head ( MatScm **head, MatScm **tail, MatScm *obj )
{
  MatScm *node = mat_scm_cons_new(obj, mat_scm_cons_new(NULL, *head));

  if (*head)
    {
      ASSERT(*tail);
      mat_scm_cons_car_set(mat_scm_cons_cdr_get(*head), node);
    }
  else
    {
      ASSERT(!*tail);
      *tail = node;
    }

  *head = node;

  return node;
}

INLINE MatScm *
mat_dlist_add_tail ( MatScm **head, MatScm **tail, MatScm *obj )
{
  MatScm *node = mat_scm_cons_new(obj, mat_scm_cons_new(*tail, NULL));

  if (*head)
    {
      ASSERT(*tail);
      mat_scm_cons_cdr_set(mat_scm_cons_cdr_get(*tail), node);
    }
  else
    {
      ASSERT(!*tail);
      *head = node;
    }

  *tail = node;

  return node;
}

INLINE MatScm *
mat_dlist_add_before ( MatScm **head, MatScm **tail, MatScm *next, MatScm *obj )
{
  MatScm *node;

  if (!next)
    return mat_dlist_add_tail(head, tail, obj);

  node = mat_scm_cons_new(obj, mat_scm_cons_new(mat_dlist_prev_get(next), next));

  mat_scm_cons_car_set(mat_scm_cons_cdr_get(next), node);

  if (mat_dlist_prev_get(node))
    {
      ASSERT(*head != next);
      mat_scm_cons_cdr_set(mat_scm_cons_cdr_get(mat_dlist_prev_get(node)), node);
    }
  else
    {
      ASSERT(*head == next);
      *head = node;
    }

  return node;
}

INLINE MatScm *
mat_dlist_add_after ( MatScm **head, MatScm **tail, MatScm *prev, MatScm *obj )
{
  MatScm *node;

  if (!prev)
    return mat_dlist_add_head(head, tail, obj);

  node = mat_scm_cons_new(obj, mat_scm_cons_new(prev, mat_dlist_next_get(prev)));

  mat_scm_cons_cdr_set(mat_scm_cons_cdr_get(prev), node);

  if (mat_dlist_next_get(node))
    {
      ASSERT(*tail != prev);
      mat_scm_cons_car_set(mat_scm_cons_cdr_get(mat_dlist_next_get(node)), node);
    }
  else
    {
      ASSERT(*tail == prev);
      *tail = node;
    }

  return node;
}

INLINE void
mat_dlist_rm ( MatScm **head, MatScm **tail, MatScm *node )
{
  ASSERT(*head != node || !mat_scm_cons_car_get(mat_scm_cons_cdr_get(node)));
  ASSERT(*tail != node || !mat_scm_cons_cdr_get(mat_scm_cons_cdr_get(node)));
  ASSERT((!*head) == (!*tail));

  node = mat_scm_cons_cdr_get(node);

  ASSERT(node);
  ASSERT(mat_scm_obj_type_get(node) == &mat_scm_cons_type);

  if (mat_scm_cons_car_get(node))
    mat_scm_cons_cdr_set(mat_scm_cons_cdr_get(mat_scm_cons_car_get(node)), mat_scm_cons_cdr_get(node));
  else
    *head = mat_scm_cons_cdr_get(node);

  if (mat_scm_cons_cdr_get(node))
    mat_scm_cons_car_set(mat_scm_cons_cdr_get(mat_scm_cons_cdr_get(node)), mat_scm_cons_car_get(node));
  else
    *tail = mat_scm_cons_car_get(node);

  ASSERT((!*head) == (!*tail));
}

INLINE void
mat_dlist_forall ( MatScm *head, void (*func) ( void *, MatScm * ), void *data )
{
  ASSERT(func);
  while (head)
    {
      func(data, mat_dlist_obj_get(head)); 
      head = mat_dlist_next_get(head);
    }
}

INLINE MatScm *
mat_dlist_prev_get ( MatScm *ent )
{
  return mat_scm_cons_car_get(mat_scm_cons_cdr_get(ent));
}

INLINE MatScm *
mat_dlist_next_get ( MatScm *ent )
{
  return mat_scm_cons_cdr_get(mat_scm_cons_cdr_get(ent));
}

INLINE MatScm *
mat_dlist_obj_get ( MatScm *ent )
{
  return mat_scm_cons_car_get(ent);
}
