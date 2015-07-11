#!/usr/bin/env lua

package.path = "t/?.lua;" .. package.path

local mrf = require 'mrf'
local unistd = require("posix.unistd")

mrf:open()
mrf:reset(true)
mrf:set_addr(0x01, 0x0A0B0C0D)
mrf:set_power('-2');
mrf:set_freq('863.74');

local limit = 10000
for i = 1, limit do
   mrf:debug()
   unistd.sleep(1)
   print("set beakon packet #" .. i)
end
