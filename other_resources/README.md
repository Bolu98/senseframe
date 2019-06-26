# Sensor Test Code
If you need to implement your own sensor class then there is a version of the code which will allow you to test it with the device giving outputs through serial.

In `sensor_interface_mbed.txt` is a link to an mbed repository with code that can be used to test your implemenations

# Casing
To download the casing follow the links in `CAD_model_for_prototype_casing` and the models are available to download in multiple different formats.

# Power System
## PCB
To access the PCB design follow the link to the Circuit Maker project in `solar-power-circuit-and-pcb-design.txt`.

The current PCB design implements 2 MPPT charge controllers in series to increase the output voltage as the prototype is running on 7-12V rather than 3.3.

The design is a 2 layer PCB with mostly through hole components except for the SPV1040 ICs which are only available in a surface mount TSSOP-8 package.

## Max Voltage and Max Current configuration
The values of resistors that are noted on the schematic will produce a system which will charge a battery up to 7.8 V, with each charge controller set to half of this, and at a maximum of 50mA. These can be modified by changing the value or ratios of certian resistors.
### Max Voltage
There is a potential divider from the output which defines the maximum output voltage of each charge controller circuit with the following formula:

`Vmax = (Ra/Rb + 1)*1.25`

Where for U0:
* Ra -> R10
* Rb -> R11
    
And for U1:
* Ra -> R5
* Rb -> R4

**Note that Ra+Rb should be high so that the power loss through these resistors is sufficciently low**

### Max Current
A value of a small current sense resistor in each circuit, R6 and R12, controls the maximum current output. The maximum current is defined as when there is 50mV accross this resistor. Therefore the formula for maximum current is as follows:
 
 `Imax = 0.05/Rsense`
