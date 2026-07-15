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

HEAD: head is a special refernce, known as a symbolic reference. This also points to a commit or reference,but has the special purpose of being interpreted as "where you are". most git commmands, unless otherwise specified, will work relative to this 
reference, and crutially when someone clones your repo (makes a copy of your project on their own computer) this is 
where they start, which makes it very important.



## Commands

Now that you get how git works in reality, from here its just learning commands and what they do:

### Intro