# Github
so you knwo how to use git, but what about github:

## What is Github

A shared remote + access control + workflow engine ontop of git

git = the tool for handling the content-adressed DAG of commits

Github:
1) hosts the remote (essentially makes it visible for all on the internet rather that the standard local remote)
2) enforces permissions (some people can only read, others can write. On git, a normal remote via git init --bare
any computer user can do whatever with it. Now, only certain ppl can write to a repo)
3) adds review, discussion, and automation layers

MODEL:

you -> git commit -> git push (to a local remote)

you -> fork/branch -> push -> PR -> review -> merge


## Core workflows of contribution

IF YOU HAVE WRITE-ACCESS:
you push branches to the main repo, and then via a pull request, request that they pull your changes into main (via merge)

IF YOU HAVE READ-ACCESS ONLY:
First fork their repo (like a clone, but you now have full control, different origin), and then make a PR
from the fork to the repo.

**NOTE**: when you fork a repo, it does not keep an origin to their account. THE FORKED REPO ITSELF IS THE "ORIGIN"
when you git clone, it automatically makes itself the origin.

(if you want to keep track of the origional repo, use ```git remote add upstream <url of og repo>```)
then:

```bash
git fetch upstream
git merge upstream/main (merges your currently checked out branch with upstream/main)
```













# Programming Practices:
how to ppl just know to use src bin all taht stuff in a coding project
im looking at it and it looks like a bunch of stuff (use claude code directory as exe)