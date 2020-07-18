 
drop table dbo.t_camera
drop table dbo.t_violation
CREATE TABLE t_camera (camera_id nchar(128) NOT NULL PRIMARY KEY		, name nchar(50) not null, location nchar(50) not null, StreamLocation nchar(512) not null,  constraint t_camera_name_location_uk unique (name,location) );
CREATE TABLE t_violation (violation_id nchar(128) NOT NULL PRIMARY KEY, time DATETIME not null, video_location nchar(256) not null, photo_location nchar(256) not null, rule_type INT not null, camera nchar(128) constraint fk_camera foreign key (camera) references t_camera (camera_id) on delete cascade on update cascade);

alter table t_camera
add constraint t_camera_name_location_uk unique (name,location);

select * from dbo.t_camera;
select * from dbo.t_violation;

select count(*) from dbo.t_camera;

select t_violation.violation_id, t_violation.time, t_violation.video_location, t_camera.name from t_camera, t_violation where t_camera.camera_id = t_violation.violation_id