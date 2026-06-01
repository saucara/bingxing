USE idol_db;

INSERT INTO member VALUES
(1, '大野智', '1980-11-26', 1999),
(2, '櫻井翔', '1982-01-25', 1999),
(3, '相葉雅紀', '1982-12-24', 1999),
(4, '二宮和也', '1983-06-17', 1999),
(5, '松本潤', '1983-08-30', 1999);

INSERT INTO concert VALUES
(1, 'ARASHI Anniversary Tour 5x20', '2018-11-03', '2019-01-13', 'Tokyo Dome', '5x20'),
(2, 'ARASHI Live Tour 2016-2017', '2016-11-30', '2017-01-15', 'Kyocera Dome', 'Are You Happy?'),
(3, 'ARASHI BLAST in Hawaii', '2014-11-03', '2014-11-04', 'Hawaii', 'BLAST');

INSERT INTO work VALUES
(1, 'Kazoku no Katachi', 2016),
(2, '99.9 Keiji Senmon Bengoshi', 2016),
(3, 'Yatterman Night', 2010);

INSERT INTO TV_drama VALUES
(1, 10, 'Fuji TV', '2016-10-14', 'Kazoku no Katachi', 2016),
(2, 10, 'TBS', '2016-04-16', '99.9 Keiji Senmon Bengoshi', 2016);

INSERT INTO movie VALUES
(3, 850000000, 'Yatterman Night', 2010);

INSERT INTO song VALUES
(1, 'Love so sweet', 245, '2007-02-14', 'theme', 1),
(2, 'Happiness', 258, '2006-10-11', 'theme', NULL),
(3, 'A-RA-SHI', 231, '1999-11-03', 'debut', NULL),
(4, 'Monster', 271, '2010-10-27', 'theme', 3),
(5, 'Sakura', 289, '2004-03-10', 'theme', NULL);

INSERT INTO original_song VALUES
(1, 'Matsumoto Takashi', 'Utada Hikaru', 'Love so sweet', 245, '2007-02-14', 'theme', 1),
(2, 'J-pop team', 'D.A.I', 'Happiness', 258, '2006-10-11', 'theme', NULL),
(3, 'EXILE HIRO', 'EXILE HIRO', 'A-RA-SHI', 231, '1999-11-03', 'debut', NULL);

INSERT INTO album VALUES
(1, 1, '5x20 All the BEST!! 1999-2019', '2019-09-18'),
(2, NULL, 'Time', '2007-03-21'),
(3, NULL, 'Arashic', '2006-07-26');

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