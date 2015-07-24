#!/usr/bin/env lua

package.path = "t/?.lua;" .. package.path

local mrf = require 'mrf'
local unistd = require("posix.unistd")

mrf:open()
mrf:reset(true)
mrf:set_addr(0x01, 0x0A0B0C0D)
mrf:set_power('-2');
mrf:set_freq('863.74');

mrf:listen();

local limit = 10000
for i = 1, limit do
   local addr, payload = mrf:recv_frame();
   local bytes = table.pack(string.byte(payload, 1, #payload));
   local hex_payload = "";
   for j = 1, #bytes do hex_payload = hex_payload .. string.format("0x%.2x ", bytes [j]) end
   
   print(string.format("frame %03d from %02x %02d bytes: %s",
                       i, addr, #payload, hex_payload))
end
