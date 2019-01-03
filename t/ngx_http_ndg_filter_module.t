# Copyright (c) 2018 by chrono
#
# sudo cpan Test::Nginx
# export PATH=/opt/nginx/sbin:$PATH
# prove t/var.t

#use Test::Nginx::Socket 'no_plan';
use Test::Nginx::Socket;

repeat_each(2);
plan tests => repeat_each() * (blocks() * 5 );

run_tests();

__DATA__

=== TEST 1 : add headers

--- config
    location = /filter {
        ndg_header x-name   chrono;
        return 201 "hello";
    }

--- request
GET /filter

--- error_code : 201
--- response_headers
x-name: chrono

--- response_body chomp
hello

--- error_log
header filter ok

--- no_error_log
body filter ok

=== TEST 2 : add footer

--- config
    location = /filter {
        ndg_footer ",nginx\n";
        return 200 "hello";
    }

--- request
GET /filter

--- response_body
hello,nginx

--- error_log
header filter ok
body filter ok

=== TEST 3 : add header&footer

--- config
    location = /filter {
        ndg_header x-name   chrono;
        ndg_footer ",nginx\n";
        return 200 "hello";
    }

--- request
GET /filter

--- response_headers
x-name: chrono

--- response_body
hello,nginx

--- error_log
header filter ok
body filter ok

=== TEST 4 : add more header&footer

--- config
    location = /filter {
        ndg_header x-name   chrono;
        ndg_header x-value  trigger;
        ndg_footer ",nginx\n";
        return 200 "hello";
    }

--- request
GET /filter

--- response_headers
x-name: chrono
x-value: trigger

--- response_body
hello,nginx

--- error_log
header filter ok
body filter ok

=== TEST 5 : todo
--- SKIP
