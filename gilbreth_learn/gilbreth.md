# Introduction to Gilbreth cluster and clustering:

> **NOTE** when entering your password, remember to put the ,push at the end so that it will send the duo notification
(once the ssh is set up you will no longer have to do this, but it si still good to know)

first thing we gotta do, is add the shell instance to my tailnet

1: add the tailscale repository to the package manager:

sudo dnf config-manager addrepo --from-repofile=https://pkgs.tailscale.com/stable/fedora/tailscale.repo
sudo dnf install tailscale

2: enable tailscale to work as a daemon
sudo systemctl enable --now tailscaled

3: initialize the tailscale instance
sudo tailscale up

4: find you user and ip for connection via ssh
tailscale ip -4
whoami


(FORGET THSI STUFF, first get the purdue VPN installed, so you can ssh from afar)

first, on the website shell, need to find your IP (local) and your username.

for username, its easy: its found using 

```bash
whoami
```
As for your IP, you can find your current local ip by using 
```bash
ip addr
```
but this will likely change, so instead use the domain name, and let your local DNS resolve it
(this is my domain name idk about you, it is found)
gilbreth-fe01.rcac.purdue.edu.

Cool, now that we have a user and domain name, its time to set up the ssh keys:




2:
on your local computer:

sudo apt update
sudo apt install ssh

ssh-keygen -t ed25519 -f ~/.ssh/id_ed25519_gilbreth_remote -C "keys from local computer to gilbreth cluster"
(keep pressing enter since you dont want a password ontop of this)



1: setting up your ssh config file:

dont worry abt what this means, just type

"vim ~/.ssh/config"

and copy this in verbatim

Host *
    AddKeysToAgent yes
    ServerAliveInterval 60
    ServerAliveCountMax 4
Host gilbreth_remote
    HostName [insert the ip address here]
    User [put your username here]
    IdentityFile [~/.ssh/path/to/identity/file]
    IdentitiesOnly yes
    StrictHostKeyChecking accept-new
    Port 22



in the formula above, enter your path to identity file (the thing in the f field)
and your username and hostname (the domain name from before)


Lastly, time to make the remote aware of your existence:
run this command blindly, trust it works

cat ~/.ssh/path/to/identity/file.pub | ssh gilbreth_remote "mkdir -p ~/.ssh && cat >> ~/.ssh/authorized_keys && chmod 700 ~/.ssh && chmod 600 ~/.ssh/authorized_keys"

Now, like magic, you can ssh into gilbreth!

type ssh gilbreth_remote, and your in




## Okay, but how do I use it:

list of the possible nodes:
https://www.rcac.purdue.edu/knowledge/gilbreth/overview
(note: one node is one group of GPUs you can access)
each node has multiple GPUs you can use
can also use 
```bash
sfeatures
```
exe:
NODELIST              CPUS   MEMORY    AVAIL_FEATURES                                    GRES
gilbreth-b[000-015]   24     190000    B,b,A30,a30                                       gpu:3,hp_a30:3,hp_cpu:24
gilbreth-d[000-007]   16     190000    D,d,A30,a30                                       gpu:3,hp_a30:3,hp_cpu:16
gilbreth-g[000-011]   128    510000    G,g,A100,a100,A100-40GB,a100-40gb                 gpu:2,hp_a100-40gb:2,hp_cpu:128
gilbreth-h[000-015]   32     512000    H,h,A10,a10                                       gpu:3,hp_a10:3,hp_cpu:32
gilbreth-i[000-004]   32     512000    I,i,A100,a100,A100-80GB,a100-80gb                 gpu:2,hp_a100-80gb:2,hp_cpu:32
gilbreth-j[000-001]   128    1020000   J,j,A100,a100,A100-80GB,a100-80gb                 gpu:4,hp_a100-80gb:4,hp_cpu:128
gilbreth-k[000-051]   64     512000    K,k,A100,a100,A100-80GB,a100-80gb                 gpu:2,hp_a100-80gb:2,hp_cpu:64
gilbreth-l[000-001]   64     503000    L,l,H100,h100                                     gpu:2,hp_h100:2,hp_cpu:64
gilbreth-m[000-002]   96     2051700   M,m,H100,h100                                     gpu:4,hp_h100:4,hp_cpu:96
gilbreth-n[000-019]   48     1010000   N,n,A100,a100,A100-40GB,a100-40gb,NVLINK,nvlink   gpu:4,hp_a100-40gb:4,hp_cpu:48

HUH? 
node list tells you the available nodes:
exe in the first like yu see that gilbreth-b nodes from 000 to 015 are available, it says how much cpus and memory
each node has, as well as the available features
(please read the link, it eplains it better than I can):

Basicically, there are two types of clusters:
cluster front-end, and subclusters:

cluster front end is the nodes you ssh to:
(read up on beowolf clusters to learn more)
it handles the routing of tasks, authentication, scheduling, and coordination

(think Kubernetes control plane / API server)

Subclusters, or the worker nodes:
this is where the actual work gets done, and output returned to the front node 

(gotta read this and familiarize self:https://www.rcac.purdue.edu/knowledge/gilbreth/overview)
