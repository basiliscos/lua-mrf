#!/usr/bin/env lua

package.path = "t/?.lua;" .. package.path

local t = require 'Test.More'
local mrf = require 'mrf'

ok(mrf)

done_testing()