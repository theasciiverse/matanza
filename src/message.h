#ifndef __MATANZA_MESSAGE_H__
#define __MATANZA_MESSAGE_H__

#include <universe.h>

typedef struct MatMessage MatMessage;

MatMessage               *mat_message_new                  ( char               *text );

void                      mat_message_free                 ( MatMessage         *message );

char                     *mat_message_text_get             ( MatMessage         *message );

void                      mat_message_text_set             ( MatMessage         *message,
                                                             char               *text );

MatMessage               *mat_message_prev_get             ( MatMessage         *message );

MatMessage               *mat_message_next_get             ( MatMessage         *message );

void                      mat_message_prev_set             ( MatMessage         *msg,
                                                             MatMessage         *prev );

void                      mat_message_next_set             ( MatMessage         *msg,
                                                             MatMessage         *next );

#endif
