# Linux Fundamentals

## Multiplexers:
Tmux is an example of a terminal multiplexer:  
lets you:  
- keep long-running files alive even when you close laptop
- split one terminal into multiple panes
- organize work into sessions/windows

> **CRUTIAL**:
> WHY IS THIS IMPORTANT?
> normally, when sshing, the chain is: your terminal app -> ssh connection -> remote shell -> your programs
> thus, when your ssh connection dies or drops, the remote shell gets a hangup and dies, and so do all programs that are currently running on it
> TMUX changes the chain:
> NOW: terminal app - ssh connection -> tmux client -> tmux server (on remote machine) -> your shells / programs
> this way, when your ssh disconnects, only loose the tmux client/viewer. the server keeps running on the remote and keeps your shell and programs
>attached to its virtual terminals
EXE:
- ssh into server `ssh user@ipaddr`
- start tmux via `tmux new -s work`
- start something long-running `python train.py`
- detach via `C-b d`, then you can even exit ssh `exit`
- Later can `ssh user@ipaddr` and `tmux attach -t work` and see your program still running


It contains 3 parts:

### Session
A workplace that persists (exe a session called server where you run ssh, logs, editors, etc)

### Window
Like a tab inside a sessions (let window 0 be editor, window 1 be logs, window 2 be shell, etc )

### Pane
a split region inside a window, (ie left pane runs code, right pane watches htop)

> **CRUTIAL**
> all commands here are based on a prefix, then a key, (prefix usually is command/control + b, followed by a key)
> we will denote command + b as just C-b for short, so C-b % is actually command b then %


### Commands:

- tmux new -s work //starts a new named session called work
- C-b % // vertically splits the panes (use exit to kill it)
- C-b " //horizontally splits the panes

- C-b (arrow keys) //to move across the panes
- C-b c //creates a new window
- C-b w //lists windows
- C-b n / p //used to switch between windows

- C-b d //detached the tmux instance, but leaves it running
- C-d //kills the pane same as exit

- tmux ls // lists all currently running tmux sessions
- tmux attach -t work //reattaches the session to tmux

- tmux kill-session -t NAME //kills the session




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

## Network

## Installation

## Shortcuts

## Generic

- man (cmd) // checks the manual for how to use a command




