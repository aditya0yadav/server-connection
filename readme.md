
# Server Connection Implementation

This project demonstrates three different server connection approaches using C: **blocking**, **non-blocking**, and **multi-threading**. Additionally, a custom thread pool implementation is included for enhanced performance and resource management.

## Features

1. **Blocking Server**:
   - Simple server-client model.
   - Each client connection blocks the server until the request is processed.

2. **Non-Blocking Server**:
   - Uses non-blocking I/O to handle multiple client connections simultaneously.
   - Improves scalability and responsiveness.

3. **Multi-Threading Server**:
   - Implements `pthread` for managing multiple client connections.
   - Each connection runs in a separate thread.

4. **Custom Thread Pool**:
   - A custom thread pool implementation to handle multiple client requests efficiently.
   - Threads are pre-allocated and reused, reducing the overhead of thread creation and destruction.
   - Implements custom job queue logic.

---

## Directory Structure

server_connection/
│
├── src/
│   ├── blocking_server.c          # Blocking server implementation
│   ├── non_blocking_server.c      # Non-blocking server implementation
│   ├── multi_threading_server.c   # Multi-threaded server implementation
│   ├── thread_pool.c              # Custom thread pool implementation
│   └── thread_pool.h              # Header file for the custom thread pool
│
├── Makefile                       # Build script
├── README.md                      # Project documentation
└── LICENSE                        # License file


---

## Setup Instructions

### Prerequisites
- GCC or Clang compiler
- Make
- A Unix-based system (Linux/MacOS)

### Build
1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd server_connection
   ```
2. Build the project using the `Makefile`:
   ```bash
   make
   ```

### Run
- Blocking Server:
  ```bash
  ./bin/blocking_server
  ```
- Non-Blocking Server:
  ```bash
  ./bin/non_blocking_server
  ```
- Multi-Threading Server:
  ```bash
  ./bin/multi_threading_server
  ```
- Thread Pool Test:
  ```bash
  ./bin/thread_pool_test
  ```

---

## Custom Thread Pool Overview

The custom thread pool is designed to manage a pool of reusable threads. Key features include:
- **Thread Pool Initialization**: Pre-allocates a fixed number of threads.
- **Job Queue**: Maintains a queue of tasks to be executed by threads.
- **Synchronization**: Uses mutexes and condition variables to handle concurrent access.

---

## Usage Examples

### Blocking Server
Handles one client at a time. Suitable for simple applications.

### Non-Blocking Server
Handles multiple clients using `select` or `poll` system calls. Ideal for I/O-bound tasks.

### Multi-Threading Server
Each client is handled by a separate thread, leveraging `pthread`. Useful for compute-intensive tasks.

### Thread Pool
Optimized for high concurrency. Efficiently handles a large number of short-lived tasks.

---

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests.

---

## License

This project is licensed under the [MIT License](LICENSE).
