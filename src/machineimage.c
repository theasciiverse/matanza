#include <matanza.h>

#if MAT_BUILD_MMP

typedef struct MatMachineImage MatMachineImage;

struct MatMachineImage
{
  int                  universe_id;
  int                  universe_id_byte;

  int                  image_id;
  int                  image_id_byte;

  MatImageVector      *vect;
  int                  vect_i;
  int                  vect_n;
};

static void               mat_machine_input_image_send     ( MatConnection      *con,
                                                             void               *funcdata );

static void               mat_machine_input_image_error    ( MatMachine         *m,
                                                             char               *desc,
                                                             int                 id );

static void               mat_machine_input_image_universe ( MatMachine         *m,
                                                             void               *funcdata,
                                                             char                key );

static void               mat_machine_input_image_id        ( MatMachine        *m,
                                                              void              *funcdata,
                                                              char               key );

INLINE void
mat_machine_image_set ( MatMachine *m )
{
  MatMachineImage *data;

  data = malloc(sizeof(MatMachineImage));
  if (!data)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  data->image_id         = 0;
  data->image_id_byte    = 0;

  data->universe_id      = 0;
  data->universe_id_byte = 0;

  mat_machine_key_handler_set(m, &mat_machine_input_image_universe, data);
}

static void
mat_machine_input_image_universe ( MatMachine *m, void *funcdata, char key )
{
  MatMachineImage *data = (MatMachineImage *) funcdata;

  ASSERT(data);

  data->universe_id = (data->universe_id << 8) + key;

  if (data->universe_id_byte ++ < 1)
    return;

  mat_machine_key_handler_set(m, &mat_machine_input_image_id, data);
}

static void
mat_machine_input_image_id ( MatMachine *m, void *funcdata, char key )
{
  MatMachineImage *data = (MatMachineImage *) funcdata;
  MatConnection     *con;
  MatUniverse       *un;

  ASSERT(m);
  ASSERT(data);

  data->image_id = (data->image_id << 8) + key;

  if (data->image_id_byte ++ < 1)
    return;

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("MMP: Image request: [universe:%d][image:%d]\n", data->universe_id, data->image_id);

  un = mat_machine_universe_get(m, data->universe_id);
  if (!un)
    {
      mat_machine_input_image_error(m, "Universe not found", data->image_id);
      free(data);
      return;
    }

  data->vect = mat_universe_image_get(un, data->image_id);
  if (!data->vect)
    {
      mat_machine_input_image_error(m, "Image not found", data->image_id);
      free(data);
      return;
    }

  data->vect_i = 0;
  data->vect_n = mat_universe_angles_get(un);

  con = mat_machine_connection_get(m);

  mat_connection_printf(con, "%c", MMP_SERVER_DEFAULT_IMGSET);

  mat_connection_out_gen_set(con, mat_machine_input_image_send, data);

  mat_connection_flush(con);

  mat_machine_default_set(m);
}

static void
mat_machine_input_image_send ( MatConnection *con, void *funcdata )
{
  MatMachineImage *data = (MatMachineImage *) funcdata;

  MatImage *img;

  ASSERT(con);
  ASSERT(data);

  ASSERT(data->vect_i < data->vect_n);

  img = mat_imagevector_get(data->vect, data->vect_i);

  mat_connection_printf(con, "%i", img);

  if (++ data->vect_i == data->vect_n)
    mat_connection_out_gen_set(con, NULL, NULL);
}

static void
mat_machine_input_image_error ( MatMachine *m, char *desc, int id )
{
  char *buffer;

  MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("MMP: Image request: %s: %d\n", desc, id);

  buffer = malloc(strlen(desc) + sizeof(": XXXXXXXXXXXXXX"));
  if (!buffer)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  sprintf(buffer, "%s: %d", desc, id);

  mat_connection_free(mat_machine_connection_get(m), MMP_SERVER_CLOSE_MMPERROR, buffer);

  free(buffer);
}

#endif
