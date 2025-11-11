#ifndef CIFRAS_BT_H
#define CIFRAS_BT_H

#include <stdbool.h>

#define NUM_COUNT 6
// MAX_SOLUTION_STEPS must be at least 4 because the internal function 
// build_candidates_stack uses it
#if NUM_COUNT >= 5
	#define MAX_SOLUTION_STEPS (NUM_COUNT - 1)
#else
	#define MAX_SOLUTION_STEPS 4
#endif

typedef struct 
	{
	int result;
	int a;
	int b;
	char op; // '+', '-', '*', '/'
	} SolutionStep;

typedef struct
	{
	SolutionStep steps[MAX_SOLUTION_STEPS];
	int count;
	} SolutionStepStack;

int steps_stack_init(SolutionStepStack* stack);
bool steps_stack_is_empty(const SolutionStepStack* stack);
int steps_stack_push(SolutionStepStack* stack, const SolutionStep* step);
int steps_stack_pop(SolutionStepStack* stack, SolutionStep* step);
int steps_stack_count(const SolutionStepStack* stack);
int steps_stack_result(const SolutionStepStack* stack);
int steps_stack_copy(SolutionStepStack* target, const SolutionStepStack* source);
void resolve_cifras(const int* numbers, int target, SolutionStepStack* best_steps);

#endif
