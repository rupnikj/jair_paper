if ~exist('datadir')
    datadir = 'd:\work\data\jair\';
end

%%
cd(mfilename('fullpath'));

load(strcat(datadir, 'mats'));
load(strcat(datadir, 'wrds'));
load(strcat(datadir, 'docs'));
load(strcat(datadir, 'widx'));
load(strcat(datadir, 'didx'));
load(strcat(datadir, 'invdoc'));
load(strcat(datadir, 'langnames'));
