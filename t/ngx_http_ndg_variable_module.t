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

=== TEST 1 : no log var

--- config
    location = /var {
        return 200 "hello\n";
    }

--- request
GET /var?uri

--- response_body
hello

--- no_error_log

=== TEST 2 : log var

--- config
    location = /var {
        ndg_variable on;
        return 200 "hello\n";
    }

--- request
GET /var

--- response_body
hello

--- error_log
log var current_method=GET ok

=== TEST 3 : $current_method var

--- config
    location = /var {
        return 200 "hello $current_method\n";
    }

--- request
GET /var

--- response_body
hello GET

--- no_error_log

