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
