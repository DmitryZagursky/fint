--This is torch script used to generate plots
require "gnuplot"
function get_data()
	for _,fname in ipairs(arg) do
		print(fname)
		if not fname then print("no files") return end

		local f=io.open(fname);
		res={};
		for l in f:lines() do
			local r={}
			for ax,ay,az,gx,gy,gz in l:gmatch(">(-?%d+) (-?%d+) (-?%d+) (-?%d+) (-?%d+) (-?%d+)|") do
				table.insert(r,{ax,ay,az,gx,gy,gz})
			end
			if #r>0 then
				table.insert(res,r)
				r.name=fname..#res
			end
		end
		f:close()
		return res
	end
end
function plot(res)
	--gnuplot.window()
	local l={}
	for _,r in pairs(res) do
		l[#l+1]={r.name, torch.Tensor(r)}
		--gnuplot.figure(1)
		--gnuplot.plot(l[#l])
	end
	gnuplot.plot(table.unpack(l))
end
plot(get_data())
