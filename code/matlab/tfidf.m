function [tf, invdoc] = tfidf(data, invdoc)

[N,M] = size(data);

if nargin < 2
    X = sum(data' > 0);    
    invdoc = log(M./X)';
    
    %Handle non appearing words
    invdoc(isinf(invdoc)) = 0;
end

IDF = sparse(invdoc);
IDF = diag(IDF);

tf = IDF * data;

if (size(tf,1) < N)
    tf(N,1) = 0;
end
if (size(tf,2) > M)
    tf(1, M) = 0;
end

