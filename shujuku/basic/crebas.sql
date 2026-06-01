drop table if exists TV_drama;

drop table if exists act;

drop table if exists album;

drop table if exists album_song;

drop table if exists concert;

drop table if exists cover_song;

drop table if exists hold;

drop table if exists member;

drop table if exists movie;

drop table if exists original_song;

drop table if exists perform;

drop table if exists setlist;

drop table if exists song;

drop table if exists work;

/*TV_drama                                              */
create table TV_drama
(
   wor_work_id          int not null,
   work_id              int not null,
   episodes             int,
   channel              varchar(50),
   air_date             date,
   title                varchar(100),
   year                 int,
   primary key (wor_work_id, work_id)
);

/*act                                                   */
create table act
(
   work_id              int not null,
   member_id            int not null,
   primary key (work_id, member_id)
);

/*album                                                 */
create table album
(
   album_id             int not null,
   concert_id           int,
   title                varchar(100),
   release_date         date,
   primary key (album_id)
);

/*album_song                                            */
create table album_song
(
   album_id             int not null,
   song_id              int not null,
   primary key (album_id, song_id)
);

/* Table: concert                                               */
create table concert
(
   concert_id           int not null,
   title                varchar(100),
   start_date           date,
   end_date             date,
   venue                varchar(100),
   tour_name            varchar(100),
   primary key (concert_id)
);

/*cover_song                                            */
create table cover_song
(
   son_song_id          int not null,
   song_id              int not null,
   original_artist      varchar(50),
   title                varchar(100),
   duration             int,
   release_date         date,
   theme_type           varchar(100),
   work_id              int,
   primary key (son_song_id, song_id)
);

/*hold                                                  */
create table hold
(
   concert_id           int not null,
   member_id            int not null,
   primary key (concert_id, member_id)
);

/*Table: member                                                */
create table member
(
   member_id            int not null,
   name                 varchar(50),
   birthday             date,
   debut_year           int,
   primary key (member_id)
);

/*movie*/
create table movie
(
   wor_work_id          int not null,
   work_id              int not null,
   box_office           int,
   title                varchar(100),
   year                 int,
   primary key (wor_work_id, work_id)
);

/*original_song*/
create table original_song
(
   son_song_id          int not null,
   song_id              int not null,
   lyricist             varchar(50),
   composer             varchar(50),
   title                varchar(100),
   duration             int,
   release_date         date,
   theme_type           varchar(100),
   work_id              int,
   primary key (son_song_id, song_id)
);

/*perform*/
create table perform
(
   song_id              int not null,
   member_id            int not null,
   primary key (song_id, member_id)
);

/*setlist*/
create table setlist
(
   concert_id           int not null,
   song_id              int not null,
   primary key (concert_id, song_id)
);

/*song*/
create table song
(
   song_id              int not null,
   title                varchar(100),
   duration             int,
   release_date         date,
   theme_type           varchar(100),
   work_id              int,
   primary key (song_id)
);



/*work*/
create table work
(
   work_id              int not null,
   title                varchar(100),
   year                 int,
   primary key (work_id)
);

alter table TV_drama add constraint FK_Inheritance_4 foreign key (wor_work_id)
      references work (work_id) on delete restrict on update restrict;

alter table act add constraint FK_act foreign key (work_id)
      references work (work_id) on delete restrict on update restrict;

alter table act add constraint FK_act2 foreign key (member_id)
      references member (member_id) on delete restrict on update restrict;

alter table album add constraint FK_record foreign key (concert_id)
      references concert (concert_id) on delete restrict on update restrict;

alter table album_song add constraint FK_album_song foreign key (album_id)
      references album (album_id) on delete restrict on update restrict;

alter table album_song add constraint FK_album_song2 foreign key (song_id)
      references song (song_id) on delete restrict on update restrict;

alter table cover_song add constraint FK_Inheritance_2 foreign key (son_song_id)
      references song (song_id) on delete restrict on update restrict;

alter table hold add constraint FK_hold foreign key (concert_id)
      references concert (concert_id) on delete restrict on update restrict;

alter table hold add constraint FK_hold2 foreign key (member_id)
      references member (member_id) on delete restrict on update restrict;

alter table movie add constraint FK_Inheritance_3 foreign key (wor_work_id)
      references work (work_id) on delete restrict on update restrict;

alter table original_song add constraint FK_Inheritance_1 foreign key (son_song_id)
      references song (song_id) on delete restrict on update restrict;

alter table perform add constraint FK_perform foreign key (song_id)
      references song (song_id) on delete restrict on update restrict;

alter table perform add constraint FK_perform2 foreign key (member_id)
      references member (member_id) on delete restrict on update restrict;

alter table setlist add constraint FK_setlist foreign key (concert_id)
      references concert (concert_id) on delete restrict on update restrict;

alter table setlist add constraint FK_setlist2 foreign key (song_id)
      references song (song_id) on delete restrict on update restrict;

