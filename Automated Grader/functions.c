#include <string.h>
#include <stdlib.h>
#include "functions.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

/*this function reallocates the ptr table*/
char *get_realloc (char *progname, int length) {
    char *help_ptr;
    
    help_ptr = (char*) realloc (progname, length);
    if (help_ptr == NULL) {
        perror("");
        exit(42);
    }
    return (help_ptr);
}

/*this function reads characters*/
int read_db (int fd, void *buf, int bytes) {
    int reading = 0, n; 

    do {    
        n = read (fd, &((char * )buf)[reading], bytes - reading);
        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }
            else {
                return (-1);
            }
        }
        if (n == 0) {
            return (reading);
        }
        reading = reading + n;

    } while (reading < bytes);
    return (reading);
}

void progname_init (arguments_t *args, char* argv[]) {
    info_t *assist_ptr;
    char  *help_ptr;

    assist_ptr = (info_t *) malloc (sizeof(info_t));
    if (assist_ptr == NULL) {
        free(args);
        exit(42);
    }
    args->progerr = assist_ptr;
    help_ptr =  (char *) malloc (sizeof(char));
    if (help_ptr == NULL) {
        free(args);
        free(assist_ptr);
        exit(42);
    }
    args->progerr->str = help_ptr;
    args->progerr->length = strlen(argv[1]) + 3;
    args->progerr->str = get_realloc(args->progerr->str, args->progerr->length);
    strncpy (args->progerr->str, argv[1], strlen(argv[1]) - 1);
    args->progerr->str[strlen(argv[1]) - 2] = '.';
    args->progerr->str[strlen(argv[1]) - 1] = 'e';
    args->progerr->str[strlen(argv[1])] = 'r';
    args->progerr->str[strlen(argv[1]) + 1] = 'r';
    args->progerr->str[strlen(argv[1]) + 2] = '\0';

    assist_ptr = (info_t *) malloc (sizeof(info_t));
    if (assist_ptr == NULL) {
        free(args);
        exit(42);
    }
    args->progname = assist_ptr;
    help_ptr =  (char *) malloc (sizeof(char));
    if (help_ptr == NULL) {
        free(args);
        free(assist_ptr);
        exit(42);
    }
    args->progname->str = help_ptr;
    args->progname->length = strlen(argv[1]) - 1;
    args->progname->str = get_realloc(args->progname->str, args->progname->length);
    strncpy (args->progname->str, argv[1], args->progname->length - 1);
    args->progname->str[args->progname->length - 1] = '\0';
}

int find_error (int searched, int fd) {
    int success;
    char c1, c2, if_error[ERROR];

    while (searched > 0) {

        success = read_db(fd, &c1, sizeof(char));
        if (success == -1) {
            return (42);
        }
        if (success == 0) {
            break;
        }
        if (c1 != ' ') {
            searched = searched - 1;
            continue;
        }
        success = read_db(fd, &c2, sizeof(char));
        if (success == -1) {
            return (42);
        }
        if (success == 0) {
            break;
        }
        if (c2 == 'e') {
            success = lseek (fd, (off_t) - 1, SEEK_CUR);
            if (success == -1) {
                perror("");
                return (42);
            }
            success = read_db(fd, if_error, sizeof(char) * ERROR);
            if (success == -1) {
                    return (42);
            }
            if_error [ERROR - 1] =  '\0';
            if (strcmp (if_error, "error:") == 0) {
                return(1);
            }
            else {
                success = lseek (fd, (off_t) - ERROR + 1, SEEK_CUR);
                if (success == -1) {
                    perror("");
                    return (42);
                }
            }
        }
        searched = searched - 1;
    }
    return(0);
}

int find_warning (int searched, int fd) {
    int success, found = 0;
    char c1, c2, if_warning[WARNING];

    while (searched > 0) {
        success = read_db(fd, &c1, sizeof(char));
        if (success == -1) {
            return (42);
        }
        if (success == 0) {
            break;
        }
        if (c1 != ' ') {
            searched = searched - 1;
            continue;
        }
        success = read_db(fd, &c2, sizeof(char));
        if (success == -1) {
            return (42);
        }
        if (success == 0) {
            break;
        }
        if (c2 == 'w') {
            success = lseek (fd, (off_t) - 1, SEEK_CUR);
            if (success == -1) {
                perror("");
                return (42);
            }
            success = read_db(fd, if_warning, sizeof(char) * WARNING);
            if (success == -1) {
                return (42);
            }
            if_warning [WARNING - 1] =  '\0';
            if (strcmp (if_warning, "warning:") == 0) {
                found += 1;
            }
            else {
                success = lseek (fd, (off_t) - ERROR + 1, SEEK_CUR);
                if (success == -1) {
                    perror("");
                    return (42);
                }
            }
        }
        searched = searched - 1;
    }  
    return(found);
}
int find_args (int fd, names_t *orismata) {
    int flag = 0, success, length = 1;
    char c;
    
    
    do {
        success = read_db (fd, &c, sizeof(char));
        if (success == 0) {
            flag = 1;
            length = length - 1;
            break;
        }
        if (success == -1) {
            perror ("");
            exit(42);
        }
        if (orismata->line_len > 1) {
            orismata->arg_names = get_realloc(orismata->arg_names, sizeof (char) * orismata->line_len );
        }
        if (c == ' ') {
            length++;
            orismata->line_len += 1;
            break;
        }
        orismata->arg_names[orismata->line_len - 1] = c;
        orismata->line_len += 1;
        length++;
    } while (orismata->arg_names[orismata->line_len - 2] != ' ');
    if (flag == 0) {
        orismata->arg_names[orismata->line_len - 2] = '\0';
    }
    else {
        orismata->arg_names[orismata->line_len - 1] = '\0';
    }
    return (length);
}

void make_args (int fdargs, names_t *orismata, char **transfer_ptr[], int *count) {
    int searched, length, success, i = 1;
    struct stat filesize;
    char **helpfull_ptr;

    success = fstat (fdargs, &filesize);
    if (success == -1) {
        perror ("");
        exit(42);
    }
    searched = filesize.st_size;
    i = 1;
    orismata->line_len = 1; 
    while (searched > 0) {
        length = find_args(fdargs, orismata);
        helpfull_ptr = (char**) realloc((*transfer_ptr), sizeof(char *) * (i + 1));
        if (helpfull_ptr == NULL) {
            exit(42);
        }
        *transfer_ptr = helpfull_ptr;
        *transfer_ptr[i] = &orismata->arg_names[orismata->line_len - length];
        searched = searched - strlen(orismata->arg_names) - 1;
        i = i + 1;
    }
    helpfull_ptr = (char**) realloc((*transfer_ptr), sizeof(char *) * (i + 1));
    if (helpfull_ptr == NULL) {
        exit(42);
    }
    *transfer_ptr = helpfull_ptr;
    *transfer_ptr[i] = NULL;
}
 
void get_free (arguments_t *args, names_t *orismata, char **transfer_ptr, int count) {
    int i;

    free(args->progerr->str);
    free(args->progerr);
    
    free(args->progname->str);
    free(args->progname);

    free(args);

    free(orismata->arg_names);

    for (i = 0; i < count ; i++) {
        free (transfer_ptr[i]);
    } 
    free (transfer_ptr);
}