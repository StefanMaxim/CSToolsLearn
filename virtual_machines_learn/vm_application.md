# One possible use for VMS:

create a HAAS platform that will allow people to use some server, create a VM on said server, and use it to play games
via the cloud. 

Main mechanism: Sunshine and Moonlight
Sunshine is the cloud-based streaming service, that will allow you to emulate the gaming experience.
Sunshine needs a display, input injection, hardware encoders, and interact closely with GPU drivers, so needs
its own VM (not just container, which is beter for lone processes) Because of the close interaction with kernel-level
drivers, containers cannot satisfy this without leaky implementaiton that is unsafe


Hypervisor: software that facilitates the creation of VMs (virtual machines), or machines that think they are their 
own entity with hardware and full software stack from kernel and up, but in-reality are existant inside of another device.



## Workflow for Idea:

First, use some lightweight hypervisor like KVM or ProxMox to handle the creation and maintenance of the virtual machines
Then, find some way to handle the GPU interactions, be that full passthrough of a GPU or vGPU if your hardware supports it
Lastly, inside of the VM run Sunshine to handle the cloud streaming software and Moonlight to serve as the client streamer.

User → Web App → Your Backend API → Proxmox → VM → Sunshine → Moonlight
ProxMox already gives:
VM creation / deletion
templates (very important)
snapshots
networking
API access (!!) THIS IS CRUTIAL, AS YOU CAN JUST ENVOKE IT VIA API


MISSING PIECE: need some control panel ontop of ProxMox that manages the more "appication" side of the project

WHAT DOES PROXMOX PROVIDE:
running VMs
VM lifecycle primitives like create/clone/start/stop/destroy
attaching virtual disks to VMs
storage backends and pools
templates and cloning
cluster/node management
an API you can call from your own backend

you do not need to reimplement virtualization or raw storage management. What you need to build is the policy/business layer on top of what Proxmox already exposes:

map users to VMIDs
decide which template to clone 
decide which storage pool to use
decide how much disk a plan gets
decide whether storage is persistent or temporary
enforce quotas
track billing state
clean up VMs/disks when a lease ends
expose a safe user-facing API instead of giving users Proxmox access

### VM Lifecycle
creates VMs from templates
starts, stops, and destroys
Allocates GPUs
Attaches Storage
Tracks which users own which VMs



### ProxMox Control:
ProxMox exposes REST API:
can use pthon's fastAPI or node.js Express


#Example calls
POST /nodes/{node}/qemu → create VM
POST /nodes/{node}/qemu/{vmid}/status/start
POST /nodes/{node}/qemu/{vmid}/status/stop