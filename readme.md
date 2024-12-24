# Server Connection Project

This project demonstrates different server connection techniques in C, including **blocking**, **non-blocking**, and **multi-threading**. It also features a **custom thread pool implementation** for efficient task handling.

## Features

1. **Blocking Server**:
   - Processes one client at a time, using a simple server-client communication model.

2. **Non-Blocking Server**:
   - Uses non-blocking I/O to handle multiple client connections concurrently without being blocked.

3. **Multi-Threaded Server**:
   - Utilizes `pthread` to handle each client request in a separate thread.

4. **Custom Thread Pool**:
   - Implements a reusable thread pool for managing multiple client connections efficiently.
   - Threads are pre-allocated to avoid the overhead of frequent thread creation/destruction.

---

## Directory Structure

```plaintext
server-connection/
│
├── .vscode/                    # VS Code configuration files
│
├── blocking.c                  # Blocking server implementation
├── nonblocking.c               # Non-blocking server implementation
├── server/                     # Directory containing server-related files
│   ├── server.c                # Multi-threaded server implementation
│   ├── threadpool.c            # Custom thread pool logic
│   ├── threadpool.h            # Thread pool header
│
├── readme.md                   # Project documentation
