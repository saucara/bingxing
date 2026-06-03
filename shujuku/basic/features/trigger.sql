USE idol_db;

DELIMITER //
CREATE TRIGGER check_song_release_date
BEFORE INSERT ON album_song
FOR EACH ROW
BEGIN
    DECLARE album_release DATE;
    DECLARE song_release DATE;

    SELECT release_date INTO album_release
    FROM album WHERE album_id = NEW.album_id;

    SELECT release_date INTO song_release
    FROM song WHERE song_id = NEW.song_id;

    IF song_release > album_release THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'Error！单曲的发行日期不能晚于专辑的发行日期！';
    END IF;
END //
DELIMITER ;