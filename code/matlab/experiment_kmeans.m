datadir = 'D:\Users\rupnik\work\data\cross-ling-event-registry\'

selectedLangs = {'en','es','de'};
[mats, wrds, docs, invdoc, didx, widx, langnames] = alignProcessDocs(selectedLangs, datadir);


%%
nrp = 500;
niter = 100;
disp('data loaded');
[projectors, centers] = kmeansprojector(mats, nrp, niter);
disp('kmeans computed');
save kmeansprojectors -v7.3;
