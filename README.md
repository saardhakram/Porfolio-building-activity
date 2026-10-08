# Line Editor in C

A small command-line text editor, built as a team of three for Activity 7. We designed the data structure, commands and edge cases on paper first, then implemented it in C.

## Features

| Command | What it does |
|---|---|
| `i <n> <text>` | Insert text as line *n* |
| `d <n>` | Delete line *n* |
| `p` | Print the document with line numbers |
| `s <file>` / `l <file>` | Save to / load from a file |
| `f <word>` | Find every line containing a word |
| `r <n> <old> <new>` | Replace a word on line *n* |
| `c` | Count lines, words and characters |
| `u` | Undo the last change |
| `h` / `q` | Help / quit |

## How it works

- The document is an **array of pointers** to heap-allocated strings (`malloc` / `free`), so lines can be inserted and deleted by shifting pointers instead of copying text.
- **Undo** keeps a snapshot of the document before every change and swaps it back.
- Every command **validates its input**, so bad line numbers or missing files print an error instead of crashing.

## Run it

```bash
gcc -Wall -o line_editor line_editor.c
./line_editor
```

```text
> i 1 Hello world
> i 2 second line
> r 1 world there
   1 | Hello there
> c
Lines: 2   Words: 4   Characters: 22
> u
Undone.
```
