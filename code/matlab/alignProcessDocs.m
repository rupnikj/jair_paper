function [mats, wrds, docs, invdoc, didx, widx, langnames] = alignProcessDocs(langnames, datadir)
%example: alignProcessDocs({'en','es','de'});
if nargin == 1
    datadir = 'd:\work\data\jair\';
end

alangs = cell(size(langnames));
adocnames = cell(size(langnames));
indices = cell(size(langnames));
words = cell(size(langnames));

unique_cell = load(strcat(datadir, 'unique_cell.mat'),'-mat'); unique_cell = unique_cell.unique_cell;
langids = loadwords(strcat(datadir, 'langnames.txt'));

for i = 1:length(langnames)
   %tempidx = find langname{i} in langids
   tidx = find(cellfun(@(x)(strcmp(x,langnames{i})),langids));
   indices{i} = unique_cell{tidx};
end

% align
idxc = aligner(indices);
disp('start')
for i = 1:length(langnames)
   temp = load(strcat(datadir, strcat(langnames{i}, '.matlab')), '-mat');
   alangs{i} = temp.spmatrix(:, idxc{i});
   temp = loadwords(strcat(datadir, strcat(langnames{i}, '.names')));
   adocnames{i} = temp(idxc{i});
   clear temp;
   words{i} = loadwords(strcat(datadir, strcat(langnames{i}, 'words.txt')));
  
end
disp('loaded and aligned')

%% prune words
plangs = cell(size(alangs));
pwords = cell(size(alangs));
pwordsIdx = cell(size(alangs));
for i = 1:length(alangs)
    [plangs{i}, idx] = wordprune(alangs{i}, 0.975, [], 500000);
    pwordsIdx{i} = idx;
    pwords{i} = words{i}(idx);
end
clear alangs;
disp('pruned')
%%
invdoc = cell(size(plangs));
tlangs = cell(size(plangs));
for i = 1:length(plangs)
   [tlangs{i}, invdoc{i}] = tfidf(plangs{i});
   tlangs{i} = normalizeSparseColumns(tlangs{i}); 
end
clear plangs;
disp('tfidf done')
%% save
% pwords: pruned word list
% pwordsIdx: indices in the full word
% plangs: pruned words, aligned languages
% idxc: idxc indices of documents that were selected (alignment)
mats = tlangs;
wrds = pwords;
docs = adocnames;
didx = idxc;
widx = pwordsIdx;

% end