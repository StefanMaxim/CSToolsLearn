# How to install Java:

## What is Java:
Java is a high-level OOP language, with motto
> "Write once, run anywhere"

this motto is implemented using compilation to bytecode, which means that javacode gets compiled
to some intermediary representation (NOT MACHINE CODE) that is then later run by all systems using the JVM

## Compilation pipeline:

STAGE 1:
Begin with some java code

```java
public class Main{
    public static void main(String[] args) {
        System.out.println("Hello World");
    }
}
```

STAGE 2:
Then, code will get compiled into bytecode:

executable javac turns it into javaclass objects, .class,
which contain BYTECODE, not MACHINE CODE

STAGE 3:
Lastly, code is run on the platform via the JVM
It 
- either interprets the code or JIT-compiles it into native machine code
- handles memory garbage collection (once object is no longer used, not need to free or allocate the memory)
- provides security + sandboxing


## Key Components:

JVM (Java Virtual Machine, aka The Runtime Engine)
- Executes Bytecode
- Platform Speciric

JRE (Java Runtime Environment)
- JVM + standard libraries (java.lang, containing Object and String classes, java.util ArrayLists, java.io for Scanners, java.net for networking stuff like Sockets, java.time, java.math, etc)
- enough to run java programs
- No Compliler

JDK (Java Development Kit)
JRE + tools like:
- javac
- java
- jdb
- jar (Java Compiler Archive, like tar but for java, in that contains multiple .class objects, resources like images, and metadata, all used for the distribution + deployment of java applications)
jar cf app.jar *.class // literally copies tar format!
**NOTE** Maven Artifacts are likely .jar files!

You want this one, ie the JDK because ofc we want the compiler!


## JDK choices
Most modern JDKs are based on OpenJDK
reccomended: Eclipse Temurin
second choice: amazon correto
Oracle JDK: good but paid for some uses


## INstallation Steps:
Normally, when installing Java, you want to install a JDK.
Its not like C, where you only install a compiler and your good.
Java, while it is a compiled language, needs a runtime, as it compiles to java bytecode
and needs the VM to run on hardware.

On mac, just use Homebrew (see Homebrew_learn)

> brew install openjdk@21 (can also use openjdk@17)

Step 2: be sure to link it:

sudo ln -sfn /opt/homebrew/opt/openjdk/libexec/openjdk.jdk /Library/Java/JavaVirtualMachines/openjdk.jdk

this creates a symbolic linc between the homebrew directory and one in /Library/Java/, as that is expected by macOS
this way, tools in /usr/libexec/java_home work

Step 3:
be sure make it visible to path

export JAVA_HOME=$(/usr/libexec/java_home)
export PATH=$JAVA_HOME/bin:$PATH

Step 4:
verify
java -version
javac -version



### INstallation Debian-Based

sudo apt update

sudo apt install openjdk-21-jdk

optional: 
set java home
export JAVA_HOME=/usr/lib/jvm/java-21-openjdk-amd64
export PATH=$JAVA_HOME/bin:$PATH



## Managing multiple JDK versions:
/usr/libexec/java_home -V //this will be used, which is a Macos utility that prints out the one with 21 in it

export JAVA_HOME=$(/usr/libexec/java_home -v 21) 

NOTE:
can have multiple JDKs installed: Temurin 17, 21, Oracle JDK, etc
living in different folders: /Library/Java/JavaVirtualMachines/

but on mac, the one which is active is the one in 
/usr/libexec/java_home
this tool:
- list available jdks
- lets you pick versions
- helps set JAVA_HOME

using:

/usr/libexec/java_home -V, it will list your available versions of java (THIS IS AN EXECUTABLE!)

to switch:
export JAVA_HOME=$(/usr/libexec/java_home -v 17)
export PATH=$JAVA_HOME/bin:$PATH











on LINUX:

sudo update-alternatives --config java //used to switch which version of java you are using
There are 3 choices for the alternative java:

  Selection    Path
-----------------------------------------------
x 1           /usr/lib/jvm/java-17-openjdk/bin/java
  2           /usr/lib/jvm/java-21-openjdk/bin/java
  3           /usr/lib/jvm/java-8-openjdk/bin/java

Press <enter> to keep current choice[x], or type selection number:



sudo apt update
sudo apt install -y wget gpg //gnu privacy guard, see maven_installation.md for more info

wget -qO- https://packages.adoptium.net/artifactory/api/gpg/key/public | sudo gpg --dearmor -o /usr/share/keyrings/adoptium.gpg

//add the adoptium public key to your gpg

echo "deb [signed-by=/usr/share/keyrings/adoptium.gpg] https://packages.adoptium.net/artifactory/deb $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/adoptium.list

sudo apt update
sudo apt install temurin-21-jdk








