USE idol_db;

DELIMITER //
CREATE PROCEDURE delete_concert(
    IN p_concert_id INT
)
BEGIN
    DECLARE concert_count INT;
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'Error: Transaction failed, rolled back!';
    END;

    START TRANSACTION;

    
    SELECT COUNT(*) INTO concert_count--检查演唱会存不存在
    FROM concert WHERE concert_id = p_concert_id;

    IF concert_count = 0 THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'Error: Concert does not exist, rolled back!';
    END IF;
    DELETE FROM setlist WHERE concert_id = p_concert_id;--删setlist里的关联记录
    DELETE FROM hold WHERE concert_id = p_concert_id;--删hold里的关联记录
    DELETE FROM concert WHERE concert_id = p_concert_id;--删concert

    COMMIT;
    SELECT CONCAT('Concert ', p_concert_id, ' deleted successfully!') AS result;
END //
DELIMITER ;