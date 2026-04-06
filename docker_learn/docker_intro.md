# Intro to Docker:

One possible use case: suppose you are trying to ssh onto a server, but you cannot control what is on it at a sudo
level, in that case, can use a container and ssh into that container. (can also use apptainer)
(module avail to see if that module is available)

Apptainer: by default, it mounts your home directory ~ into the container, so any changes there persist.

ERROR: cannot just module load apptainer, as sometimes it may be too new for current glibc version.
do module avail apptainer to see all versions, and load the older version via module load apptainer/1.2.1

NOTE: apptainer pull ubuntu.sif docker://ubuntu:22.04, will pull the os and load it into a .sif file.

(ASSUMING ON MACBOOK, WHICH IS ARM, MUST FORCE DOWNLOAD PLATFORM)
docker pull --platform linux/amd64 ubuntu:22.04
(downloads the x86_64 version)

apptainer build ubuntu_22.04.sif docker-daemon://ubuntu:22.04
(builds docker image into .sif file)
docker-daemon://ubuntu:22.04 means to use the one in the local directory
the resulting .sif is a portable container.

Docker stores files inside of a special docker vm (Hyperkit/QEMU), and stores files in special docker format, not
.iso or whatever
/var/lib/docker/

apptainer talks to the docker-daemon using API calls, notice it looks like https://, which will then return the files
needed via API!


(CAN ALSO DO THIS)
docker save ubuntu:22.04 -o ubuntu_22.04.tar (saves it as a tar)
apptainer build ubuntu_22.04.sif docker-archive://ubuntu_22.04.tar ()



(IF YOU CANNOT GET APPTAINER ON YOUR MAC, JUST GET IT AS A TARBALL, SCP IT OVER, THEN )
apptainer build ~/containers/ubuntu_22.04.sif docker-archive://~/containers/ubuntu_22.04.tar
REMEMBER: create a directory named ~/containers/ to store the sif files and such



remember to edit your ssh config to ssh into the container (ie it will create a container)
    Host purdue_cs_container
    HostName your.server.edu
    User your_username
    RemoteCommand apptainer shell docker://ubuntu:22.04



docker pull ubuntu:22.04 //pulls the docker image to usr/var/docker or something, docker pull ubuntu:22.04 -o 22.04.tar
docker save ubuntu:22.04 -o ubuntu:22.04.tar

apptainer 






## Docker Install

Docker Desktop vs Docker Engine:

Engine = the low level one that actually hosts the containers, communicate via terminal
Desktop = the GUI version

NOTE: brew install docker only install the CLI tools, not the Engine, or the Desktop

Engine Install:

 curl -fsSL https://get.docker.com -o get-docker.sh
 sudo sh ./get-docker.sh --dry-run







## Docker Volumes + Bind Mounts

To ensure persistance, use docker volumes



## NOTE

sudo systemctl start docker
sudo systemctl enable docker --now
sudo systemctl stop docker
sudo systemctl disable docker