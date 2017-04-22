function res= process_data(fname)
    s=fileread(fname);
    window=30;
    %lines=strsplit(s,{"\\|\\d\\|","\\|\-\\|"},"delimitertype","regularexpression","collapsedelimiters",1);
    [~,~,~,~,nm]=regexp(s,{"\\|([\\d\\s]+)\\|\-\\|"});
    nm=nm{1};
    j=1;
    for i=1:length(nm)
        t=str2num(nm{i}{1});
        [~,ind]=max(t);
        if ~(ind>length(t)/2 )
            tt=zeros(length(t)-1-window,1);
            for k=1:length(tt);
                tt(k)=sum(abs(t(k+1:k+window+1)-t(k:k+window)));
            end
            [~,ind2]=max(tt);
            ms(j,:)=t(ind2:ind2+window);
            ms(j,:)=ms(j,:)/max(ms(j,:));
            %ms(j,:)=t;
            %if j==2; break end
            j=j+1;
        end
    end
    figure;
    plot(ms')
    title(fname);
    c=corr(ms')';
    %max(c(c<.99)')
    
end
