
void comm_init (int *argc, char ***argv);
void comm_finalize ();

int self_rank ();
int num_tasks  ();

void send_int (int v, int dest);
void send_string (const char *s, int dest);
void send_string (const std::string &s, int dest);

int recv_int (int *src);
std::string recv_string (int *src);

inline int recv_int () { return recv_int (0); }
inline std::string recv_string () { return recv_string (0); }

#if 0
void send_htw_knot (unsigned n, bool alternating, unsigned k, int dest);
knot_diagram recv_knot (int *src);
knot_diagram recv_knot () { return recv_knot (0); }
#endif
