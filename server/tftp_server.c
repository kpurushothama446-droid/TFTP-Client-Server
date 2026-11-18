#include "tftp.h"

/*--------------------------------------------------------------
 Function: handle_rrq()
 Purpose : Handles a READ request (client downloads file).
 Steps:
   1. Open requested file in read mode.
   2. Read data block by block.
   3. Send DATA packets to client.
   4. Wait for ACK after each DATA packet.
---------------------------------------------------------------*/
void handle_rrq(int sockfd, struct sockaddr_in *clientAddr, socklen_t addrLen, const char *filename, int mode)
{
    // Open the requested file for reading
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        perror("File not found");
        return;
    }

    printf("\n[Server] Client requested file: %s\n", filename);
    printf("[Server] Mode selected: %s\n", (mode == NORMAL_MODE) ? "NORMAL" :
                                         (mode == BYTE_MODE) ? "BYTE" : "CRLF");

    // Prepare packet variables
    TFTP_Packet packet, ack;
    uint16_t block = 1;

    // Set block size based on mode
    int blockSize = (mode == BYTE_MODE) ? BYTE_MODE_SIZE : BLOCK_SIZE;

    while (1)
    {
        // Read data from file into buffer
        int bytesRead = fread(packet.type.data.data, 1, blockSize, fp);

        // Set opcode and block number
        packet.opcode = htons(DATA);
        packet.type.data.block = htons(block);

        // CRLF mode: convert '\n' → '\r\n' before sending
        if (mode == CRLF_MODE)
        {
            char converted[BLOCK_SIZE * 2];
            int j = 0;

            for (int i = 0; i < bytesRead; i++)
            {
                if (packet.type.data.data[i] == '\n')
                    converted[j++] = '\r';
                converted[j++] = packet.type.data.data[i];
            }

            memcpy(packet.type.data.data, converted, j);
            bytesRead = j;
        }

        // Send DATA packet to client
        sendto(sockfd, &packet, 4 + bytesRead, 0,
               (struct sockaddr *)clientAddr, addrLen);

        // Wait for ACK from client
        recvfrom(sockfd, &ack, sizeof(ack), 0,
                 (struct sockaddr *)clientAddr, &addrLen);

        // If less than block size, it means end of file
        if (bytesRead < blockSize)
            break;

        block++;
    }

    fclose(fp);
    printf("[Server] File sent successfully: %s\n\n", filename);
}

/*--------------------------------------------------------------
 Function: handle_wrq()
 Purpose : Handles a WRITE request (client uploads file).
 Steps:
   1. Open or create file for writing.
   2. Send ACK 0 to client (ready to receive).
   3. Receive DATA packets from client.
   4. Send ACK for each block received.
---------------------------------------------------------------*/
void handle_wrq(int sockfd, struct sockaddr_in *clientAddr, socklen_t addrLen, const char *filename, int mode)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        perror("File open failed");
        return;
    }

    printf("\n[Server] Client uploading file: %s\n", filename);
    printf("[Server] Mode selected: %s\n", (mode == NORMAL_MODE) ? "NORMAL" :
                                         (mode == BYTE_MODE) ? "BYTE" : "CRLF");

    TFTP_Packet packet, ack;
    uint16_t expectedBlock = 1;
    int blockSize = (mode == BYTE_MODE) ? BYTE_MODE_SIZE : BLOCK_SIZE;

    // Send ACK 0 to tell client "ready to receive"
    ack.opcode = htons(ACK);
    ack.type.ack.block = htons(0);
    sendto(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *)clientAddr, addrLen);

    while (1)
    {
        // Receive data packet from client
        int recvBytes = recvfrom(sockfd, &packet, sizeof(packet), 0,
                                 (struct sockaddr *)clientAddr, &addrLen);

        // Error or end of communication
        if (recvBytes <= 0)
            break;

        // Verify correct packet type and block number
        if (ntohs(packet.opcode) == DATA && ntohs(packet.type.data.block) == expectedBlock)
        {
            int len = recvBytes - 4; // Subtract header bytes

            // CRLF mode: Convert "\r\n" back to "\n" before writing
            if (mode == CRLF_MODE)
            {
                for (int i = 0; i < len; i++)
                {
                    if (packet.type.data.data[i] == '\r' && packet.type.data.data[i + 1] == '\n')
                    {
                        fputc('\n', fp);
                        i++; // Skip '\n' part
                    }
                    else
                    {
                        fputc(packet.type.data.data[i], fp);
                    }
                }
            }
            else
            {
                // Normal or byte mode: write directly
                fwrite(packet.type.data.data, 1, len, fp);
            }

            // Send ACK for this block
            ack.opcode = htons(ACK);
            ack.type.ack.block = htons(expectedBlock);
            sendto(sockfd, &ack, sizeof(ack), 0,
                   (struct sockaddr *)clientAddr, addrLen);

            // If last packet is smaller than block size, file done
            if (len < blockSize)
                break;

            expectedBlock++;
        }
    }

    fclose(fp);
    printf("[Server] File received successfully: %s\n\n", filename);
}

/*--------------------------------------------------------------
 Function: main()
 Purpose : Entry point of the TFTP server program.
           It continuously waits for client requests (RRQ or WRQ)
           and handles them accordingly.
---------------------------------------------------------------*/
int main()
{
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    TFTP_Packet packet;

    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error_exit("socket failed");

    // Prepare the server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to port 6900
    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        error_exit("bind failed");

    printf("========================================\n");
    printf("      SIMPLE TFTP SERVER RUNNING        \n");
    printf("========================================\n");
    printf("Listening for clients on port %d...\n\n", SERVER_PORT);

    // Server loop — runs forever
    while (1)
    {
        // Wait for a packet from a client
        int recvBytes = recvfrom(sockfd, &packet, sizeof(packet), 0,
                                 (struct sockaddr *)&clientAddr, &addrLen);

        if (recvBytes <= 0)
            continue;

        // Convert opcode from network to host order
        uint16_t opcode = ntohs(packet.opcode);

        // Identify transfer mode from string
        int mode;
        if (strcmp(packet.type.request.mode, "normal") == 0)
            mode = NORMAL_MODE;
        else if (strcmp(packet.type.request.mode, "byte") == 0)
            mode = BYTE_MODE;
        else
            mode = CRLF_MODE;

        // Handle request based on opcode type
        if (opcode == RRQ)
        {
            handle_rrq(sockfd, &clientAddr, addrLen, packet.type.request.filename, mode);
        }
        else if (opcode == WRQ)
        {
            handle_wrq(sockfd, &clientAddr, addrLen, packet.type.request.filename, mode);
        }
        else
        {
            printf("[Server] Unknown request type received.\n");
        }
    }

    close(sockfd);
    return 0;
}
