#ifndef __MATANZA_WIDGET_LABEL_H__
#define __MATANZA_WIDGET_LABEL_H__

#include <widget.h>

typedef enum MatWidgetLabelJust MatWidgetLabelJust;

enum MatWidgetLabelJust
{
  MAT_WIDGET_LABEL_JUST_CENTER,
  MAT_WIDGET_LABEL_JUST_LEFT,
  MAT_WIDGET_LABEL_JUST_RIGHT
};

MatScm                   *mat_widget_label_new             ( MatScm              *text,
                                                             char                 attr,
                                                             int                  x,
                                                             int                  y,
                                                             int                  width,
                                                             MatWidgetLabelJust   just );

INLINE void               mat_widget_label_text_set        ( MatScm              *label,
                                                             MatScm              *text );

#endif
