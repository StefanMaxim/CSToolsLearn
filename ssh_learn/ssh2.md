# More on SSH

for SSH Server Vscode:
Potential Issue: using too much memory, evident by 

top -o %CPU to see the CPU usage of the processes

This shows that node is using 100% of the CPU, which is crazy amounts

Then, further look into the processes:

(process identifier, parent process identifier, process cpu, process memory, etime, cmd/command on mac)

ps -u "$USER" -o pid,ppid,pcpu,pmem,etime,cmd \
| egrep 'vscode|node|tsserver|pylance|eslint|rg|watcher|python' \
| sort -k3 -nr \
| head -30

\ means continue command on next line
| means pipe or regex or, depending on location
-k3 means look at 3rd column
-n means sort numerically
-r means reverse order

egrep is just grep -E.
Normal grep uses Basic Regular Expressions (BRE)
treating all chars as literals unless specified with \

grep -E however, treats those special characters as powerful operators, allowing more advanced regex searches

notice the 'vscode|node|tsserver...'
here, the | is treated as the "OR" for the sake of the search






STEP 2:
find the workspace where those processes are working:
pwdx 36128 36114 (TELLS YOU WHERE THOSE PROCESSES ARE)
(both in the root directory)

ps -fp 34441 (This follows the parents of those processes)
-f is more info, -p is by process id, expects id to follow





pkill -u %USER PID to kill a process, 


Also, try to enable Remote-SSH listen on socket, so multiple ppl can listen at a time.
This means server will listen on a socket path rather than opening a port every time.






## MORE ON SSH

ssh -t trainer_big

-t forces ssh to allocate a pseudo-terminal on the remote computer, useful for 

-y used to see your keys

ssh-keygen -y -f ~/.ssh/id_ed25519_... will let you see the key

if you are going to keep your private keys decrypted on your computer, which you often times do, it might be wiser to use 
some sort of encryption when storing them on a server, like pgp or something.



## SSH FOR COLLEGE

ssh -X enables X11 forwarding, meaning the main windows server (windows as in creates teh GUIs you see)
can be passed over as well, letting you see their windows created ontop of just the terminal access to it
From here, you can run GUI commands like:
xclock or gedit, and the output will appear on your side no problem

NOTE: for CMU, cannot login via SSH Keys, as the file ~/.ssh/authorized_keys is in the AFS, NOT the server's local drive!

### AFS
AFS (Andrew File System) is a distributed network filesystem.
Instead of your home directory living as a unique object on the local disk ofeach server you log into, it instead
lives on a shared network filesystem

**AFS TOKENS**
AFS protects files using tokens.
Before a program can read files in your AFS, it needs a valid token proving you are allowed access

During login, ssh service tries to read:
~/.ssh/authorized_keys, but the SSH service hasnt logged you in yet, meaning no AFS token exists
(service is hosted by a daemon on a server)
SSHD cannot bypass AFS security
