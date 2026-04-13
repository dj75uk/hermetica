# Project Hermetica – System Specification (Draft)

**Version:** 0.3  
**Date:** 2026-04-13  
**Owner:** Daniel Johnson  
**Status:** Draft / In Definition  

---

## 0. Nomenclature

### 0.1 On the name "Hermetica"

### 0.2 On the name "Aetherion"

### 0.3 Definitions

#### 0.3.1 FCU

The FCU is the Hermetica hardware and software combined together to form a working, cohesive unit. It emcompasses the centralised control system and any communicating devices.

#### 0.3.2 Flight Control Stack

The Flight Control Stack is a centralised physical point, comprised of multiple vertically stacked PCB's with a common Backbone header. Is is responsible for actually flying the airframe.                                                  

#### 0.3.3 Backbone

A vertically stacking header on each PCB in the Flight Control Stack to facilitate power distribution to Stack Layers and signalling between them.

#### 0.3.4 Stack Layer

A physical PCB within the vertical Flight Control Stack.

#### 0.3.5 Node

A physical PCB within the FCU, connected to, but external to the Flight Control Stack via defined interfaces and operating semi-independantly.

#### 0.3.6 Role

A Role defines a logical set of Responsibilites within the system, independent of physical implementation.

#### 0.3.7 Authority

Authority defines the level of control a role has over system behaviour.

#### 0.3.8 Power Domain

A Power Domain is a distinct electrical rail with defined voltage, current capability, and purpose.

#### 0.3.9 System State

The System State represents the current operational mode of the FCU.

#### 0.3.10 Responsibility

A Responsibility is a defined aspect of system behaviour that a Role is accountable for, independent of implementation.

---

## 1. Purpose

Hermetica is a **modular, distributed, resiliant, scalable flight control unit (FCU) platform** designed to support UAV systems ranging from hobbyist to industrial-grade deployments.

The system is intended to:
- Provide **reliable real-time flight control**
- Enable a **modular hardware + software ecosystem**
- Establish **CAN-FD-first architecture** for UAV systems
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
- A central flight control "stack" with multiple external nodes.
- The system will be inheirantly distributed with components performing strict roles with strict responsibilities.
- 
---

### 2.2 Design Principles

- **Decoupled integration** (no hard dependency on specific airframe)
- **Deterministic real-time behavior**
- **Fail-operational where possible**
- **Hardware/software modularity**
- **Open interface specifications**

---

## 3. Functional Scope

### 3.0 Roles, Authority and Responsibilites

A Role:
- Owns a clearly defined functional domain
- Has explicitly defined inputs and outputs
- May enforce local constraints within its domain
- May participate in system-wide decision making depending on its authority

The FCU is composed of a set of defined Roles. Each Role encapsulates a specific set of Responsibilities and operates within a defined domain.

Roles are independent of physical implementation and may be assigned to any suitable Stack Layer or Node.

---

#### 3.0.5 Flight Control Role

The Flight Control Role is responsible for generating actuator commands required to achieve and maintain controlled flight.

Responsibilities:
- Stabilisation (rate and attitude control)
- Navigation (position and velocity control)
- Flight mode handling and transitions
- Command generation for actuators
- Enforcement of flight-related constraints

Inputs:
- Sensor data (IMU, GNSS, barometer, etc.)
- Pilot commands (via RC or ground interface)
- System State
- Constraints from higher-authority roles

Outputs:
- Actuator command signals
- Flight state information

---

#### 3.0.10 Sensor Processing Role

The Sensor Processing Role is responsible for acquiring, validating, and conditioning sensor data for use by other Roles.

Responsibilities:
- Sensor data acquisition
- Calibration and compensation
- Data validation and plausibility checking
- Sensor fusion (if implemented within this role)

Inputs:
- Raw sensor measurements

Outputs:
- Conditioned sensor data
- State estimates (if applicable)
- Sensor health/status information

---

#### 3.0.15 Actuation Role

The Actuation Role is responsible for converting actuator commands into physical outputs.

Responsibilities:
- Driving ESCs and servos
- Output signal generation (PWM, digital protocols, etc.)
- Monitoring actuator response (if feedback is available)
- Enforcing actuator-level safety constraints

Inputs:
- Actuator commands from Flight Control Role
- System State
- Constraints from higher-authority roles

Outputs:
- Physical actuator signals
- Actuator status and health information

---

#### 3.0.20 Power Management Role

The Power Management Role is responsible for the distribution, monitoring, and protection of electrical power within the FCU.

Responsibilities:
- Power distribution across defined Power Domains
- Voltage, current, and temperature monitoring
- Fault detection and protection
- Power domain control (enable/disable)
- Enforcement of electrical safety constraints

Inputs:
- Electrical measurements (voltage, current, temperature)
- System State

Outputs:
- Power status information
- Fault signals
- Power domain control signals

---

#### 3.0.25 Communication Role

The Communication Role is responsible for the exchange of data between system components and external interfaces.

Responsibilities:
- Message transport across internal and external interfaces
- Protocol handling and abstraction
- Routing of messages between Roles
- Ensuring data integrity and timing constraints

Inputs:
- Messages from Roles and external interfaces

Outputs:
- Routed and transmitted messages
- Communication status information

---

#### 3.0.30 System Management Role

The System Management Role is responsible for overall system coordination and state management.

Responsibilities:
- System State management and transitions
- Arming and disarming logic
- Coordination between Roles
- Enforcement of system-level constraints
- Handling of global fault conditions

Inputs:
- Status and health information from all Roles
- External control inputs

Outputs:
- System State
- Control signals affecting other Roles
- Global fault responses

---

#### 3.0.35 Payload Role (Optional)

The Payload Role is responsible for control and management of mission-specific payloads.

Responsibilities:
- Payload control and operation
- Data acquisition from payload systems
- Integration with mission objectives

Inputs:
- Commands from Flight Control or external interfaces

Outputs:
- Payload data
- Payload status information

---

### 15.10 Responsibilities

Responsibilities define the behavioural obligations of Roles within the FCU.

Each Responsibility represents a specific aspect of system behaviour that must be fulfilled for correct operation.

---

#### 15.10.5 Assignment

- Every Responsibility shall be owned by exactly one Role.  
- A Role may own multiple Responsibilities.  
- Responsibilities shall not be shared between Roles to avoid ambiguity in ownership.  

---

#### 15.10.10 Scope

- Each Responsibility shall have clearly defined boundaries.  
- Responsibilities shall not overlap in scope.  
- Responsibilities may depend on outputs from other Responsibilities but shall not duplicate them.  

---

#### 15.10.15 Inputs and Outputs

- Each Responsibility shall define its required inputs.  
- Each Responsibility shall define its expected outputs.  
- Inputs and outputs shall be explicitly defined to enable consistent integration between Roles.  

---

#### 15.10.20 Behavioural Guarantees

- Each Responsibility shall define the conditions it must maintain or enforce.  
- Responsibilities shall produce deterministic outcomes given valid inputs.  
- Responsibilities shall handle invalid or missing inputs in a defined manner.  

---

#### 15.10.25 Constraints

- Responsibilities may enforce constraints within their domain.  
- Responsibilities shall operate within constraints imposed by higher-authority Roles.  
- Conflicting constraints shall be resolved according to the Authority model.  

---

#### 15.10.30 Failure Handling

- Each Responsibility shall define failure conditions relevant to its domain.  
- Responsibilities shall detect and report failures where possible.  
- Responsibilities shall define safe behaviour in the presence of faults.  

---

#### 15.10.35 Independence

- Responsibilities shall be independent of physical implementation.  
- Responsibilities shall not assume specific hardware, communication interfaces, or execution environments.  

---

#### 15.10.40 Composability

- Responsibilities shall be designed to allow composition into larger system behaviours.  
- Responsibilities may be reused across different Roles or system configurations, provided ownership remains unambiguous.  

---


### 3.1 Control Stack



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
- [ ] Return-to-home (RTH)
- [ ] Loiter
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
- FDCAN (mandatory)

**Secondary Interfaces:**
- UART (FBUS, SBUS, GPS, debugging)
- SPI / I²C (local peripherals)

**Protocol Stack:**
- [ ] MAVLink  
- [ ] Custom CAN protocol  
- [ ] DDS / ROS2 bridge  

> **Telemetry Rate Target:** ___________  
> **Max Latency:** ___________

---

### 3.5 Payload Control

- Controlled via CAN-FD messaging
  
---

### 3.6 Logging & Telemetry

- [ ] Onboard storage (SD / flash)  
- [ ] Real-time telemetry streaming  
- [ ] Blackbox flight recorder  

**Retention Target:** ___________

---

## 4. Physical - Hardware Specification

### 4.0 Standardised Parts List

Although individual nodes will require specific hardware, it is sensible to standard parts across all components where feasible.

| Type           | Manufacturer               | Part Number | Package            |
|---|---|---|---|
| Microcontroller  | STM | STM32H755ZIT6 | LQFP-144 20x20x1.4 |
| Microcontroller  | STM | STM32G484CET6 | LQFP-48 7x7x1.4 |
|   |  |  |  |
|   |  |  |  |
|   |  |  |  |
|   |  |  |  |







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

### 4.3 Common interface usage

| Interface | Usage                                              |
|-----------|----------------------------------------------------|
| FDCAN     | Redundant primary backbone                         |
| UART      | Debugging connector, low speed communication       |
| SPI       | Local FRAM, high-speed sensors                     |
| I²C       | Low-speed sensors                                  |
| GPIO      | General low-speed signalling and input capture     |
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

The Backbone is the core communications layer of the Flight Control Stack. It contains a mixture of power and data.

The physical component for the Backbone Header has not yet been selected and is likely to change during early development stages.

#### 4.6.1 Logical Backbone Header

Without locking down a pinout, it is possible to define what power and data is required on the Backbone.

|   | Name               | Notes                                                                                         |
|---|--------------------|-----------------------------------------------------------------------------------------------|
|   | CAN1<sub>LOW</sub>, CAN1<sub>HIGH</sub>, GND<sub>SIGNAL</sub> | Primary CAN-FD communications bus                  |
|   | CAN2<sub>LOW</sub>, CAN2<sub>HIGH</sub>, GND<sub>SIGNAL</sub> | Secondary CAN-FD communications bus                |
|   | 5V<sub>STACK</sub>, GND<sub>POWER</sub>                       | 5V/3A power bus for powering the Flight Stack      |
|   | 5V<sub>NODES</sub>, GND<sub>POWER</sub>                       | 5V/3A power bus for powering the Nodes             |
|   | nRESET                       | Active low, Flight Stack reset signal             |
|   | ARMED                        | Active high, FCU "Armed" state             |
|   | PGOOD<sub>STACK</sub>                     | Active high, 5V<sub>STACK</sub> power good      |
|   | PGOOD<sub>NODES</sub>                     | Active high, 5V<sub>NODES</sub> power good      |
|   | nFAULT                       | Active low, Flight Stack catastrophic fault signal             |
|   | LOAD<sub>STACK</sub>                       | Load of 5V<sub>STACK</sub> bus          |
|   | LOAD<sub>NODES</sub>                       | Load of 5V<sub>NODES</sub> bus          |
|   | SYNC                       | Active high, periodic deterministic pulse for synchronisation |

##### 4.6.1 CAN1<sub>LOW</sub>, CAN1<sub>HIGH</sub>

##### 4.6.2 CAN2<sub>LOW</sub>, CAN2<sub>HIGH</sub>

##### 4.6.3 5V<sub>STACK</sub>, GND<sub>POWER</sub>

##### 4.6.4 5V<sub>NODES</sub>, GND<sub>NODES</sub>

##### 4.6.5 nRESET

(Hard to seperate logical from physical here.)

The nRESET signal may be set LOW by any Stack Layer in the Flight Control Stack.
The signal is pulled HIGH by default by the Stack Layer with the Power Management Role.

### 4.7 

### 4.7 Flight Stack Layers

All Flight Stack Layers communicate over the Backbone via CAN1, CAN2, or both.

#### 4.7.1 Power Distribution Board (PDB)

The PDB is the central electrical hub of a system designed to take power from the main battery and distribute it safely and efficiently to all subsystems.

- STM32G484CET6
- I/O: Debug connector
- IN: Connection for primary batteries (PD1)
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

  Roles:

#### 4.7.2 Advanced Navigation & Mission Board

- TBC

#### 4.7.2 OSD and video integration

- TBC

#### 4.7.2 Blackbox and Telemetry Storage

- TBC

#### 4.7.2 RADAR Data Processing Board

- TBC

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
| 9    | MCU<sub>TX</sub>   | UART connection to MCU TX                                                                     |
| 10   | MCU<sub>RX</sub>   | UART connection to MCU RX                                                                     |

#### 4.8.1 FITTED

FITTED is pulled to  V<sub>TARGET</sub> on the debugging board. This is to enable the detection of the debugging hardware should conditional logic be required.
For example, the FCU could refuse to arm the motors and spin the propellors if FITTED was high. You know, to save fingers.

#### 4.8.2 BOOT0

BOOT0 is broken out so that the MCU can be forced to start in bootloader mode in an emergency. It should be pulled-down on the target PCB.

#### 4.8.3 MCU<sub>TX</sub> and MCU<sub>RX</sub>
- 
MCU<sub>TX</sub> and MCU<sub>RX</sub> are broken out so that the MCU can be emergency flashed. They should be connected to a USART capable of flashing the bootloader.  
- For most STM32 MCU's this is USART1, USART2 or USART3. During normal operation, the USART may be used for debugging.  


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

- Control loop frequency: 1 kHz  
- End-to-end latency: sub 20 ms  
- Boot time: sub 3 s  
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

- TBC


