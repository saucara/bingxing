from flask import Flask, render_template, request, jsonify
import pymysql

app = Flask(__name__)

def get_connection():#连接
    return pymysql.connect(
        host='localhost',
        user='root',
        password='5127439',
        database='idol_db',
        charset='utf8mb4'
    )

@app.route('/')
def index():
    return render_template('index.html')


@app.route('/insert_album_song', methods=['POST'])#专辑添加歌曲
def insert_album_song():
    album_id = request.form.get('album_id')
    song_id = request.form.get('song_id')
    conn = None
    try:
        conn = get_connection()
        cursor = conn.cursor()
        cursor.execute("INSERT INTO album_song VALUES (%s, %s)", (album_id, song_id))
        conn.commit()
        return jsonify({'status': 'success', 'message': f'歌曲 {song_id} 已成功添加到专辑 {album_id}！'})
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)})
    finally:
        if conn:
            conn.close()

@app.route('/query_concerts', methods=['GET'])#查询演唱会
def query_concerts():
    conn = None
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
        if conn:
            conn.close()


@app.route('/update_concert', methods=['POST'])#更新演唱会信息
def update_concert():
    concert_id = request.form.get('concert_id')
    venue = request.form.get('venue')
    end_date = request.form.get('end_date')
    conn = None
    try:
        conn = get_connection()
        cursor = conn.cursor()
        cursor.execute("CALL update_concert_info(%s, %s, %s)", (concert_id, venue, end_date))
        row = cursor.fetchone()
        conn.commit()
        msg = row[0] if row else '更新成功！'
        return jsonify({'status': 'success', 'message': msg})
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)})
    finally:
        if conn:
            conn.close()


@app.route('/delete_concert', methods=['POST'])#删除演唱会
def delete_concert():
    concert_id = request.form.get('concert_id')
    conn = None
    try:
        conn = get_connection()
        cursor = conn.cursor()
        cursor.execute("CALL delete_concert(%s)", (concert_id,))
        row = cursor.fetchone()
        conn.commit()
        msg = row[0] if row else '删除成功！'
        return jsonify({'status': 'success', 'message': msg})
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)})
    finally:
        if conn:
            conn.close()


@app.route('/query_setlist', methods=['GET'])#查询演唱会歌单
def query_setlist():
    concert_id = request.args.get('concert_id')
    conn = None
    try:
        conn = get_connection()
        cursor = conn.cursor()
        cursor.execute("""
            SELECT c.title AS concert_title, s.title AS song_title,
                   s.duration, s.release_date, s.theme_type
            FROM setlist sl
            JOIN concert c ON sl.concert_id = c.concert_id
            JOIN song s ON sl.song_id = s.song_id
            WHERE sl.concert_id = %s
            ORDER BY s.title
        """, (concert_id,))
        rows = cursor.fetchall()
        if not rows:
            return jsonify({'status': 'error', 'message': '未找到该演唱会或歌单为空'})
        data = {
            'concert_title': rows[0][0],
            'songs': [
                {
                    'title': r[1],
                    'duration': f"{r[2] // 60}分{r[2] % 60}秒" if r[2] else '-',
                    'release_date': str(r[3]) if r[3] else '-',
                    'theme_type': r[4] if r[4] else '-'
                } for r in rows
            ]
        }
        return jsonify({'status': 'success', 'data': data})
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)})
    finally:
        if conn:
            conn.close()
 




if __name__ == '__main__':
    app.run(debug=True)