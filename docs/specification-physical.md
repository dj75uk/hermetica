# Project Hermetica – Physical System Specification

**Version:** 0.6  
**Date:** 2026-04-15  
**Owner:** Daniel Johnson  
**Status:** Draft  

---

## Physical System

### Standardised Bill of Materials

Although individual components will require specific hardware, it is sensible to standard parts across all components where possible.

[Standardised Bill of Materials](specification-physical-standard-bom.md)

---

### Flight Control Stack

#### Communications Backbone

#### Stack Layers

##### Power Distribution and Monitoring Layer

##### Physical Architecture

- Vertical PCB stack
- External nodes
- Backbone interconnect

---

##### Processing Architecture

- MCU: Will be dependant upon individual stack layer requirements:
  - STM32H755ZIT6 for high performance layers running at 480 MHz  
  - STM32G484CET6 for low performance requirements running at 170 MHz  

| MCU           | Core                | Clock Speed | Package            |
|---------------|---------------------|-------------|--------------------|
| STM32H755ZIT6 | M7 + M4 (Dual Core) | 480 Mhz     | LQFP-144 20x20x1.4 |
| STM32G484CET6 | M4                  | 170 Mhz     | LQFP-48 7x7x1.4    |

---

##### Memory

| MCU           | Flash   | RAM     | 
|---------------|---------|---------|
| STM32H755ZIT6 | 2048 kB | 1024 kB |
| STM32G484CET6 | 512 kB  | 128 kB  |

- External storage: Each MCU will have dedicated FRAM for local configuration.  

---


##### Electrical Architecture

---

###### System Power

- Dual 2S–8S input with automatic failover
- Multiple power domains
- Protection systems


####### Power Domain PD1

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