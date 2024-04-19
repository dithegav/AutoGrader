#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "functions.h"
#include <sys/wait.h>
#define DIFF_SIZE 9

int main (int argc, char *argv[]) {
    arguments_t *args, *help_ptr;
    int pid1, pid2, pid3, fd, fd2, fdargs, pipe_des[2],success, status, searched, errored = 0, found = 0, grade_reduction = 0, count = 1;
    struct stat filesize;
    char *hptr, **helpfull_ptr, **transfer_ptr;
    names_t orismata;
    const char p4diff[DIFF_SIZE] = {'p', '4', 'd', 'i', 'f', 'f', '.', 'c', '\0'};

    if ((argc != 6) || (atoi(argv[5]) <= 0)) {
        perror ("");
        return (1);
    }

    help_ptr = (arguments_t *) malloc (sizeof(arguments_t));
    if (help_ptr == NULL) {
        return (42);
    }
    args = help_ptr;

    progname_init (args, argv);
    printf("%s %s\n", args->progerr->str, args->progname->str);

    fd = open (args->progerr->str, O_RDWR | O_CREAT | O_TRUNC , 0700);
    if (fd == -1) {
        free (args);
        perror("");
        return (42);
    }
    pid1 = fork();
    if (pid1 == -1) {
        perror("");
        free (args);
        return(42);
    }
    if (pid1 == 0) {
        dup2 (fd, STDERR_FILENO);
        close(fd);
        success = execlp ("gcc", "gcc", "-Wall", argv[1], "-o", args->progname->str,(char *) NULL);
        if (success == -1) {
            free (args);
            perror("");
            return (42);
        }
    }
    waitpid (pid1, &status, 0);
    success = lseek (fd, 0, SEEK_SET);
    if (success == -1) {
        free (args);
        perror("");
        return (42);
    }
    success = fstat (fd, &filesize);
    if (success == -1) {
        free (args);
        return (42);
    }
    searched = filesize.st_size;
    
    errored = find_error(searched, fd);
    if (errored == 0) {
        success = lseek (fd, 0, SEEK_SET);
        if (success == -1) {
            free (args);
            perror("");
            return (42);
        }
        searched = filesize.st_size;
        found = find_warning(searched, fd);
    }
    printf ("error: %d, found: %d\n", errored, found);
    if (errored == 1) {
        grade_reduction = -100;
        printf ("score:%d\n", grade_reduction);
    }
    else {
        grade_reduction = found * (-5);
        printf ("score:%d\n", grade_reduction);
    }

    fdargs = open (argv[2], O_RDWR);
    if (fdargs == -1) {
        perror ("");
        exit(42);
    }
    hptr = (char *) malloc (sizeof(char));
    if (help_ptr == NULL) {
        free(args);
        exit(42);
    }
    orismata.arg_names = hptr;

    helpfull_ptr = (char **) malloc (sizeof (char*));
    if (helpfull_ptr == NULL) {
        return(42);
    }
    transfer_ptr = helpfull_ptr;

    transfer_ptr[0] = args->progname->str;
    make_args (fdargs, &orismata, &transfer_ptr, &count);
    
    for (success = 0; success < count; success++) {
        printf ("%s\n", transfer_ptr[success]);
    }

    execv (args->progname->str, transfer_ptr);
    perror("");
    get_free(args, &orismata, transfer_ptr, count);

    return (0);
}