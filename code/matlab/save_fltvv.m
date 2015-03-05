function [] = save_fltvv(A, fname, rowmajor, append)
mode = 'w';
if nargin <3
    rowmajor = false;    
end
if nargin == 4
    if append
        mode = 'a';
    end
end

fid = fopen(fname, mode);
fwrite(fid, size(A,1), 'int32'); %rows
fwrite(fid, size(A,2), 'int32'); %cols

fclose(fid);

if rowmajor
    A = A';
end

save_fltv(A(:), fname, true);

