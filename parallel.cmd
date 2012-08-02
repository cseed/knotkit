# mpi job, runtime max 5days
#PBS -l nodes=2:ppn=6,walltime=96:00:00
# xxx PBS -l mem=80gb
#
# sends mail if the process aborts, when it begins, and
# when it ends (abe)
#PBS -m abe
#
module load openmpi/gcc
# go to the directory with the program
cd $HOME/src/knotkit

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/u/cseed/llvm-3.1/lib
export LD_LIBRARY_PATH

echo $LD_LIBRARY_PATH

# and run it
numprocs=`wc -l <${PBS_NODEFILE}`
mpiexec -np $numprocs ./mpimain
