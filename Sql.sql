
CREATE TABLE t_camera (camera_id INT NOT NULL PRIMARY KEY IDENTITY(1,1), name TEXT, location TEXT);
CREATE TABLE t_violation (violation_id INT NOT NULL PRIMARY KEY IDENTITY(1,1), time DATETIME, video_location TEXT, photo_location TEXT, rule_type INT, camera INT);



exec sp_columns t_camera

exec sp_columns t_violation

drop table t_camera

select * from licenta.dbo.t_violation where camera = '{0910a959-9606-42a5-ac5b-f390381df5fb}';
select * from licenta.dbo.t_camera;

delete licenta.dbo.t_violation ;
delete licenta.dbo.t_camera;
delete licenta.dbo.t_camera where LICENTA.dbo.t_camera.name='20200718_211101.mp4';

drop table t_violation