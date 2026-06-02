from flask import Flask, render_template, request, jsonify
import pymysql
 
app = Flask(__name__)
 
def get_connection():
    return pymysql.connect(
        host='localhost',
        user='root',
        password='123456',
        database='idol_db',
        charset='utf8mb4'
    )
 
@app.route('/')
def index():
    return render_template('index.html')