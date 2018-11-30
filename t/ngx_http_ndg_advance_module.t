# Copyright (c) 2018 by chrono
#
# sudo cpan Test::Nginx
# export PATH=/opt/nginx/sbin:$PATH
# prove t/var.t

#use Test::Nginx::Socket 'no_plan';
use Test::Nginx::Socket;

repeat_each(2);
plan tests => repeat_each() * (blocks() * 6);

run_tests();

__DATA__

=== TEST 1 : test array/list/

--- config
    location = /advance {
        ndg_advance on;
        return 200 "hello nginx\n";
    }

--- request
GET /advance

--- response_body
hello nginx

--- error_log
ngx array ok
ngx list ok
ngx queue ok
ngx rbtree ok
ngx buf ok
ngx chain ok
advance ok

=== TEST 2 : disable test

--- config
    location = /advance {
        ndg_advance off;
        return 200 "hello nginx\n";
    }

--- request
GET /advance

--- response_body
hello nginx

--- no_error_log
[error]

