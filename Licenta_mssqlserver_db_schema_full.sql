
CREATE TABLE t_camera (camera_id TEXT NOT NULL PRIMARY KEY, name TEXT, location TEXT, StreamLocation TEXT);
CREATE TABLE t_violation (violation_id TEXT NOT NULL PRIMARY KEY, time DATETIME, video_location TEXT, photo_location TEXT, rule_type INT, camera TEXT);

