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
