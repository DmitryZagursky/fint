--file used to parse the exection times of get_data.lua
local fname=arg[1] if not fname then print('no file') return end

function file_lines(fname)
	assert(fname)
	local r=io.open(fname,'r')
	assert(r,"No such file file")
	return r:lines()
end
--serpent=require 'serpent'
local insert=function(t,e)
	t[#t+1]=e
	return e
end
local tonumber_old=tonumber
function tonumber(...)
	local ans={}
	for i,v in ipairs({...}) do
		ans[i]=tonumber_old(v)
	end
	return table.unpack(ans)
end

get_data=function(fname)
    local iter=file_lines(fname)
    local res={write={},check={},read={},loop={},simple={},conversion={}}
    for l in iter do
	local s1=tonumber(l:match("^Per write%s+([+%-]?%d+%.?%d*[eE+%-]*%d?%d?)"))
	local s2=tonumber(l:match("^Per read%s+([+%-]?%d+%.?%d*[eE+%-]*%d?%d?)"))
	local s3=tonumber(l:match("^Per conversion%s+([+%-]?%d+%.?%d*[eE+%-]*%d?%d?)"))
	local s4=tonumber(l:match("^Per simple shell%s+([+%-]?%d+%.?%d*[eE+%-]*%d?%d?)"))
	local s5=tonumber(l:match("^Per loop%s+([+%-]?%d+%.?%d*[eE+%-]*%d?%d?)"))
	if s1 then insert(res.write,s1) end
	if s2 then insert(res.read,s2) end
	if s3 then insert(res.conversion,s3) end
	if s4 then insert(res.simple,s4) end
	if s5 then insert(res.loop,s5) end
    end
    return res
end

function aver(arr)
    --print(arr)
    local av=0
    for i=1,#arr do
	av=av+arr[i]
    end
    return #arr>0 and av/#arr
end
function calculate(data)
    --print(data)
    local averages={}
    data.loop=nil
    for k,v in pairs(data) do
	averages[k]=tonumber( aver(v))
    end
    --print(averages)
    local sum=0
    --averages={1, 2, 3}
    for k,v in pairs(averages) do
	--print(sum)
	--print(tonumber(v) or 0)
	sum=sum+ (tonumber(v) or 0)
    end
    --print(sum)
    for k,v in pairs(averages) do
	averages[k]=v/sum
    end
    print("In percent")
    print(averages)
end
calculate( get_data(fname))
