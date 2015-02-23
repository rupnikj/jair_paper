function Y = doctransform(X, widx, invdoc)
%X is a sparse matrix, or set of sparse matrices (several languages)

%filters words using widx (rows of X)
%applies invdoc weights
%normalizes
if ~iscell(X)
    Y = X(widx,:);
    Y = tfidf(Y, invdoc);
    Y = normalizeSparseColumns(Y);
    
else
    Y = cell(size(X));
    for i = 1:length(X)
        Y{i} = X{i}(widx{i},:);
        Y{i} = tfidf(Y{i}, invdoc{i});
        Y{i} = normalizeSparseColumns(Y{i});
    end
end
