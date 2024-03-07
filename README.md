# ttt

An implementation of [tic-tac-toe](https://en.wikipedia.org/wiki/Tic-tac-toe) in C,
featuring an AI powered by the [negamax](https://en.wikipedia.org/wiki/Negamax) algorithm, the [reinforcement learning (RL)](https://en.wikipedia.org/wiki/Reinforcement_learning) algorithm, the [Monte Carlo tree search (MCTS)](https://en.wikipedia.org/wiki/Monte_Carlo_tree_search) algorithm. And the RL algorithm contains the [Monte Carlo learning](https://en.wikipedia.org/wiki/Monte_Carlo_method) algorithm and [TD learning](https://en.wikipedia.org/wiki/Temporal_difference_learning) algorithm.


## Build
### negamax AI
If you want to play with negamax AI:
Build the game:
```bash
$ make
```
and run:
```bash
$ ./ttt
```
### reinforcement learning AI
If you want to play with RL AI:
Train the state value table first, you could modify the hyperparameters, which are macros in `train.c`:
- **MONTE_CARLO** : Decide whether or not to use the Monte Carlo method or the TD method.
- **REWARD_TRADEOFF** : Decide the reward of the Markov decision model, which is a balance between the episode reward and the score from `get_score`. Those who want to fully customize the reward can modify it in the `init_agent` function.
- **INITIAL_MULTIPLIER** : Decide the initial state value, which is the multiplier of `get_score` in the initial state value. Those who want to fully customize the initial state value can modify it in the `init_agent` function.
- **EPSILON_GREEDY** : Decide whether or not to use epsilon-greedy exploration when training.
- **NUM_EPISODE** : the number of game episode in training 
- **LEARNING_RATE**, **GAMMA** : $\alpha$, $\gamma$ in training.
- **EPSILON_START** **EPSILON_END** : $\epsilon$ in [Epsilon-Greedy Algorithm](https://stanford-cs221.github.io/autumn2020-extra/modules/mdps/epsilon-greedy.pdf) and it would decay exponentially.


compile
```bash
$ make train
```
and run:
```
$./train
```

Build the game playing with RL agent, it would load the pretrained model from `train`:
```bash
$ make rl
```
and run:
```bash
$ ./rl
```
### MCTS AI
If you want to play with MCTS AI:  
There are several hyperparameters you can modify:
- **EXPLORATION_FACTOR**  in `agents/mcts.h` :  The exploration parameter.
- **ITERATIONS**  in `agents/mcts.h` :  Number of simulations in MCTS.

Build the game:
```bash
$ make mcts
```
and run:
```bash
$ ./mcts
```

### ELO rating system
There are several hyperparameters you can modify:
- **N_GAMES**  in `elo.c` :  The number of games played to calculate the ELO rating.
- **ELO_INIT**  in `elo.c` :  The initial ELO rating assigned to a player before any games are played.
- **ELO_K**  in `elo.c` :  The coefficient used in the ELO calculation formula to determine the impact of each game's outcome on the player's rating.

Build the elo system:
```bash
$ make elo
```
and run:
```bash
$ ./elo
```

## Run
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
