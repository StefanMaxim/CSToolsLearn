# Intro to Homebrew:

Installation:

/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

you are taking the path to your bash executable shell, then via -c you are running a command there, 
and passing the environment variable, to curl -fsSL, or fail silent, silent more, show errors, and follow redirects.

basically, this gets the file as requested from the url, and then runs the install.sh using /bin/bash on mac

IDEA:
instead of manually hunting down installers, extracting tarballs, and changing your path, this does it for you
It installs software in its own directories, and symlinks it to the Homebrew prefix (recall: symlink = still an inode, but the contents are just the path to another place in your directory)
**KEY** CELLAR IS NOT THE SAME AS THE PREFIX!!!! /opt/homebrew vs /opt/homebrew/Cellar/rack/keg

default prefix: opt/homebrew

its essentially a directory tree. it stores each version of a package under its own location, the Cellar.
and then links the ones you actually want to use in /bin /sbin and /share


Formulae: command like packages and libraries:
wget, python, git, etc...

Casks: larger MacOS applications (essentially executables + relate files bundled into .app extensions)
google-chrome, visual-studio-code, docker, etc

FORMULA = homebrew package built from upstream source (you build it on install from source)
CASK = homebrew package that installs pre-compiled binaries built and signed upstream



Vocab Words:

homebrew, when you install something says "give me a tool + everything it needs to work on my computer"

package = bundle of software prepared so you can easily install it (wget, python, etc)
homebrew calls them formulae

library = re-usable code that other programs use (instead of re-writing it, programs "borrow" it from library)

TERMINOLOGY
       formula
              Homebrew package definition that builds from upstream sources

       cask   Homebrew package definition that installs pre-compiled binaries built
              and signed by upstream

       prefix path in which Homebrew is installed, e.g. /opt/homebrew or
              /home/linuxbrew/.linuxbrew

       keg    installation destination directory of a given formula version, e.g.
              /opt/homebrew/Cellar/foo/0.1

       rack   directory containing one or more versioned kegs, e.g.
              /opt/homebrew/Cellar/foo

       keg-only
              a formula is keg-only if it is not symlinked into Homebrew’s prefix

       opt prefix
              a symlink to the active version of a keg, e.g. /opt/homebrew/opt/foo

       Cellar directory containing one or more named racks, e.g.
              /opt/homebrew/Cellar

       Caskroom
              directory containing one or more named casks, e.g.
              /opt/homebrew/Caskroom

       external command
              brew subcommand defined outside of the Homebrew/brew GitHub repository

       tap    directory (and usually Git repository) of formulae, casks and/or
              external commands

       bottle pre-built keg poured into a rack of the Cellar instead of building
              from upstream sources








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

--verbose --debug to help figure things out

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




LINUX COMPARISON:

whereas linux uses both apt and dpkg for the role of package managing 
(dpkg for creating and installing packages, and the underlying took for keeping track of them) and apt for the sharing, managing, and handling of dependencies, brew does all of that, using the /opt/homebrew/Cellar as the place where they are installed, and then linking them where they belong.

**KEY** WHAT GOES IN YOUR ZSHRC?
zshrc is a startup script that runs whenever an interactive zsh shell starts.
common things to put
1: env vars
export EDITOR=nvim
export JAVA_HOME=/opt/homebrew/opt/openjdk (recall how to switch java versions)

2: alias's
alias ll='ll -lah'
alias python='python3'

3: functions (more powerful alias's)
mkcd() {
    mkdir -p "$1"
    cd "$1" //note, teh $1 means the first argument passed to the executable.
}

4: Homebrew setup
eval "$(/opt/homebrew/bin/brew shellenv)"
this runs the shell command brew shellenv, which prints out shell commands like
export HOMEBREW_PREFIX='/opt/homebrew' etc...

$() is called command substitution, which then camptues the output, and it becomes
a string containing the export commands, then passed to eval

RECALL:

/usr/libexec/java_home -V //displays all available java versions

export JAVA_HOME=$(/usr/libexec/java_home -v 17) //exports a new version of java, which
is installed via brew btw
export PATH="$JAVA_HOME/bin:$PATH" //doing this changes the path env var, making it mermanent (if written in ~/.zshrc)

echo 'export PATH="$JAVA_HOME/bin:$PATH"' >> ~/.zshrc

have to re-add homebrew to the path