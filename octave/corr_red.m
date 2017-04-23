function res=corr_red(a,b)
 %calculates reduced correlation between a and b or between columns of a:
 % res= sum( a.*b )/ (sum(sqrt(a.^2)).*sum(sqrt(b.^2)));
 if exist("b","var")
    res= sum( a.*b )/ (sum(sqrt(a.^2)).*sum(sqrt(b.^2)));
 elseif size(a,2)>1
    n=size(a,2);
    res=eye(n);
    for i=1:n
	for j=i+1:n
	    if i<=n && j<=n
		res(i,j)=corr_red(a(:,i),a(:,j));
		res(j,i)=res(i,j);
	    end
	end
    end
 end
end
