/*
/ Trigger the measurement thread at the specified period
*/
void measurement_timer(){ measurement_thread.signal_set(0x1); }
    
void one_shot(){            //ISR for button press, triggers one off measurement
    measurement_thread.signal_set(0x1);
    }

/*
/ Sensor Reading Thread Init
*/
void measure_thread(){
    printf("Init measure_thread...\n\r");
    LowPowerTicker measurementTicker;
    measurementTicker.attach(&measurement_timer, MEASUREMENT_PERIOD);
    while (1) {
        measurement_thread.signal_wait(0x1);    //wait for set period or until button press
        data_item output = sensor.read_data();      //get the result from the sensor
        output = sensor.process(output);
        queue_mut.lock();
        tx_data.add_element(output);  //decode data and add to queue
        queue_mut.unlock();
        printf("-->Sensor Reading: OK\n\r");
        }
    }