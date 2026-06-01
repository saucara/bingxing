USE idol_db;

DELIMITER //
CREATE PROCEDURE update_concert_info(
    IN p_concert_id INT,
    IN p_new_venue VARCHAR(100),
    IN p_new_end_date DATE
)
BEGIN
    DECLARE concert_count INT;

    SELECT COUNT(*) INTO concert_count
    FROM concert
    WHERE concert_id = p_concert_id;

    IF concert_count = 0 THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'Error: Concert does not exist!';
    END IF;

    UPDATE concert
    SET venue = p_new_venue,
        end_date = p_new_end_date
    WHERE concert_id = p_concert_id;

    DELETE FROM setlist
    WHERE concert_id = p_concert_id;

    SELECT CONCAT('Concert ', p_concert_id, ' updated successfully!') AS result;
END //
DELIMITER ;