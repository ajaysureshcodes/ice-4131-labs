# ICE 4131 High Performance Computing - Lab 3

**Tutor:** Peter Butcher ([p.butcher@bangor.ac.uk](p.butcher@bangor.ac.uk))

**Lab Assistants**:

- Iwan Mitchell ([i.t.mitchell@bangor.ac.uk](i.t.mitchell@bangor.ac.uk))
- Frank Williams ([f.j.williams@bangor.ac.uk](f.j.williams@bangor.ac.uk))

### Objectives

_**NOTE:** If you have not completed the tasks from the [previous labs](../), you should complete those first!_

Today’s lab will give you some practice at profiling existing serial programs with `gprof` to identify the portions of the code that are slow. We will run some sample code on the compute nodes of the Supercomputer with output images of increasing sizes. We will run tests using the `g++` and `icc` compilers and plot graphs of the runtimes for analysis.

For today's exercise, some sample code is provided on GitHub [here](./SimpleRayTracing/). It's a small ray-tracer to produce images such as:

![SimpleRayTracing Test Image](./assets/srt-test-image.jpeg)

The serial code of a simple ray tracer is located in `SimpleRayTracing/src/main.cxx`.

- You are not expected to write any C/C++ code this week.
- You will submit jobs using SLURM (see [lab 2](../lab2/README.md)) and create a spreadsheet.

### Task List

Today's tasks are as follows:

1. [Get the Code](#step-1-get-the-code)
2. [Compiling Environments](#step-2-compiling-environments)
3. [Profiling with the GNU Compiler](#step-3-profiling-with-the-gnu-compiler)
4. [Turn Off Debugging and Profiling](#step-4-turn-off-debugging-and-profiling)
5. [Run Your Program with Various Image Sizes](#step-5-run-your-program-with-various-image-sizes)
6. [Create a Spreadsheet of Profiling Results](#step-6-create-a-spreadsheet-of-profiling-results)

---

## STEP 1: Get the Code

1. Log on to `hawklogin.cf.ac.uk` using your favourite SSH client, e.g. [PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty), `powershell` on MS Windows or VSCode.
2. If you haven't already done so, download this GitHub repository into your home directory on the Supercomputer using:

```bash
git clone https://github.com/PButcher/ice-4131-labs.git
```

3. This creates a new directory called `ice-4131-labs`. Navigate to `ice-4131-labs/lab3/SimpleRayTracing`:

```bash
cd ice-4131-labs/lab3/SimpleRayTracing
```

---

## STEP 2: Compiling Environments

1. Create two scripts to set your environment. One for the GNU C++ compiler (`g++`); one for Intel's (`icc`). In the current directory (which should be `SimpleRayTracing`), create:

   - `env-gnu.sh` that contains

   ```bash
   module purge > /dev/null 2>&1
   module load cmake
   module load gnuplot
   module load compiler/gnu/9/2.0
   ```

   - `env-intel.sh` that contains

   ```bash
   module purge > /dev/null 2>&1
   module load cmake
   module load gnuplot
   module load compiler/intel/2020/4
   ```

2. Create a directory for binaries compiled with each compiler using `mkdir`, e.g.

```bash
mkdir bin-gnu
mkdir bin-intel
```

We will output our compiled programs with the two different compilers to each of these directories to keep things neat and tidy.

---

## STEP 3: Profiling with the GNU Compiler

1. Set your environment to binaries with the GNU compiler

```bash
source env-gnu.sh
```

> **ESSENTIAL:** You'll need to run this command EVERY TIME you log in, or every time you want to change compiler. It's easy to forget, so make a note of this one!.

2. To check that the modules are loaded, you can use

```bash
module list
```

3. Go into the appropriate `bin` directory, i.e. `bin-gnu`, using the `cd` command

```bash
cd bin-gnu
```

4. Configure your project using CMake

```bash
cmake ..
```

The two dots (`..`) is the parent directory of the current directory. CMake is looking for the `CMakeLists.txt` file which we store in the parent directory so that it is easily accessible across the project.

5. Build the project

```bash
make
```

It's going to take _a long time_ to start with as it's building third party libraries to load 3D models. Be patient and don't worry, things _are_ happening.

6. Enable profiling: By default CMake will enable the `Release` mode for producing code that is fast. You don't want that for now as we need to assess the program to know what is slow.

   - Run `ccmake .` (yes, two `c`s). You can use one or two dots, it does not matter as CMake already knows where `CMakeLists.txt` is!

   ```bash
   ccmake .
   ```

   - Change the `CMAKE_BUILD_TYPE` variable to `Debug`
     - Use the arrows to go to the right line.
     - Press `ENTER` to edit.
     - Type `Debug`
     - Press `ENTER` to save the change.
   - Press `t` to "toggle".
   - In the `CMAKE_CXX_FLAGS_DEBUG` variable, add the `-pg` option. It will enable profiling.
   - Press `c` to "configure".
   - Press `g` to "generate" the project.

7. Compile again. Only the ray-tracer will compile, not the third party libraries. You'll see, it will be faser this time.

```bash
make
```

- The program can use command line arguments:

```bash
Usage: ./main <option(s)>
Options:
	-h,--help                       Show this help message
	-s,--size IMG_WIDTH IMG_HEIGHT  Specify the image size in number of pixels (default values: 2048 2048)
	-b,--background R G B           Specify the background colour in RGB, acceptable values are between 0 and 255 (inclusive) (default values: 128 128 128)
	-j,--jpeg FILENAME              Name of the JPEG file (default value: test.jpg)
```

8. Submit a job using `sbatch`. There are already scripts available for you. Have a look at `submit-serial-gnu.sh` in a text editor. You need to edit it. For example you can change the email address to use yours (remove one of the leading `#` characters to enable):

```bash
cd ..
nano submit-serial-gnu.sh
sbatch submit-serial-gnu.sh
squeue -u $USER
```

This will create a file called `gmon.out` in your current directory (`SimpleRayTracing`).

9. Wait for the job to complete. In the meantime, have a look at [https://sourceware.org/binutils/docs/gprof/](https://sourceware.org/binutils/docs/gprof/) for information about how to use the `gprof` profiler.

10. Analyse the results using:

```bash
gprof bin-gnu/main > serial-profiling.txt
nano serial-profiling.txt
```

> **PRO TIP:** The `>` character redirects the output of a command to a new file, which you can name whatever you like. In the example above, the output of `gprof bin-gnu/main` will be sent to a new file called `serial-profiling.txt`. You can then open that file instead of reading from the terminal output which should be more comfortable.

11. Identify which steps took the longest. **Make sure you document what your results are as you'll need them in your assignment.** You can always re-run profiling later if you prefer.

---

## STEP 4: Turn Off Debugging and Profiling

1. Set your environment to create binaries with the GNU compiler

```bash
source env-gnu.sh
```

3. Go into the `bin-gnu` directory using the `cd` command

```bash
cd bin-gnu
```

4. Configure your project using CMake (remember it's 2 `c`s to conifigure CMake and 1 `c` to run CMake)

```bash
ccmake ..
```

5. Change the `CMAKE_BUILD_TYPE` variable to `Release`, configure and generate.

6. Build the project with `make`:

```bash
make
```

7. Configure and build the project with the Intel compiler

```bash
cd ..
source env-intel.sh
cd bin-intel
cmake ..
make
cd ..
```

---

## STEP 5: Run Your Program with Various Image Sizes

Next we will assess the performance of the Intel compiler against the GNU compiler. We will generate images with the two programs we created, with various different image sizes to see how well the performance scales.

### 128x128

By default the images have a width of 128 pixels and a height of 128 pixels, denoted by 128x128.

- Submit the we created jobs

```bash
sbatch submit-serial-gnu.sh
sbatch submit-serial-intel.sh
```

### 256x256

- Edit `submit-serial-gnu.sh`

```bash
nano submit-serial-gnu.sh
```

- Find the width and the height of the image in the script and change the values accordingly.

- Submit the job

```bash
sbatch submit-serial-gnu.sh
```

- Edit `submit-serial-intel.sh`

```bash
nano submit-serial-intel.sh
```

- Find the width and the height of the image in the script and change the values accordingly.

- Submit the job

```bash
sbatch submit-serial-intel.sh
```

### 512x512, 1024x1024, 2048x2048, 4096x4096

Do the same but with other image sizes.

---

## STEP 6: Create a Spreadsheet of Profiling Results

Once the jobs start to complete, you can observe the results:

```bash
cat timing.csv timing-serial-intel-*.csv timing-serial-gnu-*.csv > runtime.csv
```

> **PRO TIP:** The `cat` commands reads each of the files it is passed as arguments and prints them to the command line. By using the output redirection operator (`>`) we can instead output the result to a new file (in this case `runtime.csv`), saving it for later, or for more comfortable reading.

1. Download `runtime.csv` using your preferred SFTP client, e.g. WinSCP on the lab machines.
2. Open the file using MS Excel or equivalent.

Now some questions for you to think about:

- For each image resolution, which compiler provided the fastest computations?
- What is your conclusion?

Write your thoughts in a new report document, along with your findings from [step 3, part 11](#step-3-profiling-with-the-gnu-compiler).

**This concludes lab 3**
