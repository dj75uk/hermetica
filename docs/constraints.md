# Project Hermetica – Design Constraints

**Version:** 0.1  
**Date:** 2026-04-16  
**Owner:** Daniel Johnson  
**Status:** Draft  

---

## 1. Scope

This document defines the design constraints for Project Hermetica.

---

## 2. Constraints

| ID     | Origin       | Constraint |  Notes | 
|--------|--------------|------------|-------| 
| DC-001 | Environment  | Air temperature at 20000ft can be as low as -25C    | |  
| DC-002 | Environment  | Ground temperature can be as high as 50C            | |  
| DC-003 | Power System | Primary battery minimum voltage is 6.4V             | Electrical input lower limit | 
| DC-004 | Power System | Primary battery maximum voltage is 33.6V            | Electrical input upper limit |  
| DC-005 | Airframe A   | Has a dry weight of 1.3kg                           | |  
| DC-006 | Airframe A   | Has a MTOW of 5.5kg                                 | |  
| DC-007 | Airframe A   | Has a RTOW of 4.0kg                                 | |  
| DC-008 | Airframe A   | Has ailerons requiring two servos                   | |  
| DC-009 | Airframe A   | Has airbrakes requiring two servos                  | |  
| DC-010 | Airframe A   | Has an engine layout supporting differential thrust | |  
| DC-011 | Airframe A   | Has canards requiring two servos                    | |  
| DC-012 | Airframe A   | Has elevators requiring two servos                  | |  
| DC-013 | Airframe A   | Has elevons requiring two servos                    | |  
| DC-014 | Airframe A   | Has flaps requiring two servos                      | |  
| DC-015 | Airframe A   | Has landing gear requiring one servo                | |  
| DC-016 | Airframe A   | Has rudders requiring two servos                    | |  
| DC-017 | Airframe A   | Has ruddervators requiring two servos               | |  
| DC-018 | Airframe A   | Has space for a FCU of 150mm x 130mm x 80mm         | |  
| DC-019 | Airframe A   | Has symmetric engines requiring two ESC's           | |  
| DC-020 | Airframe A   | Is a fixed-wing airframe in V-Tail configuration    | |  
| DC-021 | Airframe A   | Requires hand launching                             | Airframe may undergo rapid acceleration and mechanical stress | 
| DC-022 | Airframe B   | Has a dry weight of 1.3kg                           | |  
| DC-023 | Airframe B   | Has a MTOW of 5.5kg                                 | |  
| DC-024 | Airframe B   | Has a RTOW of 4.0kg                                 | |  
| DC-025 | Airframe B   | Has an engine layout supporting differential thrust | |  
| DC-026 | Airframe B   | Has flaperons requiring two servos                  | |  
| DC-027 | Airframe B   | Has ruddervators requiring two servos               | |  
| DC-028 | Airframe B   | Has space for a FCU of 130mm x 130mm x 60mm         | |  
| DC-029 | Airframe B   | Has symmetric engines requiring four ESC's           | |  
| DC-030 | Airframe B   | Is a fixed-wing airframe in V-Tail configuration    | |  
| DC-031 | Airframe B   | Requires hand launching                             | Airframe may undergo rapid acceleration and mechanical stress | 
| DC-032 | Airframe C   | Has a dry weight of 1.3kg                           | |  
| DC-033 | Airframe C   | Has a MTOW of 4.5kg                                 | |  
| DC-034 | Airframe C   | Has a RTOW of 3.0kg                                 | |  
| DC-035 | Airframe C   | Has a single engine requiring one ESC               | |  
| DC-036 | Airframe C   | Has ailerons requiring two servos                   | |  
| DC-037 | Airframe C   | Has an elevator requiring one servo                 | |  
| DC-038 | Airframe C   | Has flaps requiring two servos                      | |  
| DC-039 | Airframe C   | Has a rudder requiring one servo                    | |  
| DC-040 | Airframe C   | Has space for a FCU of 70mm x 60mm x 50mm           | |  
| DC-041 | Airframe C   | Is a fixed-wing airframe in T-Tail configuration    | |  
| DC-042 | Airframe C   | Requires hand launching                             | Airframe may undergo rapid acceleration and mechanical stress | 
| DC-043 | ESC A        | Has a PWM interface                                 | |  
| DC-044 | ESC B        | Has an SBUS interface                               | |  
| DC-045 | ESC B        | Has an SBUS2 interface                              | |  
| DC-046 | ESC C        | Has an FBUS interface                               | |  
| DC-047 | Receiver A   | Has a PWM interface                                 | |  
| DC-048 | Receiver B   | Has an SBUS interface                               | |  
| DC-049 | Receiver C   | Has an SBUS2 interface                              | |  
| DC-050 | Receiver D   | Has an FBUS interface                               | |  
| DC-051 | Servo A      | Has a PWM interface                                 | |  
| DC-052 | Servo A      | Has an SBUS interface                               | |  
| DC-053 | Servo A      | Has an SBUS2 interface                              | |  
| DC-054 | Servo A      | Has an FBUS interface                               | |  

---
