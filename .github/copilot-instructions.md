# Copilot Instructions for Leosac Access Control

## Project Overview
- **Leosac Access Control** is an open-source physical access control system for embedded Linux devices. It manages door access logic, typically running on a board connected to RFID readers and electric strikes.
- The architecture is modular, with each module handling a specific domain (e.g., alarms, authentication, hardware abstraction).
- Communication between modules and with hardware is primarily via ZeroMQ (see `zmqpp`), using in-process and network sockets.
- Configuration is XML-based, with default and factory configurations in `cfg/`.

## Key Components & Structure
- **src/**: Main C++ source code. Major subfolders:
  - `modules/`: Pluggable modules (alarms, authentication, etc.).
  - `hardware/`: Hardware abstraction layers and facades.
  - `tools/`: Utilities (logging, DB, etc.).
- **build/** and **scripts/**: Build scripts, network config helpers, and deployment utilities.
- **deps/**: Third-party dependencies (ZeroMQ, MQTT, JSON, spdlog, etc.).
- **cfg/**: Example and default configuration files.
- **docker/**: Dockerfiles for containerized builds and deployment.

## Build & Development Workflow
- **Build system:** CMake (see `CMakeLists.txt`). Be sure to correctly update CMakeLists.txt files when adding new source files or dependencies. Also, use correct ODB macros and variables on CMakeLists.txt files when adding new database-mapped classes.
  - Out-of-source builds are strongly recommended.
  - Example: `mkdir build; cd build; cmake ..; make`
  - Cross-compilation for Raspberry Pi is supported via toolchain files.
- **Testing:** No explicit test runner in root; some modules and dependencies have their own tests.
- **Scripts:**
  - `scripts/remote_control.py`: Interact with a running Leosac instance via ZeroMQ.
  - `scripts/gen_key.py`: Generate CurveZMQ key pairs for secure communication.
  - `scripts/ssh_rsync_source.sh`: Sync code to remote devices for cross-platform development.

## Project-Specific Patterns & Conventions
- **Module Communication:**
  - Modules use ZeroMQ (inproc and tcp) for message passing. Example: `AlarmInstance` binds to `inproc://<name>` and connects to `inproc://zmq-bus-pull`.
  - Hardware access is abstracted via facades in `src/hardware/facades/`.
- **Configuration:**
  - XML files in `cfg/` define system and module settings. Factory defaults are in `cfg/factory/kernel.xml`.
  - Resetting to factory defaults uses the `test-and-reset` module.
- **Logging:** Uses `spdlog` (header-only, see `deps/spdlog`).
- **Database:** Uses ODB ORM (see `tools/db/` and `modules/alarms/AlarmEntry_odb.h`).
- **WebSocket API:** Implemented in `src/modules/websocket/` using `websocketpp` for Core components, then directly in modules for those exposed via WebSocket (eg. `libgpiod` module). The main consumer of the WebSocket API is the web UI (see https://github.com/leosac/access-control-web).
- **External Integrations:**
  - MQTT (via Paho C/C++), ZeroMQ, SQLite, OpenSSL, Boost, LibScrypt.

## Examples
- **Adding a new module:** Place code in `src/modules/<modulename>/`, register sockets with the reactor, and use ZeroMQ for inter-module communication.
- **Custom hardware integration:** Implement a facade in `src/hardware/facades/` and expose it via the message bus.

## References
- Main documentation: [leosac.github.io/leosac-doc/doc_output/develop/](http://leosac.github.io/leosac-doc/doc_output/develop/)
- Build scripts: `scripts/`
- Docker: `docker/Dockerfile.main`

---

**If you update project structure or conventions, please update this file.**
