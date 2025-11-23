#include "cifras_bt.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>	

// ADT stack implemented in cifras_bt.h as inline functions, except
// steps_stack_copy because it is more complex
	
void steps_stack_copy(SolutionStepStack* target, const SolutionStepStack* source)
	{
	int i;
	
	assert(source != NULL);
	assert(target != NULL);		
	assert(steps_stack_count(source) >= 0 && 
		steps_stack_count(source) <= MAX_SOLUTION_STEPS);
		
	if (steps_stack_is_empty(source))
		{
		steps_stack_init(target);
		return;
		}
	
	for (i = 0; i < source->count; i++)
		target->steps[i] = source->steps[i];
	
	target->count = source->count;
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
static void build_next_numbers(int* new_array, const int* former_array,
	int old_pos1, int old_pos2, int new)
	{
	int i, j;
	
	assert(former_array != NULL);
	assert(new_array != NULL);
	assert(old_pos1 >= 0);
	assert(old_pos1 < NUM_COUNT);
	assert(old_pos2 >= 0);
	assert(old_pos2 < NUM_COUNT);
	assert(new > 0);

	new_array[0] = new;
	
	j = 1;
	for (i = 0; i < NUM_COUNT; i++)
		if (i != old_pos1 && i != old_pos2)
			new_array[j++] = former_array[i];
	}
	
static void build_candidates_stack(SolutionStepStack* stack,
	int operand1, int operand2)
	{
	SolutionStep step;
	
	assert(stack != NULL);
	assert(operand1 > 0);
	assert(operand2 > 0);
		
	steps_stack_init(stack);
	
	// Add
	step = (SolutionStep){operand1 + operand2, operand1, operand2, '+'};
	steps_stack_push(stack, &step);
	
	// Substract (no action if operand1 == operand2)
	if (operand1 > operand2)
		step = (SolutionStep){operand1 - operand2, operand1, operand2, '-'};
	else if (operand2 > operand1)
		step = (SolutionStep){operand2 - operand1, operand2, operand1, '-'};
	if (operand1 != operand2)
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
	
	assert(steps_stack_count(stack) == 3 ||  steps_stack_count(stack) == 4);
	}

// Return true if the exact number has been already found and therefore a
// solution with more steps can never be better
static bool prunable_length(const SolutionStepStack* current_steps,
	const SolutionStepStack* best_steps, int target)
	{
	if (steps_stack_is_empty(current_steps))
		return false;
	assert(steps_stack_is_empty(best_steps) == false);

	if (steps_stack_result(best_steps) != target)
		return false;

	if (steps_stack_count(current_steps) < steps_stack_count(best_steps))
		return false;

	return true;
	}

// Calculation of an additional prune.
// True if all the following conditions are satisfied:
// 1. best_steps is not empty.
// 2. The highest value obtained by combining the pending numbers is smaller
// than the target.
// 3. The highest value obtained by combining the pending numbers is further
// from the target than the result of best_steps.
static bool prunable_upper_value(const int* numbers, int numbers_count,
	int target, const SolutionStepStack* best_steps)
	{
	unsigned long int upper_value = 1;
	unsigned long int upper_value_diff;
	int i, best_diff;
	
	assert(numbers != NULL);
	assert(numbers_count > 0);
	assert(best_steps != NULL);
	
	// No prune if best_steps is empty
	if (steps_stack_is_empty(best_steps))
		return false;

	for (i = 0; i < numbers_count; i++)
		{
		assert(numbers[i] >= 1);
		
		// Upper bound estimate: multiplying by 2 will never reach a value
		// smaller than any other one combining the number 1
		if (numbers[i] == 1)
			upper_value *= 2;
		else
			upper_value *= (unsigned long int)numbers[i];
		
		// No prune if upper_value is larger than target
		if (upper_value >= (unsigned long int)target)
			return false;
		}

	assert(upper_value < (unsigned long int)target);
	upper_value_diff = (unsigned long int)target - upper_value;
	best_diff = abs(steps_stack_result(best_steps) - target);
	// If upper_value_diff == best_diff, there must be no prune because the
	// current solution may be better than the best if its steps count is smaller
	return upper_value_diff > (unsigned long int)best_diff;
	}

static void cifras_bt(const int* numbers, int numbers_count,
	int target,
	const SolutionStepStack* current_steps, SolutionStepStack* best_steps) 
	{	
	int i, j;
	SolutionStep candidate;
	SolutionStepStack candidate_steps, next_steps;
	int next_numbers[NUM_COUNT];
	
	// If current_steps reaches a better result than best_steps, then
	// mirror current_steps into best_steps
	if (steps_stack_compare(current_steps, best_steps, target) == -1)
		steps_stack_copy(best_steps, current_steps);

	// Base cases: 
	// 1. Only 1 number pending, therefore no more combinations are possible
	assert(numbers_count > 0);
	if (numbers_count == 1)
		return;
	// 2. Prune if exact has been already found and the current steps count
	// is higher than the exact solution
	if (prunable_length(current_steps, best_steps, target))
		return;
	// 3. Prune is the upper value obtained by combining all the pending
	// numbers is smaller than the target AND is further from the target than
	// the result of best_steps
	if (prunable_upper_value(numbers, numbers_count, target, best_steps))
		return;

	// From here onwards, recursive case
	for (int i = 0; i < numbers_count; i++) 
		{
		for (int j = i + 1; j < numbers_count; j++)
			{
			// Stack candidate steps
			// Operands: numbers[i] and numbers[j]
			build_candidates_stack(&candidate_steps, numbers[i], numbers[j]);
				
			// Initialize next_steps with a copy of current_steps
			steps_stack_copy(&next_steps, current_steps);
			
			// Pop candidates one by one and make a recursive call with
			// everyone of them
			while (steps_stack_is_empty(&candidate_steps) == false)
				{
				// Push next candidate
				steps_stack_pop(&candidate_steps, &candidate);
				steps_stack_push(&next_steps, &candidate);
				
				// Create numbers array for the recursive call
				build_next_numbers(next_numbers, numbers, i, j, candidate.result);
				
				// Recursive call
				cifras_bt(next_numbers, numbers_count - 1, target,
					&next_steps, best_steps);
				
				// Restore next_steps. More than one candidate step must not
				// be pushed for the same recursive call
				steps_stack_pop(&next_steps, NULL);
				}
			}
		}
	}

// Wrapper
void resolve_cifras(const int* numbers, int target, SolutionStepStack* best_steps)
	{
	int ok;
	SolutionStepStack current_steps;
	
	assert(numbers != NULL);
	assert(target >= 0);
	assert(best_steps != NULL);
	
	steps_stack_init(&current_steps);
	steps_stack_init(best_steps);
	
	cifras_bt(numbers, NUM_COUNT, target, &current_steps, best_steps);
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
	
 
