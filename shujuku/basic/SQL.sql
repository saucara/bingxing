CREATE TABLE MEMBER (
    member_id  INT         PRIMARY KEY,
    name       VARCHAR(50) NOT NULL,
    birthday   DATE,
    debut_year INT
);

CREATE TABLE CONCERT (
    concert_id INT          PRIMARY KEY,
    title      VARCHAR(100) NOT NULL,
    start_date DATE,
    end_date   DATE,
    venue      VARCHAR(100),
    tour_name  VARCHAR(100)
);

CREATE TABLE ALBUM (
    album_id     INT          PRIMARY KEY,
    title        VARCHAR(100) NOT NULL,
    release_date DATE,
    concert_id   INT,
    FOREIGN KEY (concert_id) REFERENCES CONCERT(concert_id)
);

CREATE TABLE SONG (
    song_id      INT          PRIMARY KEY,
    title        VARCHAR(100) NOT NULL,
    duration     INT,
    release_date DATE,
    work_id      INT,
    theme_type   VARCHAR(50),
    FOREIGN KEY (work_id) REFERENCES WORK(work_id)
);

CREATE TABLE ORIGINAL_SONG (
    song_id  INT         PRIMARY KEY,
    lyricist VARCHAR(50),
    composer VARCHAR(50),
    FOREIGN KEY (song_id) REFERENCES SONG(song_id)
);

CREATE TABLE COVER_SONG (
    song_id         INT          PRIMARY KEY,
    original_artist VARCHAR(50),
    FOREIGN KEY (song_id) REFERENCES SONG(song_id)
);

CREATE TABLE WORK (
    work_id INT          PRIMARY KEY,
    title   VARCHAR(100) NOT NULL,
    year    INT
);

CREATE TABLE TV_DRAMA (
    work_id  INT         PRIMARY KEY,
    episodes INT,
    channel  VARCHAR(50),
    air_date DATE,
    FOREIGN KEY (work_id) REFERENCES WORK(work_id)
);

CREATE TABLE MOVIE (
    work_id    INT PRIMARY KEY,
    box_office INT,
    FOREIGN KEY (work_id) REFERENCES WORK(work_id)
);

CREATE TABLE PERFORMS (
    member_id     INT         NOT NULL,
    song_id       INT         NOT NULL,
    solo_or_group VARCHAR(20),
    PRIMARY KEY (member_id, song_id),
    FOREIGN KEY (member_id) REFERENCES MEMBER(member_id),
    FOREIGN KEY (song_id)   REFERENCES SONG(song_id)
);

CREATE TABLE ALBUM_SONG (
    album_id INT NOT NULL,
    song_id  INT NOT NULL,
    track_no INT,
    PRIMARY KEY (album_id, song_id),
    FOREIGN KEY (album_id) REFERENCES ALBUM(album_id),
    FOREIGN KEY (song_id)  REFERENCES SONG(song_id)
);

CREATE TABLE ACTS_IN (
    member_id      INT          NOT NULL,
    work_id        INT          NOT NULL,
    character_name VARCHAR(50),
    PRIMARY KEY (member_id, work_id),
    FOREIGN KEY (member_id) REFERENCES MEMBER(member_id),
    FOREIGN KEY (work_id)   REFERENCES WORK(work_id)
);

CREATE TABLE HOLDS (
    member_id  INT NOT NULL,
    concert_id INT NOT NULL,
    PRIMARY KEY (member_id, concert_id),
    FOREIGN KEY (member_id)  REFERENCES MEMBER(member_id),
    FOREIGN KEY (concert_id) REFERENCES CONCERT(concert_id)
);

CREATE TABLE SETLIST (
    concert_id INT NOT NULL,
    song_id    INT NOT NULL,
    order_no   INT,
    PRIMARY KEY (concert_id, song_id),
    FOREIGN KEY (concert_id) REFERENCES CONCERT(concert_id),
    FOREIGN KEY (song_id)    REFERENCES SONG(song_id)
);









