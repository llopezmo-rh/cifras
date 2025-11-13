#include "cifras_bt.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Probability (percentage) that a big number shows up in a random numbers array
#define BIG_NUMBER_PROBABILITY 28
#define MIN_TARGET 100
#define MAX_TARGET 999
static const int BIG_NUMBERS[] = {10, 25, 50, 100};

static const size_t BIG_NUMBERS_COUNT = sizeof(BIG_NUMBERS) / sizeof(BIG_NUMBERS[0]);


static int random_natural(int min_val, int max_val) 
	{
	if (min_val > max_val) {
		fprintf(stderr, "Error: min_val cannot be greater than max_val\n");
		// Returning min_val as a fallback
		return -1;
	}

	// 1. Calculate the size of the range
	int range_length = (max_val - min_val) + 1;

	// 2. Get random number, apply modulo, and shift by min_val
	// (rand() % range_length) -> gives a number from 0 to (range_length - 1)
	// ... + min_val         -> shifts the range to [min_val, max_val]
	return (rand() % range_length) + min_val;
	}

static void generate_numbers(int* numbers)
	{
	int probability, i, big_number_pos;
	for (i = 0; i < NUM_COUNT; i++)
		{
		probability = random_natural(0, 100);
		if (probability <= BIG_NUMBER_PROBABILITY)
			{
			big_number_pos = random_natural(0, BIG_NUMBERS_COUNT - 1);
			numbers[i] = BIG_NUMBERS[big_number_pos];
			}
		else
			numbers[i] = random_natural(1, 9);
		}
	}

static void numbers_print(int* numbers)
	{
	int i;
	//printf("{");
	for (i = 0; i < NUM_COUNT; i++)
		{
		printf("%d", numbers[i]);
		if (i < NUM_COUNT - 1)
			printf(", ");
		}
	//printf("}\n");
	printf("\n");
	}

static void steps_stack_print(SolutionStepStack* stack)
	{
	SolutionStep step;
	if (steps_stack_is_empty(stack) == false)
		{
		steps_stack_pop(stack, &step);
		steps_stack_print(stack);
		printf("%d %c %d = %d\n", step.a, step.op, step.b, step.result);
		steps_stack_push(stack, &step);
		}
	}

int main()
	{
	int numbers[NUM_COUNT], target, result;
	SolutionStepStack steps_stack;

	srand(time(NULL));
	
	generate_numbers(numbers);
	//numbers[0] = 50;
	//numbers[1] = 5;
	//numbers[2] = 4;
	printf("Numbers: ");
	numbers_print(numbers);

	target = random_natural(MIN_TARGET, MAX_TARGET);
	//target = 11;
	printf("Target: %d\n\n", target);

	resolve_cifras(numbers, target, &steps_stack);
	
	result = steps_stack_result(&steps_stack);
	printf("Result obtained: %d", result);
	if (result == target)
		printf(" (EXACT!)");
	printf("\n\n");
	
	steps_stack_print(&steps_stack);
	}
