load retrievalCCA
%  IRsCCA                    1x10             1840  cell                
%   dim                       1x10               80  double              
%   foretellCCA               1x10               80  cell                
%   foretold_distanceCCA      1x10               80  cell                
%   resultCCA
load retrievalLSI_WITH_S
%  Name                             Size            Bytes  Class     Attributes
% 
%   IRsLSI_WITH_S                    1x10             1840  cell                
%   dim                              1x10               80  double              
%   foretellLSI_WITH_S               1x10               80  cell                
%   foretold_distanceLSI_WITH_S      1x10               80  cell                
%   resultLSI_WITH_S                 1x10             1200  cell                
load retrievalLSI_WITHOUT_S
%   Name                                Size            Bytes  Class     Attributes
% 
%   IRsLSI_WITHOUT_S                    1x10             1840  cell                
%   dim                                 1x10               80  double              
%   foretellLSI_WITHOUT_S               1x10               80  cell                
%   foretold_distanceLSI_WITHOUT_S      1x10               80  cell                
%   resultLSI_WITHOUT_S                 1x10             1200  cell                
load RetrievalKMEANS.mat
%   Name                         Size            Bytes  Class     Attributes
% 
%   IRsKMEANS                    1x10             1840  cell                
%   dim                          1x10               80  double              
%   foretellKMEANS               1x10               80  cell                
%   foretold_distanceKMEANS      1x10               80  cell                
%   resultKMEANS                 1x10             1200  cell      
hold on
%Star CCA the best
plot(dim, [resultCCA{:}], 'red');
%Closely followed by LSI with S
plot(dim, [resultLSI_WITH_S{:}], 'green');
%Followed by LSI without S
plot(dim, [resultLSI_WITHOUT_S{:}], 'blue');
%Last place, slow Kmeans
plot(dim, [resultKMEANS{:}], 'black');
legend('Star CCA', 'LSI with S', 'LSI without S', 'Kmeans');
xlabel('Number of feature vectors');
ylabel('AMPR');

