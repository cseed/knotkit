# serial job
#PBS -l nodes=1:ppn=1,walltime=4:00:00
# xxx PBS -l mem=80gb
#
# sends mail if the process aborts, when it begins, and
# when it ends (abe)
#PBS -m abe
#
# load intel compiler settings before running the program
# since we compiled it with intel 10.1
# module load intel/10.1
# go to the directory with the program
cd $HOME/src/knotkit
# and run it
LD_LIBRARY_PATH=/u/cseed/llvm-3.1/lib ./main
