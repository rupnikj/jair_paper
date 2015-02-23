function [idx,C, nor2] = kmeans2(X, k, iter , verbosity, C)

% %REMOVE DUPLICATE COLS
% test = X'*randn(size(X,1), 100);
% [~, index] = unique(test, 'rows', 'first');
% X = X(:,index);

if (nargin == 3) 
    verbosity = true;
end

n = size(X,2);
if (nargin < 5)
    per = randperm(n);
    start = per(1:k);
    C = full(X(:, start));
end

norX = zeros(n,1);
for i = 1:n
   norX(i) = norm(X(:,i))^2; 
end


nor2 = ones(1, k);
idxc = zeros(1,n);
konec = 0;
for i=1:iter
    tic
    %podobnost
    nor2 = sum(C.^2, 1);

    D =  full(C'* X) - nor2' * (0.5* ones(1, n)) - (0.5 * ones(k,1) )* norX';
    [energy,idx] = max(D);
    if (idx == idxc) 
        konec = 1;
    end
    
    %~podobnost
    clear D;    
    a = zeros(1,k);
    for j= 1:k
       idj = (idx == j);
       a(j) = sum(idj);
%        if (a(j) == 0)
%            error('empty cluster')
%        end
       if a(j) > 0
       c = full(sum(X(:,idj),2));
          C(:,j) = 1/(a(j)+1)* (c + C(:,j));
       end
       %nor2(j) = C(:,j)' * C(:,j);
    end
    
    idxc = idx;
    
    if (verbosity)
    disp([i, toc, mean(sqrt(-2*energy))]);    
    end
    save idx idx
    if konec
        break;
    end;
end

