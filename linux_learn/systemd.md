NOTE: init systems like systemd no NOT start processes
that is done by the kernel vs exec() and fork()

instead, is it the first process to be started (called init) that manages the system:

1: Bootstraps the system, meaning it lauches essential sercives like networking and login on start
2: Manages services (service = thing hosted on server, by daemon) can start, stop, and manage
3: Handles dependencies like how webserver needs networking
4: handles task scheduling, devises, adn the like, but ess


exe:
say you install nginx on Linux and want to run a webserver with it

without systemd:
1: mmust manually start nginx every time you boot
2: make sure neetworking is ready first before running it
3: restart it yourself if it crashes
thats tedious 

instead:
sudo systemctl, which interacts with systemd to make it run by itself

for something like nginx:
FILE LIKE THIS:
[Unit]
Description=NGINX Web Server
After=network.target

[Service]
ExecStart=/usr/sbin/nginx
Restart=always

[Install]
WantedBy=multi-user.target


At boot:
systemd starts (first program after the kernel)
it sees should reach multi-user.target
that target included nginx, so nginx shoudl run

check dependencies, see After=network.target, systemd waits untill networking is ready

starts nginx look at ExecStart
keeps track of it (PID, status, logs)

if it crashes, Restart=always means it will restart 


NOTE: DO NOT INTEACT WITH SYSTEMD DIRECTLY, INSTEAD USE SYSTEMCTL

sudo systemctl enable --now nginx //enable means enable it ON BOOT, --now means start 
sudo systemctl disable nginx
sudo systemctl start nginx
sudo systemctl stop nginx
sudo systemctl restart nginx
sudo systemctl status nginx

so the absolute minimum is:

1: kernel (non-negotiable)
manages memory, CPU, devices
provides system calls (fork exec, etc)
is the ONLY THING that can actually create processes
(without it, nothing runs)

2: The first process (PID 1)
after booting, kernel does something crutial:
run this program as the first userspace process
(usually systemd, or on older systems SysVinit)
why is some first process necessary:
because after kernel initializes, it asks:
"okay im ready, now i need to run a userspace program"
then, it tries to execute a program as PID1, by default /sbin/init
or whatever you specify with init=...


WHY THIS SEPARATON:
Linux works on separation of Kernel and Userspace
Kernel used to manage mem, and create proceses, but does not descide what to do after initializing.

Userspace is everything else, normal programs, which descide what to run, when to run it, and how to react to failures

This is indended, bc if kernel descided what to do next, it would lead to it being hardcoded: start this, then do that
then that... which is bad because less customizability. 

The ONLY thing you knwo it will do is start whatever the init process is, init=... usually /sbin/init. 
that program because PID 1.

PID 1 is the very first process. All other processes either come from it, either directly or indirectly.
Thus, PID1 is responsible for

1: Building the System, desciding
- what programs should run
- in what order
- under what conditions
(exe: start networking start ssh, start a web server like nginx, all done via syscalls in the process of PID1)

2: it cleans up processes
when process dies, it becomes a zombie
its parents must "reap" it (collect its exit status)
if the parent dies, the process is adopted by PID1

thus, PID1 must clean up orphaned processes for the entire system

Note: you can do just kernel, and make PID1 be /bin/sh.
so the kernel is running, and the shell is PID1 (valid)
however, noting else happens unless you do it

cannot just make anything PID 1, like /bin/git or something
then, kernel runs process, it terminales, returns exit status, but that goes to kernel, so its done



wait, what is all that orphan-zombie stuff:

NOTE: every process, except the very first, has a parent:


PID 1 (init/systemd)
 ├── bash
 │    ├── python
 │    └── vim
 └── sshd

processes are created using:

fork() -> duplicate process

exec() -> replace it with a new program

so everything forms a tree rooted at PID1



When a process finishes, it doesn't disappear instantly.

Instead:
1. it stops running
2. The kernel keeps a tiny record of
    - exit status (success/failure)
    - some accounting info
leftover process is called a Zombie Process

Why do they exist:
because parent might want to know:
1. Did my child success
2. what exit code did it return

thus, kernel waits untill parent says:
okay, ive read the result, you can fully remove it

(done via wait() and waitpid())

Running -> Exits -> Zombie -> Reaped -> Gone

Zombie is not running, or using CPU, just an entry in teh process table.

**PROBLEM**
what if parent never calls wait()?
then the zombie just stays there


## Orphans

what if:
bash (parent)
 └── python (child still running)

 and bash exits? NOW python IS AN ORPHAN

 kernel now re-assigns the orphan to PID 1

thus: PID 1 must reap all orphaned children when they exit.


programs like bash dont expect random orphaned children
dont sit in a look calling wait
and often exit quickly

That means they fail to reap zombies or exit, which crashees the system

systemd or SysVinit:
constantly mobitor child processes
call wiat appropriately
clean up zombies
manage entire process groups


## Full example:

full chain from boot -> systemd -> python script walkthrough

0: Bootloader -> kernel load (before linux runs)

1. Firmware (BIOS/UEFI) runs
2. Loads a bootloader (GRUB)

Bootloader loads:
/boot/vmlinuz (compressed linux kernel)
initramfs (temporary userspace root fielsystem)
kernel command line (eg init=/usr/lib/systemd/systemd)
then bootloader jumps into kernel entry point


1: Kernel Starts (no processes yet)

Cpu is in kernel mode
no user processes exist
no scheduler is initialized
memory manager is up

Now, kernel:
mounts initramfs as a temporary root
prepares for first userspace execution

2: Kernel creates PID 1 (systemd)
calls:
argc = number of arguments
argv = string of arguments to the command, in this case systemd
envp = environment pointer, or an array of string containing environment variable (PATH, USER) of the OS
execve("/usr/lib/systemd/systemd",argc,envp)

so kernel -> execve(systemd) -> PID 1
(now, systemd running as PID 1)


3: Systemd boot sequence (high level)

3.1: systemd initialized itself:
sets up loggin (journald)
mounts filesystems (mount -t proc proc /proc, mount -t sysfs sys /sys)
starts cgroups heirarchy (user heriarchy for login and stuff)

3.2: sysyemd starts services

for each service, exe: sshd, networking, etc

Step A: fork

pid = fork(); //NOW, THERE ARE 2 PROCESSES, Parent: systemd PID1, child, service setup process

Step B: in child -> exec()
execve("/usr/sbin/sshd",argv,envp)
Now, child becomes sshd process
still managed by systemd

Step C: parent tracks it
systemd records:
PID of child, service metadata, and restart policy



4: Systemd event loop (simplified)

systemd continuosly does things like:

waitpid(-1,&status,WNOHANG);
pid -1 means to wait for any child process that has terminated
&status is a pointer to an integer where the child's exit status will be stored if child is reaped
WNOHANG means to return immediately if no child exited, instead of waiting and blocking the parent process

Return value: >0 means child exited, adn will return value of its PID
0 means WNOHANG was used, so child still running no exit
-1 means error occured

this is likely in a loop:
while (waitpid(-1, &status, WNOHANG) > 0) {
    // Child reaped, status holds exit info
}

this is how it reaps zombie processes, detects crashes, and restarts services if needed


5: You log in -> shell starts

when you open a terminal or login:

fork(); //from systemd PID1
execve("/bin/bash",argv,envp); //inside child

so chain:
systemd -> bash (your shell)


6: Run a python script:
python myscript.py

this triggers:

6.1 shell fork:
pid = fork();

systemd -> bash -> (new clone child)

6.2
replace child with python
exevce("/usr/bin/python",["python3","myscript.py"],envp);

now child is python interpreter

systemd -> bash -> python



7: Python runs your script:
bash -> python3 -> running scropt

(inside python, it may fork() for mutliprocessing or exec() other programs, os.exec)

exe:
os.system("ls") -> internal fork() execve("/bin/sh",["sh","-c","ls"],envp);


8: Python Potential Subprocesses

subprocess.run(["echo","hello"])

becomes
fork(); //forks from python
execve("/usr/bin/echo",["echo',"hello"],envp);


9: Process exit flow (CRUTIAL)
python exits
exit(status);
//return vs exit: return returns from teh function, exit terminates the whole program and passes its exit status up the 
chain

kernel now marks old python as zombie:
exited + zombie untill parent reaps it

AT THE SAME TIME:
Bash reaps it:
waitpid(child_pid,&status,0);

now its fully removed


**NOTE**
if it parent (bash) dies:
kernel re-parents it to PID 1:

then, systemd eventually
waitpid(child,&status, ...) and cleans it up



11: FULL CHAIN

BIOS/UEFI
Bootloader (GRUB)
Kernel (/boot/vmlinuz)
execve("/sbin/init"); -> PID 1
systemd forks/exec services
login shell (bash)
bash fork/exec
python3 myscript
fork/exec subprocesses

