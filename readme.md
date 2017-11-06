# cabash

elementary cellular automata sequencer.

a toy for exploring parameters of 1-dimensional binary CA, with an eye towards applications in musical sequencing.

see [http://mathworld.wolfram.com/ElementaryCellularAutomaton.html] for more background.

## build

just `make` and run.

requirements:

- posix (supporting CLOCK_MONOTONIC)
- ncurses
- liblo
- liblua5.1

macos:
doesn't build yet, sorry. need to work around the lack of posix clock functions. see this SO thread:
https://stackoverflow.com/questions/21665641/ns-precision-monotonic-clock-in-c-on-linux-and-os-x

(i'd be interested to know if there is some dynamic library that provides a workaround, instead of adding mach functions to the C program.)


## usage

`ca [config file]`

run the `ca` executable with and see a scrolling sequence of elementary cellular automata states.

on each update, `ca` sends an OSC message to the address and port specified by configuration (port 57120 on localhost by default.) message payload is a single binary blob of all cell values (one byte per cell.)

### key commands:

- `1-8`: toggle bits in the update rule
- `[`, `]`, : move the lower boundary up and down
- `{`, `}`, : move the upper boundary up and down
- `o` : toggle left boundary mode (wrapping, fixed low, fixed high)
- `p` : toggle right boundary mode (wrapping, fixed low, fixed high)
- `-`, `=` : change update speed
- `_`, `+` : change update speed more quickly
- `space` : toggle pause
- `q, w, e, r, t, y, u, i` - set the middle 8 cells on (*)
- `a, s, d, f, g, h, j, k` - set the middle 8 cells off (*)
- `z, x, c, v, b, n, m, ,, .` - set state to various patterns (z=clear, x=single)
- `/` - randomize state (uniform, std::rand)
- `CTRL-X` : quit immediately


(*) NB: setting a cell value causes an extra row to be drawn. (ncurses is a pain sometimes.)

### configuration

some settings can be stored and loaded in configuration files (actually lua scripts.) see `conf.lua` for a basic example of all recognized fields. the global `conf` table is mandatory. any other lua code will also be executed.

to use a configuration file, enter its name as the first argument to `ca`. 



## TODO:

- add libmonome bindings to use with grids
