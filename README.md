# ttt

An implementation of [tic-tac-toe](https://en.wikipedia.org/wiki/Tic-tac-toe) in C,
featuring an AI powered by the [negamax](https://en.wikipedia.org/wiki/Negamax) algorithm and the [TD learning](https://en.wikipedia.org/wiki/Temporal_difference_learning) algorithm.

## Run
If you want to play with negamax AI:
Build the game:
```bash
$ make
```
and run:
```bash
$ ./ttt
```
If you want to play with TD learning AI:
Train the state value table first, you could modify 
- **EPSILON_START** **EPSILON_END**  in `train.c` :  $\epsilon$ in [Epsilon-Greedy Algorithm](https://stanford-cs221.github.io/autumn2020-extra/modules/mdps/epsilon-greedy.pdf) and it would decay exponentially.
- **NUM_ITERATION** in `train.c`: nums of game iteration 
- **LEARNING_RATE**, **GAMMA** in `temporal_difference.h`: $\alpha$, $\gamma$ in TD learning.


compile
```bash
$ make train
```
and run:
```
$./train
```

Build the game playing with TD agent, it would load the pretrained model from `train`:
```bash
$ make td
```
and run:
```bash
$ ./td
```

These program operate entirely in the terminal environment.
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
The winner is determined by the first player who successfully places `GOAL` of their marks in a row, whether it is vertically, horizontally, or diagonally, regardless of the board size.

Using the following 4x4 board games, whose `GOAL` is 3, as examples:

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

The player "○" wins the game since he placed his marks in a row diagonally (A3-B2-C1).

```
 1 |  o  x  
 2 |  o  x  
 3 |  o     x
 4 |  o  x
---+------------
      A  B  C  D
>
```

The player "○" wins the game if `ALLOW_EXCEED` is `1`; otherwise, the game will continue because the number of "○"s in a row exceeds `GOAL`.

## Reference
* [Mastering Tic-Tac-Toe with Minimax Algorithm in Python](https://levelup.gitconnected.com/3394d65fa88f)
* [tic-tac-toe](https://github.com/jserv/tic-tac-toe): tic-tac-toe game for terminal I/O.
