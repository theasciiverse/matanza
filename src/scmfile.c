#include <matanza.h>

typedef struct MatScmFile MatScmFile;

struct MatScmFile
{
  FILE *file;
};

static void               mat_scm_file_free                ( void               *funcdata );

MatScmType mat_scm_file_type =
{
  "file",
  mat_scm_file_free,
  NULL,
  NULL,
  NULL,
  NULL
};

static void
mat_scm_file_free ( void *funcdata )
{
  MatScmFile *data = (MatScmFile *) funcdata;
  fclose(data->file);
  free(data);
}

MatScm *
mat_scm_file_open ( char *path, char *mode )
{
  MatScmFile *data;

  data = malloc(sizeof(MatScmFile));
  if (!data)
    return NULL;

  data->file = fopen(path, mode);
  if (!data->file)
    {
      free(data);
      return NULL;
    }

  return mat_scm_obj_malloc(&mat_scm_file_type, data);
}

char
mat_scm_file_getc ( MatScm *file )
{
  MatScmFile *data = (MatScmFile *) mat_scm_obj_data_get(file);
  return fgetc(data->file);
}

char
mat_scm_file_peekc ( MatScm *file )
{
  MatScmFile *data = (MatScmFile *) mat_scm_obj_data_get(file);
  char c = fgetc(data->file);
  ungetc(c, data->file);
  return c;
}
