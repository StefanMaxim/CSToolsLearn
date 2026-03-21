# Intro to Homebrew:

Installation:

/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

you are taking the path to your bash executable shell, then via -c you are running a command there, 
and passing the environment variable, to curl -fsSL, or fail silent, silent more, show errors, and follow redirects.

basically, this gets the file as requested from the url, and then runs the install.sh using /bin/bash on mac

IDEA:
instead of manually hunting down installers, extracting tarballs, and changing your path, this does it for you
It installs software in its own directories, and symlinks it to the Homebrew prefix (recall: symlink = still an inode, but the contents are just the path to another place in your directory)

default prefix: opt/homebrew

its essentially a directory tree. it stores each version of a package under its own location, the Cellar.
and then links the ones you actually want to use in /bin /sbin and /share


Formulae: command like packages and libraries:
wget, python, git, etc...

Casks: larger MacOS applications (essentially executables + relate files bundled into .app extensions)
google-chrome, visual-studio-code, docker, etc



Vocab Words:

homebrew, when you install something says "give me a tool + everything it needs to work on my computer"

package = bundle of software prepared so you can easily install it (wget, python, etc)
homebrew calls them formulae

library = re-usable code that other programs use (instead of re-writing it, programs "borrow" it from library)









** CRUTIAl **
brew install <name> for formulae
brew install --cask <name> for casks


MUST-KNOW COMMANDS:

brew --version  (tells you the version of brew you are using)
brew help       (used for getting help)
brew search <name>      (searches brew prefix for the package in question)
brew info <name>        (gives info about the formulae installed)
brew install <formula>      (installs formulae)
brew install --cask <cask>  (installs cask, or basically an apple .app)
brew list                   (lists all formulae and casks)
brew uninstall <name>       (uninstalls cask/formulae)
brew update                 (updates homebrews knowledge about available packages)
brew upgrade                (upgrades what you have already installed) (brew upgrade gcc)
brew outdated               (show what can be updates)
brew cleanup                (removes old unnesesary versions and files)
brew doctor                 (checks setup for common problems)

NOTE: homebrew tries hard to not rely on sudo, so if you keep using it, you are doing something wrong



BOTTLES VS BUILDING FROM SOURCE:

bottle = pre-built binary package (pre-build meaning pre-compiles, so its easy to install and get straight to using)
if no bottle available, then must build from source, which takes longer and may need more dependencies


Terms 2:
prefix: again, its the root directory for homebrew, namely /opt/homebrew/
Cellar: the place where all versions live of a formulae/cask
bin: place for binary executables, symlinked executables from the Cellar
tap: an additional package/repository that homebrew knows about (CRUTIAL)
Essentially, it is an extra package source, like a github repo

to add extra repositories for sourcing packages, use:
brew tap /user/repo
brew install /user/repo/google-chrome
brew tap lists all taps
brew untap /homebrew/cask

NOTE: this is shorthand for: https://github.com/homebrew/homebrew-cask
under the scene, homebrew automatically:
1: clones the repo
2: adds it to /opt/homebrew/Library/Taps/homebrew/homebrew-cask or something as a library

same shorthand for others, so brew install --cask google-chrome
is really homebrew/cask/google-chrome in its CORE repository, where the base stuff are installed.

