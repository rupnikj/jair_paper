function [Y, idx] = wordprune(X, frac, skipfrac)

Z = sign(X); x = sum(Z, 2);

[x2,i2] = sort(-x); c = cumsum(x2)/sum(x2);

%index where c(i) > frac, c(i-1) < frac

m = c < frac;
M = c >= frac;
cutidx = find(circshift(m,1) & M);

cutidxstart = 1;
if nargin == 3
    ms = c < skipfrac;
    Ms = c >= skipfrac;
    cutidxstart = find(circshift(ms,1) & Ms);
end
idx = i2(cutidxstart:cutidx);
Y = X(idx, :);


