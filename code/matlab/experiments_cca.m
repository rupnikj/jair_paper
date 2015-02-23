%cd('D:\Users\rupnik\work\latex\jair_paper\code\matlab')
datadir = 'D:\Users\rupnik\work\data\cross-ling-event-registry\'
load 'D:\Users\rupnik\jair\svdprojectors.mat'
%%
%nrp = 5;
%[projectors, centers] = svdprojector(mats, nrp);


prj = @(P,X,c) (P*X - (P*c)*ones(1, size(X,2)));
prjCent = @(P,X,c) (P*X - c*ones(1, size(X,2)) );

% use svd to project data first
m = length(mats);
pmats = cell(size(mats));

primalCenters = cell(size(mats));
projCenters = cell(size(mats));
for i = 1:m
   primalCenters{i} = mean(mats{i},2);
   projCenters{i} = projectors{i}*primalCenters{i};
   
   pmats{i} = prjCent(projectors{i}, mats{i}, projCenters{i});
end

preproj = projectors;
[projectors, centers] = starccaprojector(pmats, nrp, preproj, projCenters);

save ccaprojectors -v7.3;
