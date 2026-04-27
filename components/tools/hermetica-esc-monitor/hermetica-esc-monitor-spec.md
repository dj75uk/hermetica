# Hermetica ESC / Motor Monitor

## Purpose

Standalone PCB to monitor and control ESC / Motor combinations for testing purposes.

Born out of interaction with T-Motor where they are sending me evaluation hardware. I wish to provide them a good solid dataset in return, aiming for future collaboration. Could also be used for other manufacturers in a similar way.

Not designed to be part of the Hermetica FCU system - but designed to optionally work with it.

The system shall be able to operate on a bench
The system shall be able to operate in an airframe on the ground
The system shall be able to operate in an airframe in the air
The system shall be capable of recording data to SD-card at a rate of 200Hz
The system shall be capable of injecting signal faults into the data stream to the ESC:
- ESC-side signal to GND
- ESC-side signal to 3.3V
- Board-ESC capacitance 
- Board-ESC resistance
Monitoring must be non-invasive and testing should be non-destructive.

## Operating Modes

The system shall be able to operate in a number of distinct modes:
- "Bench Mode", signal generation, requires no external signal from RC Receiver.  
_PCB acts as a signal generator, using specific test profiles._
- Pass-through, for use with a RC Receiver.  
_PCB acts as a signal monitor and logs data only_



## INPUTS

**Power**:
2S to 8S primary battery
5V BEC (integrated with ESC under test)

**ESC Signals**:
PWM from RCRX
UART from RCRX

Note: PWM and UART are on the same physical connector pin.

**External sesnsors:**
Temperature monitoring via NTC
Accelerometer for vibration SPI
RPM pulse

## OUTPUTS

**ESC Signals:**
PWM generated on board (standalone mode)
PWM from RCRX
UART generated on board (standalone mode)
UART from RCRX

Note: PWM and UART are on the same physical connector pin.

Signals from RCRX must be intercepted for timing.

## Example test profiles

Test profiles are only for use in "bench" mode.

All profiles are based upon changing throttle signal.

Profiles stored on SDCard

Examples:

- Baseline (0% to 100% throttle, over 300 seconds)
- Slow ramp (0% to 100% throttle, over 10 seconds)
- Fast ramp (0% to 100% throttle, over 2 seconds)
- Takeoff, cruise, landing (80%, 30%, 10%)
- Instantaneous X% from stop (for detecting spool up speed)
- Instantaneous X% to stop (for detecting spool down speed)
- Sawtooth %X to %Y to %X, repeating

## Definite Hardware
- Additional hardware will be required.
- INA238 - for voltage and current monitoring of PD1
- STM32H755ZIT6 - for control and primary sensor monitoring
- WSL27261L300FEA - current shunt, 3W, 1.3mOhm
- Single LM51606 - primary supply to 5V regulation (600mA)
- TDK VLS5045EX-330M - 33uH coil for LM51606
- Multiple LP5912Q3.3 - 5V to 3.3V local point regulation for each on board power domain
- SMBJ36A - TVS across supply for some protection
- BAT60J - standard schottky
- IIS3DWBG1TR - accelerometer, fixed to motor mount on small PCB for measuring vibration  
- SN74LVC1G17 for signal buffering where needed
- SN74LVC1G3157 SPDT analogue switch for signal routing

## External optical RPM sensor
- [Needs fleshing out]
- Minimal BOM, keep PCB as small as possible
- Will have to fit inside airframe nacel - limited space
- Ideally operates at 3.3V and provides a 3.3V pulse output
- Provides pulse when reflection detected
- Buffered on monitoring board before being fed to a timer capture

## External accelerometer 

Small external square PCB drilled to fit a standard X shaped motor mount. IIS3DWBG1TR in the centre communicating via SPI. Designed to fit between the motor mount and the engine "firewall" (usually a wooden plate in RX airframes). Could also be mounted to the firewall instead. Short length (<20cm) connection to the main PCB carrying:

- 5.0V from PD2
- GND
- SPI_CLK
- SPI_MISO
- SPI_MOSI
- SPI_CS
- INT1
- INT2

Local 5.0V supply through local LDO to 3.3V due to extremely noisy environment. PCB will be placed next to the motor, less than 2cm from the windings.
Could be left in place in airframe. The weight will be negligible.

- Uses 4-wire SPI, not I2C
- PCB 43.18mm x 43.18mm (1.6mm thick)
- Copper top and bottom with large GND pours
- Need to consider via-wall strategy around IIS3DWBG1TR 
- Single-sized assembly
- Tallest component 0805 sized SMD capacitor
- Series resistors (22R) on SPI_MISO, INT1, INT2
- Pullup on SPI_CS (10K)
- SMD capacitors sized at 0805
- SMD resistors sized at 0603
- Onboard LDO regulation of 5.0V supply to 3.3V via LP5912Q3.3
- IIS3DWBG1TR placed in exact centre of PCB
- 4 x 4.3mm mounting holes drilled in corners of PCB to match motor mount
- Keep-out zones around mounting holes
- Small LED for power indication for easy visual inspection
- Capacitors/resistors sized to avoid physical damage from vibrations
- INT1 and INT2 configured as push-pull, not open-drain

## External lines to have 22R series resistors

- SPI_CLK (close to MCU)
- SPI_CS (close to MCU)
- INT1 from accelerometer (close to accelerometer)  
- INT2 from accelerometer (close to accelerometer)  
- RPM input pulse (close to MCU)

## Power Domains

- **PD1 (2S to 8S)**   
Primary power from 2S to 8S battery.
Passes through PCB mounted shunt resistor before feeding ESC (high-side monitoring).
Will be noisy and have large transients.
Physically connected to PCB by AWG10 wire and M2.5 mounting holes (through hole to all copper layers).

- **PD2 (5.0V)**  
PD1 -> LP51606 -> PD2
5V power from LP51606, max 600mA.
Slightly less noisy that PD1 but still not great.
Delivered through BAT60J to prevent backfeeding to 5V rail.
Suitable for post-regulation.

- **PD3 (3.3V)**  
PD2 -> LP5912Q3.3 (1) -> PD3
3.3V power for MCU digital domain.
Filtered.
Directly powers MCU and digital peripherals.

- **PD4(3.3V)**  
PD2 -> LP5912Q3.3 (2) -> PD4
3.3V power for MCU analogue domain.
Filtered.
Should be _very_ clean.


## Monitoring Metrics

Average Current during 5ms Window
Peak Current during 5ms Window
Average Primary Voltage during 5ms Window
Minimum Primary Voltage during 5ms Window (to monitor voltage sag)
Motor Temperature (stator probably easier than windings)
ESC Temperature
Ambiant temperature (for calculating esc/motor temp rise)
Vibration

RPM derived from timestamped pulse intervals,
resampled into the 200Hz dataset.

Temperature measurement is slow and approximate. The delta (rise) is more important than the absolute value.

## Recording @ 200Hz

All metrics recorded at same rate, regardless of their update speed.
Writing handled by M4 core. Data streamed to local microSD-card, with dual circular buffers and DMA.

## Bench vs Monitoring mode
- Configured by physical jumper
- Defaults to Monitoring mode if no jumper fitted

## Safety Limits
Only enforced in bench mode, Monitoring Mode just allows the board or device to fail. This is a design decison.

### System design limits
- Global limits per board:
  - Maximum shunt temperature
  - Maximum shunt current
- LED indication of approaching 90% of system limit
- Performs graceful stop of currently executing test profile when limit reached for safety

### Device Under Test (DUT) limits
- Specific limits per device:
  - Maximum ESC temperature
  - Maximum Motor temperature
  - Maximum shunt current
  - Maximum RPM
- LED indication of approaching 90% of DUT limit
- Performs graceful stop of currently executing test profile when limit reached for safety

## Control flow

**Bench Mode**
- MCU generates PWM or UART signal
- 2:1 Analogue switch routes MCU output to fault network
- Fault network routes signal to ESC

**Monitoring Mode**
- External RC receiver generates PWM or UART signal
- Signal is buffered in parallel for monitoring while the primary path remains independent.
    - Primary path routed to ESC
    - Parallel path buffer via SN74LVC1G17 and routed to either MCU TIMx capture input or UARTx_RX via 2:1 analogue switch
- Fault network completely bypassed

**Jumpers: JP1 - Operating Mode Selection**
- Low : Monitoring Mode (Default if not fitted)
- High: Bench Mode

**Jumpers: JP2 - Signal Type Selection**
- Low : PWM (Default if not fitted)
- High: UART

**Analogue Switches: SW1**
- JP1 = Low/Not Fitted
- Switch controlled by JP2
- 2:1
- Position 1: Incoming signal -> MCU TIMx capture input
- Position 2: Incoming signal -> MCU UARTx_RX

**Analogue Switches: SW2**
- JP1 = High
- Switch controlled by JP2
- 2:1
- Position 1: MCU TIMx output -> Fault Network
- Position 2: MCU UARTx_TX -> Fault Network

**Routing Table**
| JP1 | JP2 | SW1 | SW2 |
|-----|-----|-----|-----|
| L   | L   | Routes incoming signal to MCU TIMx capture input in parallel to ESC | _Unused_ |  
| L   | H   | Routes incoming signal to MCU UARTx_RX in parallel to ESC | _Unused_ |  
| H   | L   | _Unused_ | Routes MCU TIMx output to Fault network |  
| H   | H   | _Unused_ | Routes MCU UARTx_TX to Fault Network |  

## MCU ADC Peripheral Usage

| Peripheral | Channel | Usage                |
|------------|---------|----------------------|
| ADC1       | IN2     | BEC Voltage          |
| ADC1       | IN3     | ESC Temperature      |
| ADC1       | IN4     | motor Temperature    |
| ADC1       | IN5     | Ambiant Temperature  |
| ADC1       | IN6     | Shunt Temperature    |
| ADC1       | IN7     | Spare / Unused       |

## Fault injection
- Only in Bench Mode

### GND -> SIGNAL_OUT
- Controlled by GPIO to N-FET
- Gate pulled down by default

### 3.3V -> SIGNAL_OUT
- Controlled by GPIO to N-FET
- Gate pulled down by default

### Signal IN -> resistance -> SIGNAL_OUT
- 1:2 switch controlled by GPIO

### Signal IN -> capacitance -> SIGNAL_OUT
- 1:2 switch controlled by GPIO

### Signal IN -> capacitance to GND -> SIGNAL_OUT
- 1:2 switch controlled by GPIO

### Hi-Z -> SIGNAL_OUT
- Analogue switch with disable

Hardware-controlled analogue switching network. For testing ESC integrity (NOT in the air!).

Fault injection to be last stage before signal sent to ESC.

## BEC Voltage monitoring

- Voltage monitoring of ESC BEC, through a fixed load to check for voltage sag under ESC throttle changes. Could potentially very useful data for manufacturers if I can get it working. Perhaps a fixed 500mA load on PCB with heatsink. Check how BEC voltage performs at high ESC dI/dt.

- Monitored via a voltage divider into an ADC channel

## External sync
- Still need to determine how it works
- Basic idea is to reset internal timers on sync pulse rising edge
- Single pulse enough or repeated periodic ones to remain in sync?
- How to determine master / slave? Wiring? Each board has SYNC_IN (slave) and SYNC OUT (master)?


## Main questions at present

- Decide how to load the BEC
- Best way to design a miniature temperature probe for measuring motor stator temperature
- MCU pinout:
  - MCU timers for signal generation and capture (TIM1 / TIM8)
  - ideal pins for PWM capture and generation
- 16MHz MCU timing crystal selection and tuning caps
- Best MCU clock speed (try 200MHz and tune)
- ADC is needed for reading temperatures via voltage dividers. Anything else? BEC voltage?
- SDCard hardware considerations (I've never designed a board with one before - learning curve).
  - Use SDMMC peripheral not SPI
  - What pullups/pulldowns on which pins?
  - Decoupling caps?
  - How to handle card-detect?
- Baseline thrust curve on bench.
- Should fit a standard "Hermetica-class" CAN-FD chain to the PCB, due to T-Motor's interest in CAN. No need to write any firmware. Hardware can just sit there doing nothing in case it's needed in future. Very small price to pay for future proofing.
- FUTURE: Consider ways to use baseline thrust-curve in the air if FCU connected via CAN.