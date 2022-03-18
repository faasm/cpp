#include "faasm/files.h"

#include <cstring>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

#define FILE_PATH "faasm://some_dir/test_file.txt"
#define FILE_DIR "faasm://some_dir"

int main(int argc, char* argv[])
{
    // Create dir if not exists
    struct stat st = { 0 };
    if (::stat(FILE_DIR, &st) == -1) {
        ::mkdir(FILE_DIR, 0700);
    }

    // Delete file if it exists
    FILE* check = fopen(FILE_PATH, "r");
    if (check != nullptr) {
        ::fclose(check);
        printf("Shared file at %s exists\n", FILE_PATH);
        ::remove(FILE_PATH);

        // Confirm file doesn't exist
        check = ::fopen(FILE_PATH, "r");
        if (check != nullptr) {
            printf("Shared file at %s exists after deletion\n", FILE_PATH);
            return 1;
        }
    }

    // Open the file for writing
    FILE* wp = ::fopen(FILE_PATH, "w");
    if (wp == nullptr) {
        printf("Could not open shared file for writing at %s\n", FILE_PATH);
        return 1;
    }

    // Write some data to the file
    const char* contents = "This is some dummy content";
    ::fprintf(wp, "%s", contents);
    ::fclose(wp);

    // Read in the data and check
    const char* actual = faasm::readFileToString(FILE_PATH);
    if (::strcmp(actual, contents) != 0) {
        printf("File contents not as expected: %s != %s\n", actual, contents);
        return 1;
    }

    return 0;
}
