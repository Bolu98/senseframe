import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import json
import os
from API_network_callbacks import *
import ssl



def on_connect(client, userdata, flags, rc):
    if rc==0:
        print("Connected.")
    else:
        print("Bad connection Returned code=",rc)

def initConnect(client, tls = False):
    # Send messages over HTTPS
    if tls:
        brokerAddr = "eu.thethings.network"
        portNo = 8080
        client.tls_set(TLS_CERTIFICATE, tls_version=ssl.PROTOCOL_TLSv1_2)
        client.tls_insecure_set(True)
        client.connect(brokerAddr, port=portNo)

    # Send messages over MQTT
    else:
        brokerAddr = "eu.thethings.network"
        portNo = 1883
        client.connect(brokerAddr, port=portNo)

    return client


# API initialize client for receiving
# *** PROCESS-BLOCKING FUNCTION ***
def initReceiver(user, password, topicList=["+/devices/+/up", "+/devices/+/events/activations"], clientID = "handle", qos=2, tls = False):
    # remove write lock if it was left by past receiver
    # if os.path.isfile(LOG_LOCK):
    #     os.remove(LOG_LOCK)
    if os.path.isfile(STATUS_LOCK):
        os.remove(STATUS_LOCK)
        
    client = mqtt.Client(clientID)

    # Attach functions to topic-wise callbacks
    client.on_message = on_message
    client.on_connect=on_connect

    # connect to broker
    client.username_pw_set(user, password)
    client = initConnect(client)

    # subscribe to topics
    for topic in topicList:
        client.subscribe(topic, qos)

    client.loop_forever()




