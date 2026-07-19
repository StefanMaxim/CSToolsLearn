# GIT

## Overview:

### Intro to Git
READ NOTES ON PAPER, BUT LOOSE IDEA IS:

Git is a version control system.
It is used to keep track of your files and folders relative to some root directory

exe:
|(root)
| --foo (tree)
|      --bar.txt (blob)
|               "hello world"
|
| --baz.txt (blob)
|           "git is awesome"

History is modeled using 3 kep parts:

**BLOB**
A blob is a regular file (txt, .mp4, .md, .py, .java, etc)

**TREE**
A tree is a special type of file, namely a directory file (more on that later)

**Commits**
A commit is a special git object that consists of a tree (namely whatever the root directory is)
plus some metadata about it: Author, committer, msg

History is stored as a DAG, or Directed Acyclic Graph
**DAG / Directed Acyclic Graph**
A DAG is a special type of graph where its edges are directed, and there are no cycles in it:

exe:

O -> O ---> O ---> O
      \           ^
       \         /
        -- > O --

Here, each node/vertex represents a commit, and the edges represent the pointer to the parent commit

WHY STRUCTURE IT LIKE THIS? WHY NOT A SINGLE LINEAR CHAIN:

1: Directed is good, because then can clearly see which is the parent commit, and the way that history flows

2: Good to have acyclic graph bc then can split history (for example, if you want to work on a feature and a bugfix
at the same time) and then merge them back together when you are done.


  (base) (base+feature)
O -> O ---> O ---> O (base + feature + bugfix)
      \           ^
       \         /
        -- > O --
            (base+bugfix)

This allows you to work in parallel, 

### Deep Dive Into The Datastructures

FIRST:
**CRUTIAL**
when you initialize a git repository inside of your directory(git init), all of the associated content will be inside of
".git", which looks like this:
COMMIT_EDITMSG  HEAD            config          hooks           info            objects
FETCH_HEAD      ORIG_HEAD       description     index           logs            refs
(will learn what everything here means)

#### Blobs
type blob = array of bytes:

A standard file on your computer is in reality just an array of bytes
The "blob" datastructure is the same. It just takes a file and compresses it using
zlib compression, and the resulting object is the blob.

These compressed objects are stored in the "objects" subdirectory, addressed by SHA1 Hash:

**KEY**
for naming objects, git uses their SHA1 Hash, an algorithm which uses the contents of a file to create a unique
40 character long string that is used to identify the object.

inside of objects, you will see sub-directories like:

ls
00      0f      1e      2c      3b      49      56      64      73      87      94      a2      b2      c5      d5      e3      f4      info
01      10      1f      2d      3c      4a      59      65      74      88      95      a3      b3      c6      d6      e4      f5      pack
03      11      20      2f      3d      4b      5a      66      75      89      96      a4      b4      c7      d7      e5      f6
04      13      21      30      3e      4c      5b      69      77      8a      97      a6      b5      c9      d8      e6      f7


HERE:
Everything here is a directory, where each 2 character directory is the first 2 characters of that object's hash:

exe:

1e9f80a9e541 -> in the direcory 1e (this is just a way of organizing the massive amount of objects using something
analogous to a hashmap)

**TIP**
can use git cat-file -p <SHA1 HASH / reference> to look at what an object is under the hood, which de-compresses it
and displays its contents

exe:

the file:

hello_world.txt, with contents "hello world"
yields a hash of 95d09f2b10159347eece71399a7e2e907ea3df4f, which will be found in the subdirectory
95 in the objects directory, as a compresses zlib file.

**TIP 2**
git automatically fills in the remainder of a Hash once unique, so you dont have to type out the full 40 character 
Hash

#### Trees
type tree = map<string,tree|blob>

HUH? what do you mean "map"?

Low-level, a directory is just a file whose contents describe the mapping between a file/directories name and its actual
contents (a table)

Trees/directories hold pairs of human readable strings, e.g. "notes.txt" and a pointer to the metadata about the file
(inode numbers in Unix)

##### Brief Asside on Unix Filesystem
(PLEASE READ filesystem_intro.md / filesystem_advanced.md for more info, this is just brief overview)

An **inode** is a datastruture that hold data like filesize, timestamps, and the address of the disk block that holds the file content

(the actual persistant copy of all data on your machine is on a harddrive/disk, however its often easier to keep
a copy of that in memory and operate on that since its much quicker and then just write the changes to disk when you are
done).

exe:

suppose you have some directory "docs" which contains the file "essay.txt":
directory file /docs has entry:
    "essay.txt" -> inode 1234 (**NOTE** this mapping is called a **dentry**, where the number is the index of the inode
                                in the **inode table** your kernel has, which is just a table mapping all the inode #s
                                your computer uses to their actual on-disk counterparts)

inode 1234:
    filesize = 2kb
    data blocks = [block 5001, block 5002]
    perms: -rw-r--r--

PERMS FORMAT:
first character:
 - -> regular file
 d -> directory file
 l -> symbolic link (symlink)

next 3:
owner perms (permissions for the user who owns the file)
flags:
r: read perms
w: write perms
x: execute
-: no perms

exe: rw- means read, write, and no execute perms
     r-x means read, execute, and no write perms

next 3:
perms for the group, meaning the user group from which the owner is themselves. exe:group "students" or "developers"

last 3:
perms for others (world perms) , who are not the owner or not in the same group as the owner

Thus:
-rw-r--r-- means its a regular file, owner can read and write, user group can read, and others can read too.

**Symlink**
a symlink is a special type of file whose contents is the path to another file in the filesystem.

exe:
symlink_file:
    /path/to/other/file/or/directory

**Special Dentries**
the dentries . and .. are special directory file entries, where . maps the the inode for the CURRENT directory, and
.. is the inode of the parent directory

WHY HAVE THESE: suppose you are inside of a directory and want to reference the current inode. This way you can.
Also, suppose you are in a directory but want to move back a few directories and go down a different chain. THis way, 
you can keep going to the parent dir and go down anywhere in the filesystem!


**MORE ON INODES**

Recall from earlier: 
Persistent copy of all data is on disk, but for the sake of speed, it is loaded into RAM and operated on there
before copying it back to disk when you are done.

Thus, there are also 2 "types" of inodes.

1. Definite Inodes
The ones that live on the disk.

**KEY**
WHen creating a filesystem, exe mkfs.ext4
it lays out several on-disk datastructures, such as superblocks, block group descriptors, inode tables, datablocks, and 
bitmaps.
each inode occupies (usually) 256 bytes on disk in an inode table:

Partition
Superblock
GRoup descriptor tabl
block group 1:
    block bitmap
    inode bitmap 
    inode table
    data blocks

when kernel wants an inode:
it first determines the inode #
then computes which block group contains that number via the inode number: group (i-1)/inodes_per_group, index is % vs /
then lastly reads corresponding inode from the inode table on disk

some inode slots are free or allocated(representing current files/directories)
disk inode tables colelctively represent all possible inodes for the system

2. Inode Structs
When you want to "load" in from the disk to RAM, the inodes are represented in the kernel as a special object
called an Inode Struct. This is the version you see more commonly, and is what the kernel uses to preform writes.

Example Inode Struct:

docs inode 6000:
    mode drwxr-xr-x
    uid/guid 1000/1000
    size 4096 bytes
    link count 3
    block pointers [1010,0,0...]

disk block 1010:

    Entry A: name="." -> inode=6000 (directory itself)
    Entry B: name=".." -> inode=27 (parent directory)
    Entry C: name="notes.txt" -> inode=98754 (inode # for the file where block data for this file is)
    Entry D: name="subdir" -> inode=13000

(NOTE): The disk block is the actual data on the disk, which is just bytes in reality, not organized in a datastructure
This is just what the bytes can be interpreted to be, which is what the kernel does when reading and interpreting the data

the docs inode 6000 on the other hand, is an inode struct, so it is already interpreted and does look like that in memory.
Key here is knowing the difference between bytes on disk, and their interpreted form which is now a datastructure in
the kernel.


**uid/guid**
This is the user identifier / group identifier, which is used by the kernel internally for identifying 
users / groups rather than names

**link count**
this is the number of hark links a file has.
**hard link** is an explicit reference to the inode #
exe: if a directory file says:
"file.txt" -> inode 1234, this counts as an additional hard link to that inode

**NOTE** by default, every inode of a directory starts off with a link count of 2:
1. the "." used inside of that directory (which we know to point to that inode)

/my/directory/
"." -> inode 1234

2. the named dentry in the parent directory that refers to it

/my/
"directory" -> inode 1234

**block pointers**
this is the pointer + offset to use when accessing the date from the drive.
The drive isnt a single contiguous blob of storage, rather it is divided into disk blocks, each of which can be 
offset by a certain amount. This way, your inode can be very clear as to which blocks it represents on the disk.

**PUTTING IT ALL TOGETHER**
How does the kernel resolve a path, like
/home/gcmain/docs (a random path)

1. Kernel starts from a known inode (/ and ~ are known inodes, and are allways loaded into memory for ease of traversal)

2. Kernel reads the block data from root. 
this is done by looking at the already loaded inode, finding its corresponding data blocks, reading them from disk, 
and then interpreting their bytes as a mapping

3. Kernel parses the block data's directory mapping, untill found "home" -> inode 12. Now, it read and interpreted the 
dentry, knowing that the string "home" would map to the inode 12

4. Kernel loads in the inode 12 from the disk from the disk, creating the inode struct with its information, as well as
loading in its contents into memory

5. Reads the block data's, and again parses for the mapping "gcmain" -> some inode number (in this canse inode 142 or something)

6. loads inode 142 into memory as a struct, and loads and interprets its memory

7. find the mapping "docs" -> inode 1678

8. loads in inode 1678, now in the final directory inode!


**NOTES**
- when you use the "mv" command within a filesystem, exe mv /my/file /new/location

 what really happens is it just first finds the inode of the old file by walking the path:
 "file" -> inode 765
 and creates a new dentry, "location" -> inode 765 in the new directory location
 and finally removes the old dentry, keeping the number of hardlinks the same and doing minimal work to 
 "copy" the data to a new location

 HOWEVER: when moving across filesystems (the drive partition) you cannot just move the old inode, as inodes
 only have meaning within a partition. Thus, you must instead actually copy over the data to a new disk with a 
 new data write, create a new inode on disk, and write the data to the new corresponding data blocks on the new partition


 - also, a file is deleted (or rather the inode is removed, giving up that space on the disk) when its number of hard
 links goes to 0, or num links = 0. Thus, when you use the rm command, you arent actually deleting anything necessarily,
 just the dentry, unless its num links go to 0, at which point the inode is deleted. 


 - like i said before, inodes live on the SSD, but the kernel can cache them in RAM (in a place called the **Inode Cache**)
 for reading, but the definitive one on the disk

 - creating inodes:
 when you run the command "touch file.txt", the kernel
 1. finds a free inode number in its table
 2. sets its metadata + sets link count to 1
 3. allocates 0 or more data blocks
 4. adds entry to dir
once you change inodes or when possible, will write to the disk

- changing inodes
chmod: in reality changes the inode perms
chown: changes the ownership metadata
touch: updates timestamps
writing to file: kernel allocates new datablocks + updates inode metadata






BACK TO TREES IN GIT:
now that you sorta understand what a directory is, the git version is the same way, just compressed using 
zlib again.

exe: suppose you have only one directory, with the file(blob) hello_world.txt:

whereas a directory would map a string to an inode number, git trees map
"hello_world.txt" -> SHA1 Hash of the blob, in this case 95d09f2b10159347eece71399a7e2e907ea3df4f

also, again, the tree itself is a compressed version of that file, referenced by its SHA1 Hash, and found
the same way as blobs in teh .git/objects/ directory

#### Commits

Commits are the last structure, and they are a struct used to represent a node/vertex:

type commit = struct {
    parents = array<commits>
    author = String
    message = String
    snapshot = tree
}

Again, this object is compressed as a file, and is referenced via its SHA1 Hash, 2206c4dd9a14fcd61178a88639ebd43740ee08

parents= is an array of pointers to that commits parents (also commits)
this is what enables the directed acyclic nature of the graph, and this series of commits, and their pointers
can be interpreted as a graph, though in reality is it just a struct

author= is the name of the author of those files

message = any message you want to include with the commit, used for bookkeeping to remember what you did

snapshot = this stores the pointer to the root tree object. All commits start from a root tree, and include
the chain recursively inside them of trees and blobs.




NOW BACK TO GIT BIG PICTURE:
as you heard, git objects are **content-addressed**

this means that on disk, 
objects = map<String, object>, where the string is the SHA1 hash of the object (that directory)

alongside the functions:

def store(object):
    id = sha1(object)
    objects[id] = object

hash function:
    takes data -> short string
    **KEY** this hashing is deterministic, so same object will have same hash every time

def load(id):
    object = objects[id]
    return object


**NOTE** blobs, trees, and commits are unified by the fact that they are all objects
when you lookup/load an object, git decompresses + parses the object data into a struct in memory.
objects/ is a key-value object database (not a hashmap!) that maps the strings to the serialized compressed object files


## .git directory parts
".git", which looks like this:
COMMIT_EDITMSG  HEAD            config          hooks           info            objects
FETCH_HEAD      ORIG_HEAD       description     index           logs            refs

objects: explained heavily, but its the key-value object database used to store the compressed git objects and the
SHA1 Hash addressing

index: file representing the index / staging area. This is a place where you store files before committing

refs: referring to git objects, especially commits, by hash is tedious, so instead can use a reference.
A reference is a string, which when used in git commands, maps to a SHA1 Hash of a commit. You can select which
commit to name, and what to name it, and can change it as you go

**NOTE** these references in git are called "branches", because they can be used to indicate
different "branches" of development. A branch starts off at a given commit, but when that commit has a child, 
it usually moves to that child by default, called **fast-forwarding** the branch. Branches, and their underlying references
are useful because it lets us more clearly see what the branches of development are.


HEAD: head is a special refernce, known as a symbolic reference. This also points to a commit or reference,but has the special purpose of being interpreted as "where you are". most git commmands, unless otherwise specified, will work relative to this 
reference, and crutially when someone clones your repo (makes a copy of your project on their own computer) this is 
where they start, which makes it very important.

## git structure

Git is primarily organized around Commits, and how to create new commits for version control over what you are working
on right now. 

To this end, git had 3 key components:

1. Past Commits (the commit structure discussed heavily up untill now)

2. The Working Directory / Working Tree
This represents the current state of your directory. This literally means just what you directory looks like right now

exe: suppose you are in a directory titled "dir", and it has the files "foo.txt" and "bar.txt"

the current working directory would look like

dir/
|
|--- foo.txt
|
|--- bar.txt

3. The staging area / index

THIS IS THE KEY MIDDLEGROUND!

one possible model would be to have just past commits and the working tree. 
Then, have a git command that would just take a snapshot of the working tree, turn it into a compressed git object
(a commit) and then store that for safekeeping.

However, what if there's a file in your directory that you really dont want to save in history, like a passwords doc
or a notes document thats not really that important? What about a massive data file that is big enough as it is that
it doesnt need to be loaded into history and compressed? The solution is the **index**

This is an area where the user can selectively add/remove files to prepare them to be committed.
Instead of committing the entire repo, just add whatever you would like to the "staging area", which in reality is just a 
file keeping track of what you want added, and then the commit command only serializes and compresses those select items. 



Most git commands are concerned with the interplay between these 3 fundamental spaces, be it adding/removing files
from the working directory to the index, commiting the index, or comparing the working directory to the index or past
commits. 


Also important to note, is how these past commits can be used for version control. Suppose you, in your current version of a program made a mistake, and you want to undo it to be like the way it was a while ago. Theres a command for that (git restore). 
What if you just want to make everything like the way it was before and run tests on that old code. Meaning, turn your 
working directory back the way it was a while ago using a commit as guidlines. Theres a command for that (git checkout).
**NOTE** however, that if you have changes made from a previous commit in the working directory, it wont let you checkout
and old commit without either commiting what you have or discarding it.

Basically, all of git is about moving in these spaces, changing between them, comparing them, and parallel development using branches.








## Commands

Now that you get how git works in reality, from here its just learning commands and what they do:

### Intro

> git init:
initializes a git repository inside of a directory (creates the .git directory that allows the commands to work)

> git help
Useful for learning how git works
can use help on any basic tooling to see what it does

> git status:
very crutial command!
this tells you all sorts of important information:

1. tells you what branch you are on
"On branch main"

NOTE: when you just created a git repo, it will also say "on branch main", even though no commits exist yet, and even no
branches.
(can test with git branch -v , which lists all branches)
That is because it is looking at where your HEAD is pointing (representing where you are).

inside of .git, the file HEAD just stores a string representing where HEAD is pointing:

ref: refs/heads/main //by default, this is said to be refs/heads/main

THus, when you type git status, it will by default tell you on branch main, because thats what it says in HEAD, 
even though there is no reference called refs/heads/main YET.

only after you first create an initial commit (which must be on branch main) will it finally create the reference, and 
from there you can make more branches and explore parallel development.

2. It also tells you whether your branch is ahread/behind of its upsteam (more on that later)

3. It also tells you about which changes remain to be committed
In other words, it tells you what files you have added to the index, and that stand to be committed

4. It also tells you which changes have not been stated for commit yet, but nonetheless exist between
HEAD and your working directory



> git add
main way to add files to your index.
If this is the first time you added this file to the index, in that it doesnt exist in any past commits, that file is 
considered **Untracked** and will have a green U next to its name

If it exists in version history, but nonetheless has modifications from the last commit, that file is considered
**Modified** and will have a yello M next to its name

Files which already exist in version control history and have no changes from head will appear as 
**Tracked** files, which have no specific markers

Only files that are untracked or modified can be staged to the index, as untracked unmodified files have no need of appearing

**NOTE** once a file is considered TRACKED, it will be automatically included in the next commit. There is no need
to re-add it every time. If you commit the index without adding an untracked file, that file will be excluded from the created commit. If you commit to the index without adding a modified file, that file will be included in the created commit, however it will appear as its most recent version in history, which will not match up with its current state

Once a file is added to the staging area, it will appear as **Added**, meaning it exists in the index, and is marked by a
green or yellow A next to it.


git add <file/dir> 
adds the file/dir to the index

git add :/ 
adds all files/directories visible to git into the staging area

git add -A 
same as above, difference being :/ means the root, so you adding the root directory and all its files

> git rm
This is used for removing files. This can mean removing them using rm and then staging the change in the repo,
or removing them from the list of tracked files

git rm file
removes the file and stages the change, telling git to no longer track it

git rm --cached file
removes the file from the index and tells git to no longer track it

git rm -r dir/ 
like regular git rm, but recursive

git rm --cached -r dir/
recursive dir flag for cached removal


> git commit
This command creates a new commit using the staging area + tracked files, and adds it to version history

git commit 
by default, makes a new commit using working directory, and sets its parent to head

git commit -m "msg"
git commits require a message, so the -m flag lets you specify it without having to vim it in

git commit -a
automatically adds and commits all changes made to currently tracked files. IE if a file is tracked and modified, will
add and commit them all


> git restore
Used for restoring a file/dir in the working directory back to the state it was in a previous commit

git restore filename
By default, restores the working file to state at HEAD

git restore -s (source) filename
Lets you restore from a specified commit/reference

git restore --staged filename
Removes the modifications of a file from the index without untracking the file. IE, restores the state that the file
appeared in the staging area before this round of adds (could be untracked, or what it was during HEAD) but restores
the file in the index without modifying the working dir. **VERY USEFUL**

> git log

git log
Makes you a visual representation of the commit DAG for visualizing version control

git log --all --graph --decorate --oneline
Useful command for making a more clean, pretty graph

**TIP** for ease of use, set this as an alias in your git config file, ~/.gitconfig:

~/.gitconfig ->

[User]
    name = Stefan Teodor Maxim
    email = stefan.teodor.maxim@gmail.com
[Alias]
    lg = log --all --graph --decorate --oneline


> git cat-file

git cat-file -p object

**VERY USEFUL FOR LEARNING GIT** lets you decompress and inspect git objects like commits, trees, and blobs to see
what they look like. good for seeing git internals

> git checkout
This is the command that lets you "check out" what different files and commits used to look like.
This works by overriding your working directory and replacing it with the specified file/commit/reference
NOTE: cannot have any pending changes at this point, as they will be discarded on checkout, so save them somehow

git checkout filename.txt 
turns your filename.txt into the verison it used to be in HEAD

git checkout -f filename.txt
Force flag, lets you discard changes in working directory and make file match version in HEAD

git checkout <ref/id>
this lets you make your working directory match the way it used to be for a given commit. VERY USEFUL

> git diff

This is used to compare the versions of files/directories between commits

git diff
by itself, git diff compares your working directory with your staging area

git diff file
compares only the file, not the entire commit vs root dir

git diff --staged
compares your index with HEAD commit

git diff -a
compares for all files tracked

git diff HEAD file
now, specifically compares working directory with teh commit/reference HEAD

git diff COMMIT1 COMMIT2 file
Now, compares Commit1 and Commit2, id/refs, and shows what it takes to go from COMMIT1 to COMMIT2

### Git Branches
Like I said earlier, a branch is just a moving reference, that represents a "branch" of development
When you checkout a branch, what that means is that HEAD points to that branch/refernce
By default, HEAD points to main, but can change that via checkout.

> git switch
used to switch branches. git commit can also do this since its overridden, but this is the correct command

> git branch
The master command for dealing with branches, be that creating, deleting, or listing them

git branch
lists all the branches

git branch -v
lists all the branches, and some info about them liek commit,msg, and how much ahead it is relative to upstream

git branch -vv
Even more verbose version, including the branch's upstream (more on that later)

git branch branch_name
Used to create a new branch

git checkout -B branch_name
creates + checks out a branch simultaneously

git branch -m oldname newname
used to "move" or rather rename a branch to a new name

git branch -M newname
forcefully renames current branch (HEAD POINT) to newname

git branch -d branchname
removes branch/reference. only useful when redundant (ie 2 branches on same commit)

git branch -D branchname
forcefully removes the branch

git branch -c oldbranch newbranch
copies old branch to newbranch, basically makes a new reference there

### Git Merging

Like I said in the very beginning, a big advantage of git is the ability to split a once linear history into branches 
then merge them in the end. Loosely, it combines the two commits into a single one, hopefully automatically, but if its
not that easy then via merge conflicts which you must manually resolve

>git merge
main command for merging branches

git merge branchname
merges your current branch with the branch branchname
**NOTE** more precisely, it merges branchname INTO your branch, meaning that the current branch reference will be
the one that moves forward, considered the "primary" branch in this transaction.

exe:

* ec9cc1c (HEAD -> feature2) feature 2
| * d08a914 (feature) feature initial branch
|/  
* 8d560e1 (main) msg

first, go to main via git switch main

now, tryo merge via git merge feature2
This will merge feature2 into main(moving mains reference)

HOWEVER: note the parent-child relationship between them. These types of merges are called **fast-forwards**
and are resolved by just moving main's reference to the commit of feature2

* ec9cc1c (HEAD -> main, feature2) feature 2
| * d08a914 (feature) feature initial branch
|/  
* 8d560e1 msg

from here, we want to combine both of those branches, so just 


git merge main feature1
merges feature1 branch into main branch, updating main's reference

in this case, we happen to get a merge conflict:

in our case, versions.py was messed up. 

**NOTE** from here, you have to resolve the merge conflict, and there are many tools at your availablity.
First, all files that have conflicting writing will appear with a "!" next to them. Furthermore, they will look something like this:

import sys

<<<<<<< HEAD
def main():
	dog()
	
def dog():
	print(f"Bark")
	
if __name__ == '__main__':
	main()
=======

def main():
    cat()

def cat():
    print("Meow")


if __name__ == '__main__':
    main()
>>>>>>> feature

Here it displays the two disparaate versions, with the top being the version from teh HEAD branch (current)
and the bottom being the feature branch you are merging into it. 


git merge --abort
Used to cancel the merge, lets you keep thinking on what do do

Otherwise, you must resolve the conflict:
To do this, you must fix the modified file by combining the two versions now interspliced in your working directory and
then add that file to the staging area via

git add file

finally, when you are done, git merge --continue to finalize the merge.
This can be done either by editing the file manually or using tools like what vscode offers to combine the file so you can add it.


Summary:

> git merge branchname
merges branchname into current brancb

> git merge branch1 branch2
merges branch2 into branch1

> git merge --abort
used to abort the merge

> git add file
used after modifying the amalgamation file in the working dir to say you are done

> git merge --continue
used to continue the merge once no more conflicts exist

### Git Remotes
Git primarily is concerned with the local git repo, however sometimes you may want to save your changes in another repo
or over the internet. That is whare git remotes come in.

A remote is a git repo that can be used as a different version of your current repo. 

Your current (local) git repo has various objects, references, and configs, which can be pushed to other
git repos for the sake of collaboration or safety. 


> git remote
lists which remotes you have available

git remote -vv gives more detail on the remotes, like address or location of the repo

> git remote add <name> <url>
lets you add a new remote to your repo

> git fetch origin 
this grabs all of the references from the remote and updates then on the local, changing the symbolic reference 
representing them

**ASIDE ON SYMBOLIC REFERENCES**
unlike regular references on your local, these ones are set primarily by fetching from the remote, not updated yourself

you can run commands like:

git remote set-branches <name> <branch>, whcih changes the branches tracked by the remote and more,like
git remtoe set-head origin master, but CRUTIALLY, THESE ONLY UPDATE YOUR LOCAL SYMBOLIC REFS, AND DO NOT PUSH THESE CHANGES ONTO THE REMOTE!!


git fetch --prune remote
this will remove all referenes not in the remote


> git push

The main way that you syncronize the git repository on your local device and the remote.
Conceptually, it performs 2 independant actions:
1. Transfer sgit objects (commits, trees, blobs, tags, etc if needed)
2. Requests updates to references (refs) on the remote

Recall:
Git stores two things

Objects (commits, trees, blobs, tages)
and

References:
exe:
refs/heads/main
refs/heads/dev
HEAD
etc...

These files simply contain SHA1/SHA256 IDs of some commit(representing the one which it points to)


What happens when you push:

remote:
A--B

local:
A--B--C

git push origin main:main

1. First, it connects to the remote origin via either SSH, HTTPS, or some other protocol
2. Remote advertises its references
exe, it says "I currently have commits:
refs/heads/main = B
refs/heads/dev = D
refs/tags/v1.0 = T
"
3. Local compares graphs
looks at the two references specified.
Sees want to push some local branch main to a remote branch algo called main (local-branch:remote-branch)

Git sees that local main points to C
Remote main points to B
Thus, git knows it want to main the remote branch point to C as well.

**NOTE** the goal of a push is to make a reference on the remote point to the same commit as on the local!!!
**THIS IS HUGE!!! ITS NOT ABOUT MATCHING THE HISTORY, JUST MAKING THE REFERENCE POINT TO THE SAME COMMIT**
THE GRAPH (HISTORY) IS CREATED ON THE FLY FROM THE REFERENCES. THE HISTORY MATCHES MORESO AS A BYPRODUCT, RATHER THAN
A GOAL!!!!!


4. Git computes missing objects (note: looks at objects, not files!)
Git looks at what is necessary to turn the remote branch in to the local branch.
Want to make the remote main point to C, so for this:
Remote missing C (must give it C)
Remote already has: A,B (the parent commits, needed for the graph)


5.  Git does more granular search for missing objects
Suppose commit C introduced
new file
blob x
tree y 
commit C

Git determines remote lacks
commit C
tree Y
blob X

Thus, git only sends blob X, tree Y, commit C

6. Git creates a packfile, which is a compressed archive of objects

PACK
blob 
tree
commit
blob
commit
(delta-compressed)

**KEY**
instead of 
Version 1
Version 2
It stores:
Version 1
+
diff (making pushes much smaller)

7. git sends update request
Update
refs/heads/main
Old = B
New = C
NOTE: this isnt saying "Make main point to C", but "ONLY UPDATE IF main is currently equals B"

(THIS IS KEY, BECAUSE IT PREVENTS RACE CONDITIONS)
exe:
suppose meanwhile somebody else pushed 
A--B--D
now, main -> D

Without the check, main would become C, whcih points to B, whcih is confusing, since its biforcated tree.
The condition prevents race conditions

8. Checks policy



INPORTANT 2:
Fast-forward vs Non-Fast-Forward

exe:

remote: A--B
local: A--B--C--D

updating the remote's main from B to D is safe because B is an ancestor of D (nothing disappears from the graph)
Just have to make it point to D, and Copy over C and D, but A and B still present.

suppose instead:

remote: A--B--E
local: A--B--C
here, neither commit is an ancestor.
Updating the remote's main from E->C would lose visibility of E (rejected)
(THE FAST FORWARD CHECK IS JUST ARE ALL COMMITS ASSOCIATED WITH TEH OLD REFERENCE VALUE ANCESTORS OF THE NEW
REFERENCE)



git push --force

This option ignores ancestry, and moves the reference anyways (still does perms check, but skips fast-forward validation)
(will just change the remote reference to the desired commit without checking that it preserved fast-forward)

git push --force-with-lease

This is the safer version of force, in that is checks that
"move ref only if remote still equals what I expect"
this way, if somebody in the mean time pushed E after you fetched, the force fails
RECALL: pushes updates include previous commit:

Update refs/heads/main
Old=B
New=D

remote uses the update to check "Is main still B", If yes it can safely change to D. If in the meantime its changed
to E from some other push, it will reject your push.

KEY: value that gets used as old depends on the type of push.
for normal push, local learns of remote from the initial fetch, then uses that information to send the update
request above. 

for force push, its different:
suppose:
remote: main -> E
local: main ->D' (custom change)

when you git push --force, it first queries the remote, it says its at 'E', includes the E as part of the update request,
the remote checks that its still at E, thats true, then its done. 
However, notice this doesnt account for the fact that yesterday, when you rewrote your history, the branch was at D. 
(just the current state of the remote)

**KEY** 
difference is between what is considered the OLD
for normal case, OLD is just what the remote has right before your push, obtained from the initial query
(this is just done to avoid race conditions when pushing)

However, --force-with-lease makes OLD be the value that it was logged as in the local, which is based on the most recent
fetch

This prevents you from erasing some new work that you were not previously aware of

EXE:
suppose yesterday, the remote looked like

A--B--C

you fetch it, and make 2 new commits:

A--B--C--D--E

now, decide, "want to combine D and E" into a single commit
cannot modify existing commits, but can create a new commit which is the same final file as D+E

A--B--C--F

now, suppose new dev, alice fetches, and pushes:

A--B--C--G, meanwhile, you locally have A--B--C--F

now, you want to replace the remote's main G with F. 
this needs a force, because fails the fast-forward test since G is not an ancestor of F


**NOTE2** to solve this dilemma, likely you would use git rebase to rebase your commit as if it were after G.
git fetch origin + git rebase main origin/main


> git push -u origin main
this sets the upstream of your main branch to origin/main.
NOTE: when no second branch specified, will push the given local branch to a remote branch of the same name

**SETTING UPSTREAM**
the "upstream" of a branch is the reference on the remote that should match it
once the upstread of a branch is set,
"git push" from within a branch is understood as "git push <remote> <local:remote>" for whatever the remote 
corresponding branch is. 


git push --prune
the prune flag means remove any references on teh remote which do not match up with ones on the local
(usually have the same name)


> git clone <url> <folder_name>
This clones the repo from the cloud (git) onto your local computer. Basically, it downloads all of the stuff on the remote
and makes your local look just like it

>git pull 
This is just a combo between git fetch and git merge branch remote/branch.
This updates one of your local branches based on its upstream. FOR THIS, UPSTREAM MUST BE SET.

git pull origin main 
does the same, but when no upsteam set, this is what to use













