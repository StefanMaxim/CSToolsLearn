NOTE: init systems like systemd do NOT start processes
Process creation is done by the kernel in responce to systemcalls like execve() and fork()



instead, is it the first process to be started (called init) that manages the system:

1: Bootstraps the system, meaning it lauches essential services like networking and login on start
2: Manages services (service = backround, long running process hosted on server, by daemon, and handled by init system) can start, stop, and manage
3: Handles dependencies by ensuring that services start in the correct order,
like how webserver needs networking (dependencies as in needs them running to run something else)
4: Managing the running system: handles task scheduling, devices, and the like, but ess


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
Description=NGINX Web Server #Human-readable description of the service
After=network.target #This means dont start NGINX untill AFTER network.target has been initialized

[Service]
ExecStart=/usr/sbin/nginx #Command systemd should run to start the service, interally systemd asks kernel to create new process using fork() and execve()
Restart=always #if nginx exits unexpectedly, restart it automatically

[Install]
WantedBy=multi-user.target #Whenever system reaches the normal multi-user operating state, 
THIS SERVICE SHOULD ALSO BE STARTED. REACHING TARGET = SYSTEMD IS WORKING TO MATCH THE TARGET BY
STARTING REQUIRED SERVICES.

exe: mutli-user.target includes:
sshd.service
nginx.service
mysql.service


**TARGET** a target in systemd is a particular system state or milestone
most common is **multi-user.target**, which means "System should have booted enough that multiple users can log in and normal system services should be running"
Boot begins
      │
      ▼
Basic system initialized
      │
      ▼
Networking available
      │
      ▼
multi-user.target
      │
      ├── SSH starts
      ├── NGINX starts
      ├── MySQL starts
      └── Cron starts


**SERVICE FILES**
this information, about the
[Unit]
[Service]
[Install]
are part of the service files, like ssh.service

They are found in 2 primary locations:

/usr/lib/systemd/system/
OR
/lib/systemd/system/
and custom services live in
/etc/systemd/system/

can view with
systemctl cat nginx
OR
find file via systemctl status nginx
Loaded: loaded (/usr/lib/systemd/system/nginx.service; enabled)
and then cat that file or view it however

SSH Actual .service file:

[Unit] # describes service and dependencies
Description=OpenBSD Secure Shell server # human-readable description
Documentation=man:sshd(8) man:sshd_config(5) # points to documentation manual for sshd and sshd_config
After=network.target nss-user-lookup.target auditd.service #dont start untill after these are up
**KEY** this doesnt force them to start, just says wait untill they do. Use Requires=  or Wants= for that
ConditionPathExists=!/etc/ssh/sshd_not_to_be_run #a condition that the file must not exist,
(! means not), so if file exists, will not start ssh

[Service] #everything about the actual daemon
EnvironmentFile=-/etc/default/ssh # loads environment variables from here, - means ignore it if it doesnt exist. **NOTE** $SSHD_OPTS comes from here
ExecStartPre=/usr/sbin/sshd -t # run this before starting, -t means test the configuration for syntax errors
ExecStart=/usr/sbin/sshd -D $SSHD_OPTS # this is teh real command, -D means dont Daemonize. **NOTE** sshd normally forks itself into the backround, but systemd doesnt want that so it can supervise it
ExecReload=/usr/sbin/sshd -t # when someone runs systemctl reload ssh, first checks config
ExecReload=/bin/kill -HUP $MAINPID # next, it config valid, send SIGHUP to main sshd process, which means reload your config without disconnecting users, MAINPID filled in systemd

**NOTES ON KILL** CONTRARY TO NAME, KILL IS USED FOR SENDING SIGNALS TO PROCESSES.
/bin/kill [signal] PID
/bin/kill -SIGNAL PID
so /bin/kill -HUP PID, sends SIGHUP to PID 1234 (like how -lc is really libc)
-HUP originally meant that terminal connection was closed, but today can be interpreted as 
reload configs, reopne logs, refhresh state without exiting, depends on program.

KillMode=process # when stopping process, treat as proces (control-group default, process, mixed, none) process means only kill the main sshd process, not every child process.
Important because every SSH login is a child process, so stopping the daemon doesnt immediatley kill logged in users


Restart=on-failure #if crashes, restart it, but dont restart if intentionally stopped
RestartPreventExitStatus=255 # one exception: if exit with status 255, dont restart it. 255 means fatar startup errors like config problem

Type=notify # how systemd knows service is ready, types in clude
simple, forking, oneshot, notify, idle.
notify means the service sends a message to systemd saying "im ready", untill then, systemd
considers it "starting"




RuntimeDirectory=sshd # creates /run/sshd before starting, which disappears on reboot.Useful for runtime files like sockets or PID-related state


RuntimeDirectoryMode=0755 # perms for runtime dir, 
0 means owner, then read = 4 write = 2 ex = 1
7 = rwx (owner)
5 = rx (group)
5 = rx (others)

[Install]
WantedBy=multi-user.target #when system reaches that target, thats when you start the service.
How it works:
this practically means "symlink this service file such that multi-user.target wants it"

CREATES THIS SYMLINK ON ENABLE
/etc/systemd/system/multi-user.target.wants/ssh.service
    -> /lib/systemd/system/ssh.service



Alias=sshd.service # creates an alias for it, which may coincide with the actual name in some
distros. EXE: some call it ssh.server, but people expect sshd.service, so this makes both work:
systemctl enable ssh
systemctl enable sshd

gclocal@gclocal-MS-7E59:/usr/lib/systemd/system$ ls -ld ssh.service
-rw-r--r-- 1 root root 561 Mar 12 07:14 ssh.service

gclocal@gclocal-MS-7E59:/usr/lib/systemd/system$ ls -ld sshd.service 
lrwxrwxrwx 1 root root 11 Apr 27 17:15 sshd.service -> ssh.service

can add in ls -i to see inode to see if its teh same




And the socket:
[Unit]
Description=OpenBSD Secure Shell server socket #human readable
Before=sockets.target ssh.service #start this socket before sockets.target, and ssh.service.
Useful when using socket activation, since systemd want the listing socket ready before it starts the ssh daemon

ConditionPathExists=!/etc/ssh/sshd_not_to_be_run #again only runs if the file not present

[Socket]
ListenStream=0.0.0.0:22 ## listens for TCP connection on IPv4 port 22.
**NOTE** 0.0.0.0 means every IPv4 address on this machine, equivalent to
192.168.1.5:22
10.0.0.3:22
127.0.0.1:22
ALL AT ONCE

ListenStream=[::]:22 # listens on IPv6 port 22, where [::] is the IPv6 equivalent of 
0.0.0.0, meaning every IPv6 address



BindIPv6Only=ipv6-only #normally, on Linux, IPv6 sockets can sometimes accept IPv4 connections
IPv4 mapped Ipv6 addresses, but this option says don tdo that, keep the IPv6 socket strictly 
IPv6. Thus, you end up with 2 sockets


Accept=no ## **VERY IMPORTANT** 
There are 2 modes:
Accept=no
means one service handles all incoming connections:
systemd
     │
 listens on :22
     │
     ▼
starts ssh.service
     │
     ▼
sshd accepts every client
(One long-running daemon)

Accept=yes
means each incoming connection gets its own service instance

client 1
      ▼
sshd@1.service

client 2
      ▼
sshd@2.service

client 3
      ▼
sshd@3.service

OpenSSH uses Accept=no because it is designed to handle multiple connections simultaneously

FreeBind=yes #Normally, a program can only bind to an IP address that already exists on teh machine.
exe:
Machine has 192.168.1.10
trying to bind 192.168.1.20 would fail, FreeBind=yes tells systemd bind anyways, even if that 
address doesnt exist yet

This is useful if:

networking isn't fully configured yet,
the IP will appear later,
the address is managed by DHCP,
the address is a floating IP in a high-availability setup.


**ASIDE ON SOCKETS**
READ SOCKETS_LEARN.MD






[Install]
WantedBy=sockets.target #symlinks this files into teh want sections of that socket like before
RequiredBy=ssh.service # this is teh inverse of Requires=, when the service is enabled, systemd
knows that it requires the socket to be enabled as well.
This is mostly for systemctl enable/disable, not so that ssh.service starts because ssh.socket is active.





Socket Activation:
When teh socket is enabled:
Boot
 │
 ▼
systemd
 │
 ▼
ssh.socket starts
 │
 ▼
Port 22 is open
 │
 ▼
(No sshd process running yet)

Then, when a client connects, 
Laptop
   │
SSH to port 22
   │
   ▼
systemd receives the connection
   │
   ▼
Starts ssh.service
   │
   ▼
Passes the already-open listening socket to sshd
   │
   ▼
sshd begins handling SSH sessions

**NOTE** generally, ssh is enabled the traditional way via systemctl enable ssh, not
this other method.


And the target are 
























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
**KEY** modern kernels use copy-on-write, meaning the forked process shares the same underlying memory pages untill a 
write occurs, because its usually not worth copying over all of that process's memory bc its often followed by execve
which deletes it all anyways

exec() -> replaces the processes's memory (all of it) with the new layout as specified in the executable
**KEY** it does NOT replace the entire process, only that process's userspace! the kernel space of that process
remains, which is the one that contains stuff like the kernel file descriptor table!
HOWEVER: each process has its own file descriptor table, so its global in visibility, but local in scope

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
BIOS is legacy or UEFI for modern system (basically its software baked into your hardware)
it initializes CPU, Ram, basic hardware
and finds a bootable device (disk, USB, etc)

2. Loads a bootloader (GRUB)
BIOS loads the first-stage bootloader from the disk (MBR)
UEFI loads an EFI executable from the EFI System Partition

Bootloader loads:
/boot/vmlinuz (compressed linux kernel) into memory
initramfs (initial RAM filesystem)
passes kernel commands (root=, quiet, etc)
kernel command line (eg init=/usr/lib/systemd/systemd)
then bootloader jumps into kernel entry point (handing over control to the kernel)


1: Kernel Starts (no processes yet)

Cpu is in kernel mode
no user processes exist
no scheduler is initialized
memory manager is up

Now, kernel:
sets up memory management, scheduler, interrupts
detects hardware and loads built-in drivers

CRUTIALLY:
mounts initramfs as a temporary root (initramfs is loaded first by the bootloader, but the kernel runs it itself
as a temp filesystem)
prepares for first userspace execution
(NOT a process yet, just mounts it as a temporary place)
then, it executed /init from the initramfs

What is initramfs?
it is tiny, temporary userspace whose job is to prepare the REAL root filesystem
/init might be:
1. a shell script (Busy-Box-stye)
2. systemd itself running in initramfs mode

what it does:
loads necessary kernel modules (disk, RAID, LVM, crypto)
discovers hardware needed to acces root filesystem
decrypts disks (if using LUKS)
assemble RAID or LVM volumes (storage)
Mounts the REAL root filesystem (/dev/sda2) (MOUNTING = MAKE CONTENTS OF STORAGE DEVICE ACCESSIBLE VIA PATH, RELATIVE TO ROOT)
Aside on mounting:
wait, isnt /dev/sda a mount already?
NO: is it not a filesystem, but just raw access to a disk partition.

cat /dev/sda1 returns binary garbage, not filenames because there is no filesystem interpretation happening
mounting takes the raw disk data:
filesystem struction (ext4, xfs)
Metadata (inodes, directories, etc)
Raw data blocks
AND INTERPRETS THEM INTO A FILESYSTEM STRUCTURE

then, pivot to the "real" init process:



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
NOTE: systemd builds dependency graph of units and executes them in paralle when possible (read above)

sets up loggin (journald)
mounts essential filesystems (mount -t proc proc /proc, mount -t sysfs sys /sys, mount -t dev /dev)
starts cgroups heirarchy (user heriarchy for login and stuff)

3.2: sysyemd starts services:

NOTE: Systemd organized everything into UNITS:
.service -> daemons
.mount -> filesystemd
.socket -> sockets (for networking)
.target -> grouping (runlevels)
.timer -> scheduled tasks

CRUTIALLY, SYSTEMD BUILDS DEPENDENCY TREE AND STARTS PARALLEL EXECUTION OF CREATING / STARTING THESE UNITS

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



KEY: how to invoke system calls.