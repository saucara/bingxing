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

@app.route('/query_concerts', methods=['GET'])
def query_concerts():
    try:
        conn = get_connection()
        cursor = conn.cursor()
        cursor.execute("""
            SELECT concert_title, venue, SUM(song_count) AS total_songs
            FROM concert_summary
            GROUP BY concert_id, concert_title, venue
            ORDER BY total_songs DESC
        """)
        rows = cursor.fetchall()
        data = [{'concert_title': r[0], 'venue': r[1], 'total_songs': r[2]} for r in rows]
        return jsonify({'status': 'success', 'data': data})
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)})
    finally:
        conn.close()


@app.route('/update_concert', methods=['POST'])
def update_concert():
    concert_id = request.form.get('concert_id')
    venue = request.form.get('venue')
    end_date = request.form.get('end_date')
    try:
        conn = get_connection()
        cursor = conn.cursor()
        cursor.callproc('update_concert_info', [concert_id, venue, end_date])
        results = []
        for result in cursor.stored_results():
            results = result.fetchall()
        conn.commit()
        msg = results[0][0] if results else 'Updated successfully!'
        return jsonify({'status': 'success', 'message': msg})
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)})
    finally:
        conn.close()
