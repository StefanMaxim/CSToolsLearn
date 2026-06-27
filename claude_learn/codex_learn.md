## How to install:

curl -fsSL https://chatgpt.com/codex/install.sh | sh (get the install script)

NOTE: to update, run the installer file again, specifically you curl again and run the install script

from there, just run codex to get started

(can also just install via homebrew if on mac)

**ALTERNATIVE, MAYBE EASIER**
use npm, assuming you have node.js

best way to get, install nvm (node version manager)

This is the link on their github:
https://github.com/nvm-sh/nvm/blob/master/install.sh

however, the raw link (used for actually obtaining the file) is:
https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.7/install.sh

(the blob is the html page, which we do not want for curl)
just remove blob and change the url 

**KEY** github has a button, RAW, which gives you the raw URL for the file


now:

curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.7/install.sh | bash

KEY:

which nvm does not work bc its a shell function, NOT an executable
to test it, run

nvm --version

nvm install --lts
(install under ~/.nvm/)

then, activate it: nvm use --lts

IMPORTANT:

nvm alias default lts/*

(this way, you dont have to keep typing nvm use --lts)
(now every terminal will by default use node)

nvm install node
nvm use node (use only if you want the latest version, not the lts)

npm install -g npm@11.17.0 to update npm

npm install -g @openai/codex (for the install)




KEY:

1. Use a project-level shell wrapper (BEST + simplest)

Create a script that forces the environment:



# Claude Install
curl -fsSL https://claude.ai/install.sh | bash
(should handle the rest)




# run_in_env.sh
#!/bin/bash
source ~/miniconda3/etc/profile.d/conda.sh
conda activate myenv
exec "$@"

Then run Claude through it:

./run_in_env.sh claude

Now every command Claude executes inherits:

activated conda env
correct python
correct pip installs