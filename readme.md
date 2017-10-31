# cabash

elementary cellular automata sequencer.

right now, just a toy for exploring parameters of 1-dimensional binary CA, with an eye towards applications in musical sequencing. 

## build

`make` and run.

requires posix and ncurses.

## usage

run the `ca` executable and see a scrolling sequence of elementary cellular automata states.

key commands:

- `1-8`: toggle bits in the update rule
- `[`, `]`, : move the lower boundary up and down
- `{`, `}`, : move the upper boundary up and down
- `p` : toggle the boundary mode (wrapping, fixed low, fixed high)
- `-`, `=` : change the speed of the sequence
- `_`, `+` : change the speed of the sequence more quickly
- `space` : pause / start the sequence
- `q, w, e, r, t, y, u, i` - set the middle 8 cells on (*)
- `a, s, d, f, g, h, j, k` - set the middle 8 cells off (*)
- `CTRL-X` : quit immediately


(*) NB: setting a cell value causes an extra row to be drawn. (ncurses is a pain sometimes.)

## TODO:

- save and recall state
- add OSC bindings to make this into a usable computer music thing
- add libmonome bindings to use with grids
- try asymmetrical boundary modes

pull requests are welcome!
