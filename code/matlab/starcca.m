function [W, D]= starcca(xcovars, vars, reg)
%xcovars = {C12, C13, C14, ..., C1m}
%vars = {C11, ..., Cmm}
if nargin < 3
    reg = 0;
end

m = length(vars);

invchols = cell(1, m);
%chol(A)'*chol(A) = A

%based on subsitution $x_i = chol(c_ii) w_i
%w_i = invchol(C_ii) * x_i
for i = 1:m
    invchols{i} = inv(chol(vars{i} + reg*eye(size(vars{i}, 1))));
end
E = cell(1, m-1); 

for i = 1:m-1
    E{i} = invchols{1}'*xcovars{i}*invchols{i+1};  %invchol(1)' C1j invchol(j)
end

F = E{1}*E{1}';
for i = 2:m-1
    F = F+ E{i}*E{i}';
end

[V,D] = eig(F);
D = diag(D);
D = flipud(D); V = fliplr(V);  

W = cell(1, m);

W{1} = invchols{1}*V;
for i = 2:m
   temp = E{i-1}'*V; %Cj1 (thats why we transpose)
   %normalize
   temp = temp* diag(sparse(sqrt(sum(temp.^2, 1)).^(-1)));
   W{i} = invchols{i}*temp;    
end
