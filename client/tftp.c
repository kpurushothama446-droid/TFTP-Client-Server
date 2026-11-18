#include "tftp.h"

/*--------------------------------------------------------------
 Function: parse_filename_mode()
 Purpose : Extract the filename and mode from user input.
 Example : Input string → "file1.txt normal"
           Output → filename = "file1.txt", mode = NORMAL_MODE
---------------------------------------------------------------*/
void parse_filename_mode(const char *input, char *filename, int *mode)
{
    // Temporary variable to hold the mode string
    char m[12];

    // Read filename and mode from input
    // Example: "test.txt byte" → filename = test.txt, m = byte
    sscanf(input, "%s %s", filename, m);

    // Compare mode string and assign corresponding enum value
    if (strcmp(m, "normal") == 0)
        *mode = NORMAL_MODE;
    else if (strcmp(m, "byte") == 0)
        *mode = BYTE_MODE;
    else if (strcmp(m, "crlf") == 0)
        *mode = CRLF_MODE;
    else
        *mode = NORMAL_MODE;  // Default if user types wrong input
}

/*--------------------------------------------------------------
 Function: error_exit()
 Purpose : Print an error message and terminate the program.
---------------------------------------------------------------*/
void error_exit(const char *msg)
{
    perror(msg);   // Print the system-generated error message
    exit(EXIT_FAILURE);
}
