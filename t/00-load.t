#!/usr/bin/env lua

package.path = "t/?.lua;" .. package.path

local t = require 'Test.More'
local mrf = require 'mrf'

ok(mrf)
mrf:open()
mrf:reset(true)
mrf:set_addr(0x01, 0x0A0B0C0D)
mrf:set_power('-2');
mrf:set_freq('863.74');

done_testing()
