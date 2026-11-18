# 🌐 TFTP CLIENT–SERVER IMPLEMENTATION
### Network Programming | UDP Socket Communication | Embedded Linux Concepts

<div align="center">

<img src="https://readme-typing-svg.demolab.com?font=Fira+Code&weight=700&size=22&pause=1000&color=00E5FF&center=true&vCenter=true&width=800&lines=Building+Network+Applications+in+C;Client-Server+Communication;UDP+Socket+Programming;DATA+%E2%86%94+ACK+%E2%86%94+DATA;Understanding+Network+Protocols+Through+Code" alt="Animated TFTP project title"/>

<br/>

<img src="https://img.shields.io/badge/Language-C-00599C?style=for-the-badge&logo=c"/>
<img src="https://img.shields.io/badge/Networking-UDP-00A98F?style=for-the-badge"/>
<img src="https://img.shields.io/badge/Programming-Socket%20API-3949AB?style=for-the-badge"/>
<img src="https://img.shields.io/badge/Platform-Linux-FCC624?style=for-the-badge&logo=linux"/>
<img src="https://img.shields.io/badge/Project-Network%20Programming-7B61FF?style=for-the-badge"/>

**PURUSHOTHAMA K | EMBEDDED SOFTWARE ENGINEER ASPIRANT**

</div>

---

## `01 / PROJECT OVERVIEW`

### Understanding File Transfer Through Network Programming

This project implements a **Trivial File Transfer Protocol (TFTP) client-server application using C programming and socket communication**.

The application demonstrates how two independent processes communicate over a network to exchange file data using UDP.

Instead of relying on high-level networking libraries, the implementation focuses on fundamental network programming concepts such as socket creation, message formatting, data transmission, acknowledgements and file handling.

### Core Engineering Objectives

- Understand client-server architecture.
- Implement UDP socket communication.
- Create and process protocol messages.
- Handle DATA and ACK packets.
- Understand network byte-order conversion.
- Support different file transfer modes.
- Handle file data and transfer operations.

---

## `02 / SYSTEM ARCHITECTURE`

### Client-Server Communication Model

```text
        CLIENT APPLICATION
        ┌───────────────────┐
        │                   │
        │  User Request     │
        │  File Selection   │
        │  Transfer Mode    │
        │                   │
        └─────────┬─────────┘
                  │
                  ▼
        ┌───────────────────┐
        │   UDP SOCKET      │
        │                   │
        │  Request Packet   │
        └─────────┬─────────┘
                  │
                  │
══════════════════╪══════════════════
                  │
              UDP NETWORK
                  │
══════════════════╪══════════════════
                  │
                  ▼
        ┌───────────────────┐
        │   UDP SOCKET      │
        │                   │
        │  SERVER PROCESS   │
        │                   │
        │  Packet Parsing   │
        │  File Processing  │
        │  ACK Generation   │
        │                   │
        └─────────┬─────────┘
                  │
                  ▼
        ┌───────────────────┐
        │   FILE STORAGE    │
        │                   │
        │  Read / Write     │
        │  Data Processing  │
        └───────────────────┘
```

**Architecture:** Application-level client-server communication over UDP sockets.

---

## `03 / HOW TFTP COMMUNICATION WORKS`

### Understanding the DATA and ACK Cycle

The TFTP protocol uses request, data and acknowledgement messages to manage file transfers.

```text
       CLIENT                         SERVER
          |                              |
          | -------- REQUEST ----------> |
          |                              |
          | <------- DATA 1 ------------ |
          |                              |
          | -------- ACK 1 ------------> |
          |                              |
          | <------- DATA 2 ------------ |
          |                              |
          | -------- ACK 2 ------------> |
          |                              |
          | <------- DATA 3 ------------ |
          |                              |
          | -------- ACK 3 ------------> |
          |                              |
          |         TRANSFER             |
          |          COMPLETE            |
```

### Communication Sequence

**STEP 01 — REQUEST**

The client initiates a file transfer request by sending the appropriate protocol request.

**STEP 02 — DATA TRANSMISSION**

The server processes the request and sends file data in numbered DATA blocks.

**STEP 03 — ACKNOWLEDGEMENT**

The receiving side sends an acknowledgement for the received data block.

**STEP 04 — NEXT DATA BLOCK**

The sender proceeds with the next block after receiving the appropriate acknowledgement.

**STEP 05 — TRANSFER COMPLETION**

The final data block is identified according to the TFTP block-size rules.

*The exact supported request directions and transfer behavior should match your implementation.*

---

## `04 / TFTP PACKET STRUCTURE`

TFTP uses structured packets containing operation codes and relevant fields.

| Packet | Purpose |
|---|---|
| RRQ | Read request |
| WRQ | Write request |
| DATA | File data transmission |
| ACK | Data acknowledgement |
| ERROR | Error reporting |

### DATA Packet Concept

```text
┌──────────────┬──────────────┬───────────────────┐
│ Opcode       │ Block Number │ Data              │
│ 2 Bytes      │ 2 Bytes      │ Up to 512 Bytes   │
└──────────────┴──────────────┴───────────────────┘
```

### ACK Packet Concept

```text
┌──────────────┬──────────────┐
│ Opcode       │ Block Number │
│ 2 Bytes      │ 2 Bytes      │
└──────────────┴──────────────┘
```

These are the standard TFTP packet formats.

---

## `05 / FILE TRANSFER MODES`

### Different Ways of Handling File Data

| Mode | Description |
|---|---|
| Netascii | Text-oriented transfer with network ASCII representation |
| Octet | Binary data transfer without text conversion |
| CRLF handling | Carriage-return and line-feed conversion associated with text-mode processing |

### Why Transfer Modes Matter

Different file types require different data-handling approaches.

Binary files must preserve their original byte values. Text files may require newline conversion depending on the transfer mode.

This project helped explore how file representation and byte-level data processing affect network communication.

---

## `06 / NETWORK PROGRAMMING CONCEPTS`

| Concept | Application |
|---|---|
| `socket()` | Creating a communication endpoint |
| `bind()` | Associating a server socket with an address |
| `sendto()` | Sending UDP messages |
| `recvfrom()` | Receiving UDP messages |
| `htons()` | Host-to-network short conversion |
| `ntohs()` | Network-to-host short conversion |
| File Handling | Reading and writing transferred data |
| UDP | Connectionless communication |

### Important Engineering Concepts

**1. UDP Communication**

UDP provides connectionless message delivery without establishing a TCP-style connection.

**2. Socket Programming**

Sockets provide the interface through which client and server applications exchange network data.

**3. Byte Order Conversion**

Network protocols use network byte order. Functions such as `htons()` and `ntohs()` help convert 16-bit values between host and network byte order.

**4. Acknowledgement Mechanism**

ACK messages help the sender identify which data block has been received.

---

## `07 / PROJECT IMPLEMENTATION`

### Development Approach

1. Created the client-server communication architecture.
2. Implemented UDP socket operations.
3. Worked with protocol message formatting.
4. Implemented DATA and ACK handling.
5. Explored file transfer modes.
6. Worked with network byte-order conversion.
7. Tested communication and file handling.

---

## `08 / DEVELOPMENT ENVIRONMENT`

| Category | Technology |
|---|---|
| Programming Language | C |
| Operating System | Linux |
| Communication Protocol | UDP |
| Network Interface | BSD Socket API |
| Compiler | GCC |
| Development Concepts | Socket Programming, File Handling |

---

## `09 / PROJECT STRUCTURE`

```text
TFTP-Client-Server/
│
├── client/
│   ├── client.c
│   └── client.h
│
├── server/
│   ├── server.c
│   └── server.h
│
├── common/
│   └── protocol.h
│
├── README.md
└── .gitignore
```

*Illustrative directory structure. Update it according to your actual source files.*

---

## `10 / BUILD AND EXECUTION`

### Prerequisites

- Linux environment
- GCC compiler
- Basic C programming knowledge
- Socket programming support

### Compilation

Use the actual source filenames in your project.

```bash
gcc client.c -o client
gcc server.c -o server
```

### Execution

Terminal 1 — Start the server:

```bash
./server
```

Terminal 2 — Start the client:

```bash
./client
```

*Adapt the commands according to your actual command-line arguments and project structure.*

---

## `11 / SKILLS DEVELOPED`

- C programming and modular development.
- Linux system programming fundamentals.
- UDP socket communication.
- Client-server architecture.
- Network packet handling.
- Protocol implementation concepts.
- File handling and data processing.
- Byte-order conversion.

---

## `12 / FUTURE ENHANCEMENTS`

- [ ] Improve error handling.
- [ ] Add timeout and retransmission handling.
- [ ] Improve transfer validation.
- [ ] Add detailed communication logging.
- [ ] Improve handling of network failures.
- [ ] Add more comprehensive protocol testing.

---

## `13 / ABOUT THE DEVELOPER`

<div align="center">

### PURUSHOTHAMA K

**Embedded C | Linux Programming | Firmware Development**

Electrical and Electronics Engineering Graduate

Interested in Embedded Software, Linux System Programming, Microcontrollers and Network Communication.

[![GitHub](https://img.shields.io/badge/GitHub-My%20Profile-181717?style=for-the-badge&logo=github)](https://github.com/kpurushothama446-droid)

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-0A66C2?style=for-the-badge&logo=linkedin)](https://www.linkedin.com/in/purushothama-k-b8413227b/)

</div>

---

<div align="center">

### UNDERSTAND THE PROTOCOL. BUILD THE COMMUNICATION. MASTER THE SYSTEM.

**DESIGN | DEVELOP | DEBUG**

</div>
