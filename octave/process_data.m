function res= process_data(fname,cut=0)
    %function res= process_data(fname,cut=0)
    %plot graphics of the results from fname
    %and print simple correlations between the lines
    %cut - if cut=0 unaltered data is plotted, otherwise, (0<cut<=1) the data is cut to most a `cut` fraction of its length, that encompasses the period of most notable changes of the values
    s=fileread(fname);
    
    %lines=strsplit(s,{"\\|\\d\\|","\\|\-\\|"},"delimitertype","regularexpression","collapsedelimiters",1);
    
    [~,~,~,~,nm]=regexp(s,{"\\|([\\d\\s]+)\\|\-\\|"});
    nm=nm{1};
    j=1;
    for i=1:length(nm)
        t=str2num(nm{i}{1});
        [~,ind]=max(t);
        if cut~=0
            %if ~(ind>length(t)/2 )
                window=max(1,floor(length(t)*cut));
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
            %end
        else
            ms(end+1,:)=t;
        end
    end
    figure;
    plot(ms')
    title(strrep(fname,"_",'-'));
    c=corr(ms')';
    %max(c(c<.99)')
    
end
