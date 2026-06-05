# Basics of QEMU

## Installing qemu

brew install qemu

qemu-system-x86_64 --version //obtain the version of it
(usually will get some .tar.gz that expands to what you need, or in general 
it will work as a package)

(this functions kinda like apt + dpkg together. Whereas dpkg is for creating and installing packages, and apt is for managing them + handling dependencies, brew does all of that)

## Create a folder for the QEMU VMs
mkdir -p ~/vms/debian-x86

now, download a Debian/Ubuntu Iso and move it to that folder
mv ~/Downloads/debian-*-amd64-netinst.iso ~/vms/debian-x86/debian-amd64-netinst.iso

## Create the VM:
qemu-img create -f qcow2 debian-x86.qcow2 40G

qemu-img        QEMU's disk-image tool
create          make a new virtual disk
-f qcow2        use QEMU's normal flexible disk format
debian-x86.qcow2 the disk file name
40G             maximum size: 40 GB

now, the folder should have:

debian-amd64-netinst.iso
debian-x86.qcow2

## Now, boot the debian installer:

qemu-system-x86_64 \
  -machine q35 \
  -m 4096 \
  -smp 4 \
  -cpu max \
  -drive file=debian-x86.qcow2,format=qcow2 \
  -cdrom ubuntu-26.04-live-server-amd64.iso \
  -boot d \
  -netdev user,id=net0,hostfwd=tcp::2222-:22 \
  -device e1000,netdev=net0 \
  -display default

Instructions:
qemu-system-x86_64     emulate an x86_64 PC
-machine q35           use a modern PC machine type
-m 4096                give the VM 4096 MB RAM
-smp 4                 give the VM 4 virtual CPU cores
-cpu max               expose a capable emulated x86 CPU
-drive ...             attach your virtual hard drive
-cdrom ...             attach the Debian installer ISO
-boot d                boot from the CD-ROM/ISO
-netdev ...            create user-mode networking
hostfwd=tcp::2222-:22  forward Mac port 2222 to VM port 22 for SSH
-device e1000          emulate an Intel network card
-display default       show a QEMU window


This will start a new terminal.
(note: do not do unless really need GUI, just to without it)


sudo poweroff  to leave

Notes on creation:
for choosing your networking interface, use enp0s2, which is QEMU's virtual network card.

leave proxy address empty

mirror address:
Protocol: http
Mirror country: United States
Mirror: deb.debian.org
Proxy: blank

(for ubuntu, use http://archive.ubuntu.com/ubuntu) (this is where apt fetches from)

Use entire disk bc VM

Then, punch in credentials

gcportable






## How to re-boot into your VM:

cd into your folder:
cd ~/vms/debian-x86

qemu-system-x86_64 \
  -machine q35 \
  -m 4096 \
  -smp 4 \
  -cpu max \
  -drive file=debian-x86.qcow2,format=qcow2 \
  -netdev user,id=net0,hostfwd=tcp::2222-:22 \
  -device e1000,netdev=net0 \
  -display default

KEY: normally, what to connect via SSH into it so you wont interact with it at all beyond the SSH terminal.



## Set up ssh
Seee ssh_learn for more info

## Create Startup Script:

start.sh

cd into the folder:
cd ~/vms/debian-x86
vim start.sh

Paste this into it:
#!/bin/bash

cd "$(dirname "$0")" //KEY: this means move into the directory where this script lives, so you can run it without being CDed into it

qemu-system-x86_64 \
  -machine q35 \
  -m 4096 \
  -smp 4 \
  -cpu max \
  -drive file=debian-x86.qcow2,format=qcow2 \
  -netdev user,id=net0,hostfwd=tcp::2222-:22 \
  -device e1000,netdev=net0 \
  -display default


KEY: make the script executable:
chmod +x start.sh

Start VM with:
~/vms/debian-x86/start.sh

## Create A headless start script:
cd ~/vms/debian-x86
vim start-headless.sh

Paste:
#!/bin/bash

cd "$(dirname "$0")"

qemu-system-x86_64 \
  -machine q35 \
  -m 4096 \
  -smp 4 \
  -cpu max \
  -drive file=debian-x86.qcow2,format=qcow2 \
  -netdev user,id=net0,hostfwd=tcp::2222-:22 \
  -device e1000,netdev=net0 \
  -display none \
  -daemonize

make it runnable:
chmod +x start-headless.sh

NOTE: do NOT kill QEMU, use sudo shutdown instead





FOR UTM:
emulated VLAN

go to port forwarding, new

Protocol: TCP
Guest Address: leave blank
Guest Port: 22
Host Address: leave blank or 127.0.0.1
Host Port: 2222

this means:
Mac localhost:2222 → Ubuntu VM port 22


KEY:
get the executable open so you can quickly make vms in terminal

sudo ln -sf /Applications/UTM.app/Contents/MacOS/utmctl /usr/local/bin/utmctl //creates symlink to the utmctl command line tool
(must be run from terminal, not some ssh terminal or on vscode)




