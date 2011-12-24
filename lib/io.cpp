
#include <lib/lib.h>

FILE *open_file (const std::string &file, const char *mode)
{
  FILE *fp = fopen (file.c_str (), mode);
  if (fp == 0)
    {
      stderror ("fopen: %s", file.c_str ());
      exit (EXIT_FAILURE);
    }
  return fp;
}

void close_file (FILE *fp)
{
  fclose (fp);
}


writer::writer (const std::string &file)
  : fp(0)
{
  fp = open_file (file, "w");
}

writer::~writer ()
{
  if (fp)
    {
      fclose (fp);
      fp = 0;
    }
}

reader::reader (const std::string &file)
  : fp(0)
{
  fp = open_file (file, "r");
}

reader::~reader ()
{
  if (fp)
    {
      fclose (fp);
      fp = 0;
    }
}

void
writer::write_char (char x)
{
  if (fwrite (&x, sizeof x, 1, fp) != 1)
    {
      stderror ("fwrite");
      exit (EXIT_FAILURE);
    }
}

void
writer::write_bool (bool x)
{
  if (fwrite (&x, sizeof x, 1, fp) != 1)
    {
      stderror ("fwrite");
      exit (EXIT_FAILURE);
    }
}

void
writer::write_int (int x)
{
  if (fwrite (&x, sizeof x, 1, fp) != 1)
    {
      stderror ("fwrite");
      exit (EXIT_FAILURE);
    }
}

void
writer::write_unsigned (unsigned x)
{
  if (fwrite (&x, sizeof x, 1, fp) != 1)
    {
      stderror ("fwrite");
      exit (EXIT_FAILURE);
    }
}

void
writer::write_uint64 (uint64 x)
{
  if (fwrite (&x, sizeof x, 1, fp) != 1)
    {
      stderror ("fwrite");
      exit (EXIT_FAILURE);
    }
}

bool
reader::read_bool ()
{
  bool x;
  if (fread (&x, sizeof x, 1, fp) != 1)
    {
      stderror ("fread");
      exit (EXIT_FAILURE);
    }
  return x;
}

char
reader::read_char ()
{
  char x;
  if (fread (&x, sizeof x, 1, fp) != 1)
    {
      stderror ("fread");
      exit (EXIT_FAILURE);
    }
  return x;
}

int
reader::read_int ()
{
  int x;
  if (fread (&x, sizeof x, 1, fp) != 1)
    {
      stderror ("fread");
      exit (EXIT_FAILURE);
    }
  return x;
}

unsigned
reader::read_unsigned ()
{
  unsigned x;
  if (fread (&x, sizeof x, 1, fp) != 1)
    {
      stderror ("fread");
      exit (EXIT_FAILURE);
    }
  return x;
}

uint64
reader::read_uint64 ()
{
  uint64 x;
  if (fread (&x, sizeof x, 1, fp) != 1)
    {
      stderror ("fread");
      exit (EXIT_FAILURE);
    }
  return x;
}
