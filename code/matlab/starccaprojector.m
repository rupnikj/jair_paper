function [projectors, centers] = starccaprojector(langs, nrp, preprojectors, precenters)
% langs: totally aligned data matrices, column examples
% nrp: number of random projections
% niter: number of kmeans iterations

m = length(langs);
centers = cell(1,m);

ndim = zeros(m,1);
for i = 1:m
    centers{i} = zeros(nrp,1);
    ndim(i) = size(langs{i},1);
end

crosscov = @(X,Y) 1/(size(X,2) -1)* (X - mean(X,2)*ones(1, size(X,2)))*(Y - mean(Y,2)*ones(1, size(Y,2)))';
covariances = cell(1,m);
crosscovariances = cell(1,m-1);
for i = 1:m    
    covariances{i} = crosscov(langs{i}, langs{i});    
end
for i = 2:m    
    crosscovariances{i-1} = crosscov(langs{1}, langs{i});    
end

disp('crosscovariances computed');

%call starcca (input are cross-covariances and variances)
reg = 0;
[Wstar, lam] = starcca(crosscovariances, covariances, reg);
disp('cica')
projectors = cell(size(preprojectors));
centers = cell(size(preprojectors));
for i = 1:m
   projectors{i} = Wstar{i}'*preprojectors{i}; 
   centers{i} =  Wstar{i}' * precenters{i};   
end
% get projectors from Wstar and lam
%projectors = Wstar;
%centers = projectors * mean(x)
