# CHIP-8-Emulator
An interpreter for [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) written in C++

## Dependencies + Compilation
- Clone the repo `git clone`
- Install [SDL](https://www.libsdl.org/)
  - I used [this](https://medium.com/@edkins.sarah/set-up-sdl2-on-your-mac-without-xcode-6b0c33b723f7) guide for Mac.
- Run `make game` in the project directory to compile the project

## Usage
The expected usage is `./play <ROM> <Delay>`
- `<ROM>` should contain the path to the ROM file.
- `<Delay>` is the number of milliseconds between emulation cycles.

## Keypad
CHIP-8 Keypad's look like this:
```
1 2 3 C
4 5 6 D
7 8 9 E
A 0 B F
```

Here is the mapping of the above to the modern QWERTY keyboard:
```
1 2 3 4    ->    1 2 3 C
Q W E R    ->    4 5 6 D
A S D F    ->    7 8 9 E
Z X C V    ->    A 0 B F
```

## Images
Playing Tetris:

<img src="https://user-images.githubusercontent.com/21336191/149680951-a7637e37-4d51-4cf0-a080-96b4fbd15106.png" width="50%" height="50%">

Displaying the IBM Logo:

<img src="https://user-images.githubusercontent.com/21336191/149680959-6a6f0555-77c8-4aab-9c85-b1e443c6554f.png" width="50%" height="50%">


## ROMs
[Here](https://johnearnest.github.io/chip8Archive/) is a link to a library of CHIP-8 games to play.

## References
- [Chip-8 Github Links](https://chip-8.github.io/links/)
- [CHIP-8 wikipedia page](https://en.wikipedia.org/wiki/CHIP-8).
