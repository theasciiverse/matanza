#include <matanza.h>

INLINE void
mat_ship_message_vprintf( MatScm *obj, char *fmt, va_list args )
{
  ASSERT(fmt);

#if MAT_BUILD_MMP
  if (mat_ship_mmp_get(obj))
    mat_machine_message_vprintf(mat_ship_mmp_get(obj), mat_ship_mmp_id_get(obj), fmt, args);
#endif
  if (mat_ship_widgetplay_get(obj))
    mat_widget_play_vprintf(mat_ship_widgetplay_get(obj), fmt, args);
}

INLINE void
mat_ship_message_printf ( MatScm *obj, char *fmt, ... )
{
  va_list args;
  va_start(args, fmt);
  mat_ship_message_vprintf(obj, fmt, args);
  va_end(args);
}
