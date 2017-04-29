function [ans,shift]=max_corr(a,b,l,fun=@corr_red)
	%function ans=max_corr(a,b,l,fun=@corr_red)
	%Calculate maximum value of function `fun` applied to subarrays of `a` and `b` of length `l`.
	%(Can be used for correlation search
	ans=0;
	shift=0;
	%disp(l)
	%fflush(stdout);
	for sh=0:length(a)-l
		z=a(end-l+1:end);
		%disp(l)
		%disp(size(z));
		%fflush(stdout);
		x=b(end-sh-l+1:end-sh);
		w=fun(z,x);
		plot(z); hold all; plot(x); title(num2str(ans)); hold off; pause(0.05);
		if ans<w
			ans=w;
			shift=-sh;
		end
	end

	for sh=0:length(b)-l
		z=a(1:l);
		x=b(1+sh:l+sh);
		w=fun(z,x);
		plot(z); hold all; plot(x); title(num2str(ans)); hold off; pause(0.05);
		if ans<w
			ans=w;
			shift=sh;
		end
	end
end
