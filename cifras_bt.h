#ifndef CIFRAS_BT_H
#define CIFRAS_BT_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#define NUM_COUNT 6
// MAX_SOLUTION_STEPS must be at least 4 because the internal function 
// build_candidates_stack uses it
#if NUM_COUNT > 4
	#define MAX_SOLUTION_STEPS (NUM_COUNT - 1)
#else
	#define MAX_SOLUTION_STEPS 4
#endif

// Type long int used because, for combinations of numbers relatively high,
// INT_MAX is overstepped. For example this one:
// 100, 100, 100, 25, 10, 9
typedef struct 
	{
	long int result;
	long int a;
	long int b;
	char op; // '+', '-', '*', '/'
	} SolutionStep;

typedef struct
	{
	SolutionStep steps[MAX_SOLUTION_STEPS];
	int count;
	} SolutionStepStack;


static inline void steps_stack_init(SolutionStepStack* stack)
	{
	assert(stack != NULL);
	stack->count = 0;
	}
static inline bool steps_stack_is_empty(const SolutionStepStack* stack) 
	{
	assert(stack != NULL);
	return stack->count == 0;
	}
static inline void steps_stack_push(SolutionStepStack* stack, const SolutionStep* step)
	{
	assert(stack != NULL);
	assert(step != NULL);
	assert(stack->count >= 0);
	assert(stack->count < MAX_SOLUTION_STEPS);
	stack->steps[stack->count++] = *step;
	}
static inline void steps_stack_pop(SolutionStepStack* stack, SolutionStep* step)
	{
	assert(stack != NULL);
	assert(stack->count > 0);
	assert(stack->count <= MAX_SOLUTION_STEPS);
	--(stack->count);
	if (step != NULL)
		*step = stack->steps[stack->count];
	}
static inline int steps_stack_count(const SolutionStepStack* stack)
	{
	assert(stack != NULL);
	assert(stack->count >= 0);
	assert(stack->count <= MAX_SOLUTION_STEPS);
	return stack->count;
	}
static inline long int steps_stack_result(const SolutionStepStack* stack)
	{
	assert(stack != NULL);
	assert(stack->count > 0);
	assert(stack->count <= MAX_SOLUTION_STEPS);
	return stack->steps[stack->count - 1].result;
	}
void steps_stack_copy(SolutionStepStack* target, const SolutionStepStack* source);
void resolve_cifras(const long int* numbers, int target, SolutionStepStack* best_steps);

#endif
