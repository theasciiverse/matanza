#include <matanza.h>

static MatScm *obj ( void *file, MatScm *env, MatScm *args );
INLINE static MatScm *quote ( MatScm *file, MatScm *env, MatScm *args );
INLINE static MatScm *special ( MatScm *file );
INLINE static void token ( MatScm *file, char **buffer, int *buflen, int (*stop) (char));
INLINE static int string_stop ( char c );
INLINE static MatScm *string ( MatScm *file );
INLINE static int symbol_stop ( char c );
INLINE static MatScm *symbol ( MatScm *file );
INLINE static MatScm *list ( MatScm *file, MatScm *env, MatScm *args );

MatScm *
mat_scm_read ( MatScm *file )
{
  ASSERT(file);
  ASSERT(mat_scm_obj_type_get(file) == &mat_scm_file_type);

  return mat_scm_proc_new(obj, file, 1);
}

static MatScm *
obj ( void *file, MatScm *env, MatScm *args )
{
  ASSERT(file);
  ASSERT(mat_scm_obj_type_get(file) == &mat_scm_file_type);

  for (;;)
    {
      char c = mat_scm_file_peekc(file);

      switch (c)
        {
        case EOF:
          return mat_scm_special_eof;
        case ';':
          while (c != '\n')
            if ((c = mat_scm_file_getc(file)) == EOF)
              return NULL;
          break;
        case '\'':
          return quote(file, env, args);
        case '#':
          return special(file);
        case '"':
          return string(file);
        case '(':
          return list(file, env, args);
        case ')':
          mat_scm_file_getc(file);
          return mat_scm_special_close;
        default:
          if (isspace(c))
            mat_scm_file_getc(file);
          else
            return symbol(file);
        }
    }
}

INLINE static MatScm *
quote ( MatScm *file, MatScm *env, MatScm *args )
{
  ASSERT(mat_scm_obj_type_get(file) == &mat_scm_file_type);
  mat_scm_file_getc(file);
  return mat_scm_cons_new(mat_scm_quote, mat_scm_cons_new(obj(file, env, args), NULL));
}

INLINE static MatScm *
special ( MatScm *file )
{
  ASSERT(mat_scm_obj_type_get(file) == &mat_scm_file_type);
  mat_scm_file_getc(file);
  switch (tolower(mat_scm_file_getc(file)))
    {
    case 't':
      return mat_scm_bool_true;
    case 'f':
      return NULL;
    case '\\':
      return mat_scm_char_new(mat_scm_file_getc(file));
    default:
      return NULL; /* TODO: Signal error! */
    }
}

INLINE static void
token ( MatScm *file, char **buffer, int *buflen, int (*stop) (char))
{
  int   bufsiz = 0;

  /* TODO: Check for out of memory error on realloc */
  /* TODO: Check for EOF */

  for (;;)
    {
      if (*buflen == bufsiz)
        {
          bufsiz = bufsiz ? bufsiz * 2 : 32;
          *buffer = realloc(*buffer, bufsiz);
        }
      (*buffer)[*buflen] = mat_scm_file_peekc(file);
      if (stop((*buffer)[(*buflen) ++]))
        {
          (*buffer)[--(*buflen)] = 0;
          return;
        }
      mat_scm_file_getc(file);
    }
}

INLINE static int
string_stop ( char c )
{
  return c == '"';
}

INLINE static MatScm *
string ( MatScm *file )
{
  char *buffer = NULL;
  int   buflen = 0;

  mat_scm_file_getc(file);
  token(file, &buffer, &buflen, string_stop);
  mat_scm_file_getc(file);
  return mat_scm_string_new(buffer, buflen);
}

INLINE static int
symbol_stop ( char c )
{
  /* TODO: Add more symbols */
  return !isalnum(c) && c != '.' && c != '-' && c != '*' && c != '+' && c != '/' && c != ':' && c != '<' && c != '>' && c != '?' && c != '!';
}

INLINE static MatScm *
symbol ( MatScm *file )
{
  char *buffer = NULL;
  int   buflen = 0;
  int   i;
  int   seennum = 0;

  token(file, &buffer, &buflen, symbol_stop);

  for (i = 0; i < buflen; i ++)
    if (!isdigit(buffer[i]) && buffer[i] != '.' && buffer[i] != '-')
      return mat_scm_symbol_new(buffer, buflen);
    else
      seennum |= isdigit(buffer[i]);

  if (!seennum)
    return mat_scm_symbol_new(buffer, buflen);

  return mat_scm_number_new_string(buffer);
}

INLINE static MatScm *
list ( MatScm *file, MatScm *env, MatScm *args )
{
  MatScm *head = NULL, *tail = NULL;

  mat_scm_file_getc(file);

  for (;;)
    {
      MatScm *t = obj(file, env, args);

      if (t == mat_scm_special_close)
        return head;

      if (!tail)
        head = tail = mat_scm_cons_new(t, NULL);
      else
        {
          mat_scm_cons_cdr_set(tail, mat_scm_cons_new(t, NULL));
          tail = mat_scm_cons_cdr_get(tail);
        }
    }
}
