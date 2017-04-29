function ans=matr_corr(a,len,fun=@max_corr)
	%function ans=matr_corr(a,len,fun=@corr_red)
	%applies function `fun` to subarrays of length `len` of columns of matrix `a`
	n=size(a,2);
	ans=zeros(n);
	for i=1:n
		for j=1:n
			ans(i,j)=fun(a(:,i),a(:,j),len);
		end
	end
end
