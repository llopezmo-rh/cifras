# cifras

## Description
CLI application which resolves the game "cifras" ("figures")

## Compilation
~~~
$ make
Compiling main.c...
gcc -O3 -march=native -flto -Wall -Wextra -c main.c -o main.o
Linking cifras...
gcc main.o cifras_bt.o -o cifras -flto
Compilation complete!
~~~
Edit `Makefile` to customize the compilation options.

## Output example
~~~
$ cifras
Introduce numbers (enter to be randomly generated)

Numbers: 10, 50, 5, 50, 6, 25
Target: 988

Result obtained: 988 (EXACT!)

10 * 50 = 500
500 - 6 = 494
494 * 50 = 24700
24700 / 25 = 988

Press "Q" to exit or any other key to play again...
~~~
