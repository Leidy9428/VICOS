#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  app_final_V1_1.py
#  

import paho.mqtt.client as mqtt
import json
from flask import Flask, render_template, request
import sqlite3
from sqlite3 import Error
from datetime import datetime
import time

app = Flask(__name__)

broker_address = "192.168.43.95"
broker_port = 1883
topic = "/prueba/measure"

def sql_connection():
	try:
		con = sqlite3.connect('db_final.db')
		return con
	except Error:
		print(Error)
		
def sql_insert(con, entities):
	print("insert")
	#print(("""INSERT INTO measures(id_device, temp, hum, currentdate, currentime, timestamp) VALUES(?, ?, ?, date('now'), time('now'), DateTime('now'))""", entities))
	cursorObj = con.cursor()
	#measure_json['id_device'], measure_json['temp'], measure_json['hum']
	cursorObj.execute("""INSERT INTO measures(id_device, temp, hum, timestamp, currentdate, currentime) VALUES(?, ?, ?, datetime('now'), date('now'), time('now','localtime'))""", entities)
	print("end insert")
	con.commit()
	conn.close()

def on_connect(client, userdata, flags, rc):
	"""
	print("Connected with result code " + str(rc))
	print("UserData= " + str(userdata))
	print("flags= " + str(flags))
	print("")
	"""
	client.subscribe(topic)

def on_message(client, userdata, message):
	
	print("Mensaje recibido=", str(message.payload.decode("utf-8")))
	print("Topic=", message.topic)
	print("Nivel de calidad [0|1|2]=", message.qos)
	print("Flag de retención=", message.retain)
	print("---------------------------------------------")
	print("")
	
	timestamp = time.time()
	print (int(timestamp))
	if message.topic == "/prueba/measure":
		print("hola")
		print("llego mensaje")
		#print(message.payload.json())
		#print(dhtreadings_json['temperature'])
		#print(dhtreadings_json['humidity'])
		#print(str(message.payload.decode("utf-8")))
		"""
		now = datetime.today()
		timestamp2 = datetime.timestamp(now)
		print(timestamp2)
		"""
		print(str(message.payload))
		print(type(str(message.payload)))
		
		msg = message.payload
		msg_str = msg.decode("utf-8")
		measure_json = json.loads(msg_str)
		
		print(measure_json)
		print(type(measure_json))
		
		entities = (measure_json['id_device'], measure_json['temp'], measure_json['hum'])
		print("entities")
		print(entities)
		con = sql_connection() # crear conexion
		sql_insert(con, entities) #insertar en tabla
		print(str(measure_json))

#client = mqtt.Client('Cliente1', userdata="UsuarioDePrueba") 
client = mqtt.Client('Cliente1',userdata="UsuarioDePrueba") 
client.on_connect = on_connect 
client.on_message = on_message 
client.connect(broker_address, broker_port, 60) 
client.loop_start()

def dict_factory(cursor, row):
	d = {}
	for idx, col in enumerate(cursor.description):
		d[col[0]] = row[idx]
	return d

@app.route("/data.json")
def data():
	con = sql_connection() # crear conexion
	con.row_factory = dict_factory
	cursor = con.cursor()
	cursor.execute("SELECT currentdate, currentime, timestamp, hum, temp from measures")
	#cursor.execute("SELECT currentdate, time(currentime,'localtime'), datetime(timestamp, 'unixepoch', 'localtime'), hum, temp from measures ")
	results = cursor.fetchall()
	print(results)
	return json.dumps(results)

@app.route("/graph")
def graph():
	return render_template('graph.html')

@app.route("/")
def main():
	con = sql_connection() # crear conexion
	con.row_factory = dict_factory
	c = con.cursor()
	c.execute("SELECT * FROM measures ORDER BY id DESC LIMIT 20")
	readings = c.fetchall()
	print(readings)
	return render_template('main.html', readings=readings)

if __name__ == '__main__':
	app.run(debug=True, threaded=True, host='0.0.0.0', port=8181)
