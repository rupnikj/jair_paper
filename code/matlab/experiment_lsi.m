%cd('D:\Users\rupnik\work\latex\jair_paper\code\matlab')
datadir = 'D:\Users\rupnik\work\data\cross-ling-event-registry\'

selectedLangs = {'en','es','de'};
[mats, wrds, docs, invdoc, didx, widx, langnames] = alignProcessDocs(selectedLangs, datadir);


%%
nrp = 500;

disp('data loaded');
[projectors, centers] = svdprojector(mats, nrp);
disp('kmeans saved');
save svdprojectors;
