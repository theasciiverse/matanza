#include <matanza.h>

typedef struct MatScmProtect MatScmProtect;

struct MatScmProtect
{
  MatScm **var;
  MatScmProtect *next;
};

static MatScmCallback scm_type;

MatScm *mat_scm_obj_type = NULL;

/* List of all MatScm objects */
static MatScm *obj_head = NULL;
static MatScm *obj_tail = NULL;

static MatScmProtect *obj_protect = NULL;

/* It is safe to use objects after this position in the list */
static MatScm *obj_free = NULL;

/* Total number of objects in the list */
static int obj_count = 0;

/* Number of free objects (objects after obj_free) */
static int obj_count_free = 0;

static int gc_mark = 0;

MatScm *
mat_scm_obj_eval ( MatScm *obj, MatScm *env )
{
  if (!obj)
    return obj;

  if (mat_scm_obj_type_get(obj) == &mat_scm_cons_type)
    return mat_scm_obj_call(mat_scm_obj_eval(mat_scm_cons_car_get(obj), env), mat_scm_cons_cdr_get(obj), env, 1);
  else if (mat_scm_obj_type_get(obj) == &mat_scm_symbol_type)
    return mat_scm_env_find(env, mat_scm_symbol_data_get(obj));
  else
    return obj;
}

INLINE MatScm *
mat_scm_obj_call ( MatScm *obj, MatScm *args, MatScm *env, int evalargs )
{
  ASSERT(obj);

  if (mat_scm_obj_type_get(obj) == &mat_scm_proc_type)
    evalargs &= mat_scm_proc_evalargs_get(obj);

  if (evalargs)
    {
      MatScm *head = NULL, *tail = NULL;

      while (args)
        {
          MatScm *obj;

          ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);

          obj = mat_scm_cons_new(mat_scm_obj_eval(mat_scm_cons_car_get(args), env), NULL);
          if (!head)
            head = tail = obj;
          else
            {
              mat_scm_cons_cdr_set(tail, obj);
              tail = obj;
            }

          args = mat_scm_cons_cdr_get(args);
        }
      args = head;
    }

  if (mat_scm_obj_type_get(obj) == &mat_scm_proc_type)
    return mat_scm_proc_call(mat_scm_obj_data_get(obj), env, args);
  else if (mat_scm_obj_type_get(obj) == &mat_scm_closure_type)
    return mat_scm_closure_call(mat_scm_obj_data_get(obj), env, args);
  else
    ASSERT(0);
}

void
mat_scm_init ( void )
{
  mat_scm_obj_protect(&mat_scm_obj_type);

  mat_scm_obj_type = mat_scm_proc_new(scm_type, NULL, 1);

  mat_scm_cons_init();
  mat_scm_char_init();
  mat_scm_string_init();
  mat_scm_symbol_init();
  mat_scm_number_init();
  mat_scm_bool_init();
  mat_scm_special_init();
  mat_scm_closure_init();
  mat_scm_list_init();
  mat_scm_env_init();
  mat_scm_load_init();
  mat_scm_if_init();
  mat_scm_case_init();
  mat_scm_cond_init();
  mat_scm_define_init();
  mat_scm_eq_init();
  mat_scm_quote_init();
  mat_scm_begin_init();
  mat_scm_let_init();
  mat_scm_image_init();
  mat_scm_imagevector_init();
  mat_scm_shiptype_init();
  mat_scm_ship_init();
  mat_scm_movingobj_init();
  mat_scm_universe_init();
  mat_scm_universetype_init();
  mat_scm_verbose_init();
}

void *
mat_scm_obj_data_get ( MatScm *obj )
{
  ASSERT(obj);
  return obj->data;
}

MatScmType *
mat_scm_obj_type_get ( MatScm *obj )
{
  ASSERT(obj);
  return obj->type;
}

void
mat_scm_obj_mark ( MatScm *obj )
{
  if (!obj || obj->gc_mark == gc_mark)
    return;

  obj->gc_mark = gc_mark;

  obj_count_free --;

  if (obj->type->mark)
    obj->type->mark(obj->data);

  /* TODO: I'm sure the following can be optimized */

  if (obj_free == obj)
    obj_free = obj->next;

  if (obj->prev)
    obj->prev->next = obj->next;
  else
    obj_head = obj->next;

  if (obj->next)
    obj->next->prev = obj->prev;
  else
    obj_tail = obj->prev;

  obj->prev = NULL;
  obj->next = obj_head;
  obj_head->prev = obj;
  obj_head = obj;
}

static void
mat_scm_obj_malloc_chunk ( unsigned int n )
{
  MatScm *head = NULL, *tail = NULL;
  unsigned int i;

  /* Create n free objects. */

  for (i = 0; i < n; i ++)
    {
      MatScm *obj;

      obj = malloc(sizeof(MatScm));

      obj->type = NULL;

      if (!head)
        head = tail = obj;
      else
        {
          tail->next = obj;
          obj->prev = tail;
          tail = obj;
        }
    }

  obj_count += i;
  obj_count_free += i;

  head->prev = obj_tail;
  tail->next = NULL;

  if (obj_tail)
    obj_tail->next = head;
  else
    obj_head = head;

  obj_tail = tail;

  obj_free = head;
}

MatScm *
mat_scm_obj_malloc ( MatScmType *type, void *data )
{
  MatScm *obj;

  if (!obj_free)
    mat_scm_obj_malloc_chunk(1000);


  ASSERT(obj_free);

  obj = obj_free;
  obj_free = obj_free->next;

  obj->gc_mark = gc_mark;

  if (obj->type && obj->type->free)
    obj->type->free(obj->data);

  obj->type = type;
  obj->data = data;

  obj_count_free --;

  return obj;
}

void
mat_scm_obj_protect ( MatScm **var )
{
  MatScmProtect *node;

  node = malloc(sizeof(MatScmProtect));

  node->var = var;
  node->next = obj_protect;

  obj_protect = node;
}

void
mat_scm_obj_gc ( void )
{
  MatScmProtect *tmp;

  if (obj_count_free >= obj_count * 0.2)
    return;

  gc_mark = !gc_mark;

  obj_free = obj_head;
  obj_count_free = obj_count;

  for (tmp = obj_protect; tmp; tmp = tmp->next)
    mat_scm_obj_mark(*tmp->var);

  while (obj_count_free < obj_count * 0.5)
    mat_scm_obj_malloc_chunk(obj_count * 0.5 - obj_count_free + 1000);
}

void
mat_scm_obj_set ( MatScm *obj, MatScmType *type, void *data )
{
  ASSERT(obj);
  obj->type = type;
  obj->data = data;
  obj->prev = obj->next = NULL;
}

int
mat_scm_obj_eq ( MatScm *obj0, MatScm *obj1 )
{
  if (obj0 == NULL || obj1 == NULL)
    return obj0 == obj1;

  if (mat_scm_obj_type_get(obj0) != mat_scm_obj_type_get(obj1))
    return 0;

  if (!obj0->type->eq)
    return obj0 == obj1;

  return obj0->type->eq(obj0, obj1);
}

int
mat_scm_obj_eqv ( MatScm *obj0, MatScm *obj1 )
{
  if (obj0 == NULL || obj1 == NULL)
    return obj0 == obj1;

  if (mat_scm_obj_type_get(obj0) != mat_scm_obj_type_get(obj1))
    return 0;

  if (!obj0->type->eqv)
    return obj0 == obj1;

  return obj0->type->eqv(obj0, obj1);
}

int
mat_scm_obj_equal ( MatScm *obj0, MatScm *obj1 )
{
  if (obj0 == NULL || obj1 == NULL)
    return obj0 == obj1;

  if (mat_scm_obj_type_get(obj0) != mat_scm_obj_type_get(obj1))
    return 0;

  if (!obj0->type->equal)
    return obj0 == obj1;

  return obj0->type->equal(obj0, obj1);
}

static MatScm *
scm_type ( void *data, MatScm *env, MatScm *args )
{
  char *name;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(!mat_scm_cons_cdr_get(args));

  name = mat_scm_cons_car_get(args) ? mat_scm_obj_type_get(mat_scm_cons_car_get(args))->name : "cons";

  return mat_scm_symbol_new(name, strlen(name));
}
