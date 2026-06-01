-- 按正确顺序建表（先建父表，再建子表）

USE idol_db;

CREATE TABLE member (
    member_id  INT         NOT NULL,
    name       VARCHAR(50),
    birthday   DATE,
    debut_year INT,
    PRIMARY KEY (member_id)
);

CREATE TABLE concert (
    concert_id INT          NOT NULL,
    title      VARCHAR(100),
    start_date DATE,
    end_date   DATE,
    venue      VARCHAR(100),
    tour_name  VARCHAR(100),
    PRIMARY KEY (concert_id)
);

CREATE TABLE work (
    work_id INT          NOT NULL,
    title   VARCHAR(100),
    year    INT,
    PRIMARY KEY (work_id)
);

CREATE TABLE TV_drama (
    work_id  INT          NOT NULL,
    episodes INT,
    channel  VARCHAR(50),
    air_date DATE,
    title    VARCHAR(100),
    year     INT,
    PRIMARY KEY (work_id),
    FOREIGN KEY (work_id) REFERENCES work(work_id)
);

CREATE TABLE movie (
    work_id    INT          NOT NULL,
    box_office INT,
    title      VARCHAR(100),
    year       INT,
    PRIMARY KEY (work_id),
    FOREIGN KEY (work_id) REFERENCES work(work_id)
);

CREATE TABLE song (
    song_id      INT          NOT NULL,
    title        VARCHAR(100),
    duration     INT,
    release_date DATE,
    theme_type   VARCHAR(50),
    work_id      INT,
    PRIMARY KEY (song_id),
    FOREIGN KEY (work_id) REFERENCES work(work_id)
);

CREATE TABLE original_song (
    song_id      INT          NOT NULL,
    lyricist     VARCHAR(50),
    composer     VARCHAR(50),
    title        VARCHAR(100),
    duration     INT,
    release_date DATE,
    theme_type   VARCHAR(100),
    work_id      INT,
    PRIMARY KEY (song_id),
    FOREIGN KEY (song_id) REFERENCES song(song_id)
);

CREATE TABLE cover_song (
    song_id         INT          NOT NULL,
    original_artist VARCHAR(50),
    title           VARCHAR(100),
    duration        INT,
    release_date    DATE,
    theme_type      VARCHAR(100),
    work_id         INT,
    PRIMARY KEY (song_id),
    FOREIGN KEY (song_id) REFERENCES song(song_id)
);

CREATE TABLE album (
    album_id     INT          NOT NULL,
    concert_id   INT,
    title        VARCHAR(100),
    release_date DATE,
    PRIMARY KEY (album_id),
    FOREIGN KEY (concert_id) REFERENCES concert(concert_id)
);

CREATE TABLE album_song (
    album_id INT NOT NULL,
    song_id  INT NOT NULL,
    PRIMARY KEY (album_id, song_id),
    FOREIGN KEY (album_id) REFERENCES album(album_id),
    FOREIGN KEY (song_id)  REFERENCES song(song_id)
);

CREATE TABLE act (
    work_id   INT NOT NULL,
    member_id INT NOT NULL,
    PRIMARY KEY (work_id, member_id),
    FOREIGN KEY (work_id)   REFERENCES work(work_id),
    FOREIGN KEY (member_id) REFERENCES member(member_id)
);

CREATE TABLE hold (
    concert_id INT NOT NULL,
    member_id  INT NOT NULL,
    PRIMARY KEY (concert_id, member_id),
    FOREIGN KEY (concert_id) REFERENCES concert(concert_id),
    FOREIGN KEY (member_id)  REFERENCES member(member_id)
);

CREATE TABLE perform (
    song_id   INT NOT NULL,
    member_id INT NOT NULL,
    PRIMARY KEY (song_id, member_id),
    FOREIGN KEY (song_id)   REFERENCES song(song_id),
    FOREIGN KEY (member_id) REFERENCES member(member_id)
);

CREATE TABLE setlist (
    concert_id INT NOT NULL,
    song_id    INT NOT NULL,
    PRIMARY KEY (concert_id, song_id),
    FOREIGN KEY (concert_id) REFERENCES concert(concert_id),
    FOREIGN KEY (song_id)    REFERENCES song(song_id)
);
