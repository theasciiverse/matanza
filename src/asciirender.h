#ifndef __MATANZA_ASCIIRENDER_H__
#define __MATANZA_ASCIIRENDER_H__

typedef struct MatFont     MatFont;

#include <window.h>

unsigned char mat_ship_palette[3][256];

void mat_asciirender_init ();

void mat_asciirender_free ();

INLINE void mat_asciirender_render_all ( MatScm *, int wipe );
void mat_asciirender_render ( MatScm *view, int x1, int y1, int x2, int y2, int wipe );
INLINE void mat_asciirender_print ( MatScm *v, int x, int y, int width, int height, MatFont *f, int color, char *text);

MatFont *graph_font;

#endif
