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

=== TEST 1 : log var

--- config
    location = /var {
        return 200 "hello\n";
    }

--- request
GET /var?uri

--- response_body
hello

--- error_log
log var /var ok

--- no_error_log
log complex

=== TEST 2 : log complex

--- config
    location = /var {
        ndg_complex_value "hello $scheme $uri";
        return 200 "hello\n";
    }

--- request
GET /var

--- response_body
hello

--- error_log
log complex hello http /var ok

--- no_error_log
log var

=== TEST 3 : log var&complex

--- config
    location = /var {
        ndg_complex_value "$request_method $scheme $args";
        return 200 "hello\n";
    }

--- request
GET /var?remote_addr

--- response_body
hello

--- error_log
log var 127.0.0.1 ok
log complex GET http remote_addr ok

