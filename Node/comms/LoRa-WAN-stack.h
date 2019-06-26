#ifndef LORA-WAN-STACK_CPP
#define LORA-WAN-STACK_CPP

/**
 * Copyright (c) 2017, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <math.h>       /* floor */
#include <string>

#include "lorawan/LoRaWANInterface.h"
#include "lorawan/system/lorawan_data_structures.h"
#include "events/EventQueue.h"

// Application helpers
#include "trace_helper.h"
#include "lora_radio_helper.h"

using namespace events;

// Max payload size can be LORAMAC_PHY_MAXPAYLOAD.
// This example only communicates with much shorter messages (<30 bytes).
// If longer messages are used, these buffers must be changed accordingly.
uint8_t tx_buffer[LORAMAC_PHY_MAXPAYLOAD]; //was 30
uint8_t rx_buffer[LORAMAC_PHY_MAXPAYLOAD]; //was 30

/*
 * Sets up an application dependent transmission timer in ms. Used only when Duty Cycling is off for testing
 */
#define TX_TIMER                        TRANSMISSION_PERIOD*1000

/**
 * Maximum number of events for the event queue.
 * 10 is the safe number for the stack events, however, if application
 * also uses the queue for whatever purposes, this number should be increased.
 */
#define MAX_NUMBER_OF_EVENTS            10

/**
 * Maximum number of retries for CONFIRMED messages before giving up
 */
#define CONFIRMED_MSG_RETRY_COUNTER     3

#include "myQueue.h"




/**
* This event queue is the global event queue for both the
* application and stack. To conserve memory, the stack is designed to run
* in the same thread as the application and the application is responsible for
* providing an event queue to the stack that will be used for ISR deferment as
* well as application information event queuing.
*/
static EventQueue ev_queue(MAX_NUMBER_OF_EVENTS *EVENTS_EVENT_SIZE);

/**
 * Event handler.
 *
 * This will be passed to the LoRaWAN stack to queue events for the
 * application which in turn drive the application.
 */
static void lora_event_handler(lorawan_event_t event);

/**
 * Constructing Mbed LoRaWANInterface and passing it the radio object from lora_radio_helper.
 */
static LoRaWANInterface lorawan(radio);

/**
 * Application specific callbacks
 */
static lorawan_app_callbacks_t callbacks;

/**
 * Entry point for application
 */
int comms_main(void){
    // setup tracing
    setup_trace();

    // stores the status of a call to LoRaWAN protocol
    lorawan_status_t retcode;

    // Initialize LoRaWAN stack
    if (lorawan.initialize(&ev_queue) != LORAWAN_STATUS_OK) {
        printf("\r\n LoRa initialization failed! \r\n");
        return -1;
    }

    printf("\r\n Mbed LoRaWANStack initialized \r\n");

    // prepare application callbacks
    callbacks.events = mbed::callback(lora_event_handler);
    lorawan.add_app_callbacks(&callbacks);

    // Set number of retries in case of CONFIRMED messages
    if (lorawan.set_confirmed_msg_retries(CONFIRMED_MSG_RETRY_COUNTER)
            != LORAWAN_STATUS_OK) {
        printf("\r\n set_confirmed_msg_retries failed! \r\n\r\n");
        return -1;
    }

    printf("\r\n CONFIRMED message retries : %d \r\n",
           CONFIRMED_MSG_RETRY_COUNTER);

    // Enable adaptive data rate
    if (lorawan.enable_adaptive_datarate() != LORAWAN_STATUS_OK) {
        printf("\r\n enable_adaptive_datarate failed! \r\n");
        return -1;
    }

    printf("\r\n Adaptive data  rate (ADR) - Enabled \r\n");

    retcode = lorawan.connect();

    if (retcode == LORAWAN_STATUS_OK ||
            retcode == LORAWAN_STATUS_CONNECT_IN_PROGRESS) {
    } else {
        printf("\r\n Connection error, code = %d \r\n", retcode);
        return -1;
    }

    printf("\r\n Connection - In Progress ...\r\n");
    #if TEST_MODE
    printf("LOG:start connection\r\n");
    #endif
    // make your event queue dispatching events forever
    ev_queue.dispatch_forever();

    return 0;
}

/**
 * Prepares a packet to be sent (marshaling of multiple data items)
 */
 #include <sstream>
 #include <string>
 #include "lz/lz.h"
uint16_t prepare_packet(char* tx_buffer, myQueue data_queue, int &data_used, bool compression_on){
    if(data_queue.is_empty()) return NULL;
    std::string payload = "";
    // fit data items into buffer
    std::string temp_str;
    std::stringstream sensor_val;
    std::stringstream timestamp;
    data_item data;
    for(data_used = 0; !data_queue.is_empty(); data_used++){
        temp_str = ""; // clear temp string
        sensor_val.str(std::string()); // clear sstream
        timestamp.str(std::string()); // clear sstream
        data = data_queue.remove_element();
        sensor_val.precision(4);
        sensor_val << data.value;
        timestamp << data.timestamp;
        temp_str += sensor_val.str() + "*" + timestamp.str() + "$";
        if (sizeof(payload) + sizeof(temp_str) < 64){
            payload += temp_str;
        }
        else break;
    }
    //// COMPRESS PAYLOAD WITH Lempel–Ziv ALGORITHM
    if (compression_on){
        unsigned char compressed_str[128];
        compressed_str[0] = (unsigned char)'a';
        int compr_size = LZ_Compress((unsigned char*)payload.c_str(), compressed_str, (unsigned int)strlen(payload.c_str()));
        for (std::size_t i = 0; i != compr_size; ++i){
            compressed_str[i] += 0x01;
        }
        char payload_buff[compr_size+1];
        for (std::size_t i = 0; i != compr_size; ++i){
            payload_buff[i] = (char)compressed_str[i];
        }
        printf("=======> COMPRESSION RATE: %d%% (of original size)\n\r", (int)((float)compr_size*100/strlen(payload.c_str())));
        payload_buff[compr_size] = 0x00;
        return sprintf((char *) tx_buffer/*, sizeof(tx_buffer)*/, "%s", (char*)payload_buff);
    }
    // NO COMPRESSION
    else return sprintf((char *) tx_buffer/*, sizeof(tx_buffer)*/, "%s", payload.c_str());
}


/**
 * Sends a message to the Network Server
 */
void send_message()
{
    uint16_t packet_len;
    int16_t retcode;
    float sensor_value;

    if(!tx_data.is_empty() || MBED_CONF_LORA_DUTY_CYCLE_ON){
        // Create copy of queue
        queue_mut.lock();
        myQueue copy_queue(tx_data);
        queue_mut.unlock();
        // Create packet from elements of the copied queue
        int data_used;
        packet_len = prepare_packet((char *) tx_buffer, copy_queue, data_used, COMPRESSION); //***last parameter is COMPRESSION***
    
        #if TEST_MODE
        printf("LOG:sending packet - payload:%d - compression:%d\r\n", packet_len, COMPRESSION);
        #endif
        
        retcode = lorawan.send(MBED_CONF_LORA_APP_PORT, tx_buffer, packet_len,
                               MSG_CONFIRMED_FLAG);
    
        if (retcode < 0) {
            
            #if TEST_MODE
            printf("LOG:sending unsuccesful\r\n");
            #endif
            
            retcode == LORAWAN_STATUS_WOULD_BLOCK ? printf("send - WOULD BLOCK\r\n")
            : printf("\r\n send() - Error code %d \r\n", retcode);
    
            if (retcode == LORAWAN_STATUS_WOULD_BLOCK) {
                //retry in 3 seconds
                if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                    ev_queue.call_in(3000, send_message);
                }
            }
            return;
        }
        // if TX successful, remove sent data from original queue
        for(int i = 0; i < data_used; i++){
            tx_data.remove_element(); 
        }
        #if TEST_MODE
        printf("LOG:sending successful\r\n");
        #endif
        printf("\r\n %d bytes scheduled for transmission \r\n", retcode);
        
        memset(tx_buffer, 0, sizeof(tx_buffer));
    }
}

/**
 * Receive a message from the Network Server
 */
void receive_message(){
    uint8_t port;
    int flags;
    int16_t retcode = lorawan.receive(rx_buffer, sizeof(rx_buffer), port, flags);

    if (retcode < 0) {
        printf("\r\n receive() - Error code %d \r\n", retcode);
        return;
    }

    printf(" RX Data on port %u (%d bytes): ", port, retcode);
    for (uint8_t i = 0; i < retcode; i++) {
        printf("%02x ", rx_buffer[i]);
    }
    printf("\r\n");
    
    memset(rx_buffer, 0, sizeof(rx_buffer));
}


/**
 * --TODO--
 * Function to get exact time from the server after successful connection.
 *
 */
 void get_server_time(){
     return;
     }

/**
 * Event handler
 */
  void lora_event_handler(lorawan_event_t event)
{
    switch (event) {
        case CONNECTED:
            printf("\r\n Connection - Successful \r\n");
            #if TEST_MODE
            printf("LOG:connection successful\r\n");
            #endif
            // TODO: Get correct time from server before starting to send data
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message();
            } else {
                ev_queue.call_every(TX_TIMER, send_message);
            }

            break;
        case DISCONNECTED:
            ev_queue.break_dispatch();
            printf("\r\n Disconnected Successfully \r\n");
            break;
        case TX_DONE:
            #if TEST_MODE
            printf("LOG:message confirmed by server\r\n");
            #endif
            printf("\r\n Message Sent to Network Server \r\n");
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message();
            }
            break;
        case TX_TIMEOUT:
        case TX_ERROR:
        case TX_CRYPTO_ERROR:
        case TX_SCHEDULING_ERROR:
            printf("\r\n Transmission Error - EventCode = %d \r\n", event);
            // try again
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message();
            }
            break;
        case RX_DONE:
            printf("\r\n Received message from Network Server \r\n");
            receive_message();
            break;
        case RX_TIMEOUT:
        case RX_ERROR:
            printf("\r\n Error in reception - Code = %d \r\n", event);
            break;
        case JOIN_FAILURE:
            printf("\r\n OTAA Failed - Check Keys \r\n");
            break;
        case UPLINK_REQUIRED:
            printf("\r\n Uplink required by NS \r\n");
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message();
            }
            break;
        default:
            MBED_ASSERT("Unknown Event");
    }
}



/*
/ Transmission Thread Init
*/
void data_tx(){
    printf("Init transmit_thread...\n\r");
    comms_main();
    }


// EOF


#endif