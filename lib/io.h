
extern FILE *open_file (const std::string &file, const char *mode);
extern void close_file (FILE *fp);

class writer
{
 public:
  FILE *fp;
  
 public:
  writer (const std::string &file);
  writer (const writer &); // doesn't exist
  ~writer ();
  
  writer &operator = (const writer &); // doesn't exist
  
  void write_bool (bool x);
  void write_char (char x);
  void write_int (int x);
  void write_unsigned (unsigned x);
  void write_uint64 (uint64 x);
};

class reader
{
 public:
  FILE *fp;

 public:
  reader (const std::string &file);
  reader (const reader &); // doesn't exist
  ~reader ();
  
  reader &operator = (const reader &); // doesn't exist
  
  bool read_bool ();
  char read_char ();
  int read_int ();
  unsigned read_unsigned ();
  uint64 read_uint64 ();
};

inline void read (reader &r, bool &x) { x = r.read_bool (); }
inline void read (reader &r, char &x) { x = r.read_char (); }
inline void read (reader &r, int &x) { x = r.read_int (); }
inline void read (reader &r, unsigned &x) { x = r.read_unsigned (); }
inline void read (reader &r, uint64 &x) { x = r.read_uint64 (); }
void read (reader &r, std::string &s);
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
void write (writer &w, const std::string &s);
template<class T> inline void write (writer &w, const T &x) { x.write_self (w); }
