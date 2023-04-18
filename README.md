<p align="center">
    <img src="https://raw.githubusercontent.com/iliasizmaylov/cheap8/master/ch8demo.gif" />
</p>

# Cheap-8

Cheap-8 is a chip-8 emulator with built-in TUI chip-8 ROM debugger with TUI.
It uses SDL2 for video and audio and uses ncurses to handle debugger TUI.

There's no way I can call this a finished product right now, let alone suggest that it is useful.
However:
- With Cheap-8 you get a nice TUI debugger interface (xoxo) that let's you pause/resume a chip-8 ROM and run opcodes step-by-step
- You can look into the whole ROM memory in it's raw hex form
- Look at registers, stack, keypad state and see a preview of a sprite that is about to be drawn

## Building

Just clone the whole thing and then run
```sh
$ make
```

You will need SDL2 and libncurses installed. Y'all probably know how to use brew, ports, apt, yum, etc. so I won't go into details.

There is no `make install` so you can run cheap8 by just running an executable from /path/to/cheap8/repo/bin/cheap8
You can use option **-r** to specify a chip-8 ROM file to play and then also specify option **-d** to enable debugger

## TODO and future plans

"Future plans" sounds funny given the subject matter but whatever - I had fun making this one.
But regardless of the fact that I had fun making cheap-8 and using it - you, on the other hand, might not.
And probably because of these problems/shortcomings:
- **FIRST AND FOREMOST:** when debugger is on and ROM is running CPU usage gets to whopping 120%+. This is disgusting and I'm not sure if I can do anything about it at this point. Good thing is that it's totally fine when ROM is paused and debugger is in step-by-step mode which is a primary usage for a debugger.
- No hires support or other variants of chip-8 - only regular vanilla chip-8 arch
- There is no option to resize SDL2 window right now
- Debugger, I think, lacks some features:
    - No way to change tact frequency for cheap-8
    - Browsing disassembly and memory is quirky and a little inconvenient
    - TBA
- SDL2 screen is flickering or rather parts of it that are constantly redrawn by cheap-8 ROMs. It'd be cool to find a way to circumvent it.
- Display sort of "THE END" type of screen when encounter jump onto self opcode
- Add some stuff like change colors of display and stuff like that
- Develop snapshot take/load mechanism

## P.S.

I'm honestly really tired of this project because I seem to not be able to leave it and call it a day at some point - I always want to add stuff.
However, I'd love to try and do something else, try out a couple of other ideas. Or just start working on NES emulator.
Anyway it'd be super duper very cool if someone finds this one usefule
