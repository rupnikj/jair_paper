if ~exist('datadir')
    datadir = 'd:\work\data\jair\';
end

save(strcat(datadir, 'mats.mat'), 'mats', '-v7.3');
save(strcat(datadir, 'wrds.mat'), 'wrds');
save(strcat(datadir, 'docs.mat'), 'docs');
save(strcat(datadir, 'widx.mat'), 'widx');
save(strcat(datadir, 'didx.mat'), 'didx');
save(strcat(datadir, 'invdoc.mat'), 'invdoc');
save(strcat(datadir, 'langnames.mat'), 'langnames');