#ifndef __MATANZA_COMMON_H__
#define __MATANZA_COMMON_H__

/* The following constants are used to control the output buffers.
 * 
 * We never allow the output buffers to grow more than MAT_CONNECTION_OUT_MAX
 * characters.  Also, when a given output buffer is bigger than
 * MAT_CONNECTION_OUT_BLOCK, we try to block the process until the buffer has
 * been flushed. */

#  define MAT_CONNECTION_OUT_BLOCK   4096
#  define MAT_CONNECTION_OUT_MAX   262144

/* The size of the input buffer. */
#  define MAT_CONNECTION_IN_SIZE     4096

/* Now some mechanisms for assertions. */

#  if MATANZA_ASSERT
#    define ASSERT(x) \
  if(!(x)) \
    fprintf(stderr, "%s:%d: ASSERT failed\n", __FILE__, __LINE__), exit(EXIT_FAILURE)
#  else /* !MATANZA_ASSERT */
#    define ASSERT
#  endif /* !MATANZA_ASSERT */

#  if MATANZA_VERBOSE

/* You probably want to check out mat_init_arg_verbose before editing the
 * following enumeration: */

enum MatVerboseType
{
  MAT_VERBOSE_DEFAULT,
  MAT_VERBOSE_NETWORK,
  MAT_VERBOSE_MMP,
  MAT_VERBOSE_MMP_LOUD,
  MAT_VERBOSE_NET_IN,
  MAT_VERBOSE_OBJPOS,
  MAT_VERBOSE_INTERFACE,
  MAT_VERBOSE_CONNECTIONS,
  MAT_VERBOSE_XML,
  MAT_VERBOSE_SCM_INTERNAL,
  MAT_VERBOSE_SCM_USER
};

#    define MAT_VERBOSE(type) if (mat_arg_verbose & (1 << type)) printf
int               mat_arg_verbose;

#  else /* !MATANZA_VERBOSE */

#    define MAT_VERBOSE

#  endif /* !MATANZA_VERBOSE */

/* Headers for libPNG */

#  if HAVE_PNG_H
#    include <png.h>
#  endif

#endif
