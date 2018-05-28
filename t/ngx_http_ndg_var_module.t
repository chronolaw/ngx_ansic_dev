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

=== TEST 1 : echo var

--- config
    location = /var {
        ndg_echo_v $uri;
    }

--- request
GET /var

--- response_body : /var

--- error_log
echo var ok

=== TEST 2 : echo complex var

--- config
    location = /cv {
        ndg_echo_cv "hello $uri\n";
    }

--- request
GET /cv

--- response_body
hello /cv

--- error_log
echo cv ok

