**This document explains how to configure the firmware for the nodes. For more detail on each file in the system please refer to the mbed repository.**

# Sensor Configuration
## If the sensor already exists in the library of classes
Simply change line 39 in `main.cpp` by modifying the example_sensor type definition to the correct class name for your sensor.
```
example_sensor sensor;
```
## If you need to implement the sensor yourself
Define a new class, in `sensorclasses.h`, for your sensor which inherits from the `Sensor` virtual class and implement the functions for:
* `setup` - run once when device is powered on, use for any initialisation necessary. Defaults to doing nothing.
* `read_data` - called each time a data value is requested, should return a data value and timestamp in a data_item structure. Defaults to returning -1 in the value.
* `process` - any post processing of data can be performed here to get a useful value from the sensor, also returns a data_item. structure. Defaults to doing nothing

As before, then make sure that the sensor object is of the correct type in `main.cpp`.

```
class Sensor{
    protected:
        int address;
    public:
        Sensor(int I2C_address){
            address = I2C_address << 1;
            }
        virtual void setup(){}  //virtual method to implement any setup of sensor, default to do nothing
        
        void write(char* message,int len){   //common method to all sensors to write a number of bytes
            i2c.write(address,message,len);
            }
            
        void read(char reg_address,char* data,int len){ //common method to all sensors to read a number of bytes, starting at a given register address
            i2c.write(address,&reg_address,1);          //assuming automatic incrementing of register address in the sensor.
            for(int i=0; i<len; i++){
                i2c.read(address,&data[i],1);
                }
            }
            
        virtual data_item read_data(){  //virtual method to read the data from a sensor, default to returning -1 in the value
            data_item dflt;
            dflt.value = -1;
            return dflt;
            }
            
        virtual data_item process(data_item data){ //virtual method to process data, default to returning input value.
            return data;
            }
    };
```
