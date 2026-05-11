Server

Proxmox VE as the hypervisor.
Create one Windows or Linux gaming VM per isolated session.
Use PCIe GPU passthrough: one physical GPU assigned to one VM for best reliability/performance. Proxmox officially supports PCI passthrough for giving a VM direct access to a GPU.
Inside each VM, install:
GPU drivers
Steam/Epic/etc.
Sunshine streaming host

Laptop

Install Moonlight client.
Connect to the VM over its Tailscale IP.
Stream video/audio from server → laptop, send keyboard/mouse/controller input back.

Sunshine + Moonlight is probably the best open-source stack here: Sunshine is a self-hosted low-latency game-stream host with AMD, Intel, and Nvidia hardware encoding support, and Moonlight is the client ecosystem for it.

For access over your tailnet, avoid SSH for the actual stream. SSH tunneling can work, but it adds friction and can hurt latency. Better:

Laptop Moonlight
   ↓ over Tailscale/WireGuard tailnet
Gaming VM running Sunshine
   ↓
Passed-through GPU

Use SSH only for management: starting VMs, debugging, updating, etc.

For scaling:

Physical server
├─ Proxmox
│  ├─ VM-1 + GPU-1 + Sunshine
│  ├─ VM-2 + GPU-2 + Sunshine
│  ├─ VM-3 + GPU-3 + Sunshine
│  └─ VM-4 + GPU-4 + Sunshine

The cleanest rule is one user/session = one VM = one GPU. GPU sharing/vGPU is possible, but it gets messy: Nvidia vGPU exists on Proxmox, but licensing and hardware support become a major constraint.

My recommendation:

Start with Proxmox + Windows VM + GPU passthrough + Sunshine + Moonlight over Tailscale.

That gives you the closest “offline GeForce Now” experience without building a custom streaming protocol.

## Part 1: 

Setting up proxmox:

