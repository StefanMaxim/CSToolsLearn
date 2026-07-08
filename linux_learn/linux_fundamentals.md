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

### grep:

Loose overview:
grep "pattern" file.txt

grep "TODO" file1.txt file2.txt (SEARCH MULTIPLE FILES)
grep "TODO" * (checks all files in current dir)
grep -r "TODO" . (recursive check through directories)

grep -i (case-insensitive)
grep -n (show line number + file, often better than just l which shows filename)

grep -l "password" * (Shows only filenames matching)

grep -v "debug" app.log (KEY: shows lines NOT containing debug)
grep -c (counts matching lines)
grep -w "cat" file.txt (matches whole words, matches cat, but not category)
grep -x "cat" * (matches only lines that say cat and nothing else)

Useful combos:
grep -rin "error" .

grep -r "useState" --include="*.jsx" . (forces that files have certain extension)
grep -r "TODO" . --exclude-dir=node_modules (excludes certain directories)
grep -r "TODO" . --exclude="*.min.js" (excludes certain files)
(NO INCLUDE DIR OPTION)

**NOW REGEX TIME!!!**

grep "^ERROR" app.log (means line must start with ERROR)

grep "done$" output.txt (means line must end with done)

grep "gr.y" file.txt (means matches a single character, can be gray, grey, doesnt matter)

grep -E "error|warning|failed" app.log (the | regex means can be either or)

grep -E "[0-9]+" file.txt (means can be any numbers from 0 to 9, + meaning 1 or more)

grep -E "[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}" file.txt
(matches email, kinda)

-F means LITERAL STRING, or FIXED STRING, WILL IGNORE THE . IN REGEX STUFF


NOTE:
if you 
ls | grep -il"java" , it will return (standard input) since it doesnt have file-type context when piping

NOTE: the ls stuff will be interpreted as a series of paths,
ls | grep -ril "java", does work, and will search all of the directories outputted by ls recursively and list out
which files contain "java", case insensitive, in any form


rincvlxwE







### NOW ONTO FIND:
find searches for files/directories that matches the rules you specify

find WHERE CONDITIONS ACTION

find . -type f (searches for files in current directory, -type d for directories, -type l for symlinks)

-name "*.py" (searches for all .py files)

-size +100M (over 100 MB)
-mtime -1 (modified less than one day ago)

-o means OR

\(...\) used to group conditions together

-prune means do not search inside of matched directory
-print means print the matches

find . -type d \( \
  -name node_modules -o \
  -name .git -o \
  -name dist -o \
  -name build -o \
  -name .next -o \
  -name .venv -o \
  -name venv -o \
  -name __pycache__ -o \
  -name .pytest_cache -o \
  -name .mypy_cache \
\) -prune -print

-iname for case independent name, better if not sure

find /path/to/directory -maxdepth 1 -type f

maxdepth useful



## System Info

### ps
ps:
lists the current processes that are active on your computer.



## Compression
Compression + Combining happens via 2 methods:
Combine files = tar, compress = gzip


### tar
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

tar -czf archive.tar.gz /folder  (MUST BE CLEAR WITH THE EXTENSIONS, AS THEY ARE CONVENTION)
(can add more than just folder, and files are added in the order specified by the command list arg)

-r is like c, but new entries appended to the archive (uncompresssed archives only)
-u like r, but only adds it modification date newer than corresponding entry in the archive
(results is called TARBALL)

-f means read the archive from or write the archive to the specified file.(changes use based on c vs xq)
**A tar archive (tar) stores:**

filenames
directory structure
file contents (bytes)
basic metadata (permissions, timestamps, owner IDs)
(this makes it OS-indepentant, ie can compress and create on linux and decompress on mac)

Tarballs = Universal , tar is OS-based but abstracts away OS specific details

to work, it uses syscalls like open() read() write() mkdir() chmod() when on POSIX compliant systems like mac and linux





### GZIP
gzip: compresses a single file using the DEFLATE algorithm

gzip file //comrpesses it via gzip
gzip -d file //de-comrpesses the file
gzip -k file //**CRUTIAL** ZIP DELETES THE ORIGIONAL FILE, SO THIS MAKES SURE TO KEEP IT
gzip -c file //does not modify file, instead writes compressed version to stdout

gzip -c file >> file.gz
or -dc

-v verbose
-r recursive compression (USEFUL) (it compresses the files in a directory one by one)




## Network

### curl
curl:
curl, by default 
curl https://example.com
will
1: download the content (holds it in memory as a stream)
2: writes it to stdout
3: does not save it to a file


-o means write output to a file (like how -O which means to download as per that file name)
-O
-o- means to write to stdout instead (same as default, but more explicit)
-f fail silently on http errors (exe 404, will not output the html error page, instead exit with error)
-s silent mode (do not output what you are doing to stdout)
-S show error (in the even of an error, print it to stdout)
-L follow redirects (301, 302 re-direct)







## Installation

## Shortcuts




## Core System

### sudo

### systemctl
main way to interact with systemd (look at systemd.md for more info)

sudo systemctl poweroff -i
-i means ignore inhibitors, so it can shutdown even with others trying to block the shutdown, like other connected users





### apt
primary debian package manager (look at packages_learn.md for more info)

### dpkg
underlying debian package installer (look at packages_learn.md for more info)


## Generic

### man
- man (cmd) // checks the manual for how to use a command

### single quotes, double quotes, and commmand substitution
- "" vs '' vs $() and how they interplay in commands like

Double QUotes:
they preserve spaces and most special characters, but still allow variable expansion and command substitution.

variable expansion:
name="Alice"
echo "Hello $name" outputs Hello Alice

command substitiion:
echo "Today is $(date)"
the $(date) gets executed

why quotes matter:

file="my file.txt" (NO SPACES)
rm $file becomes rm my file.txt, which is treated as 2 different arguments, with quotes, it becomes a single argument

rm "$file" will enforce that it is one single string, and delete "my file.txt", not the 2 files "my" and "file.txt"

**KEY**
file=my file.txt is parsed as:

file=my (string assignment, as everything is strings in the shell)
file.txt is to be executed!

**GLOBBING IS NOT PERFORMED INSIDE QUOTES**




Single Quotes:

everything is treated literally

name="Alice"
echo '$name' outputs $name

likewise
echo '$(date)' outputs $(date)


more on command substitution:
It 
1: runs a command and
2: replaces the entire expression with the commands output (KEY)

echo $(date)
might become:

echo Thu Jun 4 12:34:56 HST 2026

which then prints Thu Jun 4 12:34:56 HST 2026


another example:
files=$(ls)
files stores the output of ls

you can nest it via:

echo "$(whoami) is in $(pwd)"
george is in /home/george



















### eval vs exec
eval '$(/opt/homebrew/bin/brew shellenv)'

eval vs exec:

exec: replaces the terminal (the process as a whole) with a new process as specified by the ELF/Mach-O of another file. eval just evaluates a command
(note: if you specify a command via /, it will not execute it as a builtin, and instead create a new process for it)
echo is builtin, /bin/echo is not
NOTE: builtin zshrc in /etc/zshrc

### ln
ln: used to create hard links or symlinks in linux
ln source_file target_file

2 types of links: hard links and symlinks
hard link: the dentry directly references the inode number of the file you are referencing
(like . or ../file)

symlink: instead, a symlink is a file whose contents are a string representing the path to the file you
are linking. (path is stagnant, so moving file messes up the symlink)
(use man ln for more details)

-s creates symbolic link
-f if target exists, unlink it so link may occur
-i writes error in stderr if target already exists
-h/-n if target is a symbolic link, do not follow it. useful when replacing a symlink that may point to a directory











### cat
cat: cat copies its input to its output.
if you type cat, and then type hello, it will echo back hello
(because reading from stdin(keyboard) and writing to stdout(terminal))

<<'EOF' is the here document, meaning pretend everything up to here is a document untill you reach a line containing EOF

so 
cat <<'EOF'
hello
world
EOF

is the same as cat from a document whose input was "hello \n world\n"
(EOF here is just a marker, it could be any other string you desire, with no quotes around it)

so cat [file1] [file2] means read those files and output then to stdout or whatever is your output

thus:

cat > ~/.ssh/.gitignore <<'EOF' redirects teh output to that file, its like an easy way to do it without vim!


### shell globs
shell globs are patterns that the shell expands into matching filenames
exe, given files:
apple.txt
apple.log
banana.txt
notes.md

"mv *pattern* destination/" is expanded by teh shell into "mv apple.txt apple.log destination/"

Common Shell globs:


'*' meaning 0 or more characters:
*.txt -> apple.txt banana.txt

'?' meaning exactly one character
file?.txt

[abc] meaning one character from a set
file[12].txt -> file1.txt file2.txt (**KEY**: if only file1 exists, no file2, it will just match the files that DO exist)

[a-z] meaning one character in a range
file[1-9] -> file1 file2 ... file 9 **KEY2** it matches characters, meaning fileo.txt is perfectly valid, even if o is not a number!

[!abc] meaning one character NOT in a range
file[!0].txt -> file(some character that is not 0).txt

More common ones
[[:digit:]] one digit
[[:alpha:]] one letter
[[:alnum::]] one letter or number
[[:lower:]] one lowercase letter
[[:upper:]] one uppercase letter
[[:space:]] one whitespace

**NOTE** the only mutli-character symbol for globs is *, which can be used rather liberally

```bash
report*2025*.csv
```
matches
report2025.csv
report_final_2025.csv
report-v2-2025-backup.csv

Double Star:

"results/**/*.npz" also matches and works, which matches to directories
IT MEANS MATCH 0 OR MORE DIRECTORIES RECURSIVELY
results/file.npz                  ✅
results/run1/file.npz             ✅
results/run1/epoch10/file.npz     ✅
results/a/b/c/d/file.npz          ✅

Single Star:

results/*/*.npz also works, but it enforces it must be only 1 directory.


KEY: These shell globs will be expanded by the shell, so if you dont want that, better to use them in quotes.
When inside quotes, the shell glob will be passed as is to the executable, not expanding it, letting it use it as
a argument.



### REGEX



### Pipes and Redirects (WIP)

Pipes "|" connects teh standard output of the command on the left to the standard input of the command on the right.

**KEY**
stdin, stdout, and stderr are just file descriptors, defaulting to 
Keyboard ───► stdin (fd 0)
Terminal ◄── stdout (fd 1)
Terminal ◄── stderr (fd 2)

#### **FILE DESCRIPTORS** 
are just numbers used in a linux process to refer to different periferals, dictated by a 
kernel-side file descriptor table.


when you read(0, buf, 100);, its saying "read from whatever file descriptor 1 refers to"
The operating system keeps another data structure that represents an open file (more generally, an open I/O object).
a regular file
a terminal
a pipe
a TCP socket
a device like /dev/null


**NOTE** file descriptOR is the number, file descriptION is the datastructure representing the open file
containing things like current file offset, file status flags (O_APPEND, O_NONBLOCK, etc.), pointer to the underlying inode or file object, and reference count.

IN PRACTICE:

echo hello > out.txt

1: Before running echo, shell opens out.txt
This returns a new file descriptor, say
fd 3 -> out.txt

2: Forks process, using fork()

3: In new child process, Copies fd3 onto fd1
fd1 -> out.txt
fd3 -> out.txt

4: closes fd3
fd1 -> out.txt

5: Then, it starts echo via execve()
where the child process of echo will already have the modified file descriptor table. 
**NOTE** THIS IS BECAUSE EACH PROCESS'S FILE DESCRIPTOR TABLE IS KERNEL-SIDE, SO REMAINS AFTER EXECVE
that being said, each process has its own table, however they can point to the same description after a fork.

These file descriptions are stored in the kernel in a larger table. However
**CRUTIAL** you cannot see these tables in kernelspace!

you can only interact with them via systemcalls, like open, which return the file descriptor you can use
but you cannot directly edit the tables

**Crutial Processes**
You load in kernelspace into the process, but that stuff is shared among all processes, and has vastly different perms
than usual. Most programs cannot access kernelspace, but it is loaded into the process for speed of systemcalls. 

Virtual address space of Process A

+----------------------------+
|                            |
| User space                 |
|                            |
|  program code              |
|  heap                      |
|  stack                     |
|  shared libraries          |
|                            |
+----------------------------+
|                            |
| Kernel space               |
|                            |
| kernel code                |
| kernel data structures     |
| process tables             |
| file descriptor tables     |
| page tables                |
| device drivers             |
|                            |
+----------------------------+
You need kernel priviledges to access kernelspace ofc.


Thus, when you 

echo hello > out.txt

first open via syscall: fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC); #fd = 3 for now for example

then, duplicate it into 1: dup2(fd,1) #copies file description in 3 into entry 1

lastly, close the last one close(fd) (THIS IS FOR CHANING THE OUTPUT, THE INPUT IS DETERMINED FROM COMMAND LINE ARGS)




Before:

Process (PID 1234)

Kernel state:
    File descriptor table
        0 -> terminal
        1 -> out.txt
        2 -> terminal
    PID = 1234
    Current directory = /home/alice
    UID = 1000
    ...

User-space:
    bash code
    bash stack
    bash heap


After:
Process (PID 1234)

Kernel state:
    File descriptor table
        0 -> terminal
        1 -> out.txt
        2 -> terminal
    PID = 1234
    Current directory = /home/alice
    UID = 1000
    ...

User-space:
    echo code
    echo stack
    echo heap




For pipes, its usually different in that pipes created before the fork

exe:

ls | grep txt

the shell does:

pipe(pipefd);
fork();   // child 1
fork();   // child 2

pipe() is used when you want to have inter-communication between processes.
when you want to send stuff between processes, one method is using a pipe

A pipe is a communication channel made my the kernel, living in the kernel
One process puts bytes into it, and another reads bytes out of it.
When created, kernel allocates a pipe buffer where bytes can be stored in transit, and 2 new kernel file descriptions(aka file descriptor entries),
a read and a write object that lets you read and write to the buffer respectively

Current Process's File Descriptor Table:
Initial:

fd 0 -> stdout
fd 1 -> stdin
fd 2 -> stderr

Final:

fd 0 -> stdout
fd 1 -> stdin
fd 2 -> stderr
fd 3 -> pipe read #NOTE, doesnt have to be 3 and 4, its just whatever 2 numbers is returned in the array fd
fd 4 -? pipe write

usage:

int fd[2] # this creats a 2 element array [fd[0], fd[1]], used to store the file descriptors the kernel gives us
fd[0] is the read end, and fd[1] is the write end

pipe(fd) # here, you are passing the address of the first element of the array, and the kernel fills in the 
file descriptors for the read end and write end. NOTE: the kernel does not give you the read/write objects themselves,
just 2 numbers you can use to refer to them.

thus, after calling:

int fd[2]
pipe(fd)

how have 
fd 0 -> stdout
fd 1 -> stdin
fd 2 -> stderr
fd 3 -> pipe read #NOTE, doesnt have to be 3 and 4, its just whatever 2 numbers is returned in the array fd
fd 4 -? pipe write #NOTE, need 2 descriptors so kernel can enforce directions, only read from read and only write to write


Next, the forks, to create the two twin processes.
fork()
fork()

Then:

Child 1:

dup2(pipefd[1], STDOUT_FILENO); # now, its stdout is the write end of the pipe
execve("ls", ...);

Child 2:

dup2(pipefd[0], STDIN_FILENO); # and this stdin is the read end of the pipe
execve("grep", ...);

Thus, when you run them, child 2 wont run untill recieved input in stdin, and child 1 will run, and its output
will go to child 2's stdin, letting it run then and completing the pipe.






What about stuff like:

cat ~/.ssh/id_ed25119_linux_desktop | ssh bob@100.223.13.14 "cat >> ~/.ssh/authorized_keys"

The cats do interact here, but not directly.

flow: currently in some process, exe PID 1234


First, creates pipe. (doesnt open the file, that happens way later)

int pipefd[2]
pipe(pipefd) # creates kernel side pipe object with its buffer, and read and write objects with descirptors here

f1 0 stdin 
f2 1 stdout
fd 2 stderr
fd 3 pipe read
fd 4 pipe write

Next it forks the first child
fork()
and
dup2(pipefd[1],1)

Now
f1 0 pipe write 
f2 1 stdout
fd 2 stderr
fd 3 pipe read
fd 4 pipe write

and closes redundant fd:
close(pipefd[0]) #not needed so might as well close
close(pipefd[1])

f1 0 pipe write 
f2 1 stdout
fd 2 stderr


Next, it runs the command:

execve("/bin/cat", ["cat", "file"], ...) #which recall only overrides the userspace, the stuff we did up to now is safe

at init, cat now sees:
stdin(0) -> terminal
stdout(1) -> pipe write
stderr(2) -> screen often

However, it recieved argument "file"
so cat itself does
int fd = open(file)
and thereby makes it:

fd 0 -> keyboard
fd 1 -> pipe write
fd 2 -> stderr
fd 3 -> file

and then cat will read from file and write to stdout (the pipe)
**NOTE** no need to dup2 and close to change what stdin is here, as for input it just goes by command line args

