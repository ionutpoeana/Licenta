
CREATE TABLE t_camera (camera_id INT NOT NULL PRIMARY KEY IDENTITY(1,1), name TEXT, location TEXT);
CREATE TABLE t_violation (violation_id INT NOT NULL PRIMARY KEY IDENTITY(1,1), time DATETIME, video_location TEXT, photo_location TEXT, rule_type INT, camera INT);



exec sp_columns t_camera

exec sp_columns t_violation

drop table t_camera

select * from licenta.dbo.t_violation;
select * from licenta.dbo.t_camera;

delete licenta.dbo.t_violation;
delete licenta.dbo.t_camera;

drop table t_violation