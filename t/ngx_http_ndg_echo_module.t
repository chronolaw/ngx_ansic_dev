# Copyright (c) 2018 by chrono
#
# sudo cpan Test::Nginx
# export PATH=/opt/nginx/sbin:$PATH
# prove t/var.t

#use Test::Nginx::Socket 'no_plan';
use Test::Nginx::Socket;

repeat_each(2);
plan tests => repeat_each() * (blocks() + 2);

run_tests();

__DATA__

=== TEST 1 : test echo module

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

