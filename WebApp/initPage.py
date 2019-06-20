from flask import Flask, render_template, request, redirect, url_for, jsonify
from API_network import initReceiver
from threading import Thread
import json
import time


#read last status from status.json
def get_status():
    with open("status.json", "r") as f:
        status_dict = json.load(f)
    return status_dict

#read logged events from log.txt
def get_log():
  with open("log.txt") as f:
    content = f.readlines()
  #to remove whitespace characters like `\n` at the end of each line
  content = [x.strip() for x in content] 
  return content

#get relevant information from log to display on Events Log
def get_logStatus(log):
  output = []
  t_output = [] 
  for m_event in log:

    event = json.loads(m_event)
    t_stamp = event["timeStamp"]
  
    #check what changed in the event by comparing event update-time and component update-time
    if time.ctime(event["status"]["compass"]["lastUpdate"]) == t_stamp:
      if event["status"]["compass"]["isOpen"] == 'yes':
        was_open = "opened"
      else :
        was_open = "closed"
      output.append("Door was "+was_open)
      t_output.append(t_stamp)
    elif time.ctime(event["status"]["accelerometer"]["lastIntrusion"]) == t_stamp:
      output.append("Door was shaken ")
      t_output.append(t_stamp)
  return output, t_output
    


app = Flask(__name__)


@app.route("/update", methods=['GET', 'POST'])
def get_post_javascript_data():
    
    nodeList = ['A','B','C']

    status = get_status()
    currTime = time.time()

    for node in nodeList:
      nodeTime = float(status[node]['time'])
      timeDiff = currTime - nodeTime

      
      if(timeDiff > 60):
        status[node]['connStatus'] = 'Disconnected'
      else:
        status[node]['connStatus'] = 'Connected'
    
      
      
    aStatus = status["A"]["connStatus"]
    aUpdate = status["A"]["lastUpdate"]
    aData = status["A"]["data"]

    bStatus  = status["B"]["connStatus"]
    bUpdate = status["B"]["lastUpdate"]
    bData = status["B"]["data"]

    cStatus = status["C"]["connStatus"]
    cUpdate = status["C"]["lastUpdate"]
    cData = status["C"]["data"]

    
    return  jsonify(aStatus=aStatus, aUpdate=aUpdate, aData=aData, bStatus=bStatus, bUpdate=bUpdate, bData=bData, cStatus=cStatus, cUpdate=cUpdate, cData=cData)

#home route which renders html 
@app.route("/")
def home():
    status = get_status()
    print("Home called")
    return render_template("display.html", **status) 

 
if __name__ == "__main__":

  app.run(debug=True)