# Copyright (c) 2019 by chrono
#
# sudo cpan Test::Nginx
# sudo apt-get install libev-dev
# sudo apt-get install expect apache2-utils
# export PATH=/opt/nginx/sbin:$PATH
# export TEST_NGINX_BENCHMARK='200000 2'
# prove t/var.t

use Test::Nginx::Socket 'no_plan';
run_tests();

__DATA__

=== TEST 1 : echo benchmark

--- config
    location = /echo {
        ndg_echo "hello\n";
    }

--- request
GET /echo HTTP/1.0

--- response_body
hello

--- error_log
echo ok

