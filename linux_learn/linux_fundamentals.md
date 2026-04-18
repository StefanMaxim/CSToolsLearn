# Linux Fundamentals


## File Commands

- ls  //lists all files in a directory
- ls -la  //lists all files, even the hidden ones .vimrc for example
- cd dir //changes cwd to dir
- cd //changes cwd to ~
- mkdir dir //creates new dir names dir in cwd
- rm file //removes file file from cwd
- rm -r dir //removes a dir
- rm -f file //forcefully removes a file
- rm -rf //forcefully removes a directory
- cp file1 file2 //copies files 1 to files 2
- cp -r dir1 dir2 //copies dir1 to dir2
- mv file1 file2 //moves the file1 to file2

### Tricky:
- ln -s file link //creates a symlink link to file
> What does that mean:  
> **Symbolic Link:** a special type of file that links to the location file, so when you write `cd link` it actually runs `cd file`

- stat file // prints out the metadata of the file to the stdout
- touch file //creates a file named file
- cat > file 
> What does this mean:  
> Usually, `cat` us used like `cat file` which will output the file to the stdout, or can be piped via `cat file | `
> In this case however, it will make it so anything you type in the terminal is auto redirected to the file

- more file // outputs the files
- head file //shows first 10 lines of teh file
- tail file //shows last 10 lines of the file

## Process Management

### What is a process?
A process is essentially a container for running a piece of software/program  
> **Program** = bytes on a disk (an executable file)  
> **Process** = that program in motion + all the other stuff needed to run it in motion

The parts of a process:

#### Its own Virtual Memory:
When a process is running, it has the illusion that is has access to limitless memory(from 0x0000 upward in memory)
**THAT SPACE CONTAINS:**  
- CODE (the machine instructions for that process)  
- Static Data (stuff like globals and constants for that program alone) (**REMEMBER**, to not want it have access to stuff from other programs)  
- Heap (grows as you malloc / allocate more space for that process)
- Stack (Contains Function call frames: return addresses, local variables, saved registers)

**WHY IS THIS MEMORY VIRTUAL**:
the process thinks its continuous, but in reality, the os keeps a table that maps the process's virtual memory addresses into actual memory addresses on the RAM. This way, the process can behave as it it were continuous when in reality
is is not.

#### CPU Execution State

> **NOTE** READ THE LINUX_PIPELINE.txt to learn more about the stack and instruction pointer!!!

When the OS switches between processes, it must keep track of where it must return to via the RSP and
what is it currently executing RIP

> **NOTE** while yes, the stack does contain a return address, call and ret are not the only time that discontinuities
occur!! thus, you have to save both RSP and the stack and RIP!!

exe:

suppose inside of foo, your instructions read

mov rax, [rbp-0x10]
add rax, 7
imul rax, rxb 

(note that by fetching the thing stored at that memory, your RIP will change to that memory, and so you will
have lost your current position in the code)
THE STACK DOESNT SOLVE THIS because you are not using call or ret here, just a simple memory read that will
make the chain of execution non-continuous!!!

THUS, you (as the process), must store:
1. THE RIP
2. THE RSP
3. Other Registers

(WAIT?!?! i thought the stack was part of ISA, meaning its tied to the hardware and process independant right?)
WRONG!!! because of the aformeantioned virtual memory, each process has its own stack!
infact, a process can have multiple threads each with their own stack


> **NOTE**
> Inside of a single process, there is a shared address space (virtual memory space) that is shared by all the threads
> BUT ALSO, a separate CPU register state where the RIP and RSP and stack are stored for each thread.



#### A Process ID and Metadata
Each process has a PID, or process ID that allows the computer to keep track of all of them

#### Open Handles to Resources
In linux, aside from your computer you can have many different devices
(essentially a wraper word for other files, drives, peripherals, all that stuff)

A program can often want to interact with these different devices, so it keeps a table of
file descriptors (because remember, everything is linux is treated as a file)

0 = stdout
1 = stdin
2 = stderr
but they can also refer to:
- files
- pipes
- terminals
- sockets (dont need to know the details)



### KEY DISTINCTION:

> **THREAD**
> a thread is just a continuous execution stream inside of a process (this is what has a stack and RIP and RSP)

> **PROCESS**
> memory address space + resource table


## File Permissions

## SSH

## Searching

## System Info

## Compression
Compression + Combining happens via 2 methods:
Combine files = tar, compress = gzip

tar: tape archive, bundles multiple files + folders into one file, preserving directory perms and timestamps

tar -xzf filename.tar.gz //order is what you want to do (X) filter (gzip compression), and file

Action Tasks:
-c create archive, used to create new archives of files (combine them)
-x extract archive, used to create the normal directory structure from the tar.gz
-t lists the contents inside the file without extracting

File flag:
-f tells tar which file to use

Compression Flags
-z uses gzip, either for compression or decompression
-j bzip
-J xz compression (LZMA compression)

-v verbose

tar -czf archive.tar /folder
(results is called TARBALL)

**A tar archive (tar) stores:**

filenames
directory structure
file contents (bytes)
basic metadata (permissions, timestamps, owner IDs)
(this makes it OS-indepentant, ie can compress and create on linux and decompress on mac)

Tarballs = Universal , tar is OS-based but abstracts away OS specific details

to work, it uses syscalls like open() read() write() mkdir() chmod() when on POSIX compliant systems like mac and linux






gzip: compresses a single file using the DEFLATE algorithm

gzip file //comrpesses it via gzip
gzip -d file //de-comrpesses the file
gzip -k file //**CRUTIAL** ZIP DELETES THE ORIGIONAL FILE, SO THIS MAKES SURE TO KEEP IT
gzip -c file //does not modify file, instead writes compressed version to stdout

gzip -c file >> file.gz
or -dc

-v verbose
-r recursive compression (USEFUL)




## Network

## Installation

## Shortcuts

## Generic

- man (cmd) // checks the manual for how to use a command




