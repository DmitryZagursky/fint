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
parser:option{
    name="-o --output",
    description="Output file if any",
    args=1,
}
parser:option{
    name="-s --sizeof",
    description="How many bits are in one value in gatttool string", 
}
parser:flag{
    name="-l --log",
    description="Print time usage"
}
arg={}
local opts=parser:parse()
local config=dofile(opts.config)
local tr=require "translate"
local rtr=require "revtranslate"

local dev=config.device
local input=config.characteristics.input
local index=config.characteristics.index
local output=config.characteristics.output
local sizeof= tonumber(opts.sizeof) or tonumber(config.sizeof) or nil
local log = opts.log and print or (function() end)

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
   os.execute("sleep 0.01") 
end
local getHex=function(str)
    return str:match("%x%x %x%x %x%x %x%x")
end

local results={}
local i=0
local fails=0


local prev, cur
local T=os.time()
while true do
    local ttt=os.clock()
    local t=os.clock()
    shell(index_write(i))
    --wait()
    log("Per write",os.clock()-t)
    local t=os.clock()
    local s=shell(checkoutput)
    log("Per read", os.clock()-t)
    local t=os.clock()
    local werq=shell("echo fuuuuuu")
    t=os.clock()
    log("Per simple shell ", os.clock()-t)
    t=os.clock()
    local data= tr(getHex(s),sizeof)
    log("Per conversion",os.clock()-t)
    ------if shell(checkin):match("03") then
    --print(prev,cur)
    cur=data[#data]+data[1]
    if prev~=cur then
	--print("Per check",os.clock()-t)
	local t=os.clock()
	log(data)
	for _,v in ipairs(data) do
	    table.insert(results,v)
	end
	prev=cur
	i=i+#data
	fails=0
	if i%20==0 then print(i) end
    else
	fails=fails+1
	log('retry')
	if fails>3 then
	    break
	end
    end
    log("Per loop ",os.clock()-ttt)
end
--print("Total time ", os.difftime(os.time(),T))
if opts.output then 
    local f=io.open(opts.output,"w")
    for i=1,#results do
	f.write(results[i])
    end
    f:close()
else 
    for i=1,#results do
	print(results[i])
    end
end
