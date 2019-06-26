#ifndef NODE_SETTINGS_H
#define NODE_SETTINGS_H

///////////////////////////////////////////
//////////// DEFINE NODE NUMBER ///////////
///////////////////////////////////////////

///////////////////////////////////////////
//Define device number
// !!! MAKE SURE THIS MATCH THE DEVICE EUI IN mbed_app.json !!!
#define NODE_NUMBER         3


///////////////////////////////////////////
// DEFINE PARAMETERS FOR NODE OPERATION  //
///////////////////////////////////////////


//Define period of transmission in seconds
#define TRANSMISSION_PERIOD         15

//Define period of data measurement in seconds
#define MEASUREMENT_PERIOD          3     

//queue length. should be > tx_period/measure_period
#define QUEUE_LENGTH                25

//Toggle payload LZ compression
#define COMPRESSION                 1           



///////////////////////////////////////////
//////////// TOGGLE TEST MODE /////////////
///////////////////////////////////////////

#define TEST_MODE                   1
//#if TEST_MODE
//printf("\r\n");
//#endif
#endif