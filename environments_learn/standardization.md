# Standardization

Idea: want macbook and mac mini to be functional clones of eachother, how to facilitate:

1: pull all config into a dotfile repo
(zshrc, gitconfig, ssh config, terminal themes, editor settings etc)

can use a bare git repo, or chezmoi

Chezmoi:
KEY:
git is used for the storing of repos, chezmoi is for handling the symlinks in your system.

alternatively, just create a directory that houses your dotfiles, and manage the symlinking yourself.

```bash
mkdir ~/dotfiles
cd ~/dotfiles
git init
```
creates the repo, where inside you will store all of your dotfiles
(zshrc, gitconfig, nvim/, tmux.conf, etc)

KEY: now, move your dot files into the repo and symlink them back into place:

mv ~/.zshrc ~/dotfiles/zshrc
ln -s ~/dotfiles/zshrc ~/.zshrc //This way, its back to normal!
(note: when a process asks for ~/.zshrc, the OS transparently redirects it to ~/dotfiles/zshrc)

Then, push that stuff to github with all the files, and then just symlink those files back
where they belong.

git clone git@github.com:YOU/dotfiles.git ~/dotfiles

and
ln -s ~/dotfiles/zshrc ~/.zshrc
(NOTE: symlink replaces the path entry, so the zshrc will be overridden as the symlink)
(will either error, saying file exists, or it will override.)



KEY NOTE: TRAILING SLASH

~/.ssh vs ~/.ssh/ ARE NOT THE SAME THING

~/.ssh means the thing named ssh. it could be a directory or a file (directory is a type of file)

~/.ssh/ on the other hand, means the directory ~/.ssh and something inside it. (forces it to be a directory)

SO:
ln -s ~/dotfiles/ssh ~/.ssh/

failed bc its interpreted as create a symlink inside of the ~/.ssh directory, which
does not exist.
(here, the second term, the symlink name, cannot be a directory, it must be the directory
file, hense not needing the /)

~/.ssh/ssh -> ~/dotfiles/ssh (this is what it is trying)

no trailing slash  = operate on / create this path itself
trailing slash     = treat this path as a directory, usually operate inside it

NOTE: for .gitignore files however, you want the / so its clear that you ignore the directory, not just the file

ln 
-s for symlink

-l = long listing format
Shows permissions, link count, owner, group, size, timestamp, and filename.

-i = inode number
Shows the inode number at the start of each line.

avoid hardlinks when working on teh cloud:

**NOTES** ICLOUD IS A FILE-SYNC SERVICE, NOT A SHARED FILESYSTEM
iCloud Drive is basically a folder on each Mac + Apple’s cloud copy + a sync daemon.

Think of it as:
MacBook local disk
  ~/Library/Mobile Documents/com~apple~CloudDocs/
        ⇅ syncs
Apple iCloud servers
        ⇅ syncs
Mac mini local disk
  ~/Library/Mobile Documents/com~apple~CloudDocs/

**CRUTIALLY** Each Mac has its own local copy, icloud watches for changes, uploads
them, and then downloads the changed version onto your other devices

It FEELS like a shared filesystem but isnt!

THATS WHY YOU CANNOT USE HARDLINKS! The APFS filesystem doesnt share a common
inode table, so just because they have the same number doesnt mean anything!
That is because it, along with other text editors can do atomic-save style edits, where
it creates a new file, and then replaces the old file with the new one, thereby making it 
have a new inode entry

Symlinks on the other hand, have their own inode, and the contents of that inode is just the string path

can use github and sync over that or can treat them as just documents, and sync over icloud, but git + icloud is when things get messed up bc icloud sync is random, and can
mess up the git version of that data, plus git has version control, which is nicer

AS FOR USING GITHUB AND ICLOUD AT THE SAME TIME: regarding local files, like the .py
and .mds not an issue. Problem arises
