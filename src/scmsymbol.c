#include <matanza.h>

typedef struct MatScmSymbol MatScmSymbol;

struct MatScmSymbol
{
  char *buffer;
  int   buflen;
};

static int                sym_eqv                          ( MatScm             *obj0,
                                                             MatScm             *obj1 );

static void               sym_free                         ( void               *funcdata );

static MatScmCallback     sym_str;

MatScmType mat_scm_symbol_type =
{
  "symbol",
  sym_free,
  NULL,
  sym_eqv,
  sym_eqv,
  sym_eqv
};

void
mat_scm_symbol_init ( void )
{
  mat_scm_obj_protect(&mat_scm_symbol_string);

  mat_scm_symbol_string = mat_scm_proc_new(sym_str, NULL, 1);
}

MatScm *
mat_scm_symbol_new ( char *buffer, int buflen )
{
  MatScmSymbol *data;

  data = malloc(sizeof(MatScmSymbol));

  data->buffer = buffer;
  data->buflen = buflen;

  return mat_scm_obj_malloc(&mat_scm_symbol_type, data);
}

static void
sym_free ( void *funcdata )
{
  MatScmSymbol *data = (MatScmSymbol *) funcdata;

  free(data->buffer);
  free(data);
}

char *
mat_scm_symbol_data_get ( MatScm *obj )
{
  MatScmSymbol *data;

  ASSERT(obj);
  ASSERT(mat_scm_obj_type_get(obj) == &mat_scm_symbol_type);

  data = (MatScmSymbol *) mat_scm_obj_data_get(obj);
  ASSERT(data);

  return data->buffer;
}

static int
sym_eqv ( MatScm *obj0, MatScm *obj1 )
{
  return !strcmp(mat_scm_symbol_data_get(obj0), mat_scm_symbol_data_get(obj1));
}

static MatScm *
sym_str ( void *data, MatScm *env, MatScm *args )
{
  MatScmSymbol *sym;

  ASSERT(args);
  ASSERT(mat_scm_obj_type_get(args) == &mat_scm_cons_type);
  ASSERT(mat_scm_cons_car_get(args));
  ASSERT(mat_scm_obj_type_get(mat_scm_cons_car_get(args)) == &mat_scm_symbol_type);

  sym = (MatScmSymbol *) mat_scm_obj_data_get(mat_scm_cons_car_get(args));

  ASSERT(!mat_scm_cons_cdr_get(args));

  return mat_scm_string_new(sym->buffer, sym->buflen);
}
