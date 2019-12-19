/*struct.c*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "struct.h"
unsigned int hash_function(uint64_t  number, int number_of_byte){/*Function that return  specific byte of number*/
	int x = (number>> (8*number_of_byte)) & 0xff;
	return x;
}
void free_memory(struct relation* A,struct relation* B,struct result* ptr){
	struct result* next_node=ptr->next;
	while(1){
		free(ptr->buffertable);/*free the memory of table of result*/
		free(ptr);/* free the memory of struct result*/
		ptr=next_node;
		if(ptr==NULL)
			break;
		next_node=next_node->next;
	}
	/*Free the struct relation nodes*/
	free(A->tuples);///
	free(B->tuples);//
	free(A);//
	free(B);//
}
struct Stack_node* top(struct Stack* StackPtr){/*return the top element of stack*/
	return StackPtr->top_node;
};
struct result* SortMergeJoin(struct relation* relA,struct relation* relB){
	struct Stack * StackPtr=malloc(sizeof(struct Stack));/*Create the stack which be used*/
	struct tuple* A_Table = malloc(relA->num_tuples*sizeof(struct tuple));
	Stack_Initialization(StackPtr);
	push(&StackPtr,0,relA->num_tuples);/*Push on stack the beg-end of Table*/
	sorttuples(relA->tuples,A_Table,0,relA->num_tuples,1,StackPtr);/*Sort the relA tuples*/
	free(A_Table);
	struct tuple* B_Table = malloc(relB->num_tuples*sizeof(struct tuple));
	Stack_Initialization(StackPtr);
	push(&StackPtr,0,relB->num_tuples);
	sorttuples(relB->tuples,B_Table,0,relB->num_tuples,1,StackPtr);
	free(B_Table);
	free(StackPtr);
	struct result* result_ptr=malloc(sizeof(struct result));/*First maybe the only table of results*/
	result_ptr->current_size=0;
	result_ptr->max_size=sizeoftable();
	result_ptr->buffertable=malloc(result_ptr->max_size*sizeof(struct buffer));
	Find_results(result_ptr,relA,relB);
	return result_ptr;
}
void push(struct Stack** StackPtr,int start,int end){/*Function that push on top of Stack a node*/
	struct Stack_node* node=malloc(sizeof(struct Stack_node));
	node->start=start;
	node->next=NULL;
	node->end=end;
	if((*StackPtr)->top_node==NULL)/*Stack is empty*/
		(*StackPtr)->top_node=node;
	else{
		node->next=(*StackPtr)->top_node;
		(*StackPtr)->top_node=node;
	} 
	(*StackPtr)->size++;
}
void Stack_Initialization(struct Stack* StackPtr){
	StackPtr->top_node=NULL;
	StackPtr->size=0;
}
void pop(struct Stack** StackPtr){/*Pop the top element of Stack*/
	if(is_Empty(*StackPtr))
		return ;
	struct Stack_node* del_node=(*StackPtr)->top_node;
	(*StackPtr)->top_node=(*StackPtr)->top_node->next;
	(*StackPtr)->size--;
	free(del_node);
}
int is_Empty(struct Stack* StackPtr){/*Function that check if Stack is empty*/
	return StackPtr->size==0;
}
void write_results(struct result* result_ptr){
	FILE* f=fopen("result.csv","w");
	while(result_ptr!=NULL){
		for(int i=0;i<result_ptr->current_size;i++)
			fprintf(f,"%lu,%lu\n",result_ptr->buffertable[i].RowIdR,result_ptr->buffertable[i].RowIdS);
		result_ptr=result_ptr->next;
		if(result_ptr==NULL)
			break;
	}
	fclose(f);
}
void swap(uint64_t* a, uint64_t* b){
    uint64_t t=*a; 
    *a=*b; 
    *b=t; 
}  
int partition (struct tuple* arr,int low,int high){
	uint64_t pivot = arr[high].key;
	int i = (low-1);
	for(int j = low;j <= high-1; j++){
		if(arr[j].key <= pivot){
			i++;
			swap(&arr[i].key, &arr[j].key);
			swap(&arr[i].payload, &arr[j].payload);
		}
	}
	swap(&arr[i+1].key, &arr[high].key);
	swap(&arr[i+1].payload, &arr[high].payload);
	return (i+1);
}

void quickSort(struct tuple* arr, int low, int high){
	 
  if(low < high){ 
    int pi = partition(arr,low,high); 
  	quickSort(arr, low, pi-1);
    quickSort(arr, pi+1, high); 
  }  
}

int sizeoftable(){	
	int arxiko = 1000000-sizeof(struct result*)-2*sizeof(int);
	int size = arxiko/sizeof(struct buffer);
	return size;
}
struct relation* File_Relation(char* filename){
	FILE* f;
	f=fopen(filename,"r");
	struct relation* relation_node=malloc(sizeof(struct relation));
	uint64_t payload,key;
	relation_node->num_tuples=0;
	while(fscanf(f,"%lu,%lu",&payload,&key)!=EOF)
		relation_node->num_tuples++;
	relation_node->tuples=malloc(relation_node->num_tuples*sizeof(struct tuple));
	int size=0;
	rewind(f);
	while(fscanf(f,"%lu,%lu",&key,&payload)!=EOF){
		relation_node->tuples[size].key=key;
		relation_node->tuples[size++].payload=payload;
	}
	fclose(f);
	return relation_node;
}
void Find_results(struct result* resultptr, struct relation* R, struct relation* S){
	int pointer2 = 0;
	int pointer3 = 0;
	uint64_t previousi=0;
	struct result* current=resultptr;
	for(int i=0;i<R->num_tuples;){
		if(R->tuples[i].key<S->tuples[pointer3].key){
			i++;
			continue;
		}
		else if(R->tuples[i].key>S->tuples[pointer2].key){
			pointer3++;
			pointer2++;
			continue;
		}
		else{
			if(R->tuples[i].key==previousi){
				for(int j=pointer2;j <= pointer3; j++){
					if(R->tuples[i].key == S->tuples[j].key){
						
						if(current->current_size < current->max_size){
							current->buffertable[current->current_size].RowIdR = R->tuples[i].payload;
							current->buffertable[current->current_size++].RowIdS = S->tuples[j].payload;
						}
						else{
							struct result* newnode = malloc(sizeof(struct result));
							
							newnode->next = NULL;
							newnode->max_size = sizeoftable();
							newnode->buffertable = malloc(newnode->max_size*sizeof(struct buffer));
							current->next = newnode;
							current = newnode;
							current->current_size = 0;
							
							current->buffertable[current->current_size].RowIdR = R->tuples[i].payload;
							current->buffertable[current->current_size++].RowIdS = S->tuples[j].payload;
						}
					}
				}
			}
			else{
				int j=pointer2;
				while(1){
					if(R->tuples[i].key == S->tuples[j].key){
						
						if(current->current_size < current->max_size){
							current->buffertable[current->current_size].RowIdR = R->tuples[i].payload;
							current->buffertable[current->current_size++].RowIdS = S->tuples[j].payload;
						}
						else{
							struct result* newnode = malloc(sizeof(struct result));
							
							newnode->next = NULL;
							newnode->max_size = sizeoftable();
							newnode->buffertable = malloc(newnode->max_size*sizeof(struct buffer));
							current->next = newnode;
							current = newnode;
							current->current_size = 0;
							
							current->buffertable[current->current_size].RowIdR = R->tuples[i].payload;
							current->buffertable[current->current_size++].RowIdS = S->tuples[j].payload;
						}
					}
					else{
						pointer3=j-1;
						break;
					}
					j++;
				}
			}
		}
		previousi=R->tuples[i].key;
		i++;
	}
}

void sorttuples(struct tuple* tableR, struct tuple* tableRtonos, int start, int end, int inside_R,struct Stack* StackPtr){
	int byte_for_hashing=0;
	while(1){
		if(is_Empty(StackPtr))
			return ;
		struct Stack_node* stackptr=top(StackPtr);
		int start=stackptr->start;
		int end=stackptr->end;
		int size = end-start;
		int position;
		if(size*sizeof(struct tuple) < 64000){
			if(inside_R == 1)
				quickSort(tableR, start, end-1);
			else{
				quickSort(tableRtonos, start, end-1);
				
				for(int i = start; i < end; i++){
					struct tuple node;
					
					node.key = tableRtonos[i].key;
					node.payload = tableRtonos[i].payload;
					memcpy(&tableR[i], &node, sizeof(struct tuple));
				}
			}
			pop(&StackPtr);
			continue;
		}
		pop(&StackPtr);
		int* Hist = malloc(256*sizeof(int));
		for(int i = 0; i < 256; i++)
			Hist[i] = 0;
		for(int i=start;i<end;i++){
			if(inside_R==1)
				position=hash_function(tableR[i].key,byte_for_hashing);
			else
				position=hash_function(tableRtonos[i].key,byte_for_hashing);
			Hist[position]++;
		}
		int* Psum = malloc(256*sizeof(int));
		Psum[0] = start;
		int min = 0;
		for(int i = 1; i < 256; i++){
			if(!Hist[i]){
				Psum[i] = -1;
				continue;
			}
			Psum[i] = Hist[min]+Psum[min];
			min = i;
		}
		if(Hist[0]==0)
			Psum[0]=-1;
		int* pos = malloc(256*sizeof(int));
		for(int i = 0; i < 256; i++)
			pos[i] = Psum[i];
		for(int i = start; i < end; i++){
			struct tuple node;
			int bucket;
			
			if(inside_R == 1){
				node.key = tableR[i].key;
				node.payload = tableR[i].payload;
				bucket = hash_function(node.key, byte_for_hashing);
				memcpy(&tableRtonos[pos[bucket]], &node, sizeof(struct tuple));
			}
			else{
				node.key = tableRtonos[i].key;
				node.payload = tableRtonos[i].payload;
				bucket = hash_function(node.key, byte_for_hashing);
				memcpy(&tableR[pos[bucket]], &node, sizeof(struct tuple));
			}
			pos[bucket]++;
		}
		for(int i = 0; i < 256; i++){
			if(Psum[i] == -1||Hist[i]==0)
				continue;
			int start = Psum[i];
			int end = Psum[i]+Hist[i];
			push(&StackPtr,start,end);
		}
		inside_R*=-1;
		free(Psum);
		free(Hist);
		byte_for_hashing++;
		free(pos);
	}
}