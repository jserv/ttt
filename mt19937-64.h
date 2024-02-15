#pragma once

/* initializes mt[NN] with a seed */
void mt19937_init(unsigned long long seed);

/* generates a random number on [0, 2^64-1]-interval */
unsigned long long mt19937_rand(void);
