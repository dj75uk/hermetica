# Project Hermetica – System Specification (Draft)

**Version:** 0.1  
**Date:** 2026-04-10  
**Owner:** Daniel Johnson  
**Status:** Draft / In Definition  

---

## 1. Purpose

Hermetica is a **modular, scalable flight control unit (FCU) platform** designed to support UAV systems ranging from hobbyist to industrial-grade deployments.

The system is intended to:
- Provide **reliable real-time flight control**
- Enable a **modular hardware + software ecosystem**
- Establish **CAN-first architecture** for UAV systems
- Support progression toward **fully autonomous operation**

---

## 2. System Definition

### 2.1 High-Level Architecture

**Core Components:**
- Flight Control Unit (FCU)
- Sensor Suite (external + onboard)
- Actuation Interfaces (ESCs, servos)
- Communication Backbone (CANFD-centric)
- Ground Interface

**Architecture Model:**
- [ ] Monolithic FCU  
- [ ] Distributed (CAN nodes for sensors/actuators)  
- [ ] Hybrid (central FCU + smart peripherals)  

> **Selected Approach:** ___________

---

### 2.2 Design Principles

- **Decoupled integration** (no hard dependency on specific airframe)
- **Deterministic real-time behavior**
- **Fail-operational where possible**
- **Hardware/software modularity**
- **Open interface specifications**

---

## 3. Functional Scope

### 3.1 Flight Control

- Stabilization (rate + attitude)
- Navigation (position, velocity)
- Mission execution (waypoints, behaviours)

**Control Strategy:**
- [ ] PID  
- [ ] Cascaded PID  
- [ ] MPC  
- [ ] Adaptive / ML-assisted  

> **Selected Strategy:** ___________

---

### 3.2 Flight Modes

- [ ] Manual passthrough  
- [ ] Stabilized  
- [ ] Assisted (altitude / position hold)  
- [ ] Fully autonomous  
- [ ] Return-to-home  
- [ ] Failsafe modes  

---

### 3.3 Sensor Handling

**Supported Sensors:**
- IMU (primary + redundant)
- GNSS
- Barometer
- Magnetometer
- [ ] Optical flow  
- [ ] Airspeed sensor  
- [ ] Radar / LiDAR  

**Sensor Fusion:**
- [ ] EKF  
- [ ] UKF  
- [ ] Custom  

> **Selected Approach:** ___________

---

### 3.4 Communication

**Primary Backbone:**
- CAN / FDCAN (mandatory)

**Secondary Interfaces:**
- UART (FBUS, SBUS, GPS)
- SPI / I²C (local peripherals)
- USB (config/debug)

**Protocol Stack:**
- [ ] MAVLink  
- [ ] Custom CAN protocol  
- [ ] DDS / ROS2 bridge  

> **Telemetry Rate Target:** ___________  
> **Max Latency:** ___________

---

### 3.5 Payload Control

- Servo outputs: ___ channels  
- Digital outputs: ___  
- Smart payload interface (CAN): [ ] Yes / [ ] No  

---

### 3.6 Logging & Telemetry

- [ ] Onboard storage (SD / flash)  
- [ ] Real-time telemetry streaming  
- [ ] Blackbox flight recorder  

**Retention Target:** ___________

---

## 4. Hardware Specification

### 4.1 Processing Architecture

- MCU: Will be dependant upon individual stack layer requirements:
  - STM32H755ZIT6 for high performance layers running at 480 MHz  
  - STM32G484CET6 for low performance requirements running at 170 MHz  

| MCU           | Core                | Clock Speed | Package            |
|---------------|---------------------|-------------|--------------------|
| STM32H755ZIT6 | M7 + M4 (Dual Core) | 480 Mhz     | LQFP-144 20x20x1.4 |
| STM32G484CET6 | M4                  | 170 Mhz     | LQFP-48 7x7x1.4    |

### 4.2 Memory

| MCU           | Flash   | RAM     | 
|---------------|---------|---------|
| STM32H755ZIT6 | 2048 kB | 1024 kB |
| STM32G484CET6 | 512 kB  | 128 kB  |

- External storage: Each MCU will have dedicated FRAM for local configuration.  

---

### 4.3 I/O Budget

| Interface | Usage |
|-----------|-------|
| FDCAN     | Redundant primary backbone |
| UART      | Debugging connector, low speed communication |
| SPI       | Local FRAM, high-speed sensors |
| I²C       | Low-speed sensors |
| GPIO      | General low-speed signalling and input capture    |
| PWM       | Avoid using unless explicitly required on node     |
| ADC       | Avoid using if possible, prefer digital interfaces |
| DAC       | Avoid using if possible, prefer digital interfaces |

---

#### 4.3.1 Standard peripheral usage by MCU

| MCU           | FDCAN1           | FDCAN2           | SPI1 | USART3    |
|---------------|------------------|------------------|------|-----------|
| STM32H755ZIT6 | Primary Backbone | Primary Backbone | FRAM | Debugging |
| STM32G484CET6 | Primary Backbone | Primary Backbone | FRAM | Debugging |

- This should be considered a "standard usage" across all boards.
  
---


### 4.4 Power

- Input voltage range: 2S to 8S primary supply (6.0V to 33.6V)  
- Nominal power draw: < 500mA  
- Peak power draw: 3A  

- [x] Reverse polarity protection  
- [x] Surge / transient protection  
- [x] Power domain isolation  

---

### 4.5 Physical

- Form factor: Multiple-PCB vertical stack with a stacking header acting as a common backbone  
- Dimensions: 60mm(W) x 80mm(L) x  <50mm(H)  
- Weight target: Sub 500g including all control-cabling and nodes (excluding servos, batteries and ESC's)  
- Mounting via 4 x M3 holes, ideally on vibration mounts (airframe dependant)  

### 4.6 Backbone Header


### 4.7 Minimum Stack Layers

#### 4.7.1 Power Distribution Board (PDB)

The PDB is the central electrical hub of a system designed to take power from the main battery and distribute it safely and efficiently to all subsystems.

- STM32G484CET6
- I/O: Debug connector
- IN: Connection for primary batteries (PD1)
- OUT: (PD1) 4x high current (~100A) pass-through ESC connections
- OUT: (PD1) 2x low current (2A) pass-through VTX connections
- OUT: (PD2) Voltage regulation to 5.0V @ 3A for stack power  
- OUT: (PD3) Voltage regulation to 5.0V @ 3A for distributed node power (PD3)
- Current, voltage, temperature, coulomb counting and power monitoring
- Feeds power good signals on backbone (PD2 and PD3)
- Feeds power load signals on backbone (PD2 and PD3)
- ESD protection, reverse voltage protection, reverse current blocking, fusing
- MCU locally powered by local 3.3V regulation from PD2
- Optional CAN-FD bus termination
- Pulls backbone nRESET high to 3.3V via pull-up
- Cuts power temporarily to PD2 and PD3 on backbone nRESET signal going low

#### 4.7.1 Flight Control Board (FCB)

- STM32H755ZIT6
- I/O: Debug connector
- I/O: CAN-FD external connector
- Reads RC control messages from the CAN-FD bus from RC receivers
- Runs primary control loop, PID controllers, cascaded PID controllers services actuators
- MCU locally powered by local 3.3V regulation from PD2 via backbone  
- Functional IMU (gyroscope, accelerometer, magnetometer, barometer)  
- Primary consumer of CAN-FD messages from subsystems  
- Optional CAN-FD bus termination
- Responsible for feeding arming signal on backbone

---

### 4.8 Debug Connector

Standardised connector across all PCB's for connection to a debugging board.

- Connector: Samtec T1M, 10 pin, horizontal, single-row, [SAMTEC-T1M-10-F-SH-L](https://www.samtec.com/products/t1m-10-f-sh-l)
- Height: 4.6mm

| Pin  | Name               | Notes                                                                                         |
|------|--------------------|-----------------------------------------------------------------------------------------------|
| 1    | V<sub>TARGET</sub> | Voltage of target MCU                                                                         |
| 2    | GND                | Target GND                                                                                    |
| 3    | FITTED             | Signal to indicate the debugging board is fitted to a target board (0V to V<sub>TARGET</sub>) |
| 4    | SWDIO              | SWD - SWDIO signal                                                                            |
| 5    | SWCLK              | SWD - SWCLK signal                                                                            |
| 6    | SWO                | SWD - SWO signal                                                                              |
| 7    | BOOT0              | Pull-down on target on board                                                                  |
| 8    | nRESET             | Pull-up on target board                                                                       |
| 9    | MCU_TX             | UART connection to MCU TX                                                                     |
| 10   | MCU_RX             | UART connection to MCU RX                                                                     |

#### 4.8.1 FITTED

FITTED is pulled to  V<sub>TARGET</sub> on the debugging board. This is to enable the detection of the debugging hardware should conditional logic be required.
For example, the stack could refuse to arm the motors and spin the propellors if FITTED was high. You know, to save fingers.

#### 4.8.2 BOOT0

BOOT0 is broken out so that the MCU can be forced to start in bootloader mode in an emergency. It should be pulled-down on the target board.

#### 4.8.3 MCU_TX and MCU_RX
- 
MCU_TX and MCU_RX are broken out so that the MCU can be emergency flashed. They should be connected to a USART capable of flashing the bootloader.
For most STM32 MCU's this is USART1, USART2 or USART3. During normal operation, the USART may be used for debugging.


---

### 4.9 CAN-FD Connector

Standardised connector across all PCB's for connection to a CAN-FD nodes.

- Connector: Samtec T1M, 3 pin, horizontal, single-row, [SAMTEC-T1M-03-F-SH-L](https://www.samtec.com/products/t1m-03-f-sh-l)
- Height: 4.6mm

| Pin  | Name               | Notes         |
|------|--------------------|---------------|
| 1    | CAN<sub>LOW</sub>  | CAN Low       |
| 2    | CAN<sub>HIGH</sub> | CAN High      |
| 3    | GND                | Signal ground |

### 4.9.1 CAN<sub>LOW</sub> and CAN<sub>HIGH</sub>

CAN<sub>LOW</sub> and CAN<sub>HIGH</sub> are a differential signal and should be twisted-pair.

---

## 5. Firmware Architecture

### 5.1 Execution Model

- The primary firmware will be written in C/C++ on a *baremetal* level (HAL is acceptable)  
- Additional nodes may use whatever they like  

---

### 5.2 Task Model

| Task | Priority | Frequency | Core |
|------|--------|----------|------|
| Control loop | ___ | ___ Hz | ___ |
| Sensor fusion | ___ | ___ Hz | ___ |
| Comms | ___ | ___ Hz | ___ |
| Logging | ___ | ___ Hz | ___ |

---

### 5.3 Inter-Core Communication (if applicable)

- Mechanism: ___________ (e.g. shared memory, mailbox, UART, RPMsg)  
- Failure handling: ___________  

---

### 5.4 Update Mechanism

- [ ] USB  
- [ ] CAN  
- [ ] OTA  

- [ ] Secure boot required  

---

## 6. Safety & Fault Handling

### 6.1 Fault Classes

- Sensor failure  
- Actuator failure  
- Power instability  
- Communication loss  
- Internal software fault  

---

### 6.2 Detection Mechanisms

- [x] Watchdogs (independent + windowed)  
- [x] Heartbeats (inter-module)  
- [x] Plausibility checks  

---

### 6.3 Failsafe Behaviour

| Fault    | Description                         | Probability | Response |
|----------|-------------------------------------|-------------|----------|
| FAULT001 | Primary IMU failure                 | LOW         | Warn and log |
| FAULT002 | Secondary IMU failure               | LOW         | Warn and log |
| FAULT003 | All IMU failure                     | VERY LOW    | Catastrophic |
| FAULT004 | GNSS loss                           | HIGH        | No action |
| FAULT005 | RC link loss                        | HIGH        | Usually switch to RTH mode |
| FAULT006 | Critical FCU internal fault         | LOW         | Restart node |

---

## 7. Performance Targets

- Control loop frequency: 10 kHz  
- End-to-end latency: sub 20 ms  
- Boot time: sub 10 s  
- Max supported nodes (CANFD): 64  
- Max actuator count: 10+  

---

## 8. Environmental Requirements

- Operating temperature: -40C to 120C (low temperature handling is much more important)  
- Vibration tolerance: ___________  
- Shock tolerance: ___________  
- EMC compliance target: ___________  

---

## 9. Security

- [ ] Secure boot  
- [ ] Firmware signing  
- [ ] Encrypted communication  
- [ ] Key storage (secure element?)  

---

## 10. Verification Strategy

- [ ] SIL (software-in-loop)  
- [ ] HIL (hardware-in-loop)  
- [ ] Bench testing  
- [ ] Flight testing  

**Acceptance Criteria:**
- ___________

---

## 11. Integration Model (Critical)

Hermetica must remain **independent of Aetherion** until integration.

### 11.1 Negotiated Parameters

| Parameter | Hermetica Target                         | Aetherion Constraint | Status |
|-----------|------------------------------------------|----------------------|--------|
| Weight    | < 500g                                   | 1.0kg limit          | [x]    |
| Power     | 2S to 8S supply, 500mA total consumption | ___                  | [ ]    |
| Size      | 60mm(W) x 80mm(L) x 50mm(H)              | ___                  | [ ]    |

---

## 12. Open Questions

- What exact role does CAN play — backbone only, or full distributed compute?
- Are you committing to dual-core safety separation, or is that overkill for MVP?
- Is FBUS a first-class interface, or just supported?
- Do you want PX4 compatibility, or is this clean-slate?
- What is the minimum viable flight capability?

---

## 13. MVP Definition

**MVP must include:**
- CANFD working backbone for messaging between stack layers  
- Basic stabilized flight  
- Basic loiter flight mode  
- Gyroscope, accelerometer and magnetometer fusion  
- RC input (SBUS)  
- PWM output for ESC and servos
- Independant control of ailerons, elevators, rudders and flaps  
- Basic failsafe  

**Explicitly excluded:**
- Advanced flight modes  
- Pitot integration  
- Payload ecosystem  
- OSD integration

---

## 14. Future Extensions

- Distributed CAN sensor/actuator nodes  
- Autonomous mission engine  
- Swarm capability  
- AI-assisted control  
