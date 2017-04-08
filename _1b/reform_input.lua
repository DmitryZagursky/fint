for _,fname in ipairs(arg) do
	print(fname)
	if not fname then print("no files") return end

	f=io.open(fname);
	res={};
	for l in f:lines() do
		r={}
		for ax,ay,az,gx,gy,gz in l:gmatch(">(-?%d+) (-?%d+) (-?%d+) (-?%d+) (-?%d+) (-?%d+)|") do
			table.insert(r,{ax,ay,az,gx,gy,gz})
		end
		if #r>0 then
			table.insert(res,r)
			o='export/'..fname:sub(1,-5).."-"..#res;
			--print(#res)
			o=io.open(o,'w');
			for _,l in ipairs(r) do
				o:write(table.concat(l,', '))
				o:write('\n');
			end
			o:close();
		end
	end
	print("Measurement count ", #res)
	f:close()
	
end
