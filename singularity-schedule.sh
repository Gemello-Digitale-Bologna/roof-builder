#!/bin/bash

#SBATCH --nodes=1
#SBATCH --tasks-per-node=1
#SBATCH --cpus-per-task=32
#SBATCH --mem=2GB
#SBATCH --time=00:10:00
#SBATCH --out=slurm.%j.out
#SBATCH --err=slurm.%j.err
#SBATCH --account=cin_staff
#SBATCH -p dcgp_usr_prod
#SBATCH --gres=tmpfs:100g

export OMP_NUM_THREADS=32

echo "Setup..." > ./output/exec_log.txt 2>&1

module purge >> ./output/exec_log.txt 2>&1
module load openmpi/4.1.6--nvhpc--23.11 > ./output/exec_log.txt 2>&1

echo "Binding Volumes..." >> ./output/exec_log.txt 2>&1
export SINGULARITY_BIND=./assets:/assets,./output:/output

echo "Scheduling Script..." >> ./output/exec_log.txt 2>&1
mpirun -np 1 singularity exec si-roof.sif /build/RoofBuilder >> ./output/exec_log.txt 2>&1

echo "Removing temp files..." >> ./output/exec_log.txt 2>&1
rm -rf ./assets/temp/*