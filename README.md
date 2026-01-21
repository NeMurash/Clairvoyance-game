## Description

Hello hi I've been working on this small game for a while just to practice my programming skills and stuff.

## Gameplay

You get 5 cards and you have to guess which one will be shown to you next, and like, "try" to get a streak or something.

I got the inspiration to make this from stumbling upon this extrasensory test thingy somewhere online that utilizes those 5 [Zener cards](https://en.wikipedia.org/wiki/Zener_cards) which you will also find in my game.

## Building / Compiling the game

Building this project makes use of the GCC compiler and a Makefile through which the compiler is invoked.

To build the project, first, cd into the project folder with your terminal emulator and make use of these commands:
`make` to create an executable file.
`make run` to run the aforementioned executable. (And rebuild the project if changes have been detected in the source files)
`make clean` to delete the executable.

If something doesn't work. I don't know, go tinker with the Makefile or the source code or something, they're not that big of files.

## Dependencies

> [SDL3](https://github.com/libsdl-org) (I've used the current stable version (3.4.0) if you're interested)

## Configuration

My good friend suggested that I made the colour of the background image customizable. So I did.
In the config folder, there lies a background.conf file from which the game reads the background colour in hex format.
But be warned... The game only reads the first 6 characters from that file because the lead developer did not code the text parsing code that well.

## Hi

That's all I think.
