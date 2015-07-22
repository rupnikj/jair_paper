"""
evaluate accuracy of the story linking module. the similar articles are computed using CCA with 500 feature vector
evaluations are done using different sets of learning features
for each combination of features we print the accuracy, precision, recall, and f1
for the reviewers we've also added computation of standard deviation of the scores
"""
import os, sys, json, Orange, numpy, orngStat

fname = r"e:\Data\JAIR-Data\2015-03-23\LrnStoryLinkingFtrVec - sim_cca_with_500.tab"

data = Orange.data.Table(fname)

print "original data size: %d" % len(data)

def validExample(ex):
    langs = ex["Language pair"]
    pair = langs.value.split("-")
    if pair[0] == pair[1]:
        return False
    if pair[0] in ["eng", "deu", "spa"] and pair[1] in ["eng", "deu", "spa"]:
        return True
    return False

# remove instances where the language pair is between the same language
data = Orange.data.Table([d for d in data if validExample(d)])

print "data after removing same langs: %d" % len(data)

cca = ['CCAMatchCount', 'CCAArtMatchRatio', 'CCAAvgMatchWgt', 'CCAAvgCosSim']
semantic = ["ConceptCosSim", "ConceptJaccardSim", "EntityCosSim", "EntityJaccardSim", "KeywordJaccardSim", "KeywordCosSim"]
misc = ["TimeDiff", "HasSameLocation", "MatchingDateRatio", "MatchingDateCount", "ExtractedDateJaccardSim", "CoreTimeDiff"]

tests = [
    (cca, "cca"), (semantic, "semantic"), (misc, "misc"),
    (cca + semantic, "cca + semantic"), (cca + misc, "cca + misc"), (semantic + misc, "semantic + misc"),
    (cca + semantic + misc, "cca + semantic + misc")]

for test in tests:
    attributes = [attr for attr in data.domain.features if attr.name in test[0]]
    new_domain = Orange.data.Domain(attributes, data.domain.class_var)
    new_data = Orange.data.Table(new_domain, data)
    classIndex = list(new_data.domain.classVar.values).index("yes")

    svm = Orange.classification.svm.SVMLearner()
    res = Orange.evaluation.testing.cross_validation([svm], new_data, folds=10)
    cm = orngStat.computeConfusionMatrices(res, classIndex = classIndex)

    print "======"
    print test[1]
    print "attributes: ",
    print ",".join([attr.name for attr in new_data.domain.features])

    print "Accuracy:  %.1f%%" % (100*orngStat.CA(res)[0])
    print "Precision: %.3f" % orngStat.precision(cm)[0]
    print "Recall:    %.3f" % orngStat.recall(cm)[0]
    print "F1:        %.3f" % orngStat.F1(cm)[0]
    
    res.all_results = res.results
    res.number_of_iterations = 1

    CAs = []
    Ps = []
    Rs = []
    F1s = []
    for fold in range(10):
        # select only examples from the fold
        res.results = [result for result in res.all_results if result.iteration_number == fold]
        cm = orngStat.computeConfusionMatrices(res, classIndex = classIndex)
        CAs.append(orngStat.CA(res)[0])
        Ps.append(orngStat.precision(cm)[0])
        Rs.append(orngStat.recall(cm)[0])
        F1s.append(orngStat.F1(cm)[0])
    
    print "Accuracy:  %.1f%% +- %.1f%%" % (100*numpy.average(CAs), 100*numpy.std(CAs))
    print "Precision: %.3f +- %.3f" % (numpy.average(Ps), numpy.std(Ps))
    print "Recall:    %.3f +- %.3f" % (numpy.average(Rs), numpy.std(Rs))
    print "F1:        %.3f +- %.3f" % (numpy.average(F1s), numpy.std(F1s))
    