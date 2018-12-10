# Copyright (c) 2018 by chrono
#
# sudo cpan Test::Nginx
# export PATH=/opt/nginx/sbin:$PATH
# prove t/complex.t

#use Test::Nginx::Socket 'no_plan';
use Test::Nginx::Socket;

repeat_each(2);
plan tests => repeat_each() * (blocks() * 2 + 4);

run_tests();

__DATA__

=== TEST 1 : no log complex

--- config
    location = /complex {
        return 200 "hello\n";
    }

--- request
GET /complex

--- response_body
hello

--- no_error_log

=== TEST 2 : log complex $args

--- config
    location = /complex {
        ndg_complex_value "args=$args";
        return 200 "hello\n";
    }

--- request
GET /complex?xxx

--- response_body
hello

--- error_log
log complex args=xxx ok

=== TEST 3 : log complex

--- config
    location = /complex {
        ndg_complex_value "hello $scheme $uri";
        return 200 "hello\n";
    }

--- request
GET /complex

--- response_body
hello

--- error_log
log complex hello http /complex ok


=== TEST 4 : log plain

--- config
    location = /complex {
        ndg_complex_value "hello nginx";
        return 200 "hello\n";
    }

--- request
GET /complex

--- response_body
hello

--- error_log
log complex hello nginx ok


=== TEST 5 : log complex

--- config
    location = /complex {
        ndg_complex_value "$request_method $scheme $args";
        return 200 "hello\n";
    }

--- request
GET /complex?remote_addr

--- response_body
hello

--- error_log
log complex GET http remote_addr ok


