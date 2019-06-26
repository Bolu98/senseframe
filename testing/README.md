# Testing
## Node Side
To log information from the node for debugging or testing purposes, we included the Python script `node_log.py`. It connects to you computer serial port to which the node is connected and saves to a log file all the text lines that begin with `LOG:` sent by the node.

Every time the node is restarted a new log file is created with the following naming convention `[node number]_[timestamp of start of logging].txt`, so that no conflicts can happen between different logs.

To use the script go to the `senseframe/testing` directory on a terminal and type
```
python   node_log.py   {number of COM port to connect to}
```
For example if the port used is COM13, simply execute 
`python node_log.py 13`

## Application Side
**[TODO]**
