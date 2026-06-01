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

    --检查演唱会是否存在
    SELECT COUNT(*) INTO concert_count
    FROM concert WHERE concert_id = p_concert_id;

    IF concert_count = 0 THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'Error: Concert does not exist, rolled back!';
    END IF;

    --先删除setlist里的关联记录
    DELETE FROM setlist WHERE concert_id = p_concert_id;

    --再删除hold里的关联记录
    DELETE FROM hold WHERE concert_id = p_concert_id;

    --最后删除concert
    DELETE FROM concert WHERE concert_id = p_concert_id;

    COMMIT;
    SELECT CONCAT('Concert ', p_concert_id, ' deleted successfully!') AS result;
END //
DELIMITER ;