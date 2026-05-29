-- 清空旧数据
DELETE FROM act; DELETE FROM perform; DELETE FROM album_song; DELETE FROM hold;
DELETE FROM original_song; DELETE FROM cover_song; DELETE FROM album;
DELETE FROM setlist; DELETE FROM song; DELETE FROM work;
DELETE FROM TV_drama; DELETE FROM movie; DELETE FROM concert; DELETE FROM member;

-- 成员
INSERT INTO member VALUES
(1, '大野智', '1980-11-26', 1999),
(2, '櫻井翔', '1982-01-25', 1999),
(3, '相葉雅紀', '1982-12-24', 1999),
(4, '二宮和也', '1983-06-17', 1999),
(5, '松本潤', '1983-08-30', 1999);

-- 演唱会
INSERT INTO concert VALUES
(1, 'ARASHI Anniversary Tour 5x20', '2018-11-03', '2019-01-13', '东京巨蛋', '5x20'),
(2, 'ARASHI Live Tour 2016-2017', '2016-11-30', '2017-01-15', '京瓷巨蛋', 'Are You Happy?'),
(3, 'ARASHI BLAST in Hawaii', '2014-11-03', '2014-11-04', '夏威夷', 'BLAST');

-- 影视作品
INSERT INTO work VALUES
(1, '家庭的形式', 2016),
(2, '99.9刑事专业律师', 2016),
(3, '嘉丽', 2010);

INSERT INTO TV_drama VALUES
(1, 10, '富士电视台', '2016-10-14', '家庭的形式', 2016),
(2, 10, 'TBS', '2016-04-16', '99.9刑事专业律师', 2016);

INSERT INTO movie VALUES
(3, 850000000, '嘉丽', 2010);

-- 歌曲
INSERT INTO song VALUES
(1, 'Love so sweet', 245, '2007-02-14', '主题曲', 1),
(2, 'Happiness', 258, '2006-10-11', '主题曲', NULL),
(3, 'A·RA·SHI', 231, '1999-11-03', '出道曲', NULL),
(4, 'Monster', 271, '2010-10-27', '主题曲', 3),
(5, 'Sakura', 289, '2004-03-10', '主题曲', NULL);

INSERT INTO original_song VALUES
(1, '松本隆', '宇多田ヒカル', 'Love so sweet', 245, '2007-02-14', '主题曲', 1),
(2, 'J-pop团队', 'D.A.I', 'Happiness', 258, '2006-10-11', '主题曲', NULL),
(3, 'EXILE HIRO', 'EXILE HIRO', 'A·RA·SHI', 231, '1999-11-03', '出道曲', NULL);

-- 专辑
INSERT INTO album VALUES
(1, 1, '5x20 All the BEST!! 1999-2019', '2019-09-18'),
(2, NULL, 'Time', '2007-03-21'),
(3, NULL, 'Arashic', '2006-07-26');

-- 关联数据
INSERT INTO album_song VALUES
(1,1),(1,2),(1,3),(1,4),(1,5),
(2,1),(2,2),
(3,2),(3,3);

INSERT INTO hold VALUES
(1,1),(1,2),(1,3),(1,4),(1,5),
(2,1),(2,2),(2,3),(2,4),(2,5),
(3,1),(3,3),(3,5);

INSERT INTO perform VALUES
(1,1),(1,2),(1,3),(1,4),(1,5),
(2,2),(2,3),(3,1),(4,4),(5,5);

INSERT INTO act VALUES
(1,3),(2,2),(3,5);

INSERT INTO setlist VALUES
(1,1),(1,2),(1,3),(1,4),(1,5),
(2,2),(2,3),(2,4),
(3,1),(3,3);