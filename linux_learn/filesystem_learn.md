# Filesystem Organization

Linux follows the Filesystem Heirarchy Standard, which is built on 3 axes:



1: What is that file's role:

- Programs (binary executables)
- Configurations (how programs behave)
- Data (what programs run on)
- State (runtime-changing information like logs)

2: is it static or variable:
- Static = doesnt change unless you reinstall/update
- Variable = changes during runtime

3: who owns it
- System(OS) (installed by package manager)
- Admin(you) (installed manually)
- User (personal files)


## Master Map:

System Binaries = /bin or /usr/bin
Local/Admin Binaries = /usr/local or /opt
Config = /etc
Variable data = /var
User Data = /home
Runtime/System Internals = /proc or /sys or /run

## Layer 1: Root
root, or /, is both a directory and the namespace of the entire OS
(can access all files, processes, hardware, and even memory)

## Layer 2: Programs
/bin and /sbin hold the minimal, and essential system for the OS to boot and recover
(NO DEPENDENCIES ON OPTIONAL COMPONENTS)


/bin:
Contains binary commands for use by all users (on most Linux systems, this directory is a shortcut to /usr/bin)

/sbin:
Contains the Linux kernel and files used by the boot loader


exe:
/bin/bash
/bin/ls
/sbin/init

### /usr
/usr is the main system software tree
**NOTE** sometimes, sbin and bin are shortcuts to /usr/bin and /usr/sbin, like in Arch or RedHat linux
but usually they are different, separated by utility to boot

contains all non-essential, but standard OS software
managed by package managers (intended to be read-only in normal runtimes)

/usr/bin holds programs intended for normal users
exe:
/usr/bin/ls
/usr/bin/git
/usr/bin/python3
/usr/bin/vim



/usr/sbin holds system services / binary commands aka commands used for system administration
(often require sudo)
/usr/sbin/useradd
/usr/sbin/nginx
/usr/sbin/fsck
(less everyday tools like git and python and more system-wide things like network setup and user management)

/usr/lib stores libraries, which are .so files or things that you can add to executables without having to re-type code
(the .a and .so files from maven_learn)
library = pre-written code that other programs can use

2 main types:

shared libraries: .so
used at runtime, and dynamically loaded in

exe:
/usr/lib/x86_64-linux-gnu/libc.so.6 (standard c library, used by almost every program)

static libraries, or .a
used in compile time, code copied into executable

exe: use ldd to see which shared libraries a command is using

ldd /usr/bin/ls

otool -L /bin/ls (since again bin is often the same as /usr/bin nowadays)
(will give list of dylib or .so files its attached with)



/usr/share architecture independent data
means not tied to architecture type (x86_64 or ARM)
just data, not compiled
things programs use, not execute:
/usr/share/man        # manuals
/usr/share/icons      # icons
/usr/share/fonts      # fonts
/usr/share/doc        # documentation
/usr/share/locale     # translations


exe:
/usr/bin/git
/usr/bin/python3 (not needed for boot, but useful)
/usr/share/X11 (the windows server, at least part of it)


### /usr/local
This is executables installed by the admin, not the package manager or the OS on install
Structure mirrors /usr:
/usr/local/bin
/usr/local/lib
... (just where you put your own executables)


### /opt
Meant for isolated, third party software
(generally, self-contained)
not integrated into system heirarchy

/opt/maven
/opt/intellij


## Layer 3: Configuration (Control Plane)

/etc
contains only configs, ie files telling programs how to behave:

/etc/ssh/sshd_config
/etc/nginx/nginx.conf
/etc/environment

**NOTE** config here is global, user-dependent config goes elsewhere (usually a hidden file .config in userspace)


## Layer 4: Data and State
/var
meant for data that changes during regular operations:

logs:
/var/log/syslog
/var/log/nginx/access.log

or persistent app data:
/var/lib/mysql
/var/lib/docker

or even the cache:
/var/cache/apt


### Temporary Space
/temp
used for storing temporary, and non-persistent info
may be wiped at any time

### Runtime States
/run stores runtime only states
exe: PIDs, Sockets, Locks, etc

## Layer 5: User Domain

/home

lets each user get an isolated space:
/home/alice
/home/bob

INSIDE HOME DIRECTORY:
configs:
/home/alice/.config/
/home/alice/.bashrc 
(~/ is the same as home/<USER>/)

Or data:
~/projects
~/Downloads
~/Documents

## Layer 6: Kernel Interface (Confusing)
Here, they are not files per se, so much as interfaces to the kernel
(BEING AN OS, EVERYTHING IS A FILE, BUT THESE FILES MEAN SOMETHING MORE)

/proc
Virtual filesystem exposing kernel states:
/proc/cpuinfo
/proc/meminfo
/proc/<pid>/

/sys
Device and kernel control
(structured interface for hardware tuning)

/dev
Holds devices as files

(LINUX TREATES EVERYTHING AS A FILE INTERFACE)
/dev/sda     # disk
/dev/null    # sink
/dev/tty     # terminal


## Layer 7: Mount Points
/mnt
used for temporary mounts

/media
used for auto-mounted devices


## All together:

Static: /usr, /bin, /lib
Variable: /var /tmp /run

System: /usr /bin
Admin: /usr/local /opt
User: /home

Code: /usr /opt /usr/local
Config: /etc
Data: /var /home
Runtime: /run /proc


