# Copyright (c) 2018 by chrono
#
# sudo cpan Test::Nginx
# export PATH=/opt/nginx/sbin:$PATH
# prove t/var.t

#use Test::Nginx::Socket 'no_plan';
use Test::Nginx::Socket;

repeat_each(2);
plan tests => repeat_each() * (blocks() + 4);

run_tests();

__DATA__

=== TEST 1 : enable hello module

--- config
    location = /hello {
        ndg_hello on;
        return 200 "hello nginx\n";
    }

--- request
GET /hello

--- response_body
hello nginx

--- error_log
hello ansi c

=== TEST 2 : disable hello module

--- config
    location = /hello {
        ndg_hello off;
        return 200 "hello nginx\n";
    }

--- request
GET /hello

--- response_body
hello nginx

--- no_error_log
[error]

