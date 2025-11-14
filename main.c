#include "cifras_bt.h"

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Probability (percentage) that a big number shows up in a random numbers array
#define RANDOM_BIG_NUMBER_PROBABILITY 28
#define MIN_NUMBER 1
#define MAX_NUMBER 100
#define MIN_TARGET 100
#define MAX_TARGET 999
static const int RANDOM_BIG_NUMBERS[] = {10, 25, 50, 100};

static const size_t RANDOM_BIG_NUMBERS_COUNT = sizeof(RANDOM_BIG_NUMBERS) / sizeof(RANDOM_BIG_NUMBERS[0]);


static int random_natural(int min_val, int max_val) 
	{
	if (min_val > max_val) {
		fprintf(stderr, "Error in random_natural: min_val cannot be greater than max_val\n");
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
		if (probability <= RANDOM_BIG_NUMBER_PROBABILITY)
			{
			big_number_pos = random_natural(0, RANDOM_BIG_NUMBERS_COUNT - 1);
			numbers[i] = RANDOM_BIG_NUMBERS[big_number_pos];
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

// Return values:
// 0: regex validated
// 1: regex not validated
// -1 error
static int validate_string(const char* input, const char* regex_pattern)
	{
	regex_t regex;
	int ok;
	char buffer[256];

	// Compile regex
	ok = regcomp(&regex, regex_pattern, REG_EXTENDED);
	if (ok != 0)
		{
		regerror(ok, &regex, buffer, sizeof(buffer));
		fprintf(stderr, "Error in validate_string: regex compile error: %s\n", buffer);
		return -1;
		}
	
	// Execute regex validation
	ok = regexec(&regex, input, 0, NULL, 0);
	// If ok == 0, do nothing
	if (ok == REG_NOMATCH)
		ok = 1;
	else if (ok != 0)
		{
		fprintf(stderr, "Error in validate_string: regexec has failed\n");
		ok = -1;
		}
	
	// Free memory of the compiled regex
	regfree(&regex);
	
	// Return value depending on the previous regex validation
	return ok;
	}

// input will be modified by the function strtok.
// In case it is wanted to reuse the string there, make a previous copy
static int parse_numbers(int* numbers, char* input)
	{
	char* token;
	int i = 0;

	// Remove final new-line character
	input[strcspn(input, "\n")] = 0;

	// Add numbers into the array
	token = strtok(input, ", ");
	do
		{
		numbers[i] = atoi(token);
		if (numbers[i] <= 0)
			{
			fprintf(stderr, "Error in parse_numbers: ");
			fprintf(stderr, "Number %d not correctly parsed or not strictly positive\n",
				numbers[i]);
			return -1;
			}
		if (numbers[i] < MIN_NUMBER || numbers[i] > MAX_NUMBER)
			{
			fprintf(stderr, "Error in parse_numbers: ");
			fprintf(stderr, "number %d is not between %d and %d\n",
				numbers[i], MIN_NUMBER, MAX_NUMBER);
			return -1;
			}
		token = strtok(NULL, " ,");
		i++;
		}
	while (token != NULL && i < NUM_COUNT);
	
	if (i != NUM_COUNT)
		{
		fprintf(stderr, "Error in parse_numbers: %d numbers parsed instead of %d\n",
			i, NUM_COUNT - 1);
		return -1;
		}
	return 0;
	}

// input will be modified by the function strtok.
// In case it is wanted to reuse the string there, make a previous copy
static int parse_target(int* target, char* input)
	{
     char* token;
     int count = 0;

     // Remove final new-line character
     input[strcspn(input, "\n")] = 0;

	// Capture target number
	token = strtok(input, " ");
	*target = atoi(token);
	if (strtok(NULL, " ") != NULL)
		{
		fprintf(stderr, "Error in parse_target: target must be a single number\n");
		return -1;
		}
	if (*target < MIN_TARGET || *target > MAX_TARGET)
		{
		fprintf(stderr, "Error in parse_target: target %d is not between %d and %d\n",
			target, MIN_TARGET, MAX_TARGET);
		return -1;
		}

	return 0;
	}

int main()
	{
	int numbers[NUM_COUNT], target, result;
	SolutionStepStack steps_stack;
	char buffer[256], regex_pattern[256];
	int ok;

	printf("Introduce numbers (enter to be randomly generated)\n");
	if (fgets(buffer, sizeof(buffer), stdin) == NULL)
		{
		fprintf(stderr, "Read error in fgets\n");
		return 1;
		}
	
	if (strcmp(buffer, "\n") == 0)
		{
		srand(time(NULL));
		generate_numbers(numbers);
		target = random_natural(MIN_TARGET, MAX_TARGET);
		}
	else
		{
		// Validate numbers string
		// Generate pattern dynamically taking into account the constant NUM_COUNT
		// Alternative regex to allow several commas (besides spaces)
		// between numbers:
		// ^[[:space:]]*(0*[1-9][0-9]*([[:space:]]|,)+){%d}0*[1-9][0-9]*[[:space:]]*$
		snprintf(regex_pattern, sizeof(regex_pattern),
			"^[[:space:]]*(0*[1-9][0-9]*([[:space:]]+|([[:space:]]*,[[:space:]]*))){%d}0*[1-9][0-9]*[[:space:]]*$",
			NUM_COUNT - 1);
		ok = validate_string(buffer, regex_pattern);
		if (ok == 1)
			{
			fprintf(stderr, "Error: wrong numbers format. ");
			fprintf(stderr, "Introduce %d numbers separated with spaces or commas\n",
				NUM_COUNT);
			return 1;
			}
		else if (ok == -1)
			{
			fprintf(stderr, "Validation error in validate_string\n");
			return 1;
			}
		
		// Parse numbers
		// This includes checking that the numbers are between 
		// MIN_NUMBER AND MAX_NUMBER
		ok = parse_numbers(numbers, buffer);
		if (ok != 0)
			{
			fprintf(stderr, "Error in parse_numbers\n");
			return 1;
			}
		
		// Get target number
		printf("Introduce target: ");
		if (fgets(buffer, sizeof(buffer), stdin) == NULL)
			{
			fprintf(stderr, "Read error\n");
			return 1;
			}
		ok = validate_string(buffer, "^[[:space:]]*0*[1-9][0-9]{2}[[:space:]]*$");
		if (ok != 0)
			{
			fprintf(stderr, "Error: wrong target format\n");
			return 1;
			}
		else if (ok == -1)
			return 1;

		// Parse target number
		// This includes checking that the target is between
		// MIN_TARGET and MAX_TARGET
		// PENDING: CREATE FUNCTION
		parse_target(&target, buffer);
		if (ok != 0)
			return 1;
		}
		
	printf("Numbers: ");
	numbers_print(numbers);
	printf("Target: %d\n\n", target);

	ok = resolve_cifras(numbers, target, &steps_stack);
	if (ok != 0)
		return 1;
	
	result = steps_stack_result(&steps_stack);
	printf("Result obtained: %d", result);
	if (result == target)
		printf(" (EXACT!)");
	printf("\n\n");
	
	steps_stack_print(&steps_stack);

	return 0;
	}
