SELECT name, birthday
FROM MEMBER
WHERE birthday > '1980-01-01';


SELECT S.title AS song_title, A.title AS album_title
FROM SONG S
JOIN ALBUM_SONG SA ON S.song_id = SA.song_id
JOIN ALBUM A ON SA.album_id = A.album_id;


SELECT name
FROM MEMBER
WHERE member_id IN (
    SELECT member_id
    FROM ACTS_IN
    WHERE work_id IN (
        SELECT work_id
        FROM MOVIE
    )
);

SELECT title
FROM WORK W
WHERE EXISTS (
    SELECT 1
    FROM SONG S
    WHERE S.work_id = W.work_id
);


SELECT M.name, COUNT(A.work_id) AS work_count
FROM MEMBER M
LEFT JOIN ACTS_IN A ON M.member_id = A.member_id
GROUP BY M.member_id, M.name;