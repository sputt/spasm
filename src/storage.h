#ifndef __STORAGE_H
#define __STORAGE_H

#include "list.h"
#include "hash.h"

#if !defined(MAX_PATH) && !defined(_MSC_VER)
	#include <limits.h>
	#define MAX_PATH PATH_MAX
#endif

#define MAX_ARGS 16	//max number of args macros can have
#define MAX_LABELS 7919 //prime number for hash table
#define MAX_DEFINES 7919 //prime number for hash table

//used for defines and macros
typedef struct define {
	char *name;
	int line_num;
	char *input_file;
	char *contents;
	int num_args;
	char *args[MAX_ARGS];
} define_t;

typedef struct label {
	char *name;
	int line_num;
	char *input_file;
	int value;
} label_t;

typedef struct common_store {
	char *name;
	int line_num;
	char *input_file;
} common_store_t;

void write_labels (char *filename);
void init_storage ();
EXPORT void free_storage();
define_t *add_define (char *name, bool *redefined, bool search_local = true);
define_t *search_defines (const char *name, bool search_local = true);
define_t *search_local_defines (const char *name);
void remove_define (char *name);
void set_define (define_t *define, const char *str, int len, bool redefined);
int get_num_defines ();

label_t *add_label (char *name, int value);
label_t *search_labels (const char *name);
int get_num_labels ();

unsigned int search_reusables(int index);
int get_curr_reusable();
int set_curr_reusable(int index);
void add_reusable();
int get_num_reusables();

list_t *add_arg_set ();
void add_arg (char *name, char *value, list_t *arg_set);
void remove_arg_set (list_t *arg_set);

void set_case_sensitive(bool sensitive);
bool get_case_sensitive();
void dump_defines();

extern hash_t *label_table;

#endif
