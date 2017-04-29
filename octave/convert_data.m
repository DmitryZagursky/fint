function convert_data(file,folder)
    %function convert_data(fname,folder)
    %reads the data given by arduino from result file `file` 
    %and writes each measurement into a single file inside folder `folder`
    assert(exist(file,'file') )
    store_rows(folder,get_data(file));
end
