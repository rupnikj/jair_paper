function [] = dispw(vec, words, topk, botk)
if nargin == 2
    topk = 5;
    botk = 5;
end
if nargin == 3
    botk = 5;
end
[~, tidx] = sort(vec);
disp(strcat('top ', num2str(topk)));
disp(words(tidx(end:-1:end-topk+1))');

disp(strcat('bot ', num2str(botk)));
disp(words(tidx(1:botk))');
