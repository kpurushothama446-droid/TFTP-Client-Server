/*====================================================================
    FILE: tftp_client.h
    PURPOSE: Header file for TFTP client
=====================================================================*/

#ifndef __TFTP_CLIENT_H__
#define __TFTP_CLIENT_H__

#include "tftp.h"

// Function prototypes for client operations
void send_request(int sockfd, struct sockaddr_in *serverAddr, const char *filename, int opcode, int mode);
void handle_get(int sockfd, struct sockaddr_in *serverAddr, const char *filename, int mode);
void handle_put(int sockfd, struct sockaddr_in *serverAddr, const char *filename, int mode);

#endif
