create table users (
	id bigint unsigned not null auto_increment primary key,
	username varchar(20) not null unique,
	password char(60) binary not null, -- bcrypt hash
	display_name varchar(40)
)
