function A = normalizeSparseColumns(X)

norms = sqrt(full(sum(X.^2, 1)));

norms(norms == 0) = Inf;
norms = norms.^(-1);

N = size(X,2);

D = spconvert([(1:N)' (1:N)' norms']);
A = X*D;
