# Copyright (c) 2018 by chrono
#
# sudo cpan Test::Nginx
# export PATH=/opt/nginx/sbin:$PATH
# prove t/var.t

use Test::Nginx::Socket 'no_plan';

run_tests();

__DATA__

=== TEST 1 : enable test module

--- config
    location = /test {
        ndg_test on;
        return 200 "hello nginx\n";
    }

--- request
GET /test

--- response_body
hello nginx

--- error_log
hello ansi c

=== TEST 2 : disable test module

--- config
    location = /test {
        ndg_test off;
        return 200 "hello nginx\n";
    }

--- request
GET /test

--- response_body
hello nginx

--- no_error_log
[error]

