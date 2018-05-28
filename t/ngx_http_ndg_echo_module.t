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

=== TEST 1 : echo no args

--- config
    location = /echo {
        ndg_echo "hello\n";
    }

--- request
GET /echo

--- response_body
hello

--- error_log
echo ok

=== TEST 2 : echo with args

--- config
    location = /echo {
        ndg_echo "hello\n";
    }

--- request
GET /echo?nginx

--- response_body
nginx,hello

--- error_log
echo ok

