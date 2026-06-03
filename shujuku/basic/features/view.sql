USE idol_db;

CREATE VIEW concert_summary AS
SELECT 
    c.concert_id,
    c.title AS concert_title,
    c.venue,
    c.start_date,
    m.member_id,
    m.name AS member_name,
    COUNT(DISTINCT s.song_id) AS song_count
FROM concert c
JOIN hold h ON c.concert_id = h.concert_id
JOIN member m ON h.member_id = m.member_id
LEFT JOIN setlist s ON c.concert_id = s.concert_id
GROUP BY c.concert_id, c.title, c.venue, c.start_date, m.member_id, m.name;