import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import json
import time
import signal
import subprocess
import os
import re

STATUS_FILE = "./status.json"
STATUS_LOCK = "./lock_status.txt"
A_LOG_FILE = "./Alog.txt"
B_LOG_FILE = "./Blog.txt"
C_LOG_FILE = "./Clog.txt"
TEMPLATE_FILE = "./template.json"

nodeInfo = {                   ############           Enter Node information provided by TTN here          #############
    "A" : {
        "dev_id" : "node1",
        "dev_addr" : "26012FDF"
    },

    "B" : {
        "dev_id" : "node2",
        "dev_addr" : "26012F6D"
    },

    "C" :{        
        "dev_id" : "node3",
        "dev_addr" : "26012F8C"
    }



}






def update_log(msg, dev_id):    
    # append data to log
    update = json.dumps(msg)

    if (dev_id == 'B'):
        JSONfile = open(B_LOG_FILE, "a+")
    elif (dev_id == 'C'): 
        JSONfile = open(C_LOG_FILE, "a+")
    else : 
        JSONfile = open(A_LOG_FILE, "a+")

    JSONfile.write(update)
    JSONfile.write("\n")
    JSONfile.close()
    


def read_JSON(filename):
    # check if stats file is there 
    if os.path.isfile(filename):
        #wait for resource to be freed
        while os.path.isfile(STATUS_LOCK):
            pass
        # read file
        JSONfile = open(filename, "r+")
        data = json.load(JSONfile)
        JSONfile.close()
        return data
    # otherwise copy data from template
    else:
        with open(TEMPLATE_FILE, 'r') as myfile:
            templateData=myfile.read().replace('\n', '')
        return templateData



def write_JSON(filename, data):
    # add lock
    lock = open(STATUS_LOCK, "w")
    lock.close() 
    while os.path.isfile(STATUS_LOCK) is False:
        pass  
    # write file
    update = json.dumps(data)
    JSONfile = open(filename, "w")
    JSONfile.write(update)
    JSONfile.close()
    # remove lock
    os.remove(STATUS_LOCK)
    while os.path.isfile(STATUS_LOCK):
        pass



# DEFINE CALLBACK FUNCTIONS FOR RECEIVER

def on_message(client, userdata, message):
    # get current STATUS
    status = read_JSON(STATUS_FILE)
    topic = message.topic

    # decode message payload
    msg = json.loads(str(message.payload.decode("utf-8")))


    # Parse uplink messages, pass join requests
    if "up" in topic:
        payload = msg['payload_fields']['data']
        msg["type"] = "uplink"

        # Find the indices of all occurences of $ in the payload
        indices = list([m.start() for m in re.finditer('\$', payload)])

        if(len(indices) > 0):
            lastUpdate = payload[indices[-2] + 1 :-1]
            
        separator = lastUpdate.find('*')

        if(separator != -1):
            nodeData = lastUpdate[0 : separator]
        else:
            print("* not found")

        # update logs
        if (msg["dev_id"] == nodeInfo["B"]["dev_id"]):      # Node name - node 2
            dev_id = 'B'
            update_log(msg, dev_id)

        elif (msg["dev_id"] == nodeInfo["C"]["dev_id"]):    # Node name - node 3
            dev_id = 'C'
            update_log(msg, dev_id)

        else:
            dev_id = 'A'
            update_log(msg, dev_id)

    elif "activations" in topic:
        msg["type"] = "joinrequest"
            
        # update logs
        if (msg["dev_addr"] == nodeInfo["B"]["dev_addr"]):     # Node address - node 2
            dev_id = 'B'
            update_log(msg, dev_id)

        elif (msg["dev_addr"] == nodeInfo["C"]["dev_addr"]):    # Node address  - node 3
            dev_id = 'C'
            update_log(msg, dev_id)

        else:
            dev_id = 'A'
            update_log(msg, dev_id)


    currTime = time.ctime()

    status[dev_id]["lastUpdate"] = currTime[4:-5]
    status[dev_id]['data'] = nodeData
    status[dev_id]['time'] = time.time()

    
    
    # write JSON
    write_JSON(STATUS_FILE, status)
    
