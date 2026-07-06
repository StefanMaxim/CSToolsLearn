# Basic Commands

# Vim Grammar

Vim commands often follow this structure:

[count] operator [count] motion/text-object

Operators:
d = delete
y = yank
c = change
> = indent
< = unindent
= = auto-format/indent

Motions:
w = next word
b = previous word
e = end of word
$ = end of line
0 = beginning of line
gg = top of file
G = bottom of file

Text objects:
iw = inside word
aw = around word
i" = inside quotes
a" = around quotes
i) = inside parentheses
a) = around parentheses
ip = inside paragraph
ap = around paragraph

Examples:
dw = delete word
ciw = change inside word
di" = delete inside quotes
yap = yank around paragraph
gg=G = auto-indent whole file



## Modes

Vim has 3 modes:
Insertion Mode, which lets you enter words.
Its ititiated when you either use an insertion or change command


Normal Mode / Command Mode: the default when entering a file.
lets you run commands via :.
> NOTE  the escape key used to enter command mode

(to make a new terminal, control, alt t)

to begin, use vim (file) to open a vim swap of the file:
When you open a file, Vim loads it into a buffer.
You edit the buffer in memory.
When you run :w, Vim writes the buffer to the actual file.
A swap file is mainly for crash recovery, not the main thing you edit.

buffer = text/file loaded in memory
window = viewport showing a buffer
tab = layout containing windows

## CURSOR:
CURSOR IS A SQUARE, WITH IT GENERALLY BEING THE ONE BEFORE THE SELECTED CHARACTER, EXCEPT WHEN REPLACING CHARS.
INSERTS DEFAULT BEFORE, CHAR REPLACE DEFAULT THERE


## Quitting commands
- :x //used to exit and save changes
- :q // used to quit, unless there were changes
- :q! lets you quit without saving the changes

## Inserting commands
- i, I (enters insertion mode right before the cursor, or before the line), but still on the same line
- a, A (append after cursor, right at the end of the line)
- o, O //opens new line after cursor line vs a line before  
- r, R // replaces one char, replaces many chars

**CRUTIAL** in VIM, cursor is considered on a character, not between
then like in other editors. (cursor is just highlighted character)
thus, I inserts in front of the highlighted character whereas a will put after

# Vim Movements:
- h,j,k,l left, down, up, right 
NOTE: isnt this just a more complex arror keys?
No: since these movements pair with commands whereas the arro keys dont
KEY: pair with a number to move up or down that many lines

- w, W next word, blank delimited word 
(difference: w word includes whitespace and punctuation, W just whitespace)
(KEY: number, then w moves that many words forward, ie 3w is forward 3 words)

- b B //beginning of word, beginning of blank delimetd word (the way to go back a word, back to the start)
(inverse of w)

- e E //endc of word, end of blank delimited word

- ( ) //sentence back or forward
note: sentence is defined as to a punctuation point + whitspace /newline after it
hello.hi.howareyou is not 3 sentences

- { } //paragraph back or paragraph forward

- 0 $ beggining or end of line
- 1G G //beggining and end of file

- gg to go to the top of the file

- nG or :n //nth line of the file

- fc Fc //forward or backward to char c
lets you iterate between all of the character c in a line

- tc Tc //forward back to before char c
(goes to one space before character c)

- H M L //top, middle, and bottom of your screen


## Deleting text
 (note: all deletion commands are performed followed by a motion)

```text
    d{ for example deletes a paragraph
```
 exe: dw deletes a word

- x X deletes character on cursor, vs char before the cursor

- D deletes from teh cursor to teh end of teh line 
(TRICK: use b, then dw to go to the beginning of the word, cursor being before first char, and delete to the next word,
ie right before the next words first char, which is you last char of that word)

- dd deletes the line 
:d does the same thing

**KEY** dw means delete from cursor to teh end of the word
just like D is from cursor to end of line

## Yanking text
(same as deletion, y followed by motion)
- y //follow with motion
- yy to yank line



### Yanking to Clipboard
use system clipboard register "+

"+yy to yank line or 
"+y<motion> for more complex yank


## changing text

- c // follow with a motion to delete adn then go to insertion mode letting you change it. so cw lets you change a word

- C //change to teh end of the line
- cc // change to the end of the line

ALSO GOOD BC DOESNT DELETE THE LINE

## putting test

- p //put after position or aafter line
- P // put before position or before line

## finding text
fa     " find next a on this line
Fa     " find previous a on this line
ta     " move until before next a
Ta     " move until after previous a
;      " repeat last f/F/t/T
,      " repeat last f/F/t/T in opposite direction

## text objects
i = inside
a = around

iw     " inside word
aw     " around word
is     " inside sentence
as     " around sentence
ip     " inside paragraph
ap     " around paragraph
i"     " inside quotes
a"     " around quotes
i)     " inside parentheses
a)     " around parentheses
i}     " inside braces
a}     " around braces

exe:
ciw    " change inside word
di"    " delete inside quotes
ya)    " yank around parentheses
dap    " delete around paragraph

Format here is <operator><textoption>

operator is what to do, like d, c, y, v(visual select)

exe:

ciw => change inside work

The quick brown fox
    ^(cursor position)

ciw changes(thereby removing) only "quick", once done enters insert mode

caw is change around word, meaning work + whitespace

exe2:
The quick brown fox
   ^(cursor position)

ciw still changes quick, not the whitespace: The | brown fox (| means cursor here)
caw will remove quick(space), as its 
dw will delete just the space here, as its from cursor to end of word, so just one char

**NOTE** when on space, it deletes/changes just that space, else behaves as expected.




# Key VIM Ideas:
vim isnt just a series of commands, but a full text editing language

vim```
[count] operator [count] motion/text-object
```

THis is the count-operator model, and key to understanding how vim works

exe:
dw     " delete word
d$     " delete to end of line
ciw    " change inside word
dap    " delete around paragraph
3dd    " delete 3 lines
yG     " yank from here to end of file


Also vim has more than 3 modes:
Main modes:
- Normal mode: move, edit, run operators, run commands
- Insert mode: type text
- Visual mode: select text
- Command-line mode: commands after :, /, or ?
- Replace mode: overwrite text




# Neovim

Most people nowadays use neovim + plugins, which is must more useful

Key Differences
