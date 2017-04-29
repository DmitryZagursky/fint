function data=get_data(file)
	%function data=get_data(file)
	%reads file `file` written in _10.ino output format and returns the data as a matrix
	assert(exist(file,'file') )
    s=fileread(file);
    [~,~,~,~,nm]=regexp(s,{"\\|([\\-\\d\\s]+)\\|\\-\\|"});
    nm=nm{1};
    j=1;
    for i=1:length(nm)
        t=str2num(nm{i}{1});
		ms(end+1,:)=t;
    end
    data=ms';
end
