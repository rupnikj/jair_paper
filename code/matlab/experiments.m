if ~exist('datadir')
    datadir = 'd:\work\data\jair\';
end

loadit = false;
if ~loadit
    disp('computing');
    [mats, wrds, docs, invdoc, didx, widx, langnames] = alignProcessDocs({'en','es','de'}, datadir);
    disp('saving');
    quicksave;
else
    quickload;
end
%%
n = size(mats{1},2);
ntest = 1000;
ntrain = n - ntest;
peri = randperm(n);
tridx = peri(1:ntrain);
tsidx = peri(ntrain+1:end);
trmats = cell(size(mats));
tsmats = cell(size(mats));
m = length(mats);
for i = 1:m
   trmats{i} = mats{i}(:,tridx);
   tsmats{i} = mats{i}(:,tsidx);
end
clear mats

%%
nrp = 10;
niter = 5;
[projectors, centers] = kmeansprojector(trmats, nrp, niter);

[projectors2, centers2] = svdprojector(trmats, nrp);

prj = @(P,X,c) (P*X - (P*c)*ones(1, size(X,2)));
prjCent = @(P,X,c) (P*X - c*ones(1, size(X,2)) );

% use svd to project data first
m = length(proj);
ptrmats = cell(size(trmats));
ptsmats = cell(size(tsmats));

primalCenters = cell(size(trmats));
projCenters = cell(size(trmats));
for i = 1:m
   primalCenters{i} = mean(trmats{i},2);
   projCenters{i} = projectors2{i}*primalCenters{i};
   
   ptrmats{i} = prj(projectors2{i}, trmats{i}, primalCenters{i});
   ptsmats{i} = prj(projectors2{i}, tsmats{i}, primalCenters{i});
end

[projectors3, centers3] = starccaprojector(ptrmats, nrp, projectors2, projCenters);

