#include "app.h"

TStrV LangV;
TIntStrH IdUriH;
THash<TInt, THash<TInt, TFltIntKdV> > SimH;

void CalcSim(const TFltVV& X1, const TFltV& IdV1, const TFltVV& X2, 
        const TFltV& IdV2, const int& LangId, const int& MaxIdDiff, 
        const int& MaxN) {
    
    const int Len1 = X1.GetCols();
    const int Len2 = X2.GetCols();
    // placeholder for similarities
    TFltIntKdV SimIdV; int LastStart = 0;
    // major loop
    //for (int i = 10000; i < 10100; i++) {
    for (int i = 1; i < Len1; i++) {
        if (i % 100 == 0) { printf("%6d / %6d\n", i, Len1); }
        const int Id1 = (int)IdV1[i];
        // move start pointer to the first element within id window
        for (int j = LastStart; j < Len2; j++) {
            const int Id2 = (int)IdV2[j];
            // should stop once within id window
            if (Id1 - Id2 < MaxIdDiff) { break; }
            // else move on
            LastStart++;
        }
        // compute similarities
        for (int j = LastStart; j < Len2; j++) {
            const int Id2 = (int)IdV2[j];
            // check if we are past the id window
            if (Id2 > Id1) { break; }
            // get similarity
            const double Sim = TLinAlg::DotProduct(X1, i, X2, j);
            SimIdV.Add(TFltIntKd(Sim, Id2));
        }
        // get top-MaxN elements
        SimIdV.Sort(false); SimIdV.Trunc(MaxN);
        // save top-MaxN elements to output file
        SimH.AddDat(Id1).AddDat(LangId, SimIdV);
        // it's clean-up time everybody!
        SimIdV.Clr();
    }
}

//{
//	"uri": " ... ",
//	"similar_eng": [{ "uri": "", "wgt": 0.6}, { ... }, ...],
//	"similar_deu": [{ "uri": "", "wgt": 0.6}, { ... }, ...],
//	"similar_spa": [{ "uri": "", "wgt": 0.6}, { ... }, ...],
//}

TStr& GetUri(const int& Id) {
    IAssert(IdUriH.IsKey(Id));
    return IdUriH.GetDat(Id);
}

void Save(const TStr& OutFNm) {
    TFOut FOut(OutFNm);
    // go over all the documents
    int SimKeyId = SimH.FFirstKeyId();
    while (SimH.FNextKeyId(SimKeyId)) {
        const int Id = SimH.GetKey(SimKeyId);
        // initialize output json
        PJsonVal DocVal = TJsonVal::NewObj();
        DocVal->AddToObj("uri", GetUri(Id));
        // go over language similarities
        const THash<TInt, TFltIntKdV>& DocH = SimH[SimKeyId];
        int DocKeyId = DocH.FFirstKeyId();
        while (DocH.FNextKeyId(DocKeyId)) {
            PJsonVal LangVal = TJsonVal::NewArr();
            for (const TFltIntKd& SimId : DocH[DocKeyId]) {
                PJsonVal SimVal = TJsonVal::NewObj();
                SimVal->AddToObj("uri", GetUri(SimId.Dat));
                SimVal->AddToObj("wgt", SimId.Key);
                LangVal->AddToArr(SimVal);
            }
            const int LangId = DocH.GetKey(DocKeyId);
            DocVal->AddToObj("similar_" + LangV[LangId], LangVal);
        }
        // get string and save
        FOut.PutStrLn(DocVal->SaveStr());
    }
}

int main(int argc, char* argv[]) {
	try {
		// create app environment
		Env = TEnv(argc, argv, TNotify::StdNotify); Env.SetNoLine();
		// command line parameters
		Env.PrepArgs("CCA similarity calc", 0);
        const TStr InFNm = Env.GetIfArgPrefixStr("-in=", "glib_cca.dat", "Input matrices");
        const TStr OutFNm = Env.GetIfArgPrefixStr("-out=", "sim_cca.json", "Output Json");
        const TStr IdMapFNm = Env.GetIfArgPrefixStr("-map=", "id_uri.json", "Id-URI map");
        const int MaxIdDiff = Env.GetIfArgPrefixInt("-diff=", 300000, "Max id diff");
        const int MaxN = Env.GetIfArgPrefixInt("-top=", 10, "Max id diff");
        if (Env.IsEndOfRun()){return 0;}
        
        TFIn FIn(InFNm);
        TFltVV EnVecVV(FIn); TFltVV EnIdVV(FIn);
        printf("En: %7d x %7d\n", EnVecVV.GetRows(), EnVecVV.GetCols());
        TFltVV EsVecVV(FIn); TFltVV EsIdVV(FIn);
        printf("Es: %7d x %7d\n", EsVecVV.GetRows(), EsVecVV.GetCols());
        TFltVV DeVecVV(FIn); TFltVV DeIdVV(FIn);
        printf("De: %7d x %7d\n", DeVecVV.GetRows(), DeVecVV.GetCols());       
        
        TFltV EnIdV; EnIdVV.GetCol(1, EnIdV); 
        TFltV EsIdV; EsIdVV.GetCol(1, EsIdV);
        TFltV DeIdV; DeIdVV.GetCol(1, DeIdV);
        printf("En: %d, Es: %d, De: %d\n", EnIdV.Len(), EsIdV.Len(), DeIdV.Len());
        
        // init languages
        LangV.Add("eng"); LangV.Add("spa"); LangV.Add("deu");
        
        // load map
        printf("Loading map ... ");
        TFIn AFIn(IdMapFNm); TStr LnStr;
        while (AFIn.GetNextLn(LnStr)) {
            PJsonVal Val = TJsonVal::GetValFromStr(LnStr);
            const int Id = Val->GetObjInt("id");
            const TStr URI = Val->GetObjStr("uri");
            IdUriH.AddDat(Id, URI);
        }
        printf("Done\n");
        
        // mono-lingual
        printf("En - En\n"); CalcSim(EnVecVV, EnIdV, EnVecVV, EnIdV, 0, MaxIdDiff, MaxN);
        printf("Es - Es\n"); CalcSim(EsVecVV, EsIdV, EsVecVV, EsIdV, 1, MaxIdDiff, MaxN);
        printf("De - De\n"); CalcSim(DeVecVV, DeIdV, DeVecVV, DeIdV, 2, MaxIdDiff, MaxN);
        // cross-lingual
        printf("En - Es\n"); CalcSim(EnVecVV, EnIdV, EsVecVV, EsIdV, 1, MaxIdDiff, MaxN);
        printf("En - De\n"); CalcSim(EnVecVV, EnIdV, DeVecVV, DeIdV, 2, MaxIdDiff, MaxN);
        printf("Es - En\n"); CalcSim(EsVecVV, EsIdV, EnVecVV, EnIdV, 0, MaxIdDiff, MaxN);
        printf("Es - De\n"); CalcSim(EsVecVV, EsIdV, DeVecVV, DeIdV, 2, MaxIdDiff, MaxN);
        printf("De - En\n"); CalcSim(DeVecVV, DeIdV, EnVecVV, EnIdV, 0, MaxIdDiff, MaxN);
        printf("De - Es\n"); CalcSim(DeVecVV, DeIdV, EsVecVV, EsIdV, 1, MaxIdDiff, MaxN);
        
        Save(OutFNm);
        
	} catch (const PExcept& Except) {
		// GLib exception
		TNotify::StdNotify->OnStatus("Error: " + Except->GetMsgStr());
		return 2;
	} catch (...) {
		// other exceptions
		return 1;
	}
	return 0;
}
