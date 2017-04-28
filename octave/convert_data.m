function convert_data(file,folder)
    %function convert_data(fname,folder)
    %reads the data given by arduino from result file `file` 
    %and writes each measurement into a single file inside folder `folder`
    assert(exist(file,'file') )
    %s=fileread(file);
    
    %%lines=strsplit(s,{"\\|\\d\\|","\\|\-\\|"},"delimitertype","regularexpression","collapsedelimiters",1);
    %[~,~,~,~,nm]=regexp(s,{"\\|([\\d\\s]+)\\|\-\\|"});
    %nm=nm{1};
    %j=1;
    %for i=1:length(nm)
        %t=str2num(nm{i}{1});
	%ms(end+1,:)=t;
    %end
    store_rows(folder,get_data(file));
end
