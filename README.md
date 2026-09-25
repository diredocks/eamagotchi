# eamagotchi

Tamagotchi emulator on 4.2" eInk display

## Build

Check out `justfile`

We can't ship ROM within this repo. You can get yourself an Tamagotchi P1 ROM, named it `rom.bin` then run `just convert` to generate `rom.hpp`.

## Todo

- [x] ROM converting
- [x] Display support
- [x] Wiring Button to emulator
- [ ] Deep sleep support
- [ ] Save state

## Credits

This is a fork of [sqfmi/tamawatchy](https://github.com/sqfmi/tamawatchy)
