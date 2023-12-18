# ICE 4131 High Performance Computing - Lab 7

**Tutor:** Peter Butcher ([p.butcher@bangor.ac.uk](p.butcher@bangor.ac.uk))

**Lab Assistants**:

- Iwan Mitchell ([i.t.mitchell@bangor.ac.uk](i.t.mitchell@bangor.ac.uk))
- Frank Williams ([f.j.williams@bangor.ac.uk](f.j.williams@bangor.ac.uk))

### Objectives

_**NOTE:** If you have not completed the tasks from the [previous labs](../), you should complete those first!_

Today's lab requires you to have completed parallel implementations of SimpleRayTracing with Pthread ([lab 4](../lab4/README.md)), OpenMP ([lab 5](../lab5/README.md)), and MPI ([lab 6](../lab6/README.md)). You should have tested these implementations with various numbers of threads and processes. You should have noticed that using multiple threads is more computationally efficient compared to using multiple processes. This is because processes are heavier than threads. That said, the code we are currently running on multiple processes with MPI is not yet making use of multi-threading. To fully utilise the capabilities of Supercomputing Wales, we will now use OpenMP to further parallelise SimpleRayTracing.

### Task List

Today's tasks are as follows:

1. [Get the latest updates to this repository](#step-1-get-the-latest-updates-to-this-repository)
2. [Plot the Results of MPI Performance](#step-2-plot-the-results-of-mpi-performance)
3. [Setup Our Environment for MPI and OpenMP](#step-3-setup-our-environment-for-mpi-and-openmp)
4. [Update `CMakeLists.txt` for MPI and OpenMP](#step-4-update-cmakeliststxt-for-mpi-and-openmp)
5. [Parallelise Code with OpenMP](#step-5-parallelise-code-with-openmp)
6. [Run Your Program](#step-6-run-your-program)
7. [Full Performance Evaluation](#step-7-full-performance-evaluation)

---

## STEP 1: Get the latest updates to this repository

To get the latest updates to this repository on the Supercomputer:

1. Navigate to the `ice-4131-labs` directory you created during Lab 3.

2. Stash local changes you made by typing:

```bash
git stash
```

3. Get updates from this GitHub repository:

```bash
git pull
```

4. Re-apply your changes:

```bash
git stash apply
```

5. Your copy of `ice-4131-labs` will now be up to date!

---

## STEP 2: Plot the Results of MPI Performance

Assuming that all the jobs in `lab6` completed successfully:

- Concatenate the results you have generated so far into `timing.csv` as we have done previously.

- Use `plotTiming.py` to plot the runtimes and speedups of your serial, Pthread, OpenMP and MPI implementations.

---

## STEP 3: Setup Our Environment for MPI and OpenMP

Consult the previos lab scripts and load the appropriate modules for MPI and OpenMP as we will be using both.

---

### STEP 4: Update `CMakeLists.txt` for MPI and OpenMP

Edit `CMakeLists.txt`:

1. Add the new executable

```cmake
add_executable(main-mpi-omp src/main-mpi-omp.cxx)
```

2. Specify extra header directories

```cmake
TARGET_INCLUDE_DIRECTORIES(main-mpi-omp PUBLIC ${ASSIMP_INCLUDE_DIRS} ${MPI_INCLUDE_PATH})
```

3. The linkage

```cmake
TARGET_LINK_LIBRARIES (main-mpi-omp PUBLIC RayTracing ${ASSIMP_LIBRARY} ${MPI_CXX_LIBRARIES} OpenMP::OpenMP_CXX)
```

---

### STEP 5: Parallelise Code with OpenMP

If you haven't already done so, copy the contents of `main-mpi.cxx` into `main-mpi-omp.cxx`.

- Use a `pragma` to parallelise a for loop (see [lab 5](../lab5)).
- Compile.

---

### STEP 6: Run Your Program

1. To run your program, launch a job. **DO NOT RUN IT DIRECTLY ON `hawklogin.cf.ac.uk`**. Be considerate to other users, this is a shared resource.

2. See [Lab 2](../lab2/) for an explanation.

3. A script is provided for your convenience, [`submit-mpi-omp.sh`](../lab3/SimpleRayTracing/submit-mpi-omp.sh). Edit this file to use your project code and email address as before.

`submit-mpi-omp.sh` creates another 4*8=32 scripts named `submit-mpi-omp-*-\*.sh` and submits the jobs with 1, 2, 3, and 4 nodes with 1, 4, 8, 16, 24, 40, 80 and 160 threads on each node.

For example, the script below `submit-mpi-omp-4-40.sh` is the script used to submit a job with 40 threads on 4 nodes, i.e. a total of 160 threads.

```bash
#!/usr/bin/env bash
#
#SBATCH -A scw2139                   # Project/Account (use your own)
##SBATCH --mail-user=YOUREMAILADDRESS@bangor.ac.uk  # Where to send mail
#SBATCH --mail-type=END,FAIL         # Mail events (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --job-name=RT-MPI-omp-4-40       # Job name
#SBATCH --output ray_tracing-%j.out  #
#SBATCH --error ray_tracing-%j.err   #
#SBATCH --nodes=4                    # Use one node
#SBATCH --ntasks-per-node=1         # Number of tasks per node
#SBATCH --cpus-per-task=40            # Number of cores per task
#SBATCH --time=00:25:00              # Time limit hrs:min:sec
#SBATCH --mem=600mb                  # Total memory limit
thread_number=40
module purge > /dev/null 2>&1
module load cmake mpi/intel
COMPILER="gcc (GCC) 4.8.5 20150623 (Red Hat 4.8.5-39)"
COMPILER="icc (ICC) 18.0.2 20180210"
TEMP=`lscpu|grep "Model name:"`
IFS=':' read -ra CPU_MODEL <<< "$TEMP"
width=2048
height=2048
echo Run ./main-mpi with 40 threads.
export OMP_NUM_THREADS=40
/usr/bin/time --format='%e' mpirun  ./bin/main-mpi-omp --size 2048 2048 --jpeg mpi-omp-4-40-2048x2048.jpg 2> temp-mpi-4-40
RUNTIME=`cat temp-mpi-4-40`
echo ${CPU_MODEL[1]},MPI-omp,$thread_number,4,$COMPILER,${width}x$height,$RUNTIME >> timing-mpi-omp-4-40.csv
#rm temp-mpi-4-40
```

> Compare this SLURM script to the MPI scrips used in lab 6, note how we are now using 40 threads per node instead of 40 processes. As processes are heavier than threads, we should see noticeable improvements in performance.

4. To launch `submit-mpi-omp.sh` use:

```bash
./submit-mpi-omp.sh
```

5. Wait for the job to complete. Use:

```bash
squeue -u $USER
```

6. When the job is terminated you will have new files. Examine their content. Are the JPEG files as expected?

7. To see the new images, download them from the Supercomputer to your PC.

Only proceed to the next step when everything works as expected. If not, debug your code or seek assistance.

---

### STEP 7: Full Performance Evaluation

For the final time:

- Concatenate the results you have generated into `timing.csv` as we have done previously.

- Use `plotTiming.py` to plot the runtimes and speedups of your serial, Pthread, OpenMP, MPI and MPI-OpenMP implementations.

**This concludes lab 7**
