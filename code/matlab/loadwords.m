function words = loadwords(fnm)

f = fopen(fnm);             
g = textscan(f,'%s','delimiter','\n');
fclose(f);
words = g{1};