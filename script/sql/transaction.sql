create table transaction(
  src_user_id varchar(40),
  dst_user_id varchar(40),
  token_id varchar(40) not null,
  value decimal not null,
  tx_type varchar(20) not null,
  tx_hash varchar(64) not null,
  created_at timestamp not null,
  block_no bigint not null
);