function [] = save_fltv(x, fname, append)
mode = 'w';
if nargin == 3
    if append
        mode = 'a';
    end
end

fid = fopen(fname, mode);

fwrite(fid, length(x), 'int32'); %mxvals
fwrite(fid, length(x), 'int32'); %vals

fwrite(fid, x , 'double'); %valv
fclose(fid);

