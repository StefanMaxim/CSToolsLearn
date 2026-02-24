# Intro to changing environments + vars and stuff


## Changing your Java Version:

1: install the correct version using homebrew



2: set java home path variable:

add to zshrc:
export JAVA_HOME=$(/usr/libexec/java_home -v 14)
export PATH="$JAVA_HOME/bin:$PATH"


