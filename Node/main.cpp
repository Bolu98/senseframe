#include "mbed.h"
#include "rtos.h"
#include "time.h"
#include "data_item.h"
#include "myQueue.h"
#include "sensorclasses.h"


////////////////////////////////////////////////
/////////////////// GLOBALS ////////////////////
////////////////////////////////////////////////

InterruptIn mybutton(USER_BUTTON);          //interrupt for one off measurment
Thread measurement_thread(osPriorityNormal);//thread for getting measuremtns
Thread transmit_data(osPriorityAboveNormal);//thread for transmitting data
extern Mutex queue_mut;                     //mutex to for data queue to make sure it is thread safe

////////////////////////////////////////////////
/////////// DEFINE SENSOR USED HERE ////////////
////////////////////////////////////////////////

light_proximity_sensor sensor;
//baro_sensor sensor;
//temp_humid_sensor sensor;



////////////////////////////////////////////////
////////// INCLUDE SUBSYSTEMS HEADERS //////////
////////////////////////////////////////////////

#include "node_settings.h"
#include "LoRa-WAN-stack.h"
#include "sensor_thread.h"


////////////////////////////////////////////////
///////////////////// MAIN /////////////////////
////////////////////////////////////////////////

// ENTRY POINT FOR MAIN FUNCTION
int main() {
    
    set_time(0); //initialize time (absolute time is wrong on nodes, IMPROVEMENT: send exact time wirelessly to nodes)
    printf("\n\n\n\n================================\n================================\n\r");
    printf("Starting....\r\n");
    #if TEST_MODE
    printf("LOG:start Node%d\r\n", NODE_NUMBER);
    #endif
    sensor.setup();
    measurement_thread.start(measure_thread);   //initialise thread for measurement collection
    transmit_data.start(data_tx);               //initialise thread for data transmission
    mybutton.fall(&one_shot);                   //set up interrupt for manual data collection
    while(0){}
}