# Project Hermetica – System Requirements Specification

**Version:** 0.1  
**Date:** 2026-04-16  
**Owner:** Daniel Johnson  
**Status:** Draft  

---

## 1. Scope

This document defines the high-level system requirements for Project Hermetica.

These requirements describe what the system must achieve independent of specific implementation details.

---

## 2. Functional Requirements

### Flight Control

**FR-001 [MVP]**  
The system shall provide closed-loop stabilised flight control for fixed-wing UAV platforms.

**FR-002 [MVP]**  
The system shall support manual flight control input from at least one external RC receiver.

**FR-002 [MVP]**  
The system shall support aileron control surfaces.

**FR-002 [MVP]**  
The system shall support rudder control surfaces.

**FR-002 [MVP]**  
The system shall support elevator control surfaces.

**FR-002 [MVP]**  
The system shall support ruddervator control surfaces.

**FR-002 [FUTURE]**  
The system shall support elevon control surfaces.

**FR-002 [MVP]**  
The system shall support flap control surfaces.

**FR-002 [FUTURE]**  
The system shall support flapperon control surfaces.

**FR-002 [FUTURE]**  
The system shall support canard control surfaces.

**FR-003 [MVP]**
The system shall support at least 4 independant engines.

**FR-003 [MVP]**
The system shall support independant thrust curves for each engine.

**FR-004 [MVP]**
The system shall support multiple airframe configurations.

**FR-004 [MVP]**
The system shall support differential thrust.
- Source: DC-010, DC-025 

**FR-007 [MVP]**  
The system shall support multiple flight modes.

**FR-008 [FUTURE]**  
The system shall support autonomous mission execution.

**FR-009 [MVP]**  
The system shall generate independent control outputs for at least:
- throttle
- pitch
- roll
- yaw
- flaps

---

### Sensor Processing

**FR-012 [MVP]**  
The system shall acquire inertial measurement data from onboard sensors.

**FR-013 [MVP]**  
The system shall support fusion of:
- gyroscope
- accelerometer
- magnetometer

**FR-014 [FUTURE]**  
The system shall support optional integration of:
- GNSS
- barometric pressure
- pitot airspeed
- range sensors

---

### Distributed Architecture

**FR-015 [MVP]**  
The system shall support distributed processing across multiple nodes.

**FR-016 [MVP]**  
The system shall allow independent functional modules to be added without modification of the core flight controller.

**FR-017 [MVP]**  
The system shall support a minimum of 24 connected nodes.

---

## 3. Performance Requirements

**PR-001 [MVP]**  
The primary flight control loop shall operate at 1000 Hz with cycle-to-cycle jitter not exceeding 50 µs.

**PR-002 [MVP]**  
The system shall produce actuator command outputs at a minimum frequency of 500 Hz.

**PR-003 [MVP]**  
The system shall have and end-to-end control latency not exceeding 20 ms.

**PR-004 [MVP]**  
The system shall complete cold boot within 3 seconds.

**PR-005 [FUTURE]**  
The system shall scale to at least 24 networked nodes without loss of deterministic communication.

---

## 4. Safety Requirements

**SR-001 [MVP]**  
The system shall prevent actuator arming while debugging hardware is connected.

**SR-002 [MVP]**  
The system shall detect loss of critical sensor data.

**SR-003 [MVP]**  
The system shall detect loss of communication with critical nodes.

**SR-004 [MVP]**  
The system shall transition to a defined failsafe state upon loss of pilot control input.

**SR-005 [MVP]**  
The system shall reject commands that exceed defined airframe safety limits.

**SR-006 [MVP]**  
The system shall tolerate failure of non-critical nodes without loss of flight control.

---

## 5. Interface Requirements

### Internal Communication

**IR-001 [MVP]**  
The system shall support deterministic communication
between distributed nodes.

**IR-002 [MVP]**  
All stack layers shall communicate over a shared internal backbone.

**IR-003 [MVP]**  
The system shall support deterministic time-synchronised communication between nodes.

---

### External Interfaces

**IR-004 [MVP]**  
The system shall support at least one RC receiver interface.

**IR-005 [MVP]**  
The system shall provide a standardised debug interface on each stack layer.

**IR-006 [FUTURE]**  
The system shall support a ground telemetry interface.

---

## 6. Electrical Requirements

**ELR-001 [MVP]**  
The system shall operate from a dual redundant battery input.

**ELR-002 [MVP]**  
The system shall support an input voltage range of at least 2S to 8S LiPo equivalent.

**ELR-003 [MVP]**  
The system shall provide isolated power domains for:
- flight stack
- external nodes

**ELR-004 [MVP]**  
The system shall monitor:
- voltage
- current
- temperature
- power consumption

---

## 7. Mechanical Requirements

**MR-001 [MVP]**  
The system shall support a vertically stacked PCB architecture.

**MR-002 [MVP]**  
Individual stack layers shall be replaceable without redesign of adjacent layers.

**MR-003 [MVP]**  
External nodes shall connect using defined standard interfaces.

---

## 8. Environmental Requirements

**ENV-001 [MVP]**  
The system shall operate at temperatures between -20°C and +70°C minimum.

**ENV-002 [MVP]**  
The system shall tolerate vibration consistent with fixed-wing UAV operation.

**ENV-003 [FUTURE]**  
The system shall tolerate temporary electrical transients without system reset.

---

## 9. Maintainability Requirements

**MT-001 [MVP]**  
The system shall support firmware updates through a standardised service interface.

**MT-002 [MVP]**  
Each node shall support independent firmware programming.

**MT-003 [FUTURE]**  
The system shall support remote firmware updates.

---

## 10. Verification Requirements

**VR-001**  
Each requirement in this document shall be verifiable by one or more of:
- inspection
- analysis
- test
- demonstration

**VR-002**  
Each implemented subsystem shall trace back to one or more requirements defined in this document.

---
