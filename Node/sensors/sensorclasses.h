#include "mbed.h"
Serial pc(USBTX, USBRX); // serial interface to debug use of sensor library functions

I2C i2c(I2C_SDA , I2C_SCL );    // I2C bus interface to use for sensors
time_t timer;                        //to get timestamps for measurements



//including exisiting librarys for sensors
#include "MPL115A2.h"   

// Sensor Base class with virtual functions for the parts of I2C communicaton
// which vary between sensors, and for converting raw measurements into useful
// values to transmit.
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

//implementation of SHT31-D Temperature and humidity, set up for temperature measurment in Celcius
class temp_humid_sensor:public Sensor{
    public:
        temp_humid_sensor(int address = 0x45) : Sensor(address){}   //set I2C address
        void setup(){
            }
        data_item read_data(){
            char cmd[2] ;
            cmd[0] = 0x2C;
            cmd[1] = 0x06;
            write(cmd, 2);  //write command to take measurment
            
            wait(0.2);
            
            char raw_data[6];
            i2c.read(address,raw_data,6);   //for this sensor it is possible to immediately read all data bytes from I2C bus
            time(&timer);                   //take time of the measurement
            data_item new_output;
            new_output.value = float((raw_data[0]<<8)|raw_data[1]); //convert sensor data bytes to float
            new_output.timestamp = timer;
            return new_output;
            }
        data_item process(data_item data){
            data.value = (175.0/65535.0)*data.value - 45.0; //compensate to find actual temperature
            return data;
            }
    };
    
    
    //implementation for VCNL4010 ambient light and proximity sensor, set up to measure ambient light in lux
class light_proximity_sensor: public Sensor{
    public:
        light_proximity_sensor (int address = 0x13) : Sensor(address){} //set I2C address
        void setup(){
            }
        data_item read_data(){
            char cmd[2] ;
            cmd[0] = 0x80;
            cmd[1] = 0xF8;
            write(cmd, 2);      //write command to begin measurement
            
            wait(0.2);
            
            char raw_data[2];
            char loc = 0x85;
            read(loc,raw_data,2);   //read the 2 data bytes from correct address
            time(&timer);           //get time of reading
            data_item new_output;
            new_output.value = float((raw_data[0]<<8)|raw_data[1]); //convert data bytes to float
            new_output.timestamp = timer;
            return new_output;
            }
        data_item process(data_item data){ 
            return data;            //no additional processing requried
            }
    };

//implementation of MPL115A2 barometeric pressure sensor
class baro_sensor: public Sensor{
        MPL115A2 p_sensor;  //use object defined in library file
    public:
        baro_sensor (int address = 0x60): Sensor(address), p_sensor(&i2c,&pc){
            
            }
        void setup(){
            p_sensor.begin();   //call library setup routine
            
            }
            
        data_item read_data(){
            p_sensor.ReadSensor();  //call library read sensor metod
            time(&timer);           //get time of measuremtn
            float pressure = p_sensor.GetPressure();//get the pressure reading from data bytes read
            data_item output;
            output.value = pressure;
            output.timestamp =timer;
            return output;
        }
        data_item process(data_item data){
            return data;        //no further processing requried
            }
    };

