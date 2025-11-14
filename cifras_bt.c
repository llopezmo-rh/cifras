#include "cifras_bt.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>



static int step_copy(SolutionStep* target, const SolutionStep* source)
	{
	if (target == NULL || source == NULL)
		{
		fprintf(stderr, "Error on step_copy: solution steps cannot be NULL\n");
		return -1;
		}

	if (source->result <= 0)
		{
		fprintf(stderr, "Error on step_copy: \"result\" field is %d", source->result);
		fprintf(stderr, ", which is not strictly positive\n");
		return -1;
		}	
	target->result = source->result;
	
	if (source->a <= 0)
		{
		fprintf(stderr, "Error on step_copy: \"a\" field is %d", source->a);
		fprintf(stderr, ", which is not strictly positive\n");
		return -1;
		}
	target->a = source->a;

	if (source->b <= 0)
		{
		fprintf(stderr, "Error on step_copy: \"b\" field is %d", source->b);
		fprintf(stderr, ", which is not strictly positive\n");
		return -1;
		}
	target->b = source->b;

	if (source->op != '+' && 
		source->op != '-' && 
		source->op != '*' && 
		source->op != '/')
		{
		fprintf(stderr, "Error on step_copy: \"op\" field is \"%c\". ",
			source->op);
		fprintf(stderr, "Not valid\n");
		return -1;
		}
	target->op = source->op;
	
	return 0;
	}	

int steps_stack_init(SolutionStepStack* stack)
	{
	if (stack == NULL)
		{
		fprintf(stderr, "Error on steps_stack_init: stack cannot be NULL\n");
		return -1;
		}
		
	stack->count = 0;
	
	return 0;
	}
	
// Function defined in cifras_bt.h:
// static inline bool steps_stack_is_empty(const SolutionStepStack* stack)


int steps_stack_push(SolutionStepStack* stack, const SolutionStep* step)
	{
	if (stack == NULL)
		{
		fprintf(stderr, "Error on steps_stack_push: stack cannot be NULL\n");
		return -1;
		}
		
	if (step == NULL)
		{
		fprintf(stderr, "Error on steps_stack_push: step cannot be NULL\n");
		return -1;
		}
		
	if (stack->count < 0 || stack->count >= MAX_SOLUTION_STEPS)
		{
		fprintf(stderr, "Error on steps_stack_push: stack count is %d\n",
			stack->count);
		return -1;
		}
	
	step_copy(&(stack->steps[stack->count++]), step);
	
	return 0;
	}
	
int steps_stack_pop(SolutionStepStack* stack, SolutionStep* step)
	{
	int ok;

	if (stack == NULL)
		{
		fprintf(stderr, "Error on steps_stack_pop: stack cannot be NULL\n");
		return -1;
		}
		
	if (steps_stack_is_empty(stack))
		{
		fprintf(stderr, "Error on steps_stack_is_empty: stack cannot be empty\n");
		return -1;
		}
		
	if (stack->count < 1 || stack->count > MAX_SOLUTION_STEPS)
		{
		fprintf(stderr, "Error on steps_stack_pop: stack count is %d\n",
			stack->count);
		return -1;
		}
	
	stack->count--;
	
	if (step != NULL)
		{
		ok = step_copy(step, &(stack->steps[stack->count]));
		if (ok != 0)
			{
			fprintf(stderr, "Error on steps_stack_pop: step_copy has failed\n");
			return -1;
			}
		}
	
	return 0;
	}
	
int steps_stack_count(const SolutionStepStack* stack)
	{
	assert(stack != NULL);
	assert(stack->count >= 0);
	assert(stack->count <= MAX_SOLUTION_STEPS);
	return stack->count;
	}
	
int steps_stack_result(const SolutionStepStack* stack)
	{
	assert(stack != NULL);
	assert(stack->count >= 0);
	assert(stack->count <= MAX_SOLUTION_STEPS);
	return stack->steps[stack->count - 1].result;
	}
	
int steps_stack_copy(SolutionStepStack* target, const SolutionStepStack* source)
	{
	int i, ok;
	
	if (target == NULL || source == NULL)
		{
		fprintf(stderr, "Error on steps_stack_copy: stacks cannot be NULL\n");
		return -1;
		}
		
	assert(source->count >= 0 || source->count <= MAX_SOLUTION_STEPS);
		
	if (steps_stack_is_empty(source))
		{
		steps_stack_init(target);
		return 0;
		}
	
	for (i = 0; i < source->count; i++)
		{		
		ok = step_copy(&(target->steps[i]), &(source->steps[i]));
		if (ok != 0)
			{
			fprintf(stderr, "Error on steps_stack_copy: step copy failed\n");
			return -1;
			}
		}
	
	target->count = source->count;
	
	return 0;
	}
		
// Calculate which solution is better.
// A solution is better if the result is nearer the target.
// In case none is nearer, it is better the one with less steps.
// -1: first one better. 0: equal. 1: second one better
static int steps_stack_compare(const SolutionStepStack* stack1,
	const SolutionStepStack* stack2, int target)
	{
	int diff1, diff2;

	// If both are empty, return 0.
	// If only one is empty, the other one is considered better
	if (steps_stack_is_empty(stack1) && steps_stack_is_empty(stack2))
		return 0;
	else if (steps_stack_is_empty(stack1))
		return 1;
	else if (steps_stack_is_empty(stack2))
		return -1;
	
	diff1 = abs(steps_stack_result(stack1) - target);
	diff2 = abs(steps_stack_result(stack2) - target);
	if (diff1 < diff2)
		return -1;
	else if (diff2 < diff1)
		return 1;
	else if (diff1 == diff2)
		if (steps_stack_count(stack1) < steps_stack_count(stack2))
			return -1;
		else if (steps_stack_count(stack2) < steps_stack_count(stack1))
			return 1;
		else if (steps_stack_count(stack1) == steps_stack_count(stack2))
			return 0;
	
	// The function should never reach this point
	assert(false);
	}

// 1. Put new in new_array[0].
// 2. Copy the elements of former_array into new array starting from
// new_array[1] skiping former_array[old_pos1] and former_array[old_pos2]
static int build_next_numbers(int* new_array, const int* former_array,
	int old_pos1, int old_pos2, int new)
	{
	int i, j;
	
	if (new_array == NULL || former_array == NULL)
		{
		fprintf(stderr, "Error: arrays cannot be NULL\n");
		return -1;
 		}
	if (old_pos1 < 0 || old_pos2 < 0)
		{
		fprintf(stderr, "Error: array positions cannot be less than zero\n");
		return -1;
		}
	if (old_pos1 > NUM_COUNT || old_pos2 > NUM_COUNT)
		{
		fprintf(stderr, "Error: array positions cannot be greater than %d\n", 
			NUM_COUNT);
		return -1;
		}
	if (new <= 0)
 		{
 		fprintf(stderr, "Error: array elements must be strictly positive\n");
 		return -1;
 		}

	new_array[0] = new;
	
	j = 1;
	for (i = 0; i < NUM_COUNT; i++)
		if (i != old_pos1 && i != old_pos2)
			new_array[j++] = former_array[i];
	return 0;
	}
	
static int build_candidates_stack(SolutionStepStack* stack,
	int operand1, int operand2)
	{
	SolutionStep step;
	int count;
	
	// Validate arguments
	if (operand1 <= 0 || operand2 <= 0)
		{
		fprintf(stderr, "Error on build_candidates_stack: operands must be strictly positive\n");
        return -1;
		}
		
	steps_stack_init(stack);
	
	// Add
	step = (SolutionStep){operand1 + operand2, operand1, operand2, '+'};
	steps_stack_push(stack, &step);
	
	// Substract (no action if operand1 == operand2)
	if (operand1 > operand2)
		step = (SolutionStep){operand1 - operand2, operand1, operand2, '-'};
	else if (operand2 > operand1)
		step = (SolutionStep){operand2 - operand1, operand2, operand1, '-'};
	steps_stack_push(stack, &step);
	
	// Multiply
	step = (SolutionStep){operand1 * operand2, operand1, operand2, '*'};
	steps_stack_push(stack, &step);
	
	// Divide
	if (operand2 != 0 && operand1 % operand2 == 0)
		{
		step = (SolutionStep){operand1 / operand2, operand1, operand2, '/'};
		steps_stack_push(stack, &step);
		}
	else if (operand1 != 0 && operand2 % operand1 == 0)
		{
		step = (SolutionStep){operand2 / operand1, operand2, operand1, '/'};
		steps_stack_push(stack, &step);
		}
	
	count = steps_stack_count(stack);
	if (count < 2 || count > 4)
		{
		fprintf(stderr, "Error on build_candidates_stack: count is %d\n", count);
		return -1;
		}
		
	return 0;
	}

static int cifras_bt(const int* numbers, int numbers_count,
	int target,
	const SolutionStepStack* current_steps, SolutionStepStack* best_steps) 
	{	
	int ok;
	int i, j;
	SolutionStep candidate;
	SolutionStepStack candidate_steps, next_steps;
	int next_numbers[NUM_COUNT];
	
	// If current_steps reaches a better result than best_steps, then
	// mirror current_steps into best_steps
	if (steps_stack_compare(current_steps, best_steps, target) == -1)
		{
		ok = steps_stack_copy(best_steps, current_steps);
		if (ok != 0) return -1;
		}

	// Base case: only 1 number pending, therefore no more combinations are possible
	assert(numbers_count > 0);
	if (numbers_count == 1)
		return 0;

	// From here onwards, recursive case
	for (int i = 0; i < numbers_count; i++) 
		{
		for (int j = i + 1; j < numbers_count; j++)
			{
			// Stack candidate steps
			// Operands: numbers[i] and numbers[j]
			ok = build_candidates_stack(&candidate_steps, numbers[i], numbers[j]);
			if (ok != 0) return -1;
				
			// Initialize next_steps with a copy of current_steps
			ok = steps_stack_copy(&next_steps, current_steps);
			if (ok != 0) return -1;
			
			// Pop candidates one by one and make a recursive call with
			// everyone of them
			while (steps_stack_is_empty(&candidate_steps) == false)
				{
				// Push next candidate
				ok = steps_stack_pop(&candidate_steps, &candidate);
				if (ok != 0) return -1;
				ok = steps_stack_push(&next_steps, &candidate);
				if (ok != 0) return -1;
				
				// Create numbers array for the recursive call
				ok = build_next_numbers(next_numbers, numbers, i, j, candidate.result);
				if (ok != 0) return -1;
				
				// Recursive call
				ok = cifras_bt(next_numbers, numbers_count - 1, target,
					&next_steps, best_steps);
				if (ok != 0) return -1;
				
				// Restore next_steps. More than one candidate step must not
				// be pushed for the same recursive call
				ok = steps_stack_pop(&next_steps, NULL);
				if (ok != 0) return -1;
				}
			}
		}
	return 0;
	}

// Wrapper
int resolve_cifras(const int* numbers, int target, SolutionStepStack* best_steps)
	{
	int ok;
	SolutionStepStack current_steps;
	
	ok = steps_stack_init(&current_steps);
	if (ok != 0) return -1;
	ok = steps_stack_init(best_steps);
	if (ok != 0) return -1;
	
	return cifras_bt(numbers, NUM_COUNT, target, &current_steps, best_steps);
	}


// Functions used for debugging	
//static void steps_stack_print(const SolutionStepStack* stack)
//	{
//	int i;
//	if (stack->count >= 0 && stack->count <= MAX_SOLUTION_STEPS)
//		for (i = 0; i < stack->count; i++)
//			printf("%d %c %d = %d, ", stack->steps[i].a, stack->steps[i].op, stack->steps[i].b, stack->steps[i].result);
//	printf("| Count: %d\n", stack->count);
//	}	
//static void debug(const int* numbers, int numbers_count, int target, const SolutionStepStack* current_steps, const SolutionStepStack* best_steps, const SolutionStepStack* candidate_steps, const SolutionStepStack* next_steps, bool increase_counter)
//	{
//	int i;
//	static unsigned int exec_count = 0;
//	if (increase_counter)
//		exec_count++;
//	printf("Exec count: %d\n", exec_count);
//	printf("Numbers: ");
//	for (i = 0; i < numbers_count; i++)
//		printf("%d ", numbers[i]);
//	printf("Count: %d\n", numbers_count);
//	printf("Target: %d\n", target);
//	printf("Current steps:\n");
//	steps_stack_print(current_steps);
//	printf("Best steps:\n");
//	steps_stack_print(best_steps);
//	printf("Candidate steps:\n");
//	steps_stack_print(candidate_steps);
//	printf("Next steps:\n");
//	steps_stack_print(next_steps);
//	printf("\n\n");
//	}
	
 
