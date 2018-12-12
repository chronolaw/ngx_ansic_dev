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

=== TEST 1 : zone balance

--- main_config
    stream {
        server {
            listen 1707;
            listen 1708;
            listen 1709;
            preread_buffer_size 1k;
            ndg_echo;
        }
    }

--- http_config
    upstream backend {
        zone shups 64k;
        ndg_balance;
        server 127.0.0.1:1707;
        server 127.0.0.1:1708;
        server 127.0.0.1:1709;
    }

--- config
    location = /upstream {
        ndg_upstream_pass backend;
    }

--- request
GET /upstream?hello

--- response_body chomp
hello

--- error_log
ndg upstream ok
ndg balance ok

