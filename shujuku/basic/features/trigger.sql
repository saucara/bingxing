USE idol_db;

DELIMITER //
CREATE TRIGGER check_setlist_song_date
BEFORE INSERT ON setlist
FOR EACH ROW
BEGIN
    DECLARE concert_start DATE;
    DECLARE song_release DATE;
    DECLARE concert_name VARCHAR(100);
    DECLARE song_name VARCHAR(100);

    SELECT start_date INTO concert_start
    FROM concert WHERE concert_id = NEW.concert_id;

    SELECT release_date INTO song_release
    FROM song WHERE song_id = NEW.song_id;

    IF song_release > concert_start THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'Error！歌曲发行日期晚于演唱会开始日期，插入失败！';
    END IF;
END //
DELIMITER ;