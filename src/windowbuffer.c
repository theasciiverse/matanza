#include <matanza.h>

INLINE static void        bufreset                         ( char               *buf,
                                                             char                v,
                                                             unsigned int        px,
                                                             unsigned int        py,
                                                             unsigned int        sx,
                                                             unsigned int        sy,
                                                             unsigned int        row );

INLINE static void
bufreset ( char *buf, char v, unsigned int px, unsigned int py, unsigned int sx, unsigned int sy, unsigned int row )
{
  unsigned int i;

  buf += px;

  for (i = py; i < sy; i ++)
    memset(buf + i * row, v, sx);
}

INLINE void
mat_window_imgbuffer_reset ( MatScm *window, unsigned char value )
{
  bufreset(mat_telnet_imgbuffer_get(window), value,
           mat_window_img_pos_x_get(window), mat_window_img_pos_y_get(window),
           mat_window_img_size_x_get(window), mat_window_img_size_y_get(window),
           mat_telnet_imgwidth_get(window));
}

INLINE void
mat_window_textbuffer_reset ( MatScm *window )
{
  bufreset(mat_telnet_textbuffer_get(window), ' ',
           mat_window_chars_pos_x_get(window), mat_window_chars_pos_y_get(window),
           mat_window_chars_size_x_get(window), mat_window_chars_size_y_get(window),
           mat_telnet_xwinsize_get(window));
}

INLINE unsigned int
mat_window_img_pos_x_get ( MatScm *window )
{
  return 2 * mat_window_chars_pos_x_get(window);
}

INLINE unsigned int
mat_window_img_pos_y_get ( MatScm *window )
{
  return 2 * mat_window_chars_pos_y_get(window);
}

INLINE unsigned int
mat_window_img_size_y_get ( MatScm *window )
{
  return 2 * mat_window_chars_size_y_get(window);
}

INLINE unsigned int
mat_window_img_size_x_get ( MatScm *window )
{
  return 2 * mat_window_chars_size_x_get(window);
}

INLINE void
mat_window_print ( MatScm *window, unsigned int x, unsigned int y, char *str, char a )
{
  ASSERT(str);
  while (*str)
    mat_window_textbuffer_set(window, x++, y, *str++, a);
}

INLINE void
mat_window_textbuffer_set ( MatScm *window, unsigned int x, unsigned int y, char c, char a )
{
  ASSERT(window);

  ASSERT(x < mat_window_chars_size_x_get(window));
  ASSERT(y < mat_window_chars_size_y_get(window));

  y += mat_window_chars_pos_y_get(window);
  y *= mat_telnet_xwinsize_get(window);
  x += mat_window_chars_pos_x_get(window);

  mat_telnet_textbuffer_set(window, y + x, c, a);
}

INLINE void
mat_window_imgbuffer_set ( MatScm *window, unsigned int x, unsigned int y, unsigned char value, double alpha )
{
  unsigned int pos;

  ASSERT(0 <= alpha);
  ASSERT(alpha <= 1);

  ASSERT(x < mat_window_img_size_x_get(window));
  ASSERT(y < mat_window_img_size_y_get(window));

  y += mat_window_img_pos_y_get(window);
  y *= mat_telnet_imgwidth_get(window);
  x += mat_window_img_pos_x_get(window);

  value = mat_window_palette_get(window)[value];

  if (alpha != 1)
    {
      value *= alpha;
      value += (1 - alpha) * mat_telnet_imgbuffer_pos_get(window, y + x);
    }

  mat_telnet_imgbuffer_set(window, y + x, value);
}
