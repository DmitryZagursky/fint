--Convert gatttool reading output into normal number
local str = table.concat (arg)
--print(str)
conv=function (str)
    local mult=1
    local res={}
    for c in str:gmatch("(%x%x)") do
	table.insert(res,c)
    end
    local r={"0x"}
    for i=#res,1,-1 do
	table.insert(r,res[i])
    end
    return tonumber(table.concat(r))
end
if str~="" then print(conv(str))
else return conv end
