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

1. /usr/lib/systemd/system/
OR
2. /lib/systemd/system/
and custom services live in
3. /etc/systemd/system/

can view with
**systemctl** cat nginx
OR
find file via systemctl status nginx
Loaded: loaded (/usr/lib/systemd/system/nginx.service; enabled)
and then cat that file or view it however

**NOTE** Due to Usr Merge, ITS THE SAME PLACE!!! there are now 2 places
/usr/lib/systemd and /etc/systemd, depending on internal vs external software.




## SERVICE FILE BREAKDOWN
SSH Actual .service file:

[Unit] # describes service and dependencies
Description=OpenBSD Secure Shell server # Human-readable description of the service

Documentation=man:sshd(8) man:sshd_config(5) # points to documentation manual for sshd and sshd_config

After=network.target nss-user-lookup.target auditd.service #dont start untill after these are up
**KEY** this doesnt force them to start, just says wait untill they do. Use Requires=  or Wants= for that

ConditionPathExists=!/etc/ssh/sshd_not_to_be_run #a condition that the file must not exist,
(! means not), so if file exists, will not start ssh

[Service] #everything about the actual daemon
EnvironmentFile=-/etc/default/ssh # loads environment variables from here, - means ignore it if it doesnt exist. **NOTE** $SSHD_OPTS comes from here

ExecStartPre=/usr/sbin/sshd -t # run this before starting, -t means test the configuration for syntax errors

ExecStart=/usr/sbin/sshd -D $SSHD_OPTS # this is teh real command, -D means dont Daemonize. **NOTE** sshd normally forks itself into the backround, but systemd doesnt want that so it can supervise it. Otherwise, it will start up, fork, parent process exits, and child process detaches from the terminal and continues running in the backround. (thats traditional Unix
Daemon behavior) (read linux_intro.md for more on terminals and forking away and stuff)

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

Type=notify # how systemd knows service is ready, types include
simple, forking, oneshot, notify, idle.
notify means the service sends a message to systemd saying "im ready", untill then, systemd
considers it "starting"




RuntimeDirectory=sshd # creates /run/sshd before starting, which disappears on reboot. Useful for runtime files like sockets or PID-related state


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
**NOTE** when you write systemctl status ssh or cat ssh, you see that it says
triggered by ssh.socket, meaning it is socket activated. THus, it is not in the multi-user.target.wants or sockets.target.wants like you would expect.

systemctl is-enabled ssh

ANSWER: they are in /etc/systemd/system! reason the socket and service are both enabled is that
this way it can claim port 22 and give it to the service later for ease.


Alias=sshd.service # creates an alias for it, which may coincide with the actual name in some
distros. EXE: some call it ssh.server, but people expect sshd.service, so this makes both work:
systemctl enable ssh
systemctl enable sshd

gclocal@gclocal-MS-7E59:/usr/lib/systemd/system$ ls -ld ssh.service
-rw-r--r-- 1 root root 561 Mar 12 07:14 ssh.service

gclocal@gclocal-MS-7E59:/usr/lib/systemd/system$ ls -ld sshd.service 
lrwxrwxrwx 1 root root 11 Apr 27 17:15 sshd.service -> ssh.service

can add in ls -i to see inode to see if its teh same



## SOCKET FILE BREAKDOWN
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
this other method. Nonetheless, it keeps the socket up so that it can pass the existing listening socket to the service later.


## TARGET FILE BREAKDOWN
And the target are:

multi-user.target

      # SPDX-License-Identifier: LGPL-2.1-or-later
      #
      #  This file is part of systemd.
      #
      #  systemd is free software; you can redistribute it and/or modify it
      #  under the terms of the GNU Lesser General Public License as published by
      #  the Free Software Foundation; either version 2.1 of the License, or
      #  (at your option) any later version.

[Unit]
Description=Multi-User System  #user-generated description of the file
Documentation=man:systemd.special(7) # location for the documentation
Requires=basic.target #
Conflicts=rescue.service rescue.target
After=basic.target rescue.service rescue.target
AllowIsolate=yes



## INTERPLAY:

When A Linux Machine boots, it roughly goes :

firmware
  -> bootloader
      -> Linux kernel
          -> optional initramfs/initrd userspace (more on that later)
              -> real root filesystem
                  -> /sbin/init

where /sbin/init is usually a symlink to systemd.
since systemd is first userspace process, it runs as PID 1, making it the system init process, whose job is to bring up
and maintain userspace services.


Very early, systemd does some built-in setup like:
set up basic API filesystems like /proc, /sys, /dev
configure basic things like hostname/loopback
install signal handlers
initialize logging connections, etc...

But then it descides, "What is the initial unit I should try to activate?"

By default, that is default.target, but it can be overridden via a kernel command line option like
systemd.unit=rescue.target

**KEY** thus, boot is start PID 1, then figure out how to activate default.target, not how to read all service files.

Here is sample of default.target:

      #  SPDX-License-Identifier: LGPL-2.1-or-later
      #
      #  This file is part of systemd.
      #
      #  systemd is free software; you can redistribute it and/or modify it
      #  under the terms of the GNU Lesser General Public License as published by
      #  the Free Software Foundation; either version 2.1 of the License, or
      #  (at your option) any later version.

[Unit]
Description=Graphical Interface
Documentation=man:systemd.special(7)
Requires=multi-user.target #HUH? REASON IS THAT DEFAULT.TARGET IS THE DESTINATION, NOT THE STARTING POINT!
Wants=display-manager.service
Conflicts=rescue.service rescue.target
After=multi-user.target rescue.service rescue.target display-manager.service
AllowIsolate=yes

default.target
        |
        v
multi-user.target
   /      |      \
network  sshd   getty


### Aside on Units:
A Unit is a systemd internal object for something it can manage
exe:
sshd.service          a service process
multi-user.target     a grouping/synchronization point
sockets.target        a grouping of sockets
dbus.socket           a listening socket
home.mount            a mounted filesystem
dev-sda.device        a kernel device object

a **UNIT FILE** is an INI-style that describes those unit objects, .service, .socket, .target are such files
**NOTE** a Unit file is not the unit itself, but rather the config used to construct the in-memory unit object

Systemd units can come from many different places:
/usr/lib/systemd/system/     package-provided units
/etc/systemd/system/         admin overrides/custom units
/run/systemd/system/         runtime units
/run/systemd/generator*/     generated units
transient D-Bus-created units
kernel/device state

this is called the **Load Path** essentially where systemd finds its units, with files in earlier
path locations overriding ones in later paths


WHen systemd want ssh.service, it conceptually runs:
load_unit("sshd.service"):
    find main unit file in load path
    read [Unit], [Service], [Install], etc.
    read drop-ins, such as sshd.service.d/*.conf
    read dependency symlink directories:
        sshd.service.wants/
        sshd.service.requires/
    apply implicit dependencies
    apply default dependencies
    create/update in-memory Unit object

Result: in memory object with properties
Name=sshd.service
Type=service
LoadState=loaded
ActiveState=inactive
SubState=dead
Wants=...
Requires=...
Before=...
After=...
ExecStart=...
Restart=...

systemctl list-units only lists ones referenced directly or indirectly in teh dependency graph, not all on disk

### Unit States

Every unit has a current state, exe:

inactive
activating
active
deactivating
failed

The High-Level state is called **ACTIVE** and the more precise type-specific state is called **SUB**
exe:
ACTIVE=active
SUB=running

**KEY**
Thus, a unit object has a **current reality**:
"sshd.service is currently active/running"
But when you ask systemd to do something, you create a **requested transition**
exe:
please start sshd.service
please stop sshd.service
please restart sshd.service
That requested transition is a **job**


### Jobs
A job is a systemd internal record that says "move this unit towards a desired state"
exe:
start job for sshd.service
stop job for nginx.service
restart job for postgresql.service
reload job for systemd-journald.service

Think of a job like:
struct Job {
    Unit *unit;          // pointer to the unit it modifies
    JobType type;        // START, STOP, RESTART, RELOAD, ...
    JobState state;      // waiting, running, done, failed, canceled, ...
    JobResult result;    // success, dependency, timeout, skipped, ...
}

**KEY** A unit can have a job attached to it, called the unit's **job property**
If a unit contains a currently scheduled or executing job, the property contains it jobID and job object path, 
else the job ID is 0.

systemctl show ssh.service -p Job

systemctl show ssh.service -p Job --value
then 
systemctl list-jobs

Or over D-Bus
busctl introspect org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/job/123

**NOTE** Systemctl show gives only the JobId, but D-Bus via busctl you will see the full object path (LOOK INTO LATER)

Thus, when you run systemctl start sshd.service, systemctl does not itself start sshd, but
rather it talks to systemd PID 1 and asks it to start the unit via the D-Bus API, which 
defines a method StartUnit(), as enqueing a start job and possibly depending jobs. 
It returns a job object, and clients must watch that job if they want the final result.

systemctl start sshd.service
    -> send StartUnit("sshd.service", "replace") to PID 1
        -> PID 1 creates Job(type=start, unit=sshd.service)

KEY IDEA: when you start a service, it does not do it right away, instead it just 
creates the job object for what you want to do.


#### Aside on D-Bus
D-Bus means desktop bus, and its an inter-process communicaiton system (IPC) allowing 
programs and services to communicate.

Instead of apps communicating through custom sockets or files, dbus provides standard message bus.
exe:
network manager tells apps that network connection changed
power manager informs programs that battery is low
service exposes methods that other programs can call

**KEY** this way, clients dont need to know a PID, and can simply send messaged to a well
known D-Bus name like org.example.MyService

SYSTEM:
1. Services publish interfaces
2. Clients call methods or recieve signals
3. The bus daemon routes the messages

Relation to systemd:
whereas we, the user, interact with systemd via systemctl, it also exposes its functionality
over d-bus so processes can more easily communicate with it.

EXE:
when you systemctl start nginx
systemctl doesnt manipulate services directly!
instead, it sends a D-Bus request to systemd daemon (PID 1), asking it to start the service:
systemctl
      │
      ▼
    D-Bus
      │
      ▼
systemd (PID 1)
      │
      ▼
Starts nginx.service

D-Bus Activation:
instead of running all services all the time:
1. service registers a D-Bus name
2. The service is not running
3. A client sends a message to that name
4. D-Bus (often integrated with systemd) automatically starts that service
5. message is delivered

THis saves boot time.

EXE2:
suppose application want to reboot computer.
Instead of running reboot, send D-Bus message like:

Method:
org.freedesktop.login1.Manager.Reboot()

D-Bus activated Services:
[Service]
Type=dbus #tells systemd to wait untill the service acquires the specific D-Bus name
**NOTE** thus, it isnt just waiting for process to start, but for it to reguster a unique
name of the D-Bus before it can consider the service started. 



BusName=org.example.MyService #registers the D-Bus name the service must own
ExecStart=/usr/bin/myservice # Once that name appears, systemd considers teh service started 

**NOTE 2**
Notice, the D-Bus names look like URLs!
Thats because they reverse DNS names, made this way to guarentee global uniqueness!
EXE:
imagine if every dev could name their service "Settings" on the D-Bus, that would be a pain!
Instead, everybody uses a name based on a domain they own
freedesktop.org is org.freedesktop.*, etc (hense the reverse part)
(same convection for java package names com.google.common, org.apache.http)
That being said, every D-Bus connection gets a unique name automatically
exe:
:1.4
:1.27, assigned by D-Bus daemon

A process can then request a well-known name RequestName("")
That being said, it isnt enforced by some online DNS, just the local D-Bus daemon.
Processes use teh well known name, not the random assigned one, which is just for D-Bus internals.

##### D-Bus Commands
Structure is:
Bus name: who you are talking to
Object path: which object inside that service
Interface: what API that object exposes

Service (bus name)
    ↓
Object
    ↓
Interface
    ↓
Method


1. List bus names for all processes:
busctl list 

2. 
// Tells you what objects does this service expose
busctl tree org.freedesktop.systemd1

returns somethign like

/
└─/org
   └─/org/freedesktop
      └─/org/freedesktop/systemd1
         ├─unit
         ├─job
         └─manager
DBus lets processes expose mant objects each with their own path

**KEY**
This may LOOK like a directory tree, but thats just semantics.
Everything there is just objects, but organized this way to make things easier.
kind of like how you can have urls:
https://example.com/api
https://example.com/api/users
https://example.com/api/posts
but reperesent it as:
/
└── api
    ├── users
    └── posts

OBJECT PATHS ARE JUST IDENTIFIERS, MADE THIS WAY FOR ORGANIZATIONAL PURPOSES
THE /, /org, /org/freedesktop/, IS LIKELY JUST THERE FOR ORGANIZATIONAL PURPOSES,
AND DO NOT CORRESPOND TO REAL OBJECTS.
CAN CHECK VIA busctl introspect org.freedesktop.systemd1 /org

gclocal@gclocal-MS-7E59:~$ busctl introspect org.freedesktop.systemd1 /org
NAME                                TYPE      SIGNATURE RESULT/VALUE FLAGS
org.freedesktop.DBus.Introspectable interface -         -            -    
.Introspect                         method    -         s            -    
org.freedesktop.DBus.Peer           interface -         -            -    
.GetMachineId                       method    -         s            -    
.Ping                               method    -         -            -    
org.freedesktop.DBus.Properties     interface -         -            -    
.Get                                method    ss        v            -    
.GetAll                             method    s         a{sv}        -    
.Set                                method    ssv       -            -    
.PropertiesChanged                  signal    sa{sv}as  -            -    

This shows that /org is indeed an object, and has methods like .Introspect, .Ping, and
.Get and .Set, which are general D-Bus interfaces, not systemd specific.
Systemd just chose to register objects at those paths, even though they dont expose
any systemd functionality.

Kind of like with a web server:
for /api/users, can choose to make pages for /, /api, but not necessary

Why do this?
D-Bus specs define an interface called org.freedesktop.DBus.Introspectable,
an object implementing it returns an XML describing itself and its immediate children
by putting an introspectable object at /org, can recurse through it




3. busctl introspect

busctl introspect org.freedesktop.systemd1 /org/freedesktop/systemd1

This means "tell me everyting about this object"
could respond with somethgin like:

INTERFACE
org.freedesktop.systemd1.Manager

METHODS:
StartUnit()
StopUnit()
RestartUnit()
Reload()
ListUnits()

PROPERTIES:
SIGNALS:

**NOTE**
Notice the types of methods you can have:
NAME                                TYPE      SIGNATURE RESULT/VALUE FLAGS
org.freedesktop.DBus.Introspectable interface -         -            -    
.Introspect                         method    -         s            -    
org.freedesktop.DBus.Peer           interface -         -            -    
.GetMachineId                       method    -         s            -    
.Ping                               method    -         -            -    
org.freedesktop.DBus.Properties     interface -         -            -    
.Get                                method    ss        v            -    
.GetAll                             method    s         a{sv}        -    
.Set                                method    ssv       -            -    
.PropertiesChanged                  signal    sa{sv}as  -            -  

This is the class definition of an object.
Says:
1. What interfaces object implements
2. What methods you can call
3. What signals it emits, and 
4. what properties it exposes

Interfaces: like C++ class, such that everything that follows untill the next
interface belongs to it. ALL METHODS ARE PART OF INTERFACES. Method calls are allways
Service, Object, Interface, Method



exe: interface org.freedesktop.DBus.Peer has methods .GetMachineId, and .Ping.
leading . means it belonds to the interface.

Can have INTERFACE, METHOD, PROPERTY, SIGNAL

Signature is method signature, ss meaning 2 strings
Return type is return type
Flags is stuff like read only




4. busctl call
busctl call \
org.freedesktop.systemd1 \
/org/freedesktop/systemd1 \
org.freedesktop.systemd1.Manager \
ListUnits

means:
"Call the ListUnits method on the org.freedesktop.systemd1.Manager interface of the object at
/org/freedesktop/systemd1 owned by the service org.freedesktop.systemd1

Argument	                            Meaning
org.freedesktop.systemd1	            Which service (bus name)
/org/freedesktop/systemd1	            Which object
org.freedesktop.systemd1.Manager	    Which interface
ListUnits	                            Which method

Analogous to:

HTTP	            D-Bus
Server	            Bus name
URL path	        Object path
API namespace	    Interface
Endpoint/function	Method


Inspect systemd service:
busctl tree org.freedesktop.systemd1

View available methods:
busctl introspect org.freedesktop.systemd1 \
/org/freedesktop/systemd1

Call a method:
busctl call org.freedesktop.systemd1 \
/org/freedesktop/systemd1 \
org.freedesktop.systemd1.Manager \
ListUnits



### Transactions











Before, After, Requires, RequiredBy, Wants, WantedBy, etc...

**KEY MODEL** Units, Jobs, and Transactions

Systemd does not boot by reading files top-down like a shell, instead, it uses a graph of units

a **UNIT** can be a service, socket, target, mount, timer, device, and so on
a **UNIT FILE** is an INI-style that describes those unit objects, .service, .socket, .target are such files








































## Boot Process

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