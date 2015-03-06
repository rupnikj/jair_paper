function [projectors, centers, S] = svdprojector(langs, nrp)

m = length(langs);
centers = cell(1,m);

ndim = zeros(m,1);
for i = 1:m    
    ndim(i) = size(langs{i},1);
    centers{i} = zeros(nrp,1);
end

if size(langs,2) > size(langs,1)
    langs = langs';
end


[U,S,~] = svd_funn(cell2mat(langs),[],[],sum(ndim), size(langs{1},2),true, nrp, 2, nrp+2);

projectors = mat2cell(U, ndim, nrp);

for i = 1:m
   [Q,R] = qr(projectors{i}, 0);   
   projectors{i} = R \ Q'; %inv(R) * Q'    
end
projectors = projectors';