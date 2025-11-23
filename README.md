# cifras

## Description
CLI application which resolves the game "cifras" ("figures")

## Compilation
~~~
gcc cifras_bt.c main.c -o cifras
~~~

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
