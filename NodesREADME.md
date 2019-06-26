**This document explains how to configure the firmware for the nodes. For more detail on each file in the system please refer to the mbed repository.**

# Sensor Configuration
## Period of Measurment
The time period between each measurement is controlled by an integer number of seconds in the `main.cpp` file. Change the value defined by measurement_period at the top of the file.
```c++
#define measurement_period 3     //period of data measurement in seconds
```
## Type of Sensor
### If the sensor already exists in the library of classes
Simply change line 39 in `main.cpp` by modifying the example_sensor type definition to the correct class name for your sensor.
```c++
example_sensor sensor;
```
### If you need to implement the sensor yourself
Define a new class, in `sensorclasses.h`, for your sensor which inherits from the `Sensor` virtual class.

Make sure that the `Sensor` constructor is called correctly with the I2C address of your sensor, for example the address of the temperature and humidity sensor is 0x45.
```c++
temp_humid_sensor(int address = 0x45) : Sensor(address){}   //set I2C address
```

If a library of functions for the sensor has been developed sepeartely it is possible to implement them into the framework or you can implement the following functions yourself in your class:

* `setup` - run once when device is powered on, use for any initialisation necessary. Defaults to doing nothing.
```c++
virtual void setup(){}  //virtual method to implement any setup of sensor, default to do nothing
```
* `read_data` - called each time a data value is requested, should return a data value and timestamp in a data_item structure. Defaults to returning -1 in the value. You can use the `read` and `write` functions in the `Sensor` class to simplify your implemenation.
```c++        
virtual data_item read_data(){  //virtual method to read the data from a sensor, default to returning -1 in the value
    data_item dflt;
    dflt.value = -1;
    return dflt;
    }
```
* `process` - any post processing of data can be performed here to get a useful value from the sensor, also returns a data_item. structure. Defaults to doing nothing.
```c++
virtual data_item process(data_item data){ //virtual method to process data, default to returning input value.
    return data;
    }
```

As before, then make sure that the sensor object is of the correct type in `main.cpp`.




