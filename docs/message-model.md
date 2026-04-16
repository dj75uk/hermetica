## CAN-FD Message Model

### Overview

The Hermetica CAN-FD message model defines a deterministic, scalable, and role-aligned communication architecture for all Flight Stack Layers and external Nodes.

The model is designed to:
- Ensure predictable timing and latency
- Maintain clear data ownership
- Enable fail-operational behaviour
- Scale to 64+ nodes without degradation

---

### Design Principles

- Publisher-based architecture (no polling for critical data)
- Time-triggered delivery for flight-critical messages
- Event-triggered delivery for asynchronous conditions
- Strict ownership of data
- No ambiguity in message meaning
- Deterministic scheduling over CAN-FD

---

### Message Classes

All messages shall belong to exactly one class.

| Class   | Purpose                    | Timing        | Priority |
|---------|----------------------------|---------------|----------|
| COMMAND | Control inputs and targets | Periodic      | Highest  |
| STATE   | System and sensor state    | Periodic      | High     |
| STATUS  | Health and diagnostics     | Periodic      | Medium   |
| EVENT   | Faults and transitions     | Asynchronous  | Highest  |
| CONFIG  | Configuration and params   | On-demand     | Low      |
| DEBUG   | Development and logging    | Opportunistic | Lowest   |

---

### CAN Identifier Structure (29-bit Extended)

All messages shall use extended (29-bit) CAN identifiers.

#### Bit Layout

| Field         | Bits |
|--------------|------|
| Priority     | 3    |
| Class        | 3    |
| Source Node  | 7    |
| Message Type | 8    |
| Instance     | 8    |

#### Field Definitions

- Priority: Lower value = higher priority (CAN arbitration)
- Class: Message class identifier
- Source Node: Unique node ID (0–127)
- Message Type: Defines payload meaning
- Instance: Identifies multiple instances (e.g. IMU1, IMU2)

---

### Node Identity Model

Each node shall have a unique Node ID.

#### Reserved Node IDs

| Node ID  | Description               |
|---------|---------------------------|
| 0x00    | Flight Control Board      |
| 0x01    | Power Management          |
| 0x02–0x0F | Core Stack Layers       |
| 0x10–0x7F | External Nodes          |

---

### Message Types

#### COMMAND Messages

Ownership: Flight Control Role  
Priority: Highest  
Timing: Periodic

| Type ID | Name             | Description            |
|--------|------------------|------------------------|
| 0x01   | Actuator Command | Final actuator outputs |
| 0x02   | Attitude Target  | Roll, pitch, yaw       |
| 0x03   | Rate Target      | Angular rates          |

Requirements:
- Fixed interval transmission
- No request/response
- No delays

---

#### STATE Messages

Ownership: Sensor Processing Role  
Priority: High  
Timing: Periodic

| Type ID | Name              | Description                  |
|--------|-------------------|------------------------------|
| 0x10   | IMU State         | Accel + gyro                 |
| 0x11   | Attitude Estimate | Fused orientation            |
| 0x12   | Position Estimate | GNSS position                |
| 0x13   | Air Data          | Airspeed + pressure          |

---

#### STATUS Messages

Ownership: Each node  
Priority: Medium  
Timing: Periodic

| Type ID | Name            | Description              |
|--------|-----------------|--------------------------|
| 0x20   | Node Health     | OK/WARN/FAIL             |
| 0x21   | Power Status    | Voltage, current         |
| 0x22   | Actuator Status | Feedback                 |

---

#### EVENT Messages

Ownership: Any node  
Priority: Highest  
Timing: Asynchronous

| Type ID | Name                 | Description           |
|--------|----------------------|-----------------------|
| 0x30   | Fault Raised         | Fault detected        |
| 0x31   | Fault Cleared        | Fault cleared         |
| 0x32   | Mode Change          | Flight mode change    |
| 0x33   | Arming State Change  | Armed/disarmed        |

Requirements:
- Immediate transmission
- Rate-limited

---

#### CONFIG Messages

Ownership: System Management / Ground Interface  
Priority: Low  
Timing: On-demand

Used for:
- Parameter read/write
- Node configuration

Allows request/response.

---

#### DEBUG Messages

Ownership: Any node  
Priority: Lowest  
Timing: Opportunistic

Used for:
- Logging
- Debugging

Constraints:
- Must be droppable
- Must be rate-limited
- Disabled in flight

---

### Payload Design Rules

- Fixed-size structures preferred
- No dynamic allocation
- No strings
- Packed binary format
- Consistent units across system

#### Example: Actuator Command

```c
struct {
  int16_t throttle;
  int16_t roll;
  int16_t pitch;
  int16_t yaw;
  int16_t flaps;
  int16_t differential_throttle;
};
```

---

### Timing Model

#### Update Rates

| Data Type         | Rate        |
|------------------|------------|
| IMU              | 1000 Hz     |
| Attitude         | 500 Hz      |
| Actuator Command | 500–1000 Hz |
| GNSS             | 10 Hz       |
| Status           | 10–50 Hz    |

---

### Synchronisation

- Global SYNC signal required
- All nodes align to SYNC
- Messages transmitted in defined time slots

---

### Example Schedule

| Time (ms) | Message            |
|----------|--------------------|
| 0.0      | IMU State          |
| 0.5      | Attitude Estimate  |
| 1.0      | Actuator Command   |
| 2.0      | Status             |

---

### Bus Load Strategy

- High-rate messages restricted to core stack
- External nodes use lower rates
- Debug traffic disabled during flight

---

### Fault Handling

#### Detection

- Missing periodic messages = fault
- No ACK required

#### Behaviour

- Consumers implement timeouts
- System degrades gracefully

---

### Determinism Rules

- No blocking on CAN
- No request/response in critical paths
- All critical data is publisher-driven
- Timing must be bounded and predictable

---

### Summary

This model provides:
- Deterministic communication
- Clear ownership of data
- Predictable behaviour
- Scalable distributed architecture

---
