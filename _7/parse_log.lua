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
    local res={write={},check={},read={},loop={}}
    for l in iter do
	local s1=l:match("^wr(%d%.-%d+)")
	local s2=l:match("^ch(%d%.-%d+)")
	local s3=l:match("^re(%d%.-%d+)")
	local s4=l:match("^lo(%d%.-%d+)")
	if s1 then insert(res.write,tonumber(s1)) end
	if s2 then insert(res.check,tonumber(s2)) end
	if s3 then insert(res.read,tonumber(s3)) end
	if s4 then insert(res.loop,tonumber(s4)) end
    end
    return res
end

function aver(arr)
    local av=0
    for i=1,#arr do
	av=av+arr[i]
    end
    return av/#arr
end
function calculate(data)
    local averages={}
    for k,v in pairs(data) do
	averages[k]=aver(v)
    end
    local sum=0
    for k,v in pairs(averages) do
	sum=sum+v
    end
    for k,v in pairs(averages) do
	averages[k]=v/sum*85
    end
    print(averages)
end
calculate( get_data(fname))
