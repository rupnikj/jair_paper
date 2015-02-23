%load projectors 
%on pankretas: D:\Users\rupnik\jair\kmeansprojectors
% (same instructions for lsiprojectors and ccaprojectors)


% documents will always be columns
%langnames : language order

%to compute similarities between two TF docs (based on bow files):

%1: prune words and computed TFIDFN
%X in lanugae i (matrix with column examples, or a column vector)
%Y in language j (matrix with column examples, or a column vector)
X = doctransform(X, widx{i}, invdoc{i});
Y = doctransform(Y, widx{j}, invdoc{j});

%2: project, normalize
pX = normalizeSparseColumns(projectors{i}*X);
pY = normalizeSparseColumns(projectors{j}*Y);

%3: similarity
S = pX'*pY;