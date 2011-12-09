
inline void newline () { printf ("\n"); }
inline void comment () { printf ("%% "); }
inline void show (int i) { printf ("%d", i); }
inline void show (unsigned i) { printf ("%d", i); }
inline void display (int i) { printf ("%d\n", i); }
inline void display (unsigned i) { printf ("%d\n", i); }

template<class T> inline void show (const T &e) { e.show_self (); }
template<class T> inline void display (const T &e) { e.display_self (); }

template<class T> inline void display (const char *h, const T &e)
{
  printf ("%s", h);
  e.display_self ();
}
