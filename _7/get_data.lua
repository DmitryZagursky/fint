--A combination of regexes and bash commands to get the data over bluetooth
--depends on the presence of `gatttool` command line application
local argparse = require "argparse"
local parser = argparse()
parser:option{
    name="-c --config",
    description="Configuration of the board to communicate",
    default="config.lua",
    args=1,
}
local opts=parser:parse()
local config=dofile(opts.config)
local tr=require "translate"
local rtr=require "revtranslate"

local dev=config.device
local input=config.characteristics.input
local index=config.characteristics.index
local output=config.characteristics.output

local baseCommand='timeout --signal=SIGINT 3 gatttool -b '..dev
local check=" --char-read --handle="
local checkin=baseCommand.. check..input
local checkindex=baseCommand..check..index
local checkoutput=baseCommand..check..output

local write=" --char-write-req --handle="
local value=" --value="
local in1=baseCommand..write..input..value.."01"
local index_template=baseCommand..write..index..value
local function index_write(num)
    return index_template..rtr(num)
end

local function shell(command)
    --print(command)
    local handle = io.popen(command)
    local res=handle:read("*a")
    handle:close()
    assert(res~="", "Device not responding")
    --print(res)
    return res
end

shell(checkin)

local wait=function()
   os.execute("sleep 0.05") 
end
local getHex=function(str)
    return str:match("%x%x %x%x %x%x %x%x")
end

local results={}
local i=0
local fails=0
while true do
    shell(index_write(i))
    wait()
    if shell(checkin):match("03") then
	table.insert(results,tr(getHex(shell(checkoutput))))
	i=i+1
	fails=0
	if i%20==0 then print(i) end
    else
	fails=fails+1
	print('retry')
	if fails>3 then
	    break
	end
    end
end

for i=1,#results do
    print(results[i])
end
