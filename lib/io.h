
template<class R> class module;

class algebra_writer;
class algebra_reader;

class writer
{
 public:
  bool raw;
  algebra_writer *aw;
  
 public:
  writer (const writer &) = delete;
  writer (bool raw_ = false) : raw(raw_), aw(0) { }
  virtual ~writer () = default;
  
  writer &operator = (const writer &) = delete;
  
  virtual void write_raw (const void *p, size_t itemsize, size_t nitems) = 0;
  
  void write_bool (bool x) { write_raw (&x, sizeof x, 1); }
  void write_char (char x) { write_raw (&x, sizeof x, 1); }
  void write_uint8 (uint8 x) { write_raw (&x, sizeof x, 1); }
  void write_int (int x);
  void write_unsigned (unsigned x);
  void write_uint64 (uint64 x);
  
  template<class R> void write_mod (ptr<const module<R> > m);
};

class reader
{
  bool raw;
  algebra_reader *ar;
  
 public:
  reader (const reader &) = delete;
  reader (bool raw_) : raw(raw_), ar(0) { }
  virtual ~reader () = default;
  
  reader &operator = (const reader &) = delete;
  
  virtual void read_raw (void *p, size_t itemsize, size_t nitems) = 0;
  
  bool read_bool ()
  {
    bool x;
    read_raw (&x, sizeof x, 1);
    return x;
  }
  
  char read_char ()
  {
    char x;
    read_raw (&x, sizeof x, 1);
    return x;
  }
  
  uint8 read_uint8 ()
  {
    uint8 x;
    read_raw (&x, sizeof x, 1);
    return x;
  }
  
  int read_int ();
  unsigned read_unsigned ();
  uint64 read_uint64 ();
  virtual void read_mpz (mpz_t x);
  
  template<class R> ptr<const module<R> > read_mod ();
};

extern FILE *open_file (const std::string &file, const char *mode);
extern void close_file (FILE *fp);

extern gzFile open_gzfile (const std::string &file, const char *mode);
extern void close_gzfile (gzFile fp);

class file_writer : public writer
{
 public:
  FILE *fp;
  
 public:
  file_writer (const std::string &file, bool raw = false)
    : writer(raw),
      fp(open_file (file, "w"))
  {
  }
  file_writer (const file_writer &) = delete;
  ~file_writer () { close_file (fp); }
  
  file_writer &operator = (const file_writer &) = delete;
  
  void write_raw (const void *p, size_t itemsize, size_t nitems);
  void write_mpz (const mpz_t x);
};

class file_reader : public reader
{
 public:
  FILE *fp;
  
 public:
  file_reader (const std::string &file, bool raw = false)
    : reader(raw),
      fp(open_file (file, "r"))
  {
  }
  file_reader (const file_reader &) = delete;
  ~file_reader () { close_file (fp); }
  
  file_reader &operator = (const file_reader &) = delete;
  
  void read_raw (void *p, size_t itemsize, size_t nitems);
  void read_mpz (mpz_t x);
};

class gzfile_writer : public writer
{
 public:
  gzFile gzfp;
  
 public:
  gzfile_writer (const std::string &file)
    : writer(false),
      gzfp(open_gzfile (file, "w9"))
  {
  }
  gzfile_writer (const gzfile_writer &) = delete;
  ~gzfile_writer () { close_gzfile (gzfp); }
  
  gzfile_writer &operator = (const gzfile_writer &) = delete;
  
  void write_raw (const void *p, size_t itemsize, size_t nitems);
};

class gzfile_reader : public reader
{
 public:
  gzFile gzfp;
  
 public:
  gzfile_reader (const std::string &file)
    : reader(false),
      gzfp(open_gzfile (file, "r"))
  {
  }
  gzfile_reader (const gzfile_reader &) = delete;
  ~gzfile_reader () { close_gzfile (gzfp); }
  
  gzfile_reader &operator = (const gzfile_reader &) = delete;
  
  void read_raw (void *p, size_t itemsize, size_t nitems);
};

inline void read (reader &r, bool &x) { x = r.read_bool (); }
inline void read (reader &r, char &x) { x = r.read_char (); }
inline void read (reader &r, int &x) { x = r.read_int (); }
inline void read (reader &r, unsigned &x) { x = r.read_unsigned (); }
inline void read (reader &r, uint64 &x) { x = r.read_uint64 (); }
extern void read (reader &r, std::string &s);
template<class T> inline void read (reader &r, T &x) { x = T(r); }

inline void ctor_read (reader &r, bool *p) { *p = r.read_bool (); }
inline void ctor_read (reader &r, char *p) { *p = r.read_char (); }
inline void ctor_read (reader &r, int *p) { *p = r.read_int (); }
inline void ctor_read (reader &r, unsigned *p) { *p = r.read_unsigned (); }
inline void ctor_read (reader &r, uint64 *p) { *p = r.read_uint64 (); }
template<class T> inline void ctor_read (reader &r, T *p) { new (p) T (r); }

inline void write (writer &w, bool x) { w.write_bool (x); }
inline void write (writer &w, char x) { w.write_char (x); }
inline void write (writer &w, int x) { w.write_int (x); }
inline void write (writer &w, unsigned x) { w.write_unsigned (x); }
inline void write (writer &w, uint64 x) { w.write_uint64 (x); }
extern void write (writer &w, const std::string &s);
template<class T> inline void write (writer &w, const T &x) { x.write_self (w); }
