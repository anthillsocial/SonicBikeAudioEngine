local osc = require 'osc'
local data = osc.pack('/load', 0, "startup.wav")
local data2 = osc.pack('/play', 0)
--osc = require( 'osc' ) 
--p = osc.Message:new() 
--p.address = '/in/foo' 

-- ... send ... receive
--local url, a, b, c = osc.unpack(data)
--local osc = require 'osc'
--client = osc.Client('127.0.0.1', 12345)
--client:send('/load', 0, "startup.wav")
--client:send('/loop', 0)
--client:send('/play', 0)

