#include <matanza.h>

#if MAT_BUILD_MMP

typedef struct MatMachineInit MatMachineInit;

struct MatMachineInit
{
  char  client[256 + 1];
  int   client_len;

  char  version[256 + 1];
  int   version_len;
};

static void               mat_machine_input_init           ( MatMachine         *m,
                                                             void               *d,
                                                             char                key );

static void               mat_machine_input_init_client    ( MatMachine         *m,
                                                             void               *funcdata,
                                                             char                key );

static void               mat_machine_input_init_version   ( MatMachine         *m,
                                                             void               *funcdata,
                                                             char                key );

INLINE void
mat_machine_init_set ( MatMachine *m )
{
  mat_machine_key_handler_set(m, &mat_machine_input_init, NULL);
}

static void
mat_machine_input_init ( MatMachine *m, void *d, char key )
{
  MatMachineInit *data;
  ASSERT(m);

  switch (key)
    {
    case MMP_CLIENT_INITIAL_ID:
      data = malloc(sizeof(MatMachineInit));
      if (!data)
        {
          fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
          exit(EXIT_FAILURE);
        }

      data->client_len = 0;
      data->version_len = 0;

      mat_machine_key_handler_set(m, &mat_machine_input_init_client, data);
      break;

    default:
      mat_machine_unknown_command(m, key);
      break;
    }
}

static void
mat_machine_input_init_client ( MatMachine *m, void *funcdata, char key )
{
  MatMachineInit *data = (MatMachineInit *) funcdata;

  ASSERT(m);
  ASSERT(data);

  if (key == 0)
    mat_machine_key_handler_set(m, &mat_machine_input_init_version, data);
  else if (data->client_len < 256)
    data->client[data->client_len ++] = (char) key;
}

static void
mat_machine_input_init_version ( MatMachine *m, void *funcdata, char key )
{
  MatMachineInit *data = (MatMachineInit *) funcdata;

  ASSERT(m);
  ASSERT(data);

  if (key == 0)
    {
      MatConnection *c;

      data->client [data->client_len ] = 0;
      data->version[data->version_len] = 0;

      c = mat_machine_connection_get(m);
      ASSERT(c);

      MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Connection from %s: [%s][%s]\n", mat_connection_addr_get(c), data->client, data->version); 
      free(data);

      mat_connection_printf(c, "%c%s%c%s%c", MMP_SERVER_INITIAL_CONNECTED, PACKAGE, 0, VERSION, 0);
      mat_connection_flush(c);

      mat_machine_default_set(m);
    }
  else if (data->version_len < 256)
    data->version[data->version_len ++] = (char) key;

}

#endif
