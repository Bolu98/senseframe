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
    return render_template("display.html", **status) 

 
if __name__ == "__main__":

  app.run(debug=True)