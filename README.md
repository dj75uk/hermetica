# Project Hermetica

## Overview
**Project Hermetica** is a modular, scalable **flight control unit (FCU) platform** designed for unmanned aerial systems ranging from hobby-grade builds to industrial and commercial UAVs.

It is built from the ground up to prioritise **reliability, observability, and architectural correctness**, avoiding the typical “black box” approach seen in many existing flight controllers.

## Purpose
Hermetica exists to solve a fundamental problem in UAV systems:

> Flight controllers are often opaque and difficult to validate, when they should be transparent, deterministic, and testable.

The platform aims to:
- Provide a **robust and deterministic flight control stack**
- Enable **deep system introspection** for debugging and validation
- Support **modern communication architectures** (notably CAN-based systems)
- Serve as a foundation for **autonomous and safety-critical UAV applications**

## Design Philosophy
Hermetica is built on a set of strict engineering principles:

- **Modularity by design**  
  Components must be composable, replaceable, and independently testable  

- **Deterministic behaviour**  
  Timing, control loops, and data paths must be predictable and measurable  

- **Observability first**  
  Internal system state must be visible, logged, and analysable  

- **Fail explicitly, not silently**  
  Fault detection and handling are first-class concerns  

- **Architecture over convenience**  
  Clean design is prioritised over short-term integration hacks  

## System Architecture
Hermetica is structured as a **distributed-capable flight control system**:

- **Core FCU**  
  High-performance MCU(s) responsible for control loops, sensor fusion, and coordination  

- **Peripheral Modules**  
  Sensors, actuators, and payload interfaces connected via defined interfaces  

- **Communication Backbone**  
  CAN/FDCAN-based architecture for robust, fault-tolerant communication  

- **Telemetry & Diagnostics Layer**  
  High-rate, structured data output for monitoring and analysis  

## Core Capabilities
- Multi-mode flight control (manual, stabilised, autonomous)  
- Sensor fusion (IMU, GNSS, barometer, magnetometer, optional optical flow)  
- High-frequency, deterministic control loops  
- Integrated health monitoring and fault detection  
- Comprehensive logging and telemetry streaming  
- Support for multiple protocols (CAN, UART, SBUS/FBUS, MAVLink)  

## Hardware Approach
Hermetica hardware is designed to be:
- **Electrically robust** (ESD protection, signal integrity, stable power delivery)  
- **Expandable** with clearly defined interfaces  
- **Integration-aware** without hardcoding external constraints  
- **Unconstrained by legacy layouts**  

## Software Approach
- Bare-metal or RTOS-based execution  
- Clear separation of:
  - Real-time control paths  
  - Communication layers  
  - Diagnostics and logging  
- Designed for **testability** (HIL/SIL, fault injection)  
- Secure update and configuration mechanisms  

## Relationship to Aetherion
Hermetica is developed alongside [**Project Aetherion**](https://github.com/dj75uk/aetherion), which serves as its **real-world validation platform**.

- Hermetica defines the control system  
- Aetherion validates it under real conditions  

This separation keeps Hermetica architecturally clean while ensuring it meets real-world constraints.

## Key Differentiators
- **CAN-first architecture** instead of UART-centric designs  
- **Proper FBUS support** for high-performance peripherals  
- **Deep observability and diagnostics**, not just basic telemetry  
- **Scalable design** from hobby to industrial applications  
- **Explicit fault handling**, not optimistic assumptions  

## Target Use Cases
- Advanced hobbyist UAV platforms  
- Research and academic systems  
- Industrial UAV development  
- Autonomous flight systems  

## Long-Term Vision
Hermetica is intended to evolve into:
- A fully autonomous flight control platform  
- A modular ecosystem with open interface specifications  
- A reference architecture for next-generation UAV systems  

Future directions include:
- Distributed flight control across multiple nodes  
- AI-assisted navigation and control  
- Swarm coordination  
- Pathways toward safety-critical certification  
