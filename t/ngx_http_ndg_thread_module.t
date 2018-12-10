# Copyright (c) 2018 by chrono
#
# sudo cpan Test::Nginx
# export PATH=/opt/nginx/sbin:$PATH
# prove t/var.t

#use Test::Nginx::Socket 'no_plan';
use Test::Nginx::Socket;

repeat_each(1);
plan tests => repeat_each() * (blocks() * 2);

run_tests();

__DATA__

=== TEST 1 : thread
--- main_config
    thread_pool xxx threads=2;

--- config
    location = /thread {
        ndg_thread xxx;
    }

--- request
GET /thread

--- response_body chomp
hello nginx thread

--- error_log
thread task 0 ok


=== TEST 2 : no threadpool

--- config
    location = /thread {
        ndg_thread xxx;
    }

--- request
GET /thread

--- ignore_response

--- error_log
ngx_thread_pool_get failed

