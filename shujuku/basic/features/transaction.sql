DROP PROCEDURE IF EXISTS delete_concert;

DELIMITER //
CREATE PROCEDURE delete_concert(
    IN p_concert_id INT
)
BEGIN
    DECLARE concert_count INT;
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        RESIGNAL;
    END;

    START TRANSACTION;

    SELECT COUNT(*) INTO concert_count
    FROM concert WHERE concert_id = p_concert_id;

    IF concert_count = 0 THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'Error!演唱会不存在，回滚了！';
    END IF;
    -- 删setlist里的关联记录，删hold里的关联记录，最后删concert表里的记录
    DELETE FROM setlist WHERE concert_id = p_concert_id;
    DELETE FROM hold WHERE concert_id = p_concert_id;
    DELETE FROM concert WHERE concert_id = p_concert_id;

    COMMIT;
    SELECT CONCAT('Concert ', p_concert_id, '删除成功！') AS result;
END //
DELIMITER ;