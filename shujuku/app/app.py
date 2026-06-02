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


@app.route('/insert_album_song', methods=['POST'])
def insert_album_song():
    album_id = request.form.get('album_id')
    song_id = request.form.get('song_id')
    try:
        conn = get_connection()
        cursor = conn.cursor()
        cursor.execute("INSERT INTO album_song VALUES (%s, %s)", (album_id, song_id))
        conn.commit()
        return jsonify({'status': 'success', 'message': f'Successfully added song {song_id} to album {album_id}!'})
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)})
    finally:
        conn.close()