cd('D:\work\latex\jair_paper\code\matlab')
datadir = 'E:\cross-ling-event-registry\'

selectedLangs = {'en','es','de'};
[mats, wrds, docs, invdoc, didx, widx, langnames] = alignProcessDocs(selectedLangs, datadir);


%%
nrp = 500;
niter = 100;
disp('data loaded');
[projectors, centers] = kmeansprojector(mats, nrp, niter);
disp('kmeans saved');
save kmeansprojectors;
