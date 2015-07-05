#!/usr/bin/env lua

package.path = "t/?.lua;" .. package.path

local t = require 'Test.More'
local mrf = require 'mrf'

ok(mrf)
mrf:open()
mrf:reset()
mrf:set_addr(0x01, 0x0A0B0C0D)

done_testing()
