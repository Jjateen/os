#include <fcntl.h>    // For open()
#include <unistd.h>   // For read(), write(), lseek(), close()
#include <stdlib.h>   // For exit()

int main(int argc, char *argv[]) {
    if (argc != 2) {
        write(STDERR_FILENO, "Usage: ./file_read_skip <file_path>\n", 36);
        exit(1);
    }

    const char *file_path = argv[1];
    int fd;
    char buffer[10];  // Buffer to store 10 characters

    // Open the file in read-only mode
    fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        write(STDERR_FILENO, "Error opening file\n", 19);
        exit(1);
    }

    // Read the first 10 characters
    ssize_t bytesRead = read(fd, buffer, 10);
    if (bytesRead == -1) {
        write(STDERR_FILENO, "Error reading file\n", 19);
        close(fd);
        exit(1);
    }
    write(STDOUT_FILENO, "Read 1: ", 8);
    write(STDOUT_FILENO, buffer, bytesRead);
    write(STDOUT_FILENO, "\n", 1);

    // Skip the next 5 characters
    if (lseek(fd, 5, SEEK_CUR) == -1) {
        write(STDERR_FILENO, "Error seeking file\n", 19);
        close(fd);
        exit(1);
    }

    // Read the next 10 characters
    bytesRead = read(fd, buffer, 10);
    if (bytesRead == -1) {
        write(STDERR_FILENO, "Error reading file\n", 19);
        close(fd);
        exit(1);
    }
    write(STDOUT_FILENO, "Read 2: ", 8);
    write(STDOUT_FILENO, buffer, bytesRead);
    write(STDOUT_FILENO, "\n", 1);

    // Close the file
    close(fd);
    return 0;
}
