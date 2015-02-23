function [projectors, centers] = kmeansprojector(langs, nrp, niter)
% langs: totally aligned data matrices, column examples
% nrp: number of random projections
% niter: number of kmeans iterations
m = length(langs);
centers = cell(1,m);

ndim = zeros(m,1);
for i = 1:m
    ndim(i) = size(langs{i},1);
end

if size(langs,2) > size(langs,1)
    langs = langs';
end

[idx,C, nor2] = kmeans2(cell2mat(langs), nrp, niter);


projectors = mat2cell(C, ndim, nrp);
% pseudoinverse: use centroids as a genereal nonotrhonormal basis
% new coefs = express vectors in terms of the basis
% projectors: take x and just multiply
for i = 1:m
   [Q,R] = qr(projectors{i}, 0);   
   projectors{i} = R \ Q'; %inv(R) * Q'   
end
projectors = projectors';
