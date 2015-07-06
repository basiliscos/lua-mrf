#!/usr/bin/env lua

package.path = "t/?.lua;" .. package.path

local mrf = require 'mrf'

mrf:open()
local reinitialize_registers = arg[1] or false
mrf:reset(reinitialize_registers)
