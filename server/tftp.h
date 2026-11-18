#ifndef __TFTP_H__
#define __TFTP_H__

/*-------------------------------
  Standard Library Headers
--------------------------------*/
#include <stdio.h>      // for printf, fprintf, FILE operations
#include <stdlib.h>     // for exit(), malloc(), free()
#include <string.h>     // for strcpy, memset, strcmp
#include <unistd.h>     // for close(), read(), write()
#include <arpa/inet.h>  // for inet_ntoa(), inet_addr()
#include <sys/socket.h> // for socket APIs
#include <sys/types.h>
#include <errno.h>      // for error handling

/*-------------------------------
  Constant Definitions
--------------------------------*/
#define SERVER_PORT     6900         // Default server port number
#define MAX_BUFFER_SIZE 1024         // Buffer size for messages
#define BLOCK_SIZE      512          // Normal transfer block size
#define BYTE_MODE_SIZE  1            // 1-byte transfer mode block size

/*-------------------------------
  ENUM: For TFTP Operation Codes
--------------------------------*/
enum tftp_opcode {
    RRQ = 1,     // Read Request (Client → Server: request a file)
    WRQ,         // Write Request (Client → Server: upload a file)
    DATA,        // Data packet (used for file transfer)
    ACK,         // Acknowledgement (response for each data block)
    ERROR        // Error packet
};

/*-------------------------------
  ENUM: For Transfer Modes
--------------------------------*/
enum transfer_mode {
    NORMAL_MODE = 1,    // Standard 512 bytes per block
    BYTE_MODE,          // 1 byte per packet
    CRLF_MODE           // Convert \n → \r\n during transfer
};

/*-------------------------------
  STRUCTURE + UNION: TFTP Packet
--------------------------------
  → opcode decides which part of union is active
  → saves memory by using only one union member at a time
--------------------------------*/
typedef struct {
    unsigned short opcode;  // operation code (RRQ, WRQ, DATA, etc.)

    union {
        struct {
            char filename[256]; // file name to read or write
            char mode[12];      // mode type (normal / byte / crlf)
        } request;

        struct {
            unsigned short block;   // block number of the packet
            char data[BLOCK_SIZE];  // data buffer (512 bytes default)
        } data;

        struct {
            unsigned short block;   // block number being acknowledged
        } ack;

        struct {
            unsigned short errCode; // error code
            char errMsg[256];       // error message string
        } error;
    } type;
} TFTP_Packet;

/*-------------------------------
  COMMON FUNCTION DECLARATIONS
--------------------------------*/

// Parses command input to extract filename and transfer mode
void parse_filename_mode(const char *input, char *filename, int *mode);

// Helper function to display an error message and exit
void error_exit(const char *msg);

#endif // __TFTP_H__
