#include <matanza.h>

struct MatMessage
{
  char *text;
  unsigned int   repeat;
  MatMessage *next;
  MatMessage *prev;
};

MatMessage *
mat_message_new ( char *text )
{
  MatMessage *n;

  ASSERT(text);

  n = malloc(sizeof(MatMessage));
  if (!n)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  n->text = strdup(text);
  if (!n->text)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }

  n->prev = n->next = NULL;

  n->repeat = 0;

  return n;
}

void
mat_message_free ( MatMessage *message )
{
  free(message->text);
  free(message);
}

char *
mat_message_text_get ( MatMessage *message )
{
  ASSERT(message);
  return message->text;
}

void
mat_message_text_set ( MatMessage *message, char *text )
{
  ASSERT(message);
  ASSERT(text);

  free(message->text);
  message->text = strdup(text);
  if (!message->text)
    {
      fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
      exit(EXIT_FAILURE);
    }
}

MatMessage *
mat_message_prev_get ( MatMessage *message )
{
  ASSERT(message);
  return message->prev;
}

MatMessage *
mat_message_next_get ( MatMessage *message )
{
  ASSERT(message);
  return message->next;
}

void
mat_message_prev_set ( MatMessage *msg, MatMessage *prev )
{
  ASSERT(msg);
  msg->prev = prev;
}

void
mat_message_next_set ( MatMessage *msg, MatMessage *next )
{
  ASSERT(msg);
  msg->next = next;
}

INLINE unsigned int
mat_message_repeat_get ( MatMessage *msg )
{
  ASSERT(msg);
  return msg->repeat;
}

INLINE void
mat_message_repeat_set ( MatMessage *msg, unsigned int repeat )
{
  ASSERT(msg);
  msg->repeat = repeat;
}
