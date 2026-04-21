# Introduction to Maven:

Loose overview: Maven is basically just Makefile, but higher level:
For makefiles: you tell it how to do EVERYTHING, you define the builds and how it works atomically
For Maven, its more high level: it will build your project in the standard way

More specifically, it
- standardizes your project's structure
- downloads libraries
- compiles your code
- runs your tests
- packages the results

ALL FROM A SINGLE CONFIG FILE: "pom.xml" (Project Object Model)

## Getting Started:
go to maven wiki:
first go to guide: https://maven.apache.org/users/index.html
Order:
download maven, maven in 5 minutes, getting started tutorial
(then, POM manual if you want)

DOWNLOADING:

4 options:
Binary tar.gz archive: 
Binary zip archive:
Source tar.gz archive:
Source zip archive:

Binary = pre-compiles to machine code, ready to run (dll, so, exe)
(dll = Dynamically Linked Library, used by windows, loaded by exe at runtime) (used PE Portable Executable format)
(so = linux shared library, dll but for linux. loaded dynamically by linker (USED ELF Executable Linkable Format))
(exe = executable program, runnable application, main() or WinMain() entrypoint)

(for macos:
dylib = Dynamic Library, exe: libc++.dylib
Macos uses frameworks:
contains dylib, headers, and resources (images, metadata)
Cocoa.framework

Executables use Mach-O format, and have no extension
)

TABLE:

Executables: .exe, ELF binary, Mach-O
Dynamic Library: .dll, .so, .dylib
Enhanced Library: framework (mac only)
Static Library: .lib, .a, .a


Source = you will have to compile yourself (comes with source .c .cpp .java, good when no binary for your system)



tar.gz = compression format:
two step bundle: tar = bundles together, gzip = compresses (common in linux/macos)

tar -xvzf file.tar.gz

zip files does both (common on windows)
unzip file.zip



## Link, Checksums, Signatures:
Link is just the hyperlink that will query the server via REST API and then obtain as result the file,
kindof like curl or wget

Checksum is a way to see file integrity. It basically turns a file into a number, which changes based on file contents
same checksum = installed correctly

ON MAC: shasum executable:
shasum -h for help
-a -algorithm for algoriths (1,224,256,384, etc)
-b read in binary mode
-c reads SHA sums from FILEs and check them

shasum -a 512224 -c checksumfile
(when checking, input should be former output of the program)
HOW DOES THIS WORk:
the checksum file already contains:
<expected_hash>  <filename>
exe: 3b5d5c3712955042212316173ccf37be800b3f4e...  myfile.iso

can contain multiple lines too, each for a different file to check

019283012830918230 file1.txt
109287310928301928 file2.txt
029834098081092834 file3.txt 

checksum will go to that file, compute its checksum, and compare with the provided checksum

2cf24dba5fb0a030eae...e5c1fa7425e73043362938b9824  hello.txt
486ea46224d1bb4fb680f34f7c9bbd... (truncated) ...8c3f6d  world.txt (NOTE: 2 SPACES BETWEEN HASH AND FILENAME)

MAKING A CHECKSUM:

echo -n "hello" > hello.txt //creates a file

shasum -a 256 hello.txt > SHA256SUMS (its literally just the output redirected into a text file)

shasum -a 256 -c SHA256SUMS //this is used to verity


On Linux:
sha256sum is standard
sha256sum file.txt
sha256sum -c checksums.txt


### NOTE ON SECURITY:
checksums, and even digital signatures are just txt files, not executables
thus, they cannot do anything malicious by themselves.

"BUT I THOUGHT IF I CLICK ON A BAD LINK, THAT CAN GIVE ME A VIRUS":
no, or at least thats very rare.
Only if there are some browser or plugin vulnerabilities, but thats highly unlikely on google chrome or soemthign
USUALLY, THEY TRICK YOU TO DOWNLOAD SOMETHING (update required, or some other text)
AND then you run the file, then you can get a virus

EVEN JUST DOWNLOADING THE FILE IS OKAY:
it will just remain as bytes on the computer, and not execute anything unless you double-click

for the tar.gz or zip files:
they are just a compressed archive file.
Download = SAFE
Have on system = SAFE

WHAT ABOUT EXTRACTING IT:
tar -xzf file.tar.gz (x is extract, z is zip, f is file)
STILL OKAY, just extracting, not running
TIPS:
1. list files before executing
tar -tzf file.tar.gz //lists whats going on

2. check the filetype of the files
file filename.txt //can be sh secretly

but if you just sha256sum -c checksum.txt, it wont execute it, bc it treats it as data in the process






## NOW SIGNATURES:

a signature is proof that
1. the file hasn't been altered from what it should be 
2. it was made by who you truely believe the publisher to be

(its basically an encrypted CHECKSUM that ensures the checksum is the correct one made by the publisher)


### CORE IDEA
1. The publisher takes the file (say app.zip) and runs it through a hash function like SHA-256 → this produces a fixed “fingerprint.”

2. They then encrypt that fingerprint with their private key using Public-Key Cryptography → this becomes the digital signature (often a .sig or .asc file).

3. They publish:
    the file (app.zip)
    the signature (app.zip.sig)
    their public key (developer-public-key.asc)
    
ON THE CLIENT SIDE:
1. Download the file
2. Compute the HASH yourself of the file.
3. Decrypt their signature using their public key, and find the HASH inside

COMMON TOOL:
GNU Privacy Guard (gnupg)


Step 1: get the public key

gpg --import developer-public-key.asc // imports their public key

gpg --keyserver hkps://keys.openpgp.org --recv-keys ABC12345
(hkps is the secure HTTPS Key Server, then after that is the keyserver domain name)
(recv-keys downlaods the key into the keyring locally)
(ABCD12345 is the KEY ID, used to identify which key to install)
results downloaded into ~/.gnupg/


Step 2: compare key with version on dev's official cite

gpg --fingerprint ABC12345 (this will print out they key, based on its shortened fingerprint)
this commands visually outputs the full public key/fingerprint, which you can check with their website
(all keys have:
1: Full Public Key
2: Short fingerprint
3: metadata
)

Step 3: verify
gpg --verify app.zip.sig app.zip (sig contains the files encrypted using private key + figerprint/HASH)


## Back to the install:
so we have the binary.source, and zip/tar.gz files, but which do i download?
(NOTE: BINARY != machine code, it rather means .jar files, which are java bytecode + some shell scripts)

NOTE: on their website, they use the HASH only files, not hash and filename.
thus, cannot compare it using the -c flag bc not correct style of file

instead, use regular comparison and then diff the two
alternative: take the fiel with only the hex, and append "  fielname.txt" with 2 spaces, to match format
(note: CANNOT USE >> as that creates a new line, which doesnt work here)

This command does the append right after where you expect
sed -i 's/$/  apache-maven-3.x.x-bin.tar.gz/' file.sha512 (sed used for search/replace, -i means overrid file, )
this also works:
echo "$(cat file.sha512)  apache-maven-3.x.x-bin.tar.gz" | sha512sum -c -
can also just use vim to append that stuff

now, for the signature:

the signature looks like:
-----BEGIN PGP SIGNATURE-----

iQJnBAABCgBRFiEEhHidJN93oyQzzh8HnrgOkushNbEFAmndWwUbFIAAAAAABAAO
bWFudTIsMi41KzEuMTIsMCwzFxxzamFyYW5vd3NraUBhcGFjaGUub3JnAAoJEJ64
DpLrITWxFj8P/25tQF//z+dl3E1V5iFqXQddH0i9GBBOXT44vHMZjPjMTLHv0KGh
hNMmf8+ITBAljOmBp39TUVT6457jagk5JY548rC1V8vZCgnUO02NbyrbGiQXcOfw
05IYz/DOr5V0W51uxaDdB4JFLkbE206d4ckiMYzMZVg0pyua2aiRxNgFa2PoN3iY
JdLd7RsxclSbQtGjKpmk3aylVv2VIOBUFobYnDq21jZLZwR1JG1ZgVjx4vl+Rr6Y
zfBG9slBD8JEYSBKxMlgQPQuTRV19J/jdzZrBBqFkASMp115iEp8eypVnKXmKHBv
jsAl6YYnYO3sDv9kb0Q1QiWHWUdwJqK8+FM3yqVAEJkx4QANNexOHXmdpsxXZPMX
/DxNgnoJ17kI8YfadoVjwkdzDGsxTsblxR2TFEhyqHhJYFeXZeO+eugNFAk/wmo6
fvjGCIC/x1ohTIf8QGs/2qGMfYbSgYe+CGRaFrSkhYQQU8W1m+lQB0oC1z+YroaA
oS7JjjBeEGAvuFASopSHHmKldkNVwF6KfDAzae4EMzAr1966sO04EWBomEWrul0L
lHhdv0ra7ncBlGyA+f3bG4HA1a/oN3rnKaPYyo9t3IflUTDyZaRsgvxMDrP/8+1J
07QgCloEa6SWeOLv05PLJhPWHrxqfJlCFcXKWyiVpsc1wSfKbo5JACTp
=hzzp
-----END PGP SIGNATURE-----

It uses PGP, or Pretty Good Privacy for its encryption.
NOTE: this is NOT the public key! that you will have to get from their website.
this is the encrypted signature.




## Once verified, move file to standard location

/opt/maven (good, but need sudo)
/usr/local

THEN, TELL SYSTEM WHERE IT IS
export M2_HOME=/opt/maven
export PATH=$M2_HOME/bin:$PATH (name not important, will check the entire path to find itself)

Now, maven is installed

**NOTE**
can edit .zshrc in vim to fix up the path and make it more clean

MAVEN_HOME=
then add MAVEN_HOME to PATH
