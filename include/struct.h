/* struct.h*/
#include <stdint.h>
#include <stdio.h>

struct tuple{
	uint64_t key;
	uint64_t payload;
};
struct relation{
	struct tuple* tuples;
	uint64_t num_tuples;
};
struct Stack{
	int size;
	struct Stack_node* top_node;
};
struct Stack_node{
	int start;
	int end;
	struct Stack_node* next;
};
struct buffer{
	uint64_t RowIdR;
	uint64_t RowIdS;
};
struct result{
	struct buffer* buffertable;
	int current_size;
	int max_size;
	struct result* next;
};
struct Stack_node* top(struct Stack* StackPtr);
void free_memory(struct relation* A,struct relation* B,struct result* ptr);
unsigned int hash_function(uint64_t  number, int number_of_byte);
void write_results(struct result* result_ptr);
struct result* SortMergeJoin(struct relation* relA,struct relation* relB);
void push(struct Stack** StackPtr,int start,int end);
void Stack_Initialization(struct Stack* StackPtr);
void pop(struct Stack** StackPtr);
int is_Empty(struct Stack* StackPtr);
void swap(uint64_t* a, uint64_t* b);
int partition (struct tuple* arr,int low,int high);
void quickSort(struct tuple* arr, int low, int high);
int sizeoftable();
struct relation* File_Relation(char* filename);
void Find_results(struct result* resultptr, struct relation* R, struct relation* S);
void sorttuples(struct tuple* tableR, struct tuple* tableRtonos, int start, int end, int inside_R,struct Stack* StackPtr);