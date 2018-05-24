#!/bin/sh

ngx_path=${HOME}/nginx
src_path=${HOME}/github/ngx_ansic_dev

build_date="${USER} build at `date '+%Y.%m.%d %H:%M'`"
cc_opt="-g -O0"

prefix="--prefix=/opt/nginx"
common_opts="--with-threads --with-pcre-jit"

no_modules="--without-http_fastcgi_module \
            --without-http_uwsgi_module \
            --without-http_scgi_module
           "

modules="ngx_http_ndg_test_module
        "

http_modules=""

for m in $modules
do
    http_modules="${http_modules} --add-module=${src_path}/http/${m} "
done

modules=""

stream_modules="--with-stream "

for m in $modules
do
    stream_modules="${stream_modules} --add-module=${src_path}/stream/${m} "
done

export LUAJIT_LIB=/usr/local/lib
export LUAJIT_INC=/usr/local/include/luajit-2.1

ld_opt="-Wl,-rpath,${LUAJIT_LIB}"

ndk_module="${HOME}/github/ngx_devel_kit"
ngx_lua_module="${HOME}/github/lua-nginx-module"

ngx_lua="--add-module=${ndk_module}
         --add-module=${ngx_lua_module}
        "

opts="${prefix}           \
      ${common_opts}      \
      ${http_modules}     \
      ${stream_modules}   \
      ${ngx_lua}          \
      ${no_modules}
      "

cd $ngx_path

./configure     \
    --build="${build_date}" \
    --with-cc-opt="${cc_opt}"  \
    --with-ld-opt="${ld_opt}"   \
    ${opts}


cd -
