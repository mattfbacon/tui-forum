create table users (
	id bigint unsigned not null auto_increment primary key,
	username varchar(20),
	password char(64) binary, -- bcrypt hash
	display_name varchar(40)
)
