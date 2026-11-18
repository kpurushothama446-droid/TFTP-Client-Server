#include "tftp_client.h"

/*--------------------------------------------------------------
 Function: send_request()
 Purpose : Send RRQ (read) or WRQ (write) request to the server
---------------------------------------------------------------*/
void send_request(int sockfd, struct sockaddr_in *serverAddr, const char *filename, int opcode, int mode)
{
    TFTP_Packet packet;
    memset(&packet, 0, sizeof(packet));

    packet.opcode = htons(opcode);
    strcpy(packet.type.request.filename, filename);

    if (mode == NORMAL_MODE)
        strcpy(packet.type.request.mode, "normal");
    else if (mode == BYTE_MODE)
        strcpy(packet.type.request.mode, "byte");
    else
        strcpy(packet.type.request.mode, "crlf");

    sendto(sockfd, &packet, sizeof(packet), 0,
           (struct sockaddr *)serverAddr, sizeof(*serverAddr));
}

/*--------------------------------------------------------------
 Function: handle_get()
 Purpose : Download a file from the server
---------------------------------------------------------------*/
void handle_get(int sockfd, struct sockaddr_in *serverAddr, const char *filename, int mode)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        printf("Error: Cannot open file for writing.\n");
        return;
    }

    send_request(sockfd, serverAddr, filename, RRQ, mode);

    TFTP_Packet packet, ack;
    socklen_t addrLen = sizeof(*serverAddr);
    int blockSize = (mode == BYTE_MODE) ? BYTE_MODE_SIZE : BLOCK_SIZE;

    printf("\n[Client] Downloading '%s' from server...\n", filename);

    while (1)
    {
        int recvBytes = recvfrom(sockfd, &packet, sizeof(packet), 0,
                                 (struct sockaddr *)serverAddr, &addrLen);
        if (recvBytes <= 0)
            break;

        if (ntohs(packet.opcode) == DATA)
        {
            int dataLen = recvBytes - 4;

            // CRLF mode: convert "\r\n" → "\n"
            if (mode == CRLF_MODE)
            {
                for (int i = 0; i < dataLen; i++)
                {
                    if (packet.type.data.data[i] == '\r' &&
                        packet.type.data.data[i + 1] == '\n')
                    {
                        fputc('\n', fp);
                        i++;
                    }
                    else
                    {
                        fputc(packet.type.data.data[i], fp);
                    }
                }
            }
            else
            {
                fwrite(packet.type.data.data, 1, dataLen, fp);
            }

            // Send ACK
            ack.opcode = htons(ACK);
            ack.type.ack.block = packet.type.data.block;
            sendto(sockfd, &ack, sizeof(ack), 0,
                   (struct sockaddr *)serverAddr, addrLen);

            if (dataLen < blockSize)
                break;
        }
    }

    fclose(fp);
    printf("[Client] Download completed.\n\n");
}

/*--------------------------------------------------------------
 Function: handle_put()
 Purpose : Upload a file to the server
---------------------------------------------------------------*/
void handle_put(int sockfd, struct sockaddr_in *serverAddr, const char *filename, int mode)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        printf("Error: Cannot open file for reading.\n");
        return;
    }

    send_request(sockfd, serverAddr, filename, WRQ, mode);

    TFTP_Packet packet, ack;
    uint16_t block = 1;
    socklen_t addrLen = sizeof(*serverAddr);
    int blockSize = (mode == BYTE_MODE) ? BYTE_MODE_SIZE : BLOCK_SIZE;

    printf("\n[Client] Uploading '%s' to server...\n", filename);

    while (1)
    {
        int bytesRead = fread(packet.type.data.data, 1, blockSize, fp);
        packet.opcode = htons(DATA);
        packet.type.data.block = htons(block);

        // CRLF conversion (\n → \r\n)
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

        sendto(sockfd, &packet, 4 + bytesRead, 0,
               (struct sockaddr *)serverAddr, addrLen);

        recvfrom(sockfd, &ack, sizeof(ack), 0,
                 (struct sockaddr *)serverAddr, &addrLen);

        if (bytesRead < blockSize)
            break;

        block++;
    }

    fclose(fp);
    printf("[Client] Upload completed.\n\n");
}

/*--------------------------------------------------------------
 Function: main()
 Purpose : Display a menu and perform chosen client operations
---------------------------------------------------------------*/
int main()
{
    int sockfd;
    struct sockaddr_in serverAddr;
    char filename[256];
    char server_ip[64];
    int mode = NORMAL_MODE;
    int choice;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf("Error creating socket.\n");
        return 1;
    }

    // Initialize server address (default empty)
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);

    printf("========================================\n");
    printf("      SIMPLE MENU-DRIVEN TFTP CLIENT     \n");
    printf("========================================\n");

    while (1)
    {
        printf("\n------------ MAIN MENU ------------\n");
        printf("1. Connect to Server\n");
        printf("2. Upload File (PUT)\n");
        printf("3. Download File (GET)\n");
        printf("4. Change Transfer Mode\n");
        printf("5. Exit\n");
        printf("-----------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // clear newline from input buffer

        switch (choice)
        {
        case 1:
            printf("Enter Server IP: ");
            scanf("%s", server_ip);
            getchar();

            serverAddr.sin_addr.s_addr = inet_addr(server_ip);
            printf("[Client] Connected to server %s:%d\n",
                   server_ip, SERVER_PORT);
            break;

        case 2:
            printf("Enter filename to upload: ");
            scanf("%s", filename);
            getchar();
            handle_put(sockfd, &serverAddr, filename, mode);
            break;

        case 3:
            printf("Enter filename to download: ");
            scanf("%s", filename);
            getchar();
            handle_get(sockfd, &serverAddr, filename, mode);
            break;

        case 4:
            printf("\nSelect Transfer Mode:\n");
            printf("1. Normal (512 bytes)\n");
            printf("2. Byte (1 byte)\n");
            printf("3. CRLF (Convert \\n → \\r\\n)\n");
            printf("Enter choice: ");
            scanf("%d", &choice);
            getchar();

            if (choice == 1)
                mode = NORMAL_MODE;
            else if (choice == 2)
                mode = BYTE_MODE;
            else
                mode = CRLF_MODE;

            printf("[Client] Mode changed to: %s\n",
                   (mode == NORMAL_MODE) ? "NORMAL" :
                   (mode == BYTE_MODE) ? "BYTE" : "CRLF");
            break;

        case 5:
            printf("Exiting client...\n");
            close(sockfd);
            return 0;

        default:
            printf("Invalid choice! Try again.\n");
        }
    }

    close(sockfd);
    return 0;
}
