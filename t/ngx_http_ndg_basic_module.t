# Copyright (c) 2018 by chrono
#
# sudo cpan Test::Nginx
# export PATH=/opt/nginx/sbin:$PATH
# prove t/var.t

#use Test::Nginx::Socket 'no_plan';
use Test::Nginx::Socket;

repeat_each(2);
plan tests => repeat_each() * (blocks() * 4);

run_tests();

__DATA__

=== TEST 1 : test string/time/

--- config
    location = /basic {
        ndg_basic on;
        return 200 "hello nginx\n";
    }

--- request
GET /basic

--- response_body
hello nginx

--- error_log
ngx_str ok
ngx_time ok
basic ok

=== TEST 2 : disable test

--- config
    location = /basic {
        ndg_basic off;
        return 200 "hello nginx\n";
    }

--- request
GET /basic

--- response_body
hello nginx

--- no_error_log
[error]

