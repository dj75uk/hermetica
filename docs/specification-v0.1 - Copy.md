# Project Hermetica – System Specification

**Version:** 0.6  
**Date:** 2026-04-15  
**Owner:** Daniel Johnson  
**Status:** Draft  

---

## Nomenclature

### On the name "Hermetica"

### On the name "Aetherion"

### Definitions

#### FCU
The FCU is the Hermetica hardware and software combined together to form a working, cohesive system. It encompasses the centralised control system and any communicating devices.

#### Flight Control Stack
A centralised physical structure comprised of vertically stacked PCBs with a common Backbone, responsible for primary flight-critical functions.

#### Backbone
A vertically stacking header facilitating power distribution and signalling between Stack Layers.

#### Stack Layer
A physical PCB within the Flight Control Stack.

#### Node
A physical PCB external to the Flight Control Stack, connected via defined interfaces and operating semi-independently.

#### Role
A logical grouping of Responsibilities, independent of physical implementation.

#### Authority
The level of control a Role has over system behaviour and decision making.

#### Responsibility
A defined aspect of system behaviour that a Role is accountable for.

#### Power Domain
A distinct electrical rail with defined characteristics and purpose.

#### System State
The current operational state of the FCU and all information pertaining to it.

---

## Units

Wherever possible, SI units (and derived units) will be used across the system.

| Value | Unit | Symbol | Examples |
|---|---|---|---|
| Time           | Seconds | s | |
| Attitute       | degrees | ° | Roll, Pitch, Yaw |
| Attitute Rates | degrees per second | °s<sup>-1</sup> | Roll-Rate, Pitch-Rate, Yaw-Rate |
| Heading|degrees|°| Magnetic Heading, True Heading |
| Scalar speed |metres per second | ms<sup>-1</sup> | Airspeed, Groundspeed, VSI |
| Altitude | metres | m | Barometric altitude, GPS altitude, IMU altitude, Ranged altitude |
| Current | Amps  | A | Primary bus current |
| Voltage | Volts | V | Primary bus voltage |
| Power   | Watts | W | Primary bus power   |
|Frequency|Hertz|Hz| Main loop cycle frequency |
|Temperature|Celcius|°C| Outside air temperature, cabin temperature |
|Pressure|Pascals|Pa| Barometric pressure, Pitot pressure |
|Distance|metres|m| Total flight distance, distance to home |

---

## Purpose

Hermetica is a **modular, distributed, resiliant, scalable flight control unit (FCU) platform** designed to support UAV systems ranging from hobbyist to industrial-grade deployments.

The system is intended to:
- Provide **deterministic real-time flight control**
- Enable a **modular ecosystem**
- Establish a **CAN-FD-first architecture**
- Support progression toward **full autonomy**

---


## System Architecture

### Architectural Model

Hermetica follows a **hybrid architecture**:
- A **scalable, centralised Flight Control Stack** for flight-critical functions
- **Distributed Nodes** for extensibility and scalability

### Core Components

- Flight Control Stack
- External Nodes
- Sensor Systems
- Actuation Systems
- CAN-FD Backbone
- Ground Interface

### Design Principles

- **Strict role separation**
- **Decoupled integration** (no hard dependency on specific airframe)
- **Deterministic real-time behavior**
- **Fail-operational where possible**
- **Hardware/software modularity**
- **Open interface specifications**

---

## Logical System Model

### [CAN-FD Message Model](message-model.md)

### System State

The System State is effectively a normalised view of the current operating conditions of the FCU and the airframe.

| Variable              | Data Type | Notes |
|-----------------------|-----------|-------|
| Bench Mode            | BOOL      |       |
| Launched              | BOOL      |       |
| Armed                 | BOOL      |       |
| Current Flight Mode   | ENUM      |       |
| Current Throttle Rate | FLOAT     |       |
| Current Pitch Rate    | FLOAT     |       |
| Current Roll Rate     | FLOAT     |       | 
| Current Yaw Rate      | FLOAT     |       | 
| Current Throttle      | FLOAT     |       |
| Current Pitch         | FLOAT     |       |
| Current Roll          | FLOAT     |       | 
| Current Yaw           | FLOAT     |       | 
| Current Flaps         | FLOAT     |       | 
| Current VSI           | FLOAT     |       | 
| Current Barometric Pressure         | FLOAT     |       | 
| Current Pitot Pressure         | FLOAT     |       | 
| Current Barometric Altitude         | FLOAT     |       | 
| Current GPS Altitude         | FLOAT     |       | 
| Current Ranged Altitude         | FLOAT     |       | 

| Variable              | Data Type | Notes |
|-----------------------|-----------|-------|
| Input Throttle        | FLOAT     |       |
| Input Pitch           | FLOAT     |       |
| Input Roll            | FLOAT     |       | 
| Input Yaw             | FLOAT     |       | 
| Input Flaps           | FLOAT     |       | 
| Input Differential Throttle           | FLOAT     | Left/Right Throttle bias for twin-engines      | 
| Input Flight Mode    | ENUM     |       | 

| Variable              | Data Type | Notes |
|-----------------------|-----------|-------|
| Output Throttle      | FLOAT     |       |
| Output Pitch         | FLOAT     |       |
| Output Roll          | FLOAT     |       | 
| Output Yaw           | FLOAT     |       | 
| Output Flaps           | FLOAT     |       | 
| Output Differential Throttle | FLOAT     |       | 


### Roles

A Role:
- Owns a clearly defined functional domain
- Has explicitly defined inputs and outputs
- May enforce local constraints within its domain
- May participate in system-wide decision making depending on its authority

The FCU is composed of a set of defined Roles. Each Role encapsulates a specific set of Responsibilities and operates within a defined domain.

Roles are independent of physical implementation and may be assigned to any suitable Stack Layer or Node.

---

#### Flight Control Role

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

#### Sensor Processing Role

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

#### Actuation Role

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

#### Power Management Role

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

#### Communication Role

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

#### System Management Role

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


#### Payload Role

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

### Responsibilities

Responsibilities define behavioural obligations of Roles.

#### Assignment

- Every Responsibility shall be owned by exactly one Role.  
- A Role may own multiple Responsibilities.  
- Responsibilities shall not be shared between Roles to avoid ambiguity in ownership.  

---

#### Scope

- Each Responsibility shall have clearly defined boundaries.  
- Responsibilities shall not overlap in scope.  
- Responsibilities may depend on outputs from other Responsibilities but shall not duplicate them.  

---

#### Inputs and Outputs

- Each Responsibility shall define its required inputs.  
- Each Responsibility shall define its expected outputs.  
- Inputs and outputs shall be explicitly defined to enable consistent integration between Roles.  

---

#### Behavioural Guarantees

- Each Responsibility shall define the conditions it must maintain or enforce.  
- Responsibilities shall produce deterministic outcomes given valid inputs.  
- Responsibilities shall handle invalid or missing inputs in a defined manner.  

---

#### Constraints

- Responsibilities may enforce constraints within their domain.  
- Responsibilities shall operate within constraints imposed by higher-authority Roles.  
- Conflicting constraints shall be resolved according to the Authority model.  

---

#### Failure Handling

- Each Responsibility shall define failure conditions relevant to its domain.  
- Responsibilities shall detect and report failures where possible.  
- Responsibilities shall define safe behaviour in the presence of faults.  

---

#### Independence

- Responsibilities shall be independent of physical implementation.  
- Responsibilities shall not assume specific hardware, communication interfaces, or execution environments.  

---

#### Composability

- Responsibilities shall be designed to allow composition into larger system behaviours.  
- Responsibilities may be reused across different Roles or system configurations, provided ownership remains unambiguous.  

---

### Authority Model

- Higher-authority Roles define intent and constraints
- Lower-authority Roles execute within constraints
- Lower layers may reject unsafe commands
- System Management Role arbitrates conflicts
- Power Management Role has ultimate override in electrical faults

---

### Hierarchical Control Architecture

The FCU implements a hierarchical control architecture where high-level intent is progressively transformed into physical actuation.

---

#### Strategic Level (may be omitted)
- Defines long-term objectives (map this entire area)

#### Tactical Level
- Defines short-term objectives (waypoints, timing)

#### Execution Level
- Generates paths and vectors

#### Flight Dynamics Level
- Converts paths into attitude/altitude targets

#### Flight Control Level
- Stabilises via control loops

#### Actuation Level
- Drives physical outputs

---

## Functional Capabilities

### Flight Control

- Stabilisation
- Navigation
- Mission execution

(Control strategy TBD)

---

### Flight Modes

- **Disabled / Fully Manual Control**  
Direct RC receiver pass-through with no protection from pilot error

#### Building Blocks

- **Controlled Throttle**  
Throttle is directly proportional to pilot input within configurable safety margins
- **Controlled Pitch-Rate**  
Pitch-rate is directly proportional to pilot input within configurable safety margins
- **Controlled Pitch**  
Pitch is directly proportional to pilot input within configurable safety margins
- **Controlled Roll-Rate**  
Roll-rate is directly proportional to pilot input within configurable safety margins
- **Controlled Roll**  
Roll is directly proportional to pilot input within configurable safety margins
- **Controlled Yaw-Rate**  
Yaw-rate is directly proportional to pilot input within configurable safety margins

- **Auto Pitch-Rate**  
Pitch is controlled by the FCU, any pilot input disables **Auto Pitch-Rate** and enables **Controlled Pitch-Rate**

- **Auto Pitch**  
Pitch is controlled by the FCU, any pilot input disables **Auto Pitch** and enables **Controlled Pitch**

- **Auto Roll-Rate**  
Pitch is controlled by the FCU, any pilot input disables **Auto Roll-Rate** and enables **Controlled Roll-Rate**

- **Auto Roll**  
Pitch is controlled by the FCU, any pilot input disables **Auto Roll** and enables **Controlled Roll**


- **Altitude Hold**


- **Airspeed Hold**
  - Enables **Controlled Throttle**

#### Basic Flight Modes

- **Semi-Manual**  
Limited RC receiver pass-through with configurable airframe and flight-envelope protection
- **Stabilised**  
IMU assisted flight, airframe will maintain whatever *attitude-rates* the pilot commands
  - Enables **Controlled Throttle**
  - Enables **Controlled Pitch-Rate**
  - Enables **Controlled Roll-Rate**
  - Enables **Controlled Yaw-Rate**
- **Autolevel**  
IMU assisted flight, airframe will maintain whatever *attitude* the pilot commands until the command is ceased, then the airframe will self-level pitch and roll
  - Pilot throttle input controls throttle directly
  - Pilot yaw input controls rudder directly
  - Pilot pitch input controls final pitch angle
  - Pilot roll input controls final roll angle

- **Launch** *TODO: This seems unsafe, review*    
Aircraft will maintain a preset throttle and pitch angle until the mode is disengaged
  - Pilot throttle input controls are disabled, the aircraft maintains the configured launch throttle
  - Pilot yaw input controls are disabled, the aircraft maintains the yaw angle of launch
  - Pilot pitch input controls are disabled, the aircraft maintains the configured launch pitch
  - Pilot roll input controls are disabled, the aircraft maintains a roll angle of zero  


- **Loiter**  
Aircraft will fly in a circle around the location the aircraft is at when the mode is enabled, maintaining a fixed altitude. If no GPS/GNSS is available then the aircraft will use the IMU to maintain a constant yaw rate while maintaining a fixed altitude.

#### Advanced Flight Modes

- **Return To Home (RTH)**  
Aircraft will return to the point the flight started then enter **Loiter** mode at that point


- **Failsafe**
- **Autonomous**

---

| Mode        | Throttle  |  Yaw | Pitch | Roll | Flaps |
|-------------|-----------|------|-------|------|-------|
| Full-Manual | Direct    | Direct | Direct | Direct | Direct |
| Semi-Manual | Limited   | Limited | Limited | Limited | Limited |
| Stabilised  |-----------|------|-------|------|-------|
| Autolevel   |-----------|------|-------|------|-------|
| Loiter      |-----------|------|-------|------|-------|
| RTH         |-----------|------|-------|------|-------|
| Launch      |-----------|------|-------|------|-------|
| Full-manual |-----------|------|-------|------|-------|
| Full-manual |-----------|------|-------|------|-------|
| Full-manual |-----------|------|-------|------|-------|
| Full-manual |-----------|------|-------|------|-------|
| Full-manual |-----------|------|-------|------|-------|
| Full-manual |-----------|------|-------|------|-------|
| Full-manual |-----------|------|-------|------|-------|
| Full-manual |-----------|------|-------|------|-------|
| Full-manual |-----------|------|-------|------|-------|
| Full-manual |-----------|------|-------|------|-------|

---

### Sensor Handling

- IMU, GNSS, Barometer, Magnetometer
- Optional sensors

Sensor fusion TBD

---

### Communication

#### Flight Stack Backbone:

The Backbone must provide both power, data about the power and communications and signalling. 

**Power:**

| Name | Notes                                                   |
|-----------------------|----------------------------------------|
| 5V<sub>STACK</sub>    | Power bus for the Flight Stack         |
| 5V<sub>NODES</sub>    | Power bus for the Nodes                | 
| GND<sub>POWER</sub>   | Power ground                           |
| GND<sub>SIGNAL</sub>  | Signal ground                          |

**Communications & Signalling:**

| Name                  | Data type | Description                         |
|-----------------------|-----------|-------------------------------------|
| PG<sub>STACK</sub>    | BOOL      | 5V<sub>STACK</sub> power good       |
| LOAD<sub>STACK</sub>  | SCALAR    | Load of 5V<sub>STACK</sub> bus      |
| PG<sub>NODES</sub>    | BOOL      | 5V<sub>NODES</sub> power good       |
| LOAD<sub>NODES</sub>  | SCALAR    | Load of 5V<sub>NODES</sub> bus      |
| CAN1                  | DIFF      | CAN-FD communications bus           |
| CAN2                  | DIFF      | CAN-FD communications bus           |
| RESET                 | BOOL      | Flight Stack reset signal           |
| ARMED                 | BOOL      | FCU "Armed" state                   |
| SYNC                  | BOOL      | Deterministic synchronisation pulse |
| BENCH                 | BOOL      | Debugger plugged in somewhere       |




**Secondary Interfaces:**
- UART (FBUS, SBUS, GPS, debugging)
- SPI / I²C (local peripherals)


---

### Payload Control

- CAN-FD controlled payloads

---

### Logging & Telemetry

- Storage
- Streaming
- Blackbox

---

## Physical System

### Standardised Parts List

Although individual nodes will require specific hardware, it is sensible to standard parts across all components where feasible.

| Type           | Manufacturer               | Part Number | Package            |
|---|---|---|---|
| Microcontroller  | STM | STM32H755ZIT6 | LQFP-144 20x20x1.4 |
| Microcontroller  | STM | STM32G484CET6 | LQFP-48 7x7x1.4 |
|   |  |  |  |
|   |  |  |  |
|   |  |  |  |
|   |  |  |  |

---

### Physical Architecture

- Vertical PCB stack
- External nodes
- Backbone interconnect

---

### Processing Architecture

- MCU: Will be dependant upon individual stack layer requirements:
  - STM32H755ZIT6 for high performance layers running at 480 MHz  
  - STM32G484CET6 for low performance requirements running at 170 MHz  

| MCU           | Core                | Clock Speed | Package            |
|---------------|---------------------|-------------|--------------------|
| STM32H755ZIT6 | M7 + M4 (Dual Core) | 480 Mhz     | LQFP-144 20x20x1.4 |
| STM32G484CET6 | M4                  | 170 Mhz     | LQFP-48 7x7x1.4    |

---

### Memory

| MCU           | Flash   | RAM     | 
|---------------|---------|---------|
| STM32H755ZIT6 | 2048 kB | 1024 kB |
| STM32G484CET6 | 512 kB  | 128 kB  |

- External storage: Each MCU will have dedicated FRAM for local configuration.  

---

### Electrical Architecture

- Dual 2S–8S input with automatic failover
- Multiple power domains
- Protection systems

---

#### System Power

##### Power Domain PD1

Source: Primary Batteries (2S to 8S), Dual configuration with automatic failover  
Voltage: 6.0V to 33.6V  
Current: 8A max  
Usage: Source for PD2, PD3, PD4  

---

##### Power Domain PD2

Source: PD1  
Voltage: 6.0V to 33.6V  
Current: 2A max  
Usage: Primary power pass-through for VTX etc  

---

##### Power Domain PD3

Source: PD1  
Voltage: 5V  
Current: 3A max  
Usage: Primary 5V for Flight Stack  

---

##### Power Domain PD4

Source: PD1  
Voltage: 5V  
Current: 3A max  
Usage: Primary 5V for Nodes  

---




### Hardware Platform

- STM32H755ZIT6
- STM32G484CET6
- FRAM per node

---

### Interfaces

- FDCAN (dual)
- UART
- SPI
- I²C
- GPIO

---

### Backbone

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


| Name | Notes                                                   |
|-----------------------|----------------------------------------|
| 5V<sub>STACK</sub>    | Power bus for the Flight Stack         |
| 5V<sub>NODES</sub>    | Power bus for the Nodes                | 
| GND<sub>POWER</sub>   | Power ground                           |
| GND<sub>SIGNAL</sub>  | Signal ground                          |

**Communications & Signalling:**

| Name                  | Data type | Description                         |
|-----------------------|-----------|-------------------------------------|
| PG<sub>STACK</sub>    | BOOL      | 5V<sub>STACK</sub> power good       |
| LOAD<sub>STACK</sub>  | SCALAR    | Load of 5V<sub>STACK</sub> bus      |
| PG<sub>NODES</sub>    | BOOL      | 5V<sub>NODES</sub> power good       |
| LOAD<sub>NODES</sub>  | SCALAR    | Load of 5V<sub>NODES</sub> bus      |
| CAN1                  | DIFF      | CAN-FD communications bus           |
| CAN2                  | DIFF      | CAN-FD communications bus           |
| RESET                 | BOOL      | Flight Stack reset signal           |
| ARMED                 | BOOL      | FCU "Armed" state                   |
| SYNC                  | BOOL      | Deterministic synchronisation pulse |
| BENCH                 | BOOL      | Debugger plugged in somewhere       |


##### CAN1<sub>LOW</sub>, CAN1<sub>HIGH</sub>

##### CAN2<sub>LOW</sub>, CAN2<sub>HIGH</sub>

##### 5V<sub>STACK</sub>, GND<sub>POWER</sub>

##### 5V<sub>NODES</sub>, GND<sub>NODES</sub>

##### nRESET

(Hard to seperate logical from physical here.)

The nRESET signal may be set LOW by any Stack Layer in the Flight Control Stack.
The signal is pulled HIGH by default by the Stack Layer with the Power Management Role.


---

### Flight Stack Layers

All Flight Stack Layers communicate over the Backbone via CAN1, CAN2, or both.

#### Power Distribution Board

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

#### Flight Control Board

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

#### Future Boards
- Navigation
- OSD
- Storage
- Radar

---

### Connectors

#### Debug Connector

Standardised connector across all PCB's for connection to a debugging board.

- Connector: Samtec T1M, 10 pin, horizontal, single-row, [SAMTEC-T1M-10-F-SH-L](https://www.samtec.com/products/t1m-10-f-sh-l)
- Height: 4.6mm

| Pin  | Name               | Notes                                                                                         |
|------|--------------------|-----------------------------------------------------------------------------------------------|
| 1    | V<sub>TARGET</sub> | Voltage of target MCU                                                                         |
| 2    | GND                | Target GND                                                                                    |
| 3    | nDebuggerPresent   | Signal to indicate the debugging board is fitted to any Node or Flight Stack Layer (0V to V<sub>TARGET</sub>) |
| 4    | SWDIO              | SWD - SWDIO signal                                                                            |
| 5    | SWCLK              | SWD - SWCLK signal                                                                            |
| 6    | SWO                | SWD - SWO signal                                                                              |
| 7    | BOOT0              | Pull-down on target on board                                                                  |
| 8    | nRESET             | Pull-up on target board                                                                       |
| 9    | MCU<sub>TX</sub>   | UART connection to MCU TX                                                                     |
| 10   | MCU<sub>RX</sub>   | UART connection to MCU RX                                                                     |


#### FITTED

FITTED is pulled to  V<sub>TARGET</sub> on the debugging board. This is to enable the detection of the debugging hardware should conditional logic be required.
For example, the FCU could refuse to arm the motors and spin the propellors if FITTED was high. You know, to save fingers.

#### BOOT0

BOOT0 is broken out so that the MCU can be forced to start in bootloader mode in an emergency. It should be pulled-down on the target PCB.

#### MCU<sub>TX</sub> and MCU<sub>RX</sub>
- 
MCU<sub>TX</sub> and MCU<sub>RX</sub> are broken out so that the MCU can be emergency flashed. They should be connected to a USART capable of flashing the bootloader.  
- For most STM32 MCU's this is USART1, USART2 or USART3. During normal operation, the USART may be used for debugging.  

---

#### CAN-FD Connector
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

## Firmware Architecture

### Execution Model
- The primary firmware will be written in C/C++ on a *baremetal* level (HAL is acceptable)  

---

## Safety & Fault Handling

### Fault Classes

- Sensor failure  
- Actuator failure  
- Power instability  
- Communication loss  
- Internal software fault  

---

### Detection Mechanisms

- [x] Watchdogs (independent + windowed)  
- [x] Heartbeats (inter-module)  
- [x] Plausibility checks  

---

### Failsafe Behaviour

|TBC

---

## Performance Targets

- Control loop frequency: 1 kHz  
- End-to-end latency: sub 20 ms  
- Boot time: sub 3 s  
- Minimum supported nodes (CANFD): 24  
- Minimum actuator count: 8  

---

## Environmental Requirements

- Operating temperature: -40C to 120C (low temperature handling is much more important)  
- TBC

---

## Security

TBC 

---

## Verification Strategy

TBC

---

## Integration Model

Hermetica must remain **independent of Aetherion** until integration.

### Negotiated Parameters

| Parameter | Hermetica Target                         | Aetherion Constraint | Status |
|-----------|------------------------------------------|----------------------|--------|
| Weight    | < 500g                                   | 1.0kg limit          | [x]    |
| Power     | 2S to 8S supply, 500mA total consumption | ___                  | [ ]    |
| Size      | 60mm(W) x 80mm(L) x 50mm(H)              | ___                  | [ ]    |


---

## MVP Definition

**MVP must include:**
- CAN-FD working backbone for messaging between Flight Stack Layers and Nodes
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

## Future Extensions

TBC