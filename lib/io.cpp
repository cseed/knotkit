
// #include <lib/lib.h>
#include <algebra/algebra.h>

writer::writer (bool raw_)
  : raw(raw_),
    aw(new algebra_writer)
{
}

writer::~writer ()
{
  delete aw;
}

void
writer::write_int (int x)
{
  if (raw)
    write_raw (&x, sizeof x, 1);
  else
    {
      uint8 buf[5];
      unsigned n = 0;
      
      bool more = 1;
      while (more)
	{
	  uint8 b = (uint8)(x & 0x7f);
	  x >>= 7;
      
	  if ((x == 0
	       && ! (b & 0x40))
	      || (x == -1
		  && (b & 0x40) == 0x40))
	    more = 0;
	  else
	    b |= 0x80;
      
	  assert (n < 5);
	  buf[n] = b;
	  n ++;
	}
      
      write_raw (buf, sizeof buf[0], n);
    }
}

void
writer::write_unsigned (unsigned x)
{
  if (raw)
    write_raw (&x, sizeof x, 1);
  else
    {
      uint8 buf[5];
      unsigned n = 0;
  
      bool more = 1;
      while (more)
	{
	  uint8 b = (uint8)(x & 0x7f);
	  x >>= 7;
      
	  if ((x == 0
	       && ! (b & 0x40)))
	    more = 0;
	  else
	    b |= 0x80;
      
	  assert (n < 5);
	  buf[n] = b;
	  n ++;
	}
  
      write_raw (buf, sizeof buf[0], n);
    }
}

void
writer::write_uint64 (uint64 x)
{
  if (raw)
    write_raw (&x, sizeof x, 1);
  else
    {
      uint8 buf[10];
      unsigned n = 0;
  
      bool more = 1;
      while (more)
	{
	  uint8 b = (uint8)(x & 0x7f);
	  x >>= 7;
      
	  if ((x == 0
	       && ! (b & 0x40)))
	    more = 0;
	  else
	    b |= 0x80;

	  assert (n < 10);
	  buf[n] = b;
	  n ++;
	}
  
      write_raw (buf, sizeof buf[0], n);
    }
}

void
writer::write_mpz (const mpz_t x)
{
  assert (!raw);
  
  size_t count;
  void *buf = mpz_export (nullptr, &count, -1, 1, -1, 0, x);
  
  write_unsigned ((unsigned)count);
  write_raw (buf, 1, count);
  
  free (buf);
}

reader::reader (bool raw_)
  : raw(raw_),
    ar(new algebra_reader)
{
}

reader::~reader ()
{
  delete ar;
}

int
reader::read_int ()
{
  int x = 0;
  
  if (raw)
    read_raw (&x, sizeof x, 1);
  else
    {
      int shift = 0;
      for (;;)
	{
	  uint8 b = read_uint8 ();
	  x |= ((int)(b & 0x7f) << shift);
	  shift += 7;
	  if (! (b & 0x80))
	    {
	      if (shift < int_bits
		  && (b & 0x40))
		x = (x << (int_bits - shift)) >> (int_bits - shift);
	      break;
	    }
	}
    }
  
  return x;
}

unsigned
reader::read_unsigned ()
{
  unsigned x = 0;
  
  if (raw)
    read_raw (&x, sizeof x, 1);
  else
    {
      unsigned shift = 0;
      for (;;)
	{
	  uint8 b = read_uint8 ();
	  x |= ((unsigned)(b & 0x7f) << shift);
	  shift += 7;
	  if (! (b & 0x80))
	    break;
	}
    }
  
  return x;
}

uint64
reader::read_uint64 ()
{
  uint64 x = 0;

  if (raw)
    read_raw (&x, sizeof x, 1);
  else
    {
      uint64 shift = 0;
      for (;;)
	{
	  uint8 b = read_uint8 ();
	  x |= ((uint64)(b & 0x7f) << shift);
	  shift += 7;
	  if (! (b & 0x80))
	    break;
	}
    }
  
  return x;
}

void
reader::read_mpz (mpz_t x)
{
  assert (!raw);
  
  unsigned count = read_unsigned ();
  void *p = malloc (count);
  if (!p)
    {
      stderror ("malloc");
      exit (EXIT_FAILURE);
    }
  
  read_raw (p, 1, count);
  
  mpz_import (x, count, -1, 1, -1, 0, p);
  
  free (p);
}

FILE *open_file (const std::string &file, const char *mode)
{
  FILE *fp = fopen (file.c_str (), mode);
  if (!fp)
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

gzFile
open_gzfile (const std::string &file, const char *mode)
{
  gzFile gzfp = gzopen (file.c_str (), mode);
  if (!gzfp)
    {
      stderror ("gzopen: %s", file.c_str ());
      exit (EXIT_FAILURE);
    }
  return gzfp;
}

extern void close_gzfile (gzFile gzfp)
{
  int r = gzclose (gzfp);
  if (r != Z_OK)
    {
      stderror ("gzclose");
      exit (EXIT_FAILURE);
    }
}

void
file_writer::write_raw (const void *p, size_t itemsize, size_t nitems)
{
  if (fwrite (p, itemsize, nitems, fp) != nitems)
    {
      stderror ("fwrite");
      exit (EXIT_FAILURE);
    }
}

void
file_writer::write_mpz (const mpz_t x)
{
  mpz_out_raw (fp, x);
}

void
file_reader::read_raw (void *p, size_t itemsize, size_t nitems)
{
  if (fread (p, itemsize, nitems, fp) != nitems)
    {
      stderror ("fread");
      exit (EXIT_FAILURE);
    }
}

void
file_reader::read_mpz (mpz_t x)
{
  mpz_inp_raw (x, fp);
}

void
gzfile_writer::write_raw (const void *p, size_t itemsize, size_t nitems)
{
  unsigned nbytes = itemsize*nitems;
  if (gzwrite (gzfp, p, nbytes) != nbytes)
    {
      stderror ("gzwrite");
      exit (EXIT_FAILURE);
    }
}

void
gzfile_reader::read_raw (void *p, size_t itemsize, size_t nitems)
{
  unsigned nbytes = itemsize*nitems;
  if (gzread (gzfp, p, nbytes) != nitems)
    {
      stderror ("fread");
      exit (EXIT_FAILURE);
    }
}

void
read (reader &r, std::string &s)
{
  unsigned n = r.read_unsigned ();
  
  char buf[n + 1];
  r.read_raw (buf, sizeof (char), n + 1);
  assert (buf[n] == 0);
  
  s = std::string (buf);
}

void
write (writer &w, const std::string &s)
{
  unsigned n = s.length ();
  w.write_unsigned (n);
  
  w.write_raw (s.c_str (), sizeof (char), n + 1);
}
