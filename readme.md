# cabash

elementary cellular automata sequencer.

right now, just a toy for exploring parameters of 1-dimensional binary CA, with an eye towards applications in musical sequencing.

see [http://mathworld.wolfram.com/ElementaryCellularAutomaton.html] for more background.

## build

just `make` and run.

requirements:

- posix (supporting CLOCK_MONOTONIC)
- ncurses
- liblo

macos:
doesn't build yet. need to work around the lack of posix clock functions. see this SO thread:
https://stackoverflow.com/questions/21665641/ns-precision-monotonic-clock-in-c-on-linux-and-os-x


## usage

run the `ca` executable and see a scrolling sequence of elementary cellular automata states.

on each update, an OSC message is sent to localhost on port 57120, containing a blob of all cell values (one byte per cell.)

key commands:

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

## TODO:

- save and recall state
- add libmonome bindings to use with grids

pull requests are welcome!