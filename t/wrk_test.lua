-- Copyright (c) 2019 by Chrono

-- wrk -t1 -c1 -d1 -s wrk_test.lua http://127.0.0.1

local counter = 0

local urls = {
    'echo', 'filter', 'variable'
}

function request()
    counter = counter + 1
    wrk.headers.x_name = 'chrono'.. counter

    return wrk.format(nil,
        wrk.path .. urls[counter % 3 + 1])
end
