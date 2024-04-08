#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macros.h"
#include "line_utils.h"

/*creates and allocates an empty macro*/
struct macro *new_macro(char *name){
	struct macro *pt;/*a pointer to the new macro*/
	pt = malloc(sizeof(struct macro));
	if(pt == NULL)
		return NULL;
	pt -> name = strdup(name);
	if(pt -> name == NULL){
		free(pt);
		return NULL;
	}
	pt -> head = NULL;
	pt -> tail = NULL;
	return pt;
}

struct macro_line *new_macro_line(char *data){
	struct macro_line *pt;/*a pointer to the line*/
	pt = malloc(sizeof(struct macro_line));
	if(pt == NULL)
		return NULL;
	pt -> data = strdup(data);
	if(pt->data == NULL){
		free(pt);
		return NULL;
	}
	pt -> next = NULL;
	return pt;
}

/*returns a pointer to the next line, thus making freeing a full macro a fun task*/
struct macro_line *free_line(struct macro_line *pt){
	struct macro_line *next = pt -> next;
	free(pt -> data);
	free(pt);
	return next;
}

void set_next_macro_line(struct macro_line *curr, struct macro_line *next){
	curr->next = next;
}

char *get_line_data(struct macro_line *lpt){
	return lpt->data;
}

int add_macro_line(struct macro *mpt, char* line){
	struct macro_line *newLine;
	newLine = new_macro_line(line);
	if(newLine == NULL)
		return -1;/*failed allocation*/
	if(mpt->head == NULL){
		mpt->head = newLine;
		mpt->tail = newLine;
	}
	else{
		set_next_macro_line(mpt->tail, newLine);
		mpt->tail = newLine;
	}
	return 0;
}

void free_macro(struct macro *mpt){
	struct macro_line *lpt;
	free(mpt->name);
	lpt = mpt->head;
	while(lpt != NULL){
		lpt = free_line(lpt);
	}
	free(mpt);
}

void write_macro(struct macro *mpt, FILE *fp){
	struct macro_line *lpt = mpt->head;
	while(lpt != NULL){
		fputs(get_line_data(lpt),fp);
		lpt = lpt->next;
	}
}

char* get_macro_name(struct macro *mpt){
	return strdup(mpt->name);
}
