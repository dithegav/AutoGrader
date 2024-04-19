#ifndef _PROJECT4_
#define _PROJECT4_

#define ERROR 7
#define WARNING 9
typedef struct {
    char *str;
    int length;
}info_t;

typedef struct {
    info_t *progerr;
    info_t *progname;
    int timeout;
} arguments_t;

typedef struct {
    char *arg_names;
    int line_len;
} names_t;

void progname_init (arguments_t *args, char * argv[]);

int find_error (int searched, int fd);

int find_warning (int searched, int fd);

void make_args ( int fdargs, names_t *orismata, char **transfer_ptr[], int *count);

int find_args (int fd ,names_t *orismata);

void get_free (arguments_t * args, names_t *orismata, char ** transfer_ptr, int count);
#endif 
