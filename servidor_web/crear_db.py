#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  database_script.py

import sqlite3
 
from sqlite3 import Error
 
def sql_connection():
    try:
        con = sqlite3.connect('db_final.db')
        return con
    except Error:
        print(Error)
 
def sql_table(con):
    cursorObj = con.cursor()
    #cursorObj.execute("CREATE TABLE employees(id integer primary key AUTOINCREMENT, name text, salary real, department text, position text, hireDate text)")
    #cursorObj.execute("CREATE TABLE measures(id INTEGER PRIMARY KEY AUTOINCREMENT, id_device TEXT, temp NUMERIC, hum NUMERIC, currentdate DATE, currentime TIME,timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL)")
    #cursorObj.execute("CREATE TABLE measures(id INTEGER PRIMARY KEY AUTOINCREMENT, id_device TEXT, temp NUMERIC, hum NUMERIC, currentdate DATE, currentime TIME,timestamp DATETIME)")
    cursorObj.execute("CREATE TABLE measures(id INTEGER PRIMARY KEY AUTOINCREMENT, id_device TEXT, temp NUMERIC, hum NUMERIC, currentdate DATE, currentime TIME,timestamp DATETIME )")
    con.commit()
    con.close()

def sql_insert(con, entities):
    cursorObj = con.cursor()
    cursorObj.execute('INSERT INTO employees(name, salary, department, position, hireDate) VALUES(?, ?, ?, ?, ?)', entities)
    con.commit()

def sql_update(con):
    cursorObj = con.cursor()
    cursorObj.execute('UPDATE employees SET name = "Rogers" where id = 2')
    con.commit()
 
def sql_fetch(con):
    cursorObj = con.cursor()
    cursorObj.execute('SELECT * FROM employees')
    rows = cursorObj.fetchall()
    for row in rows:
        print(row)
 

con = sql_connection() # crear conexion

sql_table(con) # crear tabla

#entities = ('Andrew', 800, 'IT', 'Tech', '2018-02-06')
#sql_insert(con, entities) #insertar en tabla

#sql_update(con)

#sql_fetch(con)
