# Set Game Analysis

------------------

### Overview

For actual information on the game, see:

* Wikipedia page: https://en.wikipedia.org/wiki/Set_(card_game)
* Official Instructions: https://www.setgame.com/sites/default/files/instructions/SET%20INSTRUCTIONS%20-%20ENGLISH.pdf
* 3rd Party tutorial: https://www.youtube.com/watch?v=4txi20-Z8-8

I assume you are already familiar with this game for the purposes of this repo. The purpose of this game is to get better at programming and mathematics, and do a fun exploration of a game that I love.

### Gameplay Observations

Cards have four attributes in three configurations for each attribute:

* Color
    * Red
    * Purple
    * Green
* Count
    * 1
    * 2
    * 3
* Fill
    * Solid
    * Striped
    * Open
* Shape
    * Capsule
    * Diamond
    * Squiggle

This means that the total number of cards in a deck is 3**4 = 81 total cards, since every card is unique, differing along at least one quality. 

In many ways, this is a perfect game. Each card is as useful as any other card. Every set is equal to every other set. Any card can be part of a set with any other card, and the third card is described by those two cards. 

However, there is a noteable situation which can be seen as a "flaw" in the game. In regular play, 12 cards are uncovered for players to find sets. Sometimes, there will not be any valid sets in those 12 cards. In this situation, 3 more cards are provided. Sometimes, there is still not a valid set in those 15 cards, in which case the extra 3 cards are discarded and replaced with 3 more. 

### Research Questions:

I aim to address the following questions: 

#### Broad possibility space parameter/statistic questions:
* Of all possible 12 card displays, how many do not have a valid set?
    * initlaly tried generating every 12 card board, but there are 70_724_320_184_700 possible boards. 
    * One solution- sample
    * eliminating reflections and rotations, how many unique boards are there?
* Of all possible 15 card displays, how many do not have a valid set?
    * sample again probably
* SOLVED What proportion of sets have any given number of monoattributes?

#### Gameplay questions:
* How many games (cycling through all 81 cards until no valid sets remain), require extra cards?
    * How may times will extra cards be required?
    * How many will require extra cards beyond 15?
* Accounting for the element of player choice, how many possible games are possible?
* over the course of the game, how do probabilites change?
    * how much more likely is needing extra cards past 12? past 15?
    * how much more liley are sets with no monoattributes?

#### Game Design questions:
* SOLVED What is the minimum board size to always guarantee a set?
    * this is a solved problem. the answer is 20. (Set Cap/ Affine Geometry)
        * proven by Guiseppe Pellegrino in 1970, 4 years before the game was created in 1974???
    * Experimentally prove the result: start with a pair, mark of possibles and impossibles, add the first possible. 
    * What are the patterns it has to abide by?
    * how many unique 20-card boards are there?
* Because of card replacement, if a board has one set, then you take it out, what is the chance that it gets replaced by cards that have no set? (peter norvig)
    * 15 board probability is higher than initially supposed?
* At 21-card boards standard, how would the game change?

### Stretch Goals:
* Make a playable game
    * HTML/JS? Pygame? Bash?
* Develop strategies for more optimized play
    * When there are one/two cards with singular qualities, are they likely to be used?
    * What's the best way to detect when there is no set?
    * Breaking down the attack surface:
        * Relative rarity of a trait across a board
        * visually clustering like cards to treat them as a unit?
        * Id your own visual biases
        * Static search strategy???
        * Walking/pivoting based on the one that doesn't fit?