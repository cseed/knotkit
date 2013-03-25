
devel = 1

BISON = bison
FLEX = flex

# CXX = g++
# CXX = OMPI_CXX=clang++ mpic++ -fno-color-diagnostics --stdlib=libc++ --std=c++11 -I/u/cseed/llvm-3.1/lib/c++/v1
CXX = clang++ -fno-color-diagnostics --stdlib=libc++ --std=c++11

INCLUDES = -I. -I/opt/local/include 

OPTFLAGS = -g
# OPTFLAGS = -O2 -g
# OPTFLAGS = -O2 -DNDEBUG

LDFLAGS = -L/opt/local/lib

CXXFLAGS = $(OPTFLAGS) -DHOME="\"`pwd`\"" -Wall -Wno-unused $(INCLUDES)

LIB_OBJS = lib/refcount.o \
  lib/lib.o lib/smallbitset.o lib/bitset.o lib/setcommon.o lib/io.o lib/directed_multigraph.o
ALGEBRA_OBJS = algebra/algebra.o algebra/grading.o algebra/polynomial.o
KNOTKIT_OBJS = planar_diagram.o dt_code.o knot_diagram.o cube.o steenrod_square.o \
  spanning_tree_complex.o \
  smoothing.o cobordism.o knot_tables.o sseq.o \
  knot_parser/knot_parser.o knot_parser/knot_scanner.o \
  rd_parser/rd_parser.o rd_parser/rd_scanner.o

COMMON_OBJS = $(KNOTKIT_OBJS) $(ALGEBRA_OBJS) $(LIB_OBJS) 

LIB_HEADERS = lib/lib.h lib/show.h lib/refcount.h lib/pair.h lib/maybe.h lib/vector.h \
  lib/set_wrapper.h lib/set.h lib/hashset.h \
  lib/ullmanset.h lib/bitset.h lib/smallbitset.h lib/setcommon.h \
  lib/map_wrapper.h lib/map.h lib/hashmap.h lib/ullmanmap.h lib/mapcommon.h \
  lib/unionfind.h lib/priority_queue.h lib/io.h \
  lib/directed_multigraph.h
ALGEBRA_HEADERS = algebra/algebra.h algebra/grading.h algebra/module.h \
  algebra/Z2.h algebra/linear_combination.h \
  algebra/Z.h algebra/Zp.h algebra/Q.h \
  algebra/polynomial.h algebra/multivariate_polynomial.h \
  algebra/multivariate_laurentpoly.h algebra/fraction_field.h
KNOTKIT_HEADERS = knotkit.h planar_diagram.h dt_code.h knot_diagram.h \
  smoothing.h cobordism.h cube.h steenrod_square.h \
  spanning_tree_complex.h cube_impl.h sseq.h simplify_chain_complex.h

LIBS = -lgmp -lz

all: kk

%.o : %.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

kk: kk.o $(COMMON_OBJS)
	$(CXX) $(LDFLAGS) -o kk $^ $(LIBS)

main: main.o $(COMMON_OBJS)
	$(CXX) $(LDFLAGS) -o main $^ $(LIBS)

mpimain: mpimain.o mpi_aux.o $(COMMON_OBJS)
	$(CXX) $(LDFLAGS) -o mpimain $^ $(LIBS)

testlib: testlib.o $(COMMON_OBJS)
	$(CXX) $(LDFLAGS) -o testlib $^

ifeq ($(devel),1)
knot_parser/knot_parser.cc knot_parser/knot_parser.hh: knot_parser/knot_parser.yy
	$(BISON) knot_parser/knot_parser.yy -o knot_parser/knot_parser.cc

knot_parser/knot_scanner.cc: knot_parser/knot_scanner.ll
	$(FLEX) -o knot_parser/knot_scanner.cc knot_parser/knot_scanner.ll

javakh_parser/javakh_parser.cc javakh_parser/javakh_parser.hh: javakh_parser/javakh_parser.yy
	$(BISON) javakh_parser/javakh_parser.yy -o javakh_parser/javakh_parser.cc

javakh_parser/javakh_scanner.cc: javakh_parser/javakh_scanner.ll
	$(FLEX) -o javakh_parser/javakh_scanner.cc javakh_parser/javakh_scanner.ll

rd_parser/rd_parser.cc rd_parser/rd_parser.hh: rd_parser/rd_parser.yy
	$(BISON) rd_parser/rd_parser.yy -o rd_parser/rd_parser.cc

rd_parser/rd_scanner.cc: rd_parser/rd_scanner.ll
	$(FLEX) -o rd_parser/rd_scanner.cc rd_parser/rd_scanner.ll
endif

.PHONY: parser_files
parser_files: \
  knot_parser/knot_parser.cc knot_parser/knot_parser.hh \
  knot_parser/knot_scanner.cc \
  javakh_parser/javakh_parser.cc javakh_parser/javakh_parser.hh \
  javakh_parser/javakh_scanner.cc \
  rd_parser/rd_parser.cc rd_parser/rd_parser.hh \
  rd_parser/rd_scanner.cc

.PHONY: clean
clean:
	rm -f *.o lib/*.o algebra/*.o knot_parser/*.o rd_parser/*.o
	rm -f main kk mpimain
	rm -f gmon.out

.PHONY: realclean
realclean: clean
	rm -f knot_parser/knot_parser.cc knot_parser/knot_parser.hh
	rm -f knot_parser/knot_scanner.cc
	rm -f knot_parser/location.hh knot_parser/position.hh knot_parser/stack.hh
	rm -f javakh_parser/javakh_parser.cc javakh_parser/javakh_parser.hh
	rm -f javakh_parser/javakh_scanner.cc
	rm -f javakh_parser/location.hh javakh_parser/position.hh javakh_parser/stack.hh
	rm -f rd_parser/rd_parser.cc rd_parser/rd_parser.hh
	rm -f rd_parser/rd_scanner.cc
	rm -f rd_parser/location.hh rd_parser/position.hh rd_parser/stack.hh

$(LIB_OBJS): $(LIB_HEADERS)
$(ALGEBRA_OBJS): $(ALGEBRA_HEADERS) $(LIB_HEADERS)
$(KNOTKIT_OBJS) main.o mpimain.o kk.o: $(KNOTKIT_HEADERS) $(ALGEBRA_HEADERS) $(LIB_HEADERS)

mpimain.o mpi_aux.o: mpi_aux.h
