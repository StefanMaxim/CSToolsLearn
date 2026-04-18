# How does SSH Encryption Work:

each computer has public and private key

when you type:
```bash
ssh user@ipaddr
```

Step 1: Handshake starts 

note: ssh uses AES or ChaCha20, which are symetric encryption methods, which are more efficient for continuous data streams


Explanation: Diffie-Hellman key exchange

(goal: two computers create the same secret without ever sharing it)

Mechanics:
publicly, both sides agree on a large prime p, and a base g (not secret)

client:
picks a secret number, a
computes A = g to the a mod p
sends A to server

Server:
server picks a secret number b, computes
B = g to the b mod p, sends b to the client

Magic:
client computes shared = B to teh a mod p

server computes A to teh b mod p

both get g to the ab mod p (the shares secret code)

(attackers can see g,p,A,B, but to get the code they must solve A = g to the a mod p and solve for a, which is a discrete logarithm, which is
computationally infeasible for large numbers)






## phase 2: how do you knwo you are talking to the real server?
(using RSA)

Idea:
server has public key (n,e)
private key d

where 
n = p times q (2 large primes)
e times d = 1 mod (phi n) (eulers totient function, means the number of integers from 1 to n that are coprime with n)

How signing works: server proves identity by signing something else:
signature = m to the d mod n

cleint verifies m = (signature) ot the e mod n (if matches, then it is telling the truth for its public key, and only real server made it)

why: m to the d to the e =  m to teh de = m mod n



## Part 3:
symmetric crypto:

after the key exchange, both sides share a sercret key
now switch to fast encryption like AES or ChaCha20 (stream ciphers)

plaintext xor keystream = cyphertext

cypertext xor keystream = plaintext



## all together:
1: key exchange via diffie-Hellman -> shared secret g to teh ab
2: authentication, servers handshake via RSA/Ed25519
3: key derivation, shared secret -> session keys via hashing
4: secure communication: data encrypted using symmetric cipher (AES ChaCha20), integrid checked with MAC or AEAD

(Review notes on number theory from discrete math for this stuff)





# XZ

way to compress data on linux

when compressed and then de-compressed, must come back exactly the same

EXE: take some string, "Never"
represent as a stream of characters: "004E 0065 0076 0065 0072"
this works, but its inefficient.
for larger strings, some characters will appear multiple times, like n appears 114 and o 320 or something

what if give the more frequent letters shorter codes, and rarer ones can affort to be long:

solution: list all the characters in order of how often they show up

(e,415), (a,312), (o,224), (p,212), ... (z, 15) ,(;,12)

from here, take the 2 least frequent items and combine them into a pair whose frequency is the sum of the two that is represents, and insert it back in to the tree

exe:
(a,4), (b,3), (c,2), (d,1) //here, pair c and d together, forming a pair with frequencey of their sum

((c,2)(d,1),3) // where
list is now (a,4), (b,3), (cd,3)
keep repeating and inserting back into list. eventually will create a tree datastructure, where it has letters as leafs and
the combines letter's frequenceies as nodes. this is called a Huffman Tree

to get codes, step the tree:
step right = 1
step left = 0

this way, more commonly appearing symbols are naturally at the top of the tree.
less common is at the bottom
(CHARACTER ENCODING)

## stage 2

Problem:
this just looks at symbol frequency, not actual work frequency
exe: the work "the" comes up a lot, but its treated piecewise, whereas it might be more efficent if it had context of the words

what if instead looked at chunks:

exe:

"never gonna give you up never gonna let you down"

to start, keep a rolling dictionary of what you have written so far, however when you get to a chunk that repeats from what you preivously
read, instead use 2 numbers, one indicating how far back to look and how many characters to copy

Never gonna give you up <24,12>let<23,5>down<25,13> (then, when de-compressing, just loop back and copy the chunks back into place)
(algorithm called LZ77)

(DICTIONARY BASED ENCODING)
whats before youve encoded= dictionary
whats after = what you still have to encode
words by repeatedly finding the largest repeat from what came first with what you will have to put in later


more precise:
given: abcabcabc

first:
step 1:
seen = ""
remaining "adcabcabc"
no matches yet, so outputs literal (0,0,'a') //a is the next char after the match

step 2:
seen = "a"
remaining "bcabcabc"
no matches, another literal (0,0,'b')

step 3:
seen = "ab"
remaining = "cabcabc"
no matches, lit (0,0,'c')

step 4:
seen = 'abc'
remaining = 'abcabc"
matches, so (3,3,'a') //only a bc its the next character after the match NOTE: technically for all the others, its still the next char after the match!

step 4:
seen: "abcabc"
next: bc
match! (2,2,NULL) //NULL bc that the next character after the match!

in teh end:

(0,0,a), (0,0,b), (0,0,c), (3,3,a), (2,2,NULL) this series of triples can be decoded to work for any string!

WHY (offset, length, char)
offset = used to see how far to go back
length = used to see how much to copy
char = used so worst case can copy one character

### Confusion
> **NOTE** MAJOR CONFUSION:
the triple standard is just for teaching, and it the way it was proposed as the algorithm, but isnt how its implemented:

in reality, the LZ77 used in things like DEFLATE writes it in 2 different formats:
THERE ARE 3 PARTS HERE:
1: LZ77 used to find patterns
2: DEFLATE turns the patterns into symbols
3: Huffman turns symbols into bytes

exe: given abcabca
after lz, becomes:
abc(3,3)a




In decoding: doesnt store the seen and next as strings, just processes the triples incrementally
it uses the output so far as its dictionary, used for seeing what is meant by the first 2 element in the triple

In decoding: doesnt need separate string, just used the input string, but partitions it into 2 parts:
sliding window and the lookahead buffer
[sliding window | lookahead buffer], where the sliding window moves int that range and the lookahead buffer shrinks every time a new 
triple is created.
this is done via pointers / indicies

Step-by-step:

The encoder looks at the first character in lookahead, say 'a'
It searches the sliding window for matches of the sequence starting at 'a'
It compares character by character:
For each position i in sliding window:
    Compare window[i:i+length] with lookahead[0:length] //will incremement
If it finds the longest match, it outputs (offset, length, next_char)
Then it advances the sliding window by length + 1 (match plus next char)
Repeat for the new position










### BETTER EXPLANATION FOR LZ77 DICTIONARY BASED ENCODING
DEFLATE = LZ77 + Huffman Encoding

process overview:
input stream
LZ77 finds repeated substrings and references
symbol stream finds literals or length-distance pairs
Huffman Encoding: entropy compression ()
bitstream

GZIP = wrapper around DEFLATE:
adds header with metadata and footer with stuff like length

#### Phase 1 LZ77:
uses sliding window model
[already processes | lookahead buffer]
(lookahead = new bytes to encode, searches buffer/past to see what already encoded up to 32 kb)

It produces a sequence of:

1:  Literal Bytes (exe: 'A')
2:  Length-Distance pairs (length = 5, distance = 12) means copy 5 bytes from 12 bytes ago

EXE: ABCABCABC -> ABC(6,3)

> WAIT, Why (6,3)?
the way it works low-level:
if currently at i:
find largest match between:
1. A substring starting at i AND
2. A substring starting at j<i (CRUTIAL)

so, for ABCABCABC, now that we are at ABC(here,A)BCABC
largest starting at i is ABCABC, where j=0 (start of the buffer)
thus, j starting at 0, and then at i, length is 6, and start back is 3
NOTE: this is okay to do because the DECODER works one byte at a time:
so when it sees ABC(6,3), it starts 3 back, and then starts copying over 6, but by the time its done with the first ABC, there are another 3, 
and it just copies those over too!

> NOTES ON ENCODING:
max distance = 32kb
length range (3 to 258) (not worth encoding under 3)

#### Phase 2, Symbol Stream
started with: ABCABCABC
got: ABC(6,3)
CRUTIAL NOTE: recall, we are not encoding just characters, but bytes as a whole!

the actual input is just a bytestream, so 
ABCABCABC = 65 66 67 65 66 67 65 66 67, where the "numbers" are just the base 10 version of the binary they represent, exe 65 means 01101010 or something
because its in bytes, can range from 0 to 255 in decimal.
> NOTE: these #s ARE NOT ASCII NECESSARILY, ITS JUST BYTES, THAT CAN BE INTERPRETED AS ASCII IF YOU WANT
so, in reality its more like:

65 66 67 (6,3)
KEY: this output is NO LONGER raw bytes
its now literals and matches:
(literal 65), (literal 66), (literal 67), match(6,3) (NOTE: the literal and match keywords are abstract and implied here)
You basically have a sequence of tokens like this, at least in structure

>PROBLEM: Huffman needs a discrete alphabet (works symbol -> binary, so must turn these tokens into symbols)
tokens are 0-255 possible literals and arbitrary many (length, distance) pairs

(these "symbols" are abstract, not bit-level. just the idea that there will be some symbol representing the bytes and matches)
Literal/length encoding:
0–255	    literal bytes
256	        end-of-block
257–285	    length codes

Distance Encoding (different alphabet):
0-30        distance codes

From there:
literal(65) -> symbol 55

match(length,distance) -> length_code + length_extra_bits + distance_code + distance_extrabits
(why: because some distances are more common than others.)
(use the length/distance codes for the huffman, and the extra bits for fine adjustments)

>HUH?
recall:
length = 3 to 258
distance = 1 to 32,768

if you huffman directly, need 256 chars for legnth and 32k chars for distance (HUGE)
INSTEAD: group values into ranged (buckets)
each bucked has fixed base value and number of bits (THIS )










DEFLATE does not encode RAW bytes anymore, INSTEAD

1. Literal byts (0-255)
2. End-of-Block marker (256)
3. Length Codes (257-285)
4. Distance Codes (different alphabet)




















## Stage 3

problem: some of these symbols more common than others
they have their own frequencies.

what if you first use the LZ77 compression, then feed it into a huffman tree?
used in tool called deflate, know for the .zip files it creates.

## Stage 4:

huffman only used frequency of chunk repeating.
however, after each token, there is an inherent probability for another chunk showing up:

exe: compressing never gonna give you up

after never gonna, the next token can be:

"give you up"
"let you down"
"say goodbye"
"tell a lie and hurt you"

all with different probabilities


Solution: use a Markov chain to encrypt the data so that the more common next chunks cost mroe bits and the less common ones
cost more bits.

if you combine this with a bigger search window, you get the 

Lempel Ziv Markov chian Algorithm, or LZMA.
THis is the linux's LZ algorithm
