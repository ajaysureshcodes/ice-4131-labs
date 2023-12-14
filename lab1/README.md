# ICE 4131 High Performance Computing - Lab 1

**Tutor:** Peter Butcher ([p.butcher@bangor.ac.uk](p.butcher@bangor.ac.uk))

**Lab Assistants**:

- Iwan Mitchell ([i.t.mitchell@bangor.ac.uk](i.t.mitchell@bangor.ac.uk))
- Frank Williams ([f.j.williams@bangor.ac.uk](f.j.williams@bangor.ac.uk))

### Objectives

Today’s lab will introduce you to the GNU/Linux environment used by Supercomputing Wales ([https://supercomputing.wales](https://supercomputing.wales)). You will learn how to:

- Log in
- Write and compile code
- Run your first parallel tasks using POSIX threads (aka. Pthreads)

### Task List

Today's tasks are as follows:

1. [Getting Access to the Supercomputer](#step-1-getting-access-to-the-supercomputer)
2. [Logging in to the Supercomputer](#step-2-logging-in-to-the-supercomputer)
3. [What you now have access to](#step-3-what-you-now-have-access-to)
4. [Basic Linux Commands](#step-4-basic-linux-commands)
5. [Compile some code](#step-5-compile-some-code)
6. [Parallelise some code](#step-6-parallelise-some-code)

---

## STEP 1: Getting Access to the Supercomputer

To begin with, you will need a user account. Follow the instructions here under “Applying for a user account”:

[https://portal.supercomputing.wales/index.php/getting-access/](https://portal.supercomputing.wales/index.php/getting-access/)

When applying for an account, you should state that you are joining project: **scw2139**

It will take a few minutes for your account to be activated, so please be patient once you have submitted your application to join the supercomputer. Begin reading the next steps but do not attempt to log in to the supercomputer until you are notified that your account is active!

---

## STEP 2: Logging in to the Supercomputer

Once you have been notified that your account has been opened, follow the instructions on the “Accessing the system” page:

[https://portal.supercomputing.wales/index.php/index/accessing-the-system/](https://portal.supercomputing.wales/index.php/index/accessing-the-system/)

There are different instructions for accessing the supercomputer on Windows and Linux/MacOS.

Bangor users must log in to Cardiff’s system:

```bash
hawklogin.cf.ac.uk
```

Your username must be prefixed by `b.` for example, my username is:

```bash
b.ptb18xhf
```

An example login would therefore be:

```bash
b.ptb18xhf@hawklogin.cf.ac.uk
```

Please note: If you have 2 failed login attempts and fail to login after the third attempt, the IP address of the machine you are using will be banned from accessing the supercomputer for 24 hours. As such, if you cannot remember your password after 2 attempts, reset it at [https://my.supercomputing.wales](https://my.supercomputing.wales).

---

## STEP 3: What you now have access to

Once logged in, you will have access to:

- 201 nodes,
- totalling 8,040 cores,
- 46.080 TBytes total memory

Each node has

- CPU: 2x Intel(R) Xeon(R) Gold 6148 CPU @ 2.40GHz with 20 cores each
- RAM: 192 GB by default, or 384GB on high memory and GPU nodes
- GPU: 26x nVidia P100 GPUs with 16GB of RAM on 13 nodes
- Storage:
  - 692TB (usable) scratch space on a Lustre filesystem
  - 420TB of home directory space over NFS

We will not be submitting jobs to the supercomputer today but will be writing and compiling code to run in your own personal home environment on the supercomputer.

---

## STEP 4: Basic Linux Commands

### Command Prompt Basics

- `man man`

  Displays manual information on the manual command

- `man [command]`

  Displays manual information on command

- `clear`

  Clears the screen

- `exit`

  Exits the command interpreter

### Manipulating Directories

- `cd ..`

  Change to the parent directory

- `cd [directory]`

  Change to directory `[directory]`

- `mkdir [directory]`

  Create directory `[directory]`

- `rmdir [directory]`

  Remove directory `[directory]`

### Listing Files

- `ls`

  Display list of files and sub directories in standard format `<name>` excluding hidden files

- `ls -a`

  Display list of files and sub directories in standard format `<name>` including hidden files

- `ls -l`

  Display list of files and sub directories in long format `<permissions  owner  group  size  datetime name>`

- `ls -lh`

  Display list of files and sub directories in long format `<permissions  owner  group  size  datetime name>` with human readable size

- `ls -lt`

  Display list of files and sub directories in long format `<permissions  owner  group  size  datetime name>` sorted by time

- `ls -lr`

  Display list of files and sub directories in long format `<permissions  owner  group  size  datetime name>` in reverse order

- `ls -ltrh`

  Display list of files and sub directories in long format `<permissions  owner  group  size  datetime name>` sorted by time, in reverse order, with human readable size

### Moving Files

- `mv [source] [dest]`

  Move file `[source]` to file `[dest]`

- `mv -i [source] [dest]`

  - Move file `[source]` to file `[dest]`
  - Prompt before overwriting `[dest]` if it exists

- `mv -f [source] [dest]`
  - Move file `[source]` to file `[dest]`
  - Overwrite `[dest]` if it exists

### Removing Files

- `rm [file]`

  Remove file `[file]`

- `rm -i [file]`

  - Remove file `[file]`
  - Prompt before removing

- `rm -R [directory]`
  - Remove directory `[directory]`
  - Remove all sub directories and files

### Copying Files

- `cp [source][dest]`

  Copy file `[source]` to file `[dest]`

- `cp -i [source][dest]`

  - Copy file `[source]` to file `[dest]`
  - Prompt before overwriting `[dest]` if it exists

- `cp -R [source][dest]`

  Copy directory [source] to directory [dest]Copy all sub directories and files

### Displaying Files

- `more [file]`
  - Display `[file]` on the screen
  - Will scroll through one screen at a time
  - Press space to scroll one screen at a time
  - Press enter to scroll one line at a time

### Comparing Files

- `diff [file1] [file2]`

  Display differences between `[file1]` and `[file2]`

- `fgrep "string" [file]`

  Find “`string`” in `[file]`

- `sort [file]`

  Sort `[file]`

### Command Modifiers

- Wildcards allow you to specify multiple items to operate on
  - `ls *.txt`
  - `rm *.txt`
- Redirection allows you to direct the output of one command to a file
  - `sort unsorted.txt > sorted.txt`
- Filters are external commands that change data in some manner
  - `fgrep "string" [file]`
- Pipes let you direct the output of one command as input to another
  - `ls | find "txt"`

### Other Commands

- `who`

  Show who is logged on

- `top`

  Show which tasks are running

- `watch`

  Run a task repeatedly

- `history`

  Show which tasks you ran

- `date`

  Display or set the date and time

- `cat`

  Concatenate files and print on screen

- `head`

  Print top of file on screen

- `tail`

  Print bottom of file on screen

- `uniq`

  Report or omit repeated lines

- `pwd`

  Display name of current/working directory

- `hostname`

  Display the system’s host name

- `touch [filename]`

  Create a file [filename]

### Editing Files

There are three main console-based text editor:

- Nano (`nano`)
- Emacs (`emacs`)
- Vi (`vi`)

If you are not familiar with Vi nor Emacs, then I recommend you to use Nano. It is easier to handle.

---

## STEP 5: Compile some code

We’re going to write and compile some C++ code on the supercomputer.

In your home (`cd ~`) directory, create a new sub-directory called `lab1`, enter that directory and create a new subdirectory called `helloworld`, create a new file called `helloworld.cxx` and type the source code as follows using your favourite text editor:

![helloworld.png](assets/helloworld.png)

To create an executable file, you need to compile your source code.

With a UNIX system such as Supercomputing Wales, it can get very complicated for the end user to maintain many different versions of available programs. To simply matters, we use the **module environment** – which enables the user to pull in software as and when it is needed. To find out what software is available, type:

```bash
module avail
```

This will list all the available software for you to use. To look at compilers only, type:

```bash
module avail compiler
```

This will return a list of available compilers. For us, we will requier one tool to compile some source code, and to run the resulting executabel code. At present compilation tools are not available, for example, if you type:

```bash
icc
```

The commend prompt will respond with:

```bash
-bash: icc: command not found
```

If you type `g++ --version` you will get an old version of g++. We will be using one of the latest GNU compilers, to install it, type:

```bash
module load compiler/gnu/9/2.0
```

This will load in the new compiler we wish to use. Now if you type in `g++ --version` you will see `g++ (GCC) 9.2.0`

To save having to type this in each time we log into the machine, we have put these commands in a batch script for you - this is a short text file that contains the above commands. Let's now start to look at compiling the example code, and using the batch script file to save typing. This is covered in the next section.

To compile your source code, just type:

```bash
g++ helloworld.cxx -o helloworld
```

If you didn’t make any errors, it should compile without errors and a new file helloworld was created. To run it, type:

```bash
./helloworld
```

---

## STEP 6: Parallelise some code

### General Concepts

A very common strategy to parallelise code is to identify for/while loops and replace them by parallel code. In the listing above, there is a for loop at Line 5. We are going to parallelise it using POSIX Threads (usually referred to as Pthreads). It is a POSIX standard for threads. Pthreads are a simple and effective way of creating a multi-threaded application.

There are 5 main steps to convert serial code to parallel code with Pthreads:

1. All C/C++ programs using Pthreads need to include the pthread.h header file.
2. `#include <pthread.h>` at the top of your file.
3. Create an entry point for the thread
   - When creating a thread using Pthreads, you need to point it to a function for it to start execution.
   - It is the thread's callback function.
   - It returns `void*` and take a single `void*` argument.
   - For example, if you want the function to take an integer argument, you will need to pass the address of the integer and dereference it later. This may sound complicated but, as is shown below, it's pretty simple. An example function signature would be:

```c
void* callback_function(void* param);
```

4. Define thread reference variables
   - The variable type `pthread_t` is a means of referencing threads.
   - There must be a `pthread_t` variable for every thread being created!
   - For example, it can be: `pthread_t thread0;`
5. Create the thread
   - Once the `pthread_t` variable has been defined and the entry point function created, we can create the thread using `pthread_create`.
   - This method takes four arguments:
     1. A pointer to the `pthread_t` variable,
     2. Any extra attributes (don't worry about this for now - just set it to `NULL`),
     3. A pointer to the function to call (i.e. the name of the callback) and
     4. The pointer being passed as the argument to the function.
   - Now there's a lot of pointers in that call, but don't stress - it's not as tricky as it sounds. This call will look something like:

```c
pthread_create(&thread0, NULL, callback_function, &parameter);
```

6. Join everything back up
   - When the newly-created thread has finished doing it's bits, we need to join everything back up.
   - This is done by the `pthread_join` function which takes two parameters: the `pthread_t` variable used when `pthread_create` was called (not a pointer this time) and a pointer to the return value pointer (don't worry about this for now - just set it to `NULL`).
   - This call will look something like:

```c
pthread_join(thread0, NULL);
```

And that's all there is to it. The function used as the thread entry point can call other functions, create variables or do anything any other function can do. It can also use the variables set by the other thread.

### First example: parallel `helloworld`

Below is the `helloworld` program with parallelisation using Pthread. The for loop has been replaced by concurrent threads.
Create a new file `helloworld-pthread1.cxx` with the source code below.

![helloworld-pthread1.png](assets/helloworld-pthread1.png)

When compiling the program, you will also need to add `-lpthread` to the compile command:

```bash
g++ helloworld-pthread1.cxx -lpthread -o helloworld-pthread1
```

Now run `helloworld-pthread1`. **Can you explain what happens?**

To make it a little bit easier to understand what is happening, complete the next section.

### Second example: passing parameters

In this example, we show how to pass parameters to a thread.
We add an extra `vector` to store a unique numerical ID (as `unsigned int`) for each thread (see Lines 20 and 25).
Remember that the address of the variable is passed to the thread (see `&` at Line 26).
In the callback, the address as to be converted from `void*` to `unsigned int*` (see Line 44).
Then it is possible to retrieve the numerical value (see Line 47).

![helloworld-pthread2.png](assets/helloworld-pthread2.png)

Create a new file `helloworld-pthread2.cxx` with this source code. You can copy `helloworld-pthread1.cxx` into `helloworld-pthread2.cxx` to save some time:

```bash
$ cp helloworld-pthread1.cxx helloworld-pthread2.cxx
```

Now modify the content of `helloworld-pthread2.cxx`.
Compile your new program and run it several times.
**Can you explain what is happening?**

### Third example: Mutual exclusion

A critical section is a piece of code that accesses a shared resource.
In the previous example, threads shared the standard output and tried to write into it at the same time.
This is why the output is so messy.
To tidy it up, we want to make sure that only one thread writes in the console at a time.
In other words, we will ensure that no two concurrent threads are in their critical section at the same time.
This mechanism is called "Mutual exclusion" (commonly known as "mutex").

We will add three lines of code to the previous example.
On Line 10, we declare and initialise a mutex.
Before the critical section (i.e. before writing in the console), we lock the mutex (see Line 55).
After the critical section, we release the mutex (see Line 60).

This is what we do in a new file `helloworld-pthread3.cxx`:

![helloworld-pthread3.png](assets/helloworld-pthread3.png)

Compile and run `./helloworld-pthread3.cxx`.

The output is presented as expected. Only one thread writes in the standard output at a time.

**This concludes lab 1**
