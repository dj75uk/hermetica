# Project Hermetica – Physical System Specification

## Physical System

### Standardised Bill of Materials

| Type               | Manufacturer | Part Number       | Package            | Usage            |
|--------------------|--------------|-------------------|--------------------|------------------|
| Microcontroller    | STM          | STM32H755ZIT6     | LQFP-144 20x20x1.4 | High speed compute |
| Microcontroller    | STM          | STM32G484CET6     | LQFP-48 7x7x1.4    | Low speed compute                 |
| CAN-FD Transceiver | TI           | TCAN3403-Q1       |                    | CAN-FD |
| ESD Protection     | Nexperia     | PESD2CANFD24V-QCZ |                    | CAN-FD ESD Protection |
| Voltage Regulation | TI           | TPSM63603         |                    | 8S to 5.0V 3A bus regulation |
| Voltage Regulation | TI           | LP5912Q3.3        |                    | 5V to 3.3V 500mA point regulation |
| Choke              | TDK          | ACT1210D1012PTL00 |                    | CAN-FD |
| Diode              | STM          | BAT60FILM         |                    | General |
| FRAM               | Fujitsu      | MB85RS1MT         |                    | SPI Configuration storage |

---
