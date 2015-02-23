function idxc = aligner(cells) 

idx = cells{1};
for i = 2:length(cells)
    idx = intersect(idx, cells{i});
end

idxc = cell(size(cells));
for i = 1:length(cells)
   [~, idxc{i}, ~] = intersect(cells{i}, idx); 
end