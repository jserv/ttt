# ttt

An implementation of [tic-tac-toe](https://en.wikipedia.org/wiki/Tic-tac-toe) in C,
featuring an AI powered by the [negamax](https://en.wikipedia.org/wiki/Negamax) algorithm.

This program operates entirely in the terminal environment.
Below is its appearance as it awaits your next move:
```
 1 |  ×
 2 |     ○
 3 |
---+----------
      A  B  C
>
```

To execute a move, enter `[column][row]`. For example:
```
> a3
```

Press Ctrl-C to exit.

## Game Rules
The winner is determined by the first player who successfully places three of their marks in a row, whether it be vertically, horizontally, or diagonally, regardless of the board size.

For these two 4x4 board games as examples,

```
 1 |  ×  ×
 2 |     ○  ×
 3 |     ○
 4 |     ○
---+------------
      A  B  C  D
>
```

The player "○" wins the game since he placed his marks in a row vertically (B2-B3-B4).

```
 1 |  ×  ×  ○
 2 |  ×  ○  
 3 |  ○  
 4 |     
---+------------
      A  B  C  D
>
```

The player "○" wins the game since he placed his marks in a row diagonally (A3-B2-B1).

## Reference
* [Mastering Tic-Tac-Toe with Minimax Algorithm in Python](https://levelup.gitconnected.com/3394d65fa88f)
* [tic-tac-toe](https://github.com/jserv/tic-tac-toe): tic-tac-toe game for terminal I/O.
