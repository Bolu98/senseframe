# IoT Network Framework
## Imperial College London - 3rd Year Group Project - Group 11
**Alessandro Serena** - M.Eng. Electronic and Information Engineering - as6316@ic.ac.uk  
**Mateo Sarjanovic**- M.Eng. Electronic and Information Engineering - ms6616@ic.ac.uk  
**Mobolurin Edun** - M.Eng. Electrical and Electronic Engineering - me2016@ic.ac.uk  
**Oliver Spillett** - M.Eng. Electrical and Electronic Engineering - obs16@ic.ac.uk  


## Overview
This project aims to create a framework for building a network of distributed IoT nodes.  
  
The network will be composed by 2 types of devices:
+ **Sensor Nodes**: these devices will execute the task of collecting data through sensors, process it, and send it to the central station. They are self-powered (for more info on the power module click here[---insert link---] and communicate using LoRa-WAN technology.  
+ **Central Station**: this device will deal with the configuration of the Nodes, will receive data from them and it will forward it to the application layer of your system. The Central Station is designed to be main-powered and communicates with the Nodes thanks to the LoRa-WAN protocol.  
  
_Modularity_ was a key element when we designed this system. 
+ The Nodes support all main sensor interfaces (I2C, ...?), so that you can choose the sensor(s) that you need and simply plug them in the Node board.  
+ Thanks to LoRa-WAN, the Central Station supports any number of nodes. Just make sure that the Nodes are in the Central Station range and enjoy seamless, secure, and reliable connectivity. (Mesh networking is coming soon!)



## How to use the system

### 1. Build the Sensor Nodes
### 2. Build the Central Station
### 3. Build the Application
