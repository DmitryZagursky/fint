--A combination of regexes and bash commands to get the data over bluetooth
--depends on the presence of `gatttool` command line application
--intermittently queries both boards to get the hit status
local argparse = require "argparse"
local parser = argparse()
parser:option{
    name="-c --config",
    description="Configuration of the board to communicate",
    default="tap_config.lua",
    args=1,
}
parser:option{
    name="-o --output",
    description="Output file if any",
    args=1,
}
arg={}
local opts=parser:parse()
local config=dofile(opts.config)
local tr=require "translate"
local rtr=require "revtranslate"

local dev1=config.device1
local dev2=config.device2
local input=config.characteristics.input
local output=config.characteristics.output
local hittime=config.hittime

local baseCommand='timeout --signal=SIGINT 3 gatttool -b '
local check=" --char-read --handle="


local write=" --char-write-req --handle="
local value=" --value="
local write_1=write..input..value.."01"
local write_0=write..input..value.."00"

local start1=baseCommand..dev1..write_1
local start2=baseCommand..dev2..write_1
local stop1=baseCommand..dev1..write_0
local stop2=baseCommand..dev2..write_0
local check1=baseCommand..dev1..check..output
local check2=baseCommand..dev2..check..output
local checkin1=baseCommand..dev1..check..input
local checkin2=baseCommand..dev2..check..input
local writein1=baseCommand..dev1..write_1
local writein2=baseCommand..dev2..write_1

local function shell(command,ignore)
    --print(command)
    local handle = io.popen(command)
    --print('end')
    local res=handle:read("*a")
    --print('read')
    handle:close()
    --print'close'
    if not ignore and (res=="") then
	print("Device not responding")
	print(command)
    end
    --print('return')
    return res
end

local wait=function()
   os.execute("sleep 0.01") 
end
local gethex=function(str)
    --print(str)
    local s=str:match(" (%x%x)")
    if not s then
	print("Wrong string:")
	print(str)
    end
    return s
end
local ishit1=function() 
    return tr(gethex(shell(check1)))[1]==1
end
local ishit2=function() 
    return tr(gethex(shell(check2)))[1]==1
end

local wilhelm="paplay wilhelm.wav"
local sound=function()
    shell(wilhelm,true)
end

local point1=function() sound() end
local point2=function() sound() end
--local point1=function() end
--local point2=function() end

print('Initialization')
shell(checkin1)
shell(checkin2)
shell(writein1)
shell(writein2)
print("Fight")

local hit1,hit2
local timeout
local t=os.time()
while true do
    local r1,r2=ishit1(), ishit2()
    --print(r1,r2,timeout)
    if r1  then
	hit1=true
	timeout=timeout or hittime
    elseif r2 then
	hit2=true
	timeout=timeout or hittime
    end
    if timeout then
	timeout=timeout - 1
	if timeout < 0 then
	    if (hit1 and not hit2) then 
		point1()
		print("P1 score")
	    elseif (hit2 and not hit1) then 
		point2()
		print("P2 score")
	    else
		print("TIE")
	    end
	    hit1,hit2,timeout=nil,nil,nil
	end
    end
    --print"3"
    wait()
    --print(os.time(),(os.time()-t),(os.time()-t)%10)
    
    if (os.time()-t)%10 == 0 then
	print('.')
    end
end
