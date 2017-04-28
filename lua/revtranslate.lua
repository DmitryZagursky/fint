local str = table.concat (arg)
rev=function (str)
    --translates number to the hex format used by ble
    local ind=tonumber(str)
    assert(ind and ind<256^3,"Numbers up to 256^3 are allowed")

    local div=1
    local res={}
    for i=1,4 do
	--print(ind%(div*256),ind%div)
	res[i]=string.format("%02x",(ind%(div*256)-ind%div)):sub(1,2)
	div=div*256
    end
    return table.concat(res)
end
if str~='' then print(rev(str))
else return rev end
