## Packages


What is a Package:

A package (pkg on mac, .deb on linux) is a way to bundle software in a way that it is 
easy to handle.

A software package usually contains:

The program itself (executables)
Libraries it depends on
Configuration files
Metadata (name, version, author)
Instructions for installation/removal

exe:

Package: Firefox
Version: 121.0
Contains:
  /usr/bin/firefox
  /usr/lib/firefox/*
  dependencies: gtk, nss, etc.

NOTES ON PACKAGE MANAGERS:

when you type: sudo apt install curl

curl, in this case, is a package, specifically a .deb for debian systems:
This is just an archive file, consisting of:
binaries, libraries, headers, install scripts, configs, docs

HOWEVER: install script is NOT for moving the packages to their respective spots in memory:
inside package, files are named with their indended location relative to the package:
exe:
user/bin/curl
usr/lib/libcurl.so
/usr/share/docs/curldocs

when installing, package manager just takes those files, and maps then to the root directory,
instead of the package:
usr/bin/curl -> /usr/bin/curl etc

BUT HOW DOES IT KNOW HOW TO DELETE PACKAGES:
inside, in var/lib/dpkg/info/curl.list
(apt calls dpkg under the hood)

What this does it it stores a list of all the files associated with the curl command:
/usr/bin/curl
/usr/share/docs/curldocs
/usr/share/man/man1/curl1.1.gz

and when uninstalling, it just goes through the list and removes all the files associated with the package in question




## How to make a package

Given some source looking like this:
mypackage.deb (just a zip-like archive)

Inside:
├── DEBIAN/
│   ├── control
│   ├── postinst   (optional script)
│   ├── prerm      (optional script)
│
├── usr/
│   ├── bin/
│   │   └── myapp
│   ├── lib/
│   │   └── libmylib.so
│   ├── include/
│   │   └── mylib.h
│   └── share/doc/mypackage/
│       └── README

Key Idea:
/usr stores the files it will install on your system, positioned the way they 
ought to be for the mapping. Handled by var/lib/dpkg/info/curl.list:

/.
/usr
/usr/bin
/usr/bin/curl
/usr/share
/usr/share/doc
/usr/share/doc/curl
/usr/share/doc/curl/README.Debian
/usr/share/doc/curl/copyright
/usr/share/man
/usr/share/man/man1
/usr/share/man/man1/curl.1.gz
/usr/share/zsh
/usr/share/zsh/vendor-completions
/usr/share/zsh/vendor-completions/_curl
/usr/share/doc/curl/changelog.Debian.gz

when you sudo apt install curl, apt is roughly:
1: find package metadata
2: resolve dependencies
3: download curl_..._amd64.deb
4: ask dpkg to unpack/configure it
5: record installed files in /var/lib/dpkg/info/curl.list

Metadata: metadata that tells dpkg the package name, version, dependencies, scripts, checksums, and so on

dpkg-deb is the underlying tool that builds and inspects binary debian archive files,
whereas dpkg is what installs and manages them

dpkg-deb --build is used to build them.

GENERALLY, HOWEVER, YOU DO NOT MANUALLY BUILD THEM, INSTEAD RELYING ON 
dpkg-buildpackage
debian/rules
debhelper / dh
fakeroot
lintian

with dpkg-buildpackage being the gold standard. If your source tree contains a 
debian packaging file, /Debian. debian-buildpackage will orchestrate the entire build process.

Firstly, it will clean up any files from previous builds, done by invoking debian/rules clean, where debian/rules is a package-specific build script (usually implemented via make)

Next, it compiles the source into binary files and library files, depending on which is 
which

Then, after compilation, the binary debian archive is bundled
Then, generates .changes metadata file, describing what was built
(package names
version numbers
architecture(s)
checksums of generated files
maintainer information)
useful for uploading changes to debian repository, to know which artifacts belong to each
upload. (ARTIFACT = OUTPUT OF THE BUILD PROCESS)

Lastly, the .buildinfo records information about the build environment, containing 
things like:
package versions used during the build
architecture
build dependencies
build path details

### KEY DISTINCTION: SOURCE PACKAGE vs BINARY PACKAGE

Source package:
  curl_8.x.y.orig.tar.xz (compressed archives)
  curl_8.x.y-1.debian.tar.xz
  curl_8.x.y-1.dsc

Binary package:
  curl_8.x.y-1_amd64.deb

Source package = what maintainers work on
Binary Package = what users install



Now, back to teh backage, suppose Curl looks like this:

~/build/
└── curl-8.8.0/
    ├── CHANGES
    ├── COPYING
    ├── README.md
    ├── configure
    ├── Makefile.am
    ├── Makefile.in
    ├── docs/
    │   ├── curl.1
    │   └── ...
    ├── include/
    │   └── curl/
    ├── lib/
    │   └── ...
    ├── scripts/
    │   └── completion.pl
    ├── src/
    │   ├── tool_main.c
    │   └── ...
    └── tests/
        └── ...

At this points, its just source code, but adding a /debian directory debianizes the sources
curl-8.8.0/
├── CHANGES
├── COPYING
├── README.md
├── configure
├── docs/
├── include/
├── lib/
├── scripts/
├── src/
├── tests/
└── debian/
    ├── changelog
    ├── control
    ├── copyright
    ├── rules
    ├── source/
    │   └── format
    ├── curl.install
    ├── curl.docs
    ├── curl.manpages
    ├── README.Debian
    └── patches/
        ├── series
        └── ...

Most important ones there are:
1: control
2: changelog
3: copywrite
4: rules (the debian/rules clean we spoke about) (likely a makefile, so you are really running make clean)

