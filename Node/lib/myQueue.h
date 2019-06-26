#ifndef MYQUEUE_H
#define MYQUEUE_H

#include "mbed.h"
#include "rtos.h"
#include "time.h"
#include <algorithm>    // std::copy


#define queue_length 25            //queue length. should be > tx_period/measure_period


#ifndef QUEUE_MUT
#define QUEUE_MUT
Mutex queue_mut;    //mutex to for data queue to make sure it is thread safe
#endif


class myQueue{      //defining our own queue class
        data_item data_store[queue_length]; //queue length is specified at top of file
        int front ;
        int back ;
        int n_items ;
    public:
        myQueue(){front = 0;back=-1;n_items=0;}
        myQueue(myQueue &_template);
        void add_element(data_item new_data);
        data_item remove_element();
        data_item read_element();
        bool is_full(){return n_items == queue_length;} //simple function for knowing if
        bool is_empty(){return n_items == 0;}           //the queue is full or empty
        int count_items();
    };
    
/* Copy constructor */
myQueue::myQueue(myQueue &_template){
    front = _template.front;
    back = _template.back;
    n_items = _template.n_items;
    for(int i = 0; i<queue_length; i++){data_store[i] = _template.data_store[i];}
}

// Get number of items in queue
int myQueue::count_items(){return n_items;}
   
void myQueue::add_element(data_item new_data){  //add data to front of queue
    if(!is_full()){
        data_store[front] = new_data;//add to front of queue
        front ++;               //advance front pointer
        if(front == queue_length){front = 0;} //if overflow then loop around
        n_items ++;     //update item count
        }
    }

data_item myQueue::remove_element(){        //remove and return last element in queue
    if(!is_empty()){
        back ++;        //move back pointer to last element in the list
        if(back == queue_length){back = 0;} //if overflow then loop around
        n_items --; //update item count
        return data_store[back];    //return data
        }
    data_item temp;
    temp.value = 0;
    temp.timestamp = time(NULL);
    return temp;
    }
    
    
// Return the last element in the queue (without removing it)
data_item myQueue::read_element(){
    if(!is_empty()){
        back ++;        //move back pointer to last element in the list
        if(back == queue_length){back = 0;} //if overflow then loop around
        //n_items --; //update item count
        data_item data_out = data_store[back];    //return data
        back --;
        if(back == -1){back = queue_length-1;}
        return data_out;
        }
    data_item temp;
    temp.value = 0;
    temp.timestamp = time(NULL);
    return temp;
    }




// DECLARE DATA BUFFER
static myQueue tx_data;


#endif