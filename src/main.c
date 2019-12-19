/* main.c*/
#include <stdio.h>
#include <stdlib.h>
#include "struct.h"
int main(int argc,char** argv){
	
	struct relation* relR=File_Relation(argv[1]);/*Save the Columns of Dataset1.txt with their payloads*/
	struct relation* relS=File_Relation(argv[2]);/*Save the Columns of Dataset2.txt with their payloads*/
	struct result* resultptr=SortMergeJoin(relR,relS);/*Sort Merge Join Implementation*/
	write_results(resultptr);/*Write to the result.csv the results*/
	free_memory(relR,relS,resultptr);/*Freeing dynamic allocated Memory*/
	return EXIT_SUCCESS;
}

