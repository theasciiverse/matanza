#include <matanza.h>

#if MAT_BUILD_MMP

static void               mat_machine_input_default        ( MatMachine         *m,
                                                             void               *d,
                                                             char                key );

INLINE void
mat_machine_default_set ( MatMachine *m )
{
  mat_machine_key_handler_set(m, &mat_machine_input_default, NULL);
}

static void
mat_machine_input_default ( MatMachine *m, void *d, char key )
{
  switch (key)
    {
    case MMP_CLIENT_DEFAULT_JOIN:
      MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("MMP: Command: join\n");
      mat_machine_join_set(m);
      break;

    case MMP_CLIENT_DEFAULT_SHIPADD:
      MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("MMP: Command: shipadd\n");
      mat_machine_shipadd_set(m);
      break;

    case MMP_CLIENT_DEFAULT_SHIPCMD:
      MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("Command: shipcmd\n");
      mat_machine_shipcmd_set(m);
      break;

    case MMP_CLIENT_DEFAULT_IMGGET:
      MAT_VERBOSE(MAT_VERBOSE_DEFAULT)("MMP: Command: imgget\n");
      mat_machine_image_set(m);
      break;

#if 0
    case MMP_CLIENT_DEFAULT_UNIVERSES:
      <Send a list with all the universes to machine |m|>;
      break;
#endif

    default:
      mat_machine_unknown_command(m, key);
      break;
    }
}

#endif
