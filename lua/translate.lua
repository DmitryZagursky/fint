--Convert gatttool reading output into normal number

local str = table.concat (arg)
--print(str)
conv=function (str, sizeof)
--conv=function (str, sizeof)
--`str` - string to convert
--`sizeof` - number of bytes per number, if none, then whole string is a single number
    local sss={}
    local ans={}
    for c in str:gmatch("(%x%x)") do
	table.insert(sss,c)
    end
    local pos=#sss
    sizeof = sizeof or pos
    while pos-sizeof>=0 do
	local r={"0x"}
	for i=pos,pos-sizeof+1,-1 do
	    table.insert(r,sss[i])
	end
	table.insert(ans,tonumber(table.concat(r)))
	pos=pos-sizeof
    end
    return ans
end
if str~="" then print(conv(str))
else return conv end
