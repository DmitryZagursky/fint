function [cor,pos]=max_corr(a,b,l,fun=@corr_red)
	%function [cor,pos]=max_corr(a,b,l,fun=@corr_red)
	%Calculate maximum value of function `fun` applied to subarrays of `a` and `b` of length `l`.
	%returns:
	%	cor - max value of the function
	%	pos=[p1 p2] - vector of starting positions in the input, which gives the maximum value
	%(Can be used for correlation search)
	cor=0;
	shift=[0 0];
	if length(a)>length(b)
		v1=a; v2=b;
	else
		v1=b; v2=a;
	end
	l=length(v2);
	if size(v1,1)==1
		v1=[zeros(1,l) v1 zeros(1,l)];
	else
		v1=[zeros(l,1); v1; zeros(l,1)];
	end
	for i=1:length(v1)-l
		c=0;
		x=v1(i:i+l-1);
		c=v2'*x;
		if cor < c
			cor=c;
			shift=i;
		end
		%plot(v2); hold all; plot(x); title(num2str(cor)); hold off; pause(0.01);
	end
	cor=fun(v1(shift:shift+l-1),v2);
	shift=shift-l;
end
%function [cor,pos]=max_corr(a,b,l,fun=@corr_red)
	%%function [cor,pos]=max_corr(a,b,l,fun=@corr_red)
	%%Calculate maximum value of function `fun` applied to subarrays of `a` and `b` of length `l`.
	%%returns:
	%%	cor - max value of the function
	%%	pos=[p1 p2] - vector of starting positions in the input, which gives the maximum value
	%%(Can be used for correlation search)
	%cor=0;
	%shift=[0 0];
	%a=[zeros(1,l)'; a; zeros(1,l)'];
	%b=[zeros(1,l)'; b; zeros(1,l)'];
	%%step=100;
	%%step=1;
	%for i=1:length(b)-l+1;
		%for j=1:length(a)-l+1;
			%z=a(j:j+l-1);
			%%z=a(end-l-j+1:end-j);
			%x=b(i:i+l-1);
			%%plot(z); pause(0.01);
			%%c = fun(z,x);
			%c=z'*x;
			%if cor < c
				%cor=c;
				%shift=[i j];
			%%end 
			%%if mod(j,step)==0
				%plot(z); hold all; plot(x); title(num2str(cor)); hold off; pause(0.01);
			%end
		%end
	%end
	%disp(shift)
	%cor=fun(a(shift(1):shift(1)+l-1),b(shift(2):shift(2)+l-1));
%end
