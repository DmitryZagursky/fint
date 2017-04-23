function store_rows(fname,res)
    %function store_rows(fname,res)
    %takes matrix `res` and folder name `fname` and stores matrix rows in separate .txt files
    if ~exist('fname','var')
        fname='';
    end
    while strcmp(fname,'')||isdir(fname)
        old_fname=fname;
        fname=input(['Enter another name for the folder "' old_fname '"' "\n"],"s");
    end
    succ=mkdir(fname);
    assert(succ==1,"Something gone wrong, folder not created");
    for i=1:size(res,1);
        file=fopen([fname '/' num2str(i)],'w');
        fdisp(file, res(i,:)');
        fclose(file);
    end
end
