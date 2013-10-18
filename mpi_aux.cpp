
#include <mpi.h>
#include <mpi_aux.h>

// #include <knotkit.h>

#define DATATAG 1

void
comm_init (int *argc, char ***argv)
{
  MPI_Init (argc, argv);
}

void
comm_finalize ()
{
  MPI_Finalize ();
}

int
self_rank ()
{
  int node;
  MPI_Comm_rank (MPI_COMM_WORLD, &node);
  return node;
}

int
num_tasks  ()
{
  int ntasks;
  MPI_Comm_size (MPI_COMM_WORLD, &ntasks);
  return ntasks;
}

void
send_int (int v, int dest)
{
  MPI_Send (&v, 1, MPI_INT, dest, DATATAG, MPI_COMM_WORLD);
}

void
send_string (const char *s, int dest)
{
  int n = strlen (s);
  MPI_Send (&n, 1, MPI_INT, dest, DATATAG, MPI_COMM_WORLD);
  MPI_Send ((void *)s, n, MPI_CHAR, dest, DATATAG, MPI_COMM_WORLD);
}

void
send_string (const std::string &s, int dest)
{
  send_string (s.c_str (), dest);
}

int
recv_int (int *src)
{
  int v;
  MPI_Status status;
  MPI_Recv (&v,                /* message buffer */
	    1,                 /* one data item */
	    MPI_INT,           /* of type int */
	    MPI_ANY_SOURCE,    /* receive from any sender */
	    MPI_ANY_TAG,       /* any type of message */
	    MPI_COMM_WORLD,    /* default communicator */
	    &status);          /* info about the received message */
  if (src)
    *src = status.MPI_SOURCE;
  return v;
}

std::string
recv_string (int *src)
{
  int n;
  MPI_Status status;
  MPI_Recv (&n,                /* message buffer */
	    1,                 /* one data item */
	    MPI_INT,           /* of type int */
	    MPI_ANY_SOURCE,    /* receive from any sender */
	    MPI_ANY_TAG,       /* any type of message */
	    MPI_COMM_WORLD,    /* default communicator */
	    &status);          /* info about the received message */
  char *s = new char[n + 1];
  MPI_Recv (s,                 /* message buffer */
	    n,                 /* one data item */
	    MPI_CHAR,          /* of type int */
	    status.MPI_SOURCE, /* receive from any sender */
	    MPI_ANY_TAG,       /* any type of message */
	    MPI_COMM_WORLD,    /* default communicator */
	    &status);          /* info about the received message */
  if (src)
    *src = status.MPI_SOURCE;
  s[n] = '\0';
  std::string s_std (s);
  delete [] s;
  return s_std;
}

#if 0
void
send_htw_knot (unsigned n, bool alternating, unsigned k, int dest)
{
  int data[3];
  data[0] = (int)n;
  data[1] = (int)alternating;
  data[2] = (int)k;
  MPI_Send (&data, 3, MPI_INT, dest, DATATAG, MPI_COMM_WORLD);
}

knot_diagram
recv_knot (int *src)
{
  int data[3];
  MPI_Status status;
  MPI_Recv (&data,             /* message buffer */
	    3,                 /* one data item */
	    MPI_INT,           /* of type int */
	    MPI_ANY_SOURCE,    /* receive from any sender */
	    MPI_ANY_TAG,       /* any type of message */
	    MPI_COMM_WORLD,    /* default communicator */
	    &status);          /* info about the received message */
  if (src)
    *src = status.MPI_SOURCE;
  
  dt_code k = htw_knot ((unsigned)data[0],
			(bool)data[1],
			(unsigned)data[2]);
  return knot_diagram (k);
}
#endif
