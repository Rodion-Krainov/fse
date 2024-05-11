#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define RESET_COLOR "\033[0m"
#define DIR_COLOR "\033[1;34m"
#define FILE_COLOR "\033[0;37m"

void displayHelp() {
    printf("Usage of the program:\n");
    printf("  -H                        Show all files and directories in the current directory with additional info.\n");
    printf("  -S <file_name>            Search for file in the current directory and all its subdirectories.\n");
    printf("  -SF <file_name>           Search for file in the entire filesystem.\n");
    printf("  -C                        Print current directory name.\n");
    printf("  -CF                       Print current directory full path.\n");
    printf("  -U                        Move up one directory level.\n");
    printf("  -D <subdirectory_name>    Change to a specified subdirectory.\n");
    printf("  -help                     Provide detailed information about each command.\n");
}

void moveUpOneDirectory() {
    if (chdir("..") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Moved to upper directory: %s\n", cwd);
        } else {
            perror("getcwd() error");
        }
    } else {
        printf("You are already at the top level directory.\n");
    }
}

void changeToSubdirectory(char *subdir) {
    if (chdir(subdir) == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Changed to subdirectory: %s\n", cwd);
        } else {
            perror("getcwd() error");
        }
    } else {
        printf("No such subdirectory: %s\n", subdir);
    }
}

void listCurrentDirectory() {
    DIR *d;
    struct dirent *dir;
    struct stat fileStat;

    d = opendir(".");
    if (d) {
        printf("%-30s\t%-10s\t%-25s\n", "Name", "Size (Bytes)", "Last Modified");
        printf("---------------------------------------------------------------\n");

        while ((dir = readdir(d)) != NULL) {
            if (stat(dir->d_name, &fileStat) == 0) {
                // Determine whether it's a directory or a file
                const char *color = (dir->d_type == DT_DIR) ? DIR_COLOR : FILE_COLOR;
                printf("%s%-30s%s\t%-10ld\t%-25s", color, dir->d_name, RESET_COLOR, fileStat.st_size, ctime(&fileStat.st_mtime));
            }
        }
        closedir(d);
    } else {
        perror("Failed to open directory");
    }
}

void searchFile(const char *fileName, const char *startDir) {
    DIR *d;
    struct dirent *dir;
    char path[1024];
    int found = 0;

    if ((d = opendir(startDir)) != NULL) {
        while ((dir = readdir(d)) != NULL && !found) {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                snprintf(path, sizeof(path), "%s/%s", startDir, dir->d_name);
                if (dir->d_type == DT_DIR) {
                    searchFile(fileName, path);
                } else if (strcmp(dir->d_name, fileName) == 0) {
                    printf("File found: %s\n", path);
                    found = 1;
                    break;
                }
            }
        }
        closedir(d);
    }
}

void printCurrentDirectory() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current directory: %s\n", cwd);
    } else {
        perror("getcwd() error");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s -H | -S <filename> | -SF <filename> | -C | -CF\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-help") == 0) {
        displayHelp();
    } else if (strcmp(argv[1], "-H") == 0) {
        listCurrentDirectory();
    } else if (strcmp(argv[1], "-S") == 0 && argc > 2) {
        searchFile(argv[2], ".");
    } else if (strcmp(argv[1], "-SF") == 0 && argc > 2) {
        searchFile(argv[2], "/");
    } else if (strcmp(argv[1], "-C") == 0) {
        printf("Current directory name: %s\n", strrchr(getcwd(NULL, 0), '/') + 1);
    } else if (strcmp(argv[1], "-CF") == 0) {
        printCurrentDirectory();
    } else if (strcmp(argv[1], "-U") == 0) {
        moveUpOneDirectory();
    } else if (strcmp(argv[1], "-D") == 0 && argc > 2) {
        changeToSubdirectory(argv[2]);
    } else {
        fprintf(stderr, "Invalid arguments or insufficient number of arguments.\n");
        return 1;
    }

    return 0;
}
