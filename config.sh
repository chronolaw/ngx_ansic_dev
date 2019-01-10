#!/bin/sh

ngx_path=${HOME}/nginx
src_path=${HOME}/github/ngx_ansic_dev

build_date="${USER} build at `date '+%Y.%m.%d %H:%M'`"
cc_opt="-g -O0"
#cc_opt="-g -O0 -DNGX_DEBUG_PALLOC"

prefix="--prefix=/opt/nginx"
common_opts="--with-threads --with-pcre-jit"
debug_opts="--with-debug"

# sudo apt-get install google-perftools
gperf_module="--with-google_perftools_module"

no_modules="--without-http_fastcgi_module
            --without-http_uwsgi_module
            --without-http_scgi_module
           "

aux_module="--add-module=${src_path}/misc"

modules="ngx_http_ndg_hello_module

         ngx_http_ndg_basic_module
         ngx_http_ndg_advance_module

         ngx_http_ndg_echo_module
         ngx_http_ndg_filter_module

         ngx_http_ndg_upstream_module
         ngx_http_ndg_balance_module

         ngx_http_ndg_subrequest_module

         ngx_http_ndg_variable_module
         ngx_http_ndg_complex_value_module

         ngx_http_ndg_thread_module

         ngx_http_ndg_debug_module
        "

http_modules=""

for m in $modules
do
    http_modules="${http_modules} --add-module=${src_path}/http/${m} "
done

modules="ngx_stream_ndg_hello_module
         ngx_stream_ndg_discard_module
         ngx_stream_ndg_time_module
         ngx_stream_ndg_echo_module
        "

stream_modules="--with-stream "

for m in $modules
do
    stream_modules="${stream_modules} --add-module=${src_path}/stream/${m} "
done

misc_modules="--add-module=${src_path}/misc"

opts="${prefix}
      ${common_opts}
      ${debug_opts}
      ${gperf_module}
      ${aux_module}
      ${http_modules}
      ${stream_modules}
      ${misc_modules}
      ${no_modules}
      "

cd $ngx_path

./configure                     \
    --build="${build_date}"     \
    --with-cc-opt="${cc_opt}"   \
    ${opts}


cd -
