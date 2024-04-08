struct macro_line{
	char *data;
	struct macro_line *next;
};

struct macro{
	char *name;
	struct macro_line *head;
	struct macro_line *tail;
};

/*creates and allocates an empty macro*/
struct macro *new_macro(char *name);

/*creates and allocates a macro_line with data as its content*/
struct macro_line *new_macro_line(char *data);

/*returns a pointer to the next line, thus making freeing a full macro a fun task*/
struct macro_line *free_line(struct macro_line *pt);

/*sets the next field of a macro_line to be @param next*/
void set_next_macro_line(struct macro_line *curr, struct macro_line *next);

char *get_line_data(struct macro_line *lpt);

/*adds a macro_line to the end of a macro*/
int add_macro_line(struct macro *mpt, char* line);

/*frees allocated macro and its lines*/
void free_macro(struct macro *pt);

/*writes a macro into a file*/
void write_macro(struct macro *pt, FILE *fp);

char* get_macro_name(struct macro *mpt);
