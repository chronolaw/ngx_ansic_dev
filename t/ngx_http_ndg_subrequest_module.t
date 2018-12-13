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

=== TEST 1 : allow

--- config
    location = /allow {
        internal;
        return 200 "ok";
    }
    location = /sub {
        ndg_subrequest "/allow";
        ndg_echo "hello\n";
    }

--- request
GET /sub

--- response_body
hello

--- error_log
ndg subrequest ok, body is ok
echo ok

=== TEST 2 : deny

--- config
    location = /deny {
        internal;
        return 403 "fibidden";
    }
    location = /sub {
        ndg_subrequest "/deny";
        ndg_echo "hello\n";
    }

--- request
GET /sub

--- error_code : 403

--- error_log
ndg subrequest ok, body is fibidden

