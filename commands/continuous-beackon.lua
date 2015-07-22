#!/usr/bin/env lua

package.path = "t/?.lua;" .. package.path

local mrf = require 'mrf'
local unistd = require("posix.unistd")

mrf:open()
mrf:reset(true)
mrf:set_addr(0x01, 0x0A0B0C0D)
mrf:set_power('-2');
mrf:set_freq('863.74');

local limit = arg[1] or 1000
for i = 1, limit do
   mrf:send_frame(0x0, string.char(0x01,0x02,0x03,0x04,0x05,0x06));
   -- mrf:debug()
   print("set beakon packet #" .. i)
end
-- unistd.sleep(1)
