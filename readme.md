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
├── nonblocking.c              # Non-blocking server implementation
├── server/                    # Directory containing server-related files
│   ├── server.c              # Multi-threaded server implementation
│   ├── threadpool.c          # Custom thread pool logic
│   ├── threadpool.h          # Thread pool header
│
└── readme.md                  # Project documentation
```

## Build Instructions

### Prerequisites

- Compiler: GCC or Clang
- Development Environment: Unix-based OS (Linux/MacOS)
- Make: Installed on the system

### Build the Project

1. Clone the repository:
```bash
git clone https://github.com/aditya0yadav/server-connection.git
cd server-connection
```

2. Use make to build the project:
```bash
make
```

The compiled executables will be placed in the `bin/` directory.

## Usage

### Run the Servers

1. Blocking Server:
```bash
./bin/blocking
```

2. Non-Blocking Server:
```bash
./bin/nonblocking
```

3. Multi-Threaded Server:
```bash
./bin/server
```

### Custom Thread Pool Test

To test the thread pool implementation:
```bash
./bin/threadpool_test
```

## Highlights of Custom Thread Pool

- **Pre-allocated Threads**: Threads are initialized at the start and reused for processing tasks.
- **Job Queue**: Client requests are added to a job queue, and threads pick up jobs for execution.
- **Synchronization**: Implements pthread_mutex and pthread_cond for thread-safe access to shared resources.

## Contributing

Feel free to fork the repository, submit pull requests, or open issues for any bugs or feature requests. Contributions are welcome!

## License

This project is licensed under the MIT License.
