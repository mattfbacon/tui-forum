create table users (
	id bigserial primary key,
	username varchar(20) not null unique,
	password bytea not null, -- bcrypt hash
	display_name varchar(40)
)
