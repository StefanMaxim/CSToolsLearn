# Basic Commands

## Modes

Vim has 3 modes:
Insertion Mode, which lets you enter words.
Its ititiated when you either use an insertion or change command


Normal Mode / Command Mode: the default when entering a file.
lets you run commands via :.
> NOTE  the escape key used to enter command mode

(to make a new terminal, control, alt t)

to begin, use vim (file) to open a vim swap of the file:
(everything you do is first done to the swap, then written to the file)


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

- w, W next word, blank delimited word 
(difference: w word includes whitespace and punctuation, W just whitespace)

- b B //beginning of word, beginning of blank delimetd word

- e E //endc of word, end of blank delimited word

- ( ) //sentence back or forward
note: sentence is defined as to a punctuation point

- { } //paragraph back or paragraph forward

- 0 $ beggining or end of line
- 1G G //beggining and end of file

- gg to go to the top of the line

- nG or :n //nth line of the file

- fc Fc //forward or backward to char c
lets you iterate between all of the character c in a line

- tc Tc //forward back to before char c
(goes to one space before character c)

- H M L //top, middle, and bottom of your screen


## Deleting text
 (note: all deletion commands are performed followed by a motion)
 exe: dw deletes a word

- x X deletes character on cursor, vs char before the cursor

- D deletes from teh cursor to teh end of teh line

- d deletes the line
:d does the same thing

## Yanking text
(same as deletion, y followed by motion)
- y //follow with motion
- yy to yank line

## changing text

- c // follow with a motion to delete adn then go to insertion mode letting you change it. so cw lets you change a word

- C //change to teh end of the line
- cc // change to the end of the line

## putting test

- p //put after position or aafter line
- P // put before position or before line