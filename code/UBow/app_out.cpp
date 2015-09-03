#include <base.h>
#include <mine.h>
#include <algorithm>
#include <functional>
#include <iostream>

TStrV LangV;
TIntStrH IdUriH;
THash<TInt, THash<TInt, TFltIntKdV> > SimH;

void PrintVec(TFltV& Vec){
	int n = Vec.Len();
	for (int i = 0; i < n; i++){
		printf("%g\t", Vec[i].Val);
		printf("\n");
	}
}

void my_partial_sort(double* v, double* result, int *idx, const int len, const int& top = 100) {
	// initialize original index locations
	// sort indexes based on comparing values in v
	std::partial_sort(idx, idx + top, idx + len,
		[&v](size_t i1, size_t i2) {return v[i1] > v[i2]; });
	for (int i = 0; i < top; i++){
		result[i] = v[idx[i]];
	}
}

//What are the most similar articles for doc in X1 in X2 with language id LangId
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


void CalcSimRow(const TFltVV& X1, const TFltV& IdV1, const TFltVV& X2,
	const TFltV& IdV2, const int& LangId, const int& MaxIdDiff,
	const int& MaxN) {

	const int Len1 = X1.GetRows();
	const int Len2 = X2.GetRows();
	// placeholder for similarities
	TFltIntKdV SimIdV; int LastStart = 0;
	// major loop
	//for (int i = 10000; i < 10100; i++) {
	TTmStopWatch time;
	time.Start();
	for (int i = 1; i < Len1; i++) {
		if (i % 10000 == 0) {
			printf("%6d / %6d\n", i, Len1); 	time.Stop(""); time.Start();
			printf("\n");
		}
		const int Id1 = (int)IdV1[i];
		//printf("Doc %d\n", Id1);
		// move start pointer to the first element within id window
		TIntV Idx;
		for (int j = LastStart; j < Len2; j++) {
			const int Id2 = (int)IdV2[j];
			// should stop once within id window
			if (Id1 - Id2 < MaxIdDiff) { break; }
			// else move on
			LastStart++;
		}
		// compute similarities
		int Last = LastStart;
		for (int j = LastStart; j < Len2; j++) {
			const int Id2 = (int)IdV2[j];
			// check if we are past the id window
			if (Id2 >= Id1) { Last = j-1;  break; }
			Idx.Add(Id2);
			// get similarity
		}
		if (Last < LastStart) continue;
		//printf("Sim %d\n", i);
		TFltV Doc;
		const_cast<TFltVV &>(X1).GetRowPtr(i, Doc);
		//printf("Doc len %d\n", Doc.Len());
		TFltVV Window;
		const_cast<TFltVV &>(X2).GetRowsPtr(LastStart, Last, Window);
		//printf("Window %d %d\n", Window.GetRows(), Window.GetCols());
		TFltV SimVec; SimVec.Gen(Last - LastStart + 1, Last - LastStart + 1);
		TLinAlg::Multiply(Window, Doc, SimVec);
		TFltV TopSim; TopSim.Gen(MIN(MaxN, SimVec.Len()), MIN(MaxN, SimVec.Len()));
		TIntV Order;
		Order.Gen( SimVec.Len(), SimVec.Len());
		int klen = Order.Len();
		for (int k = 0; k < klen; k++){
			Order[k] = k;
		}
		my_partial_sort(&SimVec[0].Val, &TopSim[0].Val, &Order[0].Val, Order.Len(), MIN(MaxN, SimVec.Len()));

		//const double Sim = TLinAlg::DotProduct(X1, i, X2, j);
		//SimIdV.Add(TFltIntKd(Sim, Id2));
		// get top-MaxN elements
		//SimIdV.Sort(false); SimIdV.Trunc(MaxN);
		// save top-MaxN elements to output file
		SimIdV.Gen(MIN(MaxN, SimVec.Len()), MIN(MaxN, SimVec.Len()));
		/*if (i % 10000 == 0){
			printf("Window size, start: %d, end: %d, size: %d", LastStart, Last, Window.GetYDim());
			TStr& Url = IdUriH.GetDat(Id1);
			printf("Id: %d\t Url: %s\t Sim: %g\n", Id1, Url.CStr());
			//PrintVec(Doc);
			//char c;
			//std::cin >> c;

		}*/
		for (int ii = 0; ii < MIN(MaxN, SimVec.Len()); ii++){
			
			/*if (i % 10000 == 0){
				TStr& Url = IdUriH.GetDat(Idx[Order[ii]]);
				printf("Id: %d\t Url: %s\t Sim: %g\n", Idx[Order[ii]].Val, Url.CStr(), TopSim[ii].Val);
			}*/

			SimIdV[ii] = TFltIntKd(TopSim[ii], Idx[Order[ii]]);
		}
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
        const TStr InFNm = Env.GetIfArgPrefixStr("-in=", "glib_lsi_row.dat", "Input matrices");
        const TStr OutFNm = Env.GetIfArgPrefixStr("-out=", "sim_lsi.json", "Output Json");
        const TStr IdMapFNm = Env.GetIfArgPrefixStr("-map=", "id_uri.json", "Id-URI map");
        const int MaxIdDiff = Env.GetIfArgPrefixInt("-diff=", 300000, "Max id diff");
        const int MaxN = Env.GetIfArgPrefixInt("-top=", 10, "Num of top Sim");
        if (Env.IsEndOfRun()){return 0;}

		/*SimH.AddDat(1).AddDat(1, TFltIntKdV::GetV(TFltIntKd(0.0, 1)) );
		SimH.AddDat(1).AddDat(2, TFltIntKdV::GetV(TFltIntKd(0.0, 1)));
		THash<TInt, TFltIntKdV> Value;
		SimH.IsKeyGetDat(1, Value);
		printf("Velikost: %d\n", Value.Len());
		return 0;*/
        
        TFIn FIn(InFNm);
        TFltVV EnVecVV(FIn); TFltVV EnIdVV(FIn);
        printf("En: %7d x %7d\n", EnVecVV.GetRows(), EnVecVV.GetCols());
		//TFltV Vec; EnVecVV.GetRowPtr(9999, Vec);
		//printf("testing");
		//PrintVec(Vec);
        TFltVV EsVecVV(FIn); TFltVV EsIdVV(FIn);
        printf("Es: %7d x %7d\n", EsVecVV.GetRows(), EsVecVV.GetCols());
        TFltVV DeVecVV(FIn); TFltVV DeIdVV(FIn);
        printf("De: %7d x %7d\n", DeVecVV.GetRows(), DeVecVV.GetCols());       
   
        TFltV EnIdV; EnIdVV.GetRow(1, EnIdV); 
        TFltV EsIdV; EsIdVV.GetRow(1, EsIdV);
        TFltV DeIdV; DeIdVV.GetRow(1, DeIdV);
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
        printf("En - En\n"); CalcSimRow(EnVecVV, EnIdV, EnVecVV, EnIdV, 0, MaxIdDiff, MaxN);
        printf("Es - Es\n"); CalcSimRow(EsVecVV, EsIdV, EsVecVV, EsIdV, 1, MaxIdDiff, MaxN);
        printf("De - De\n"); CalcSimRow(DeVecVV, DeIdV, DeVecVV, DeIdV, 2, MaxIdDiff, MaxN);
        // cross-lingual
        printf("En - Es\n"); CalcSimRow(EnVecVV, EnIdV, EsVecVV, EsIdV, 1, MaxIdDiff, MaxN);
        printf("En - De\n"); CalcSimRow(EnVecVV, EnIdV, DeVecVV, DeIdV, 2, MaxIdDiff, MaxN);
        printf("Es - En\n"); CalcSimRow(EsVecVV, EsIdV, EnVecVV, EnIdV, 0, MaxIdDiff, MaxN);
        printf("Es - De\n"); CalcSimRow(EsVecVV, EsIdV, DeVecVV, DeIdV, 2, MaxIdDiff, MaxN);
        printf("De - En\n"); CalcSimRow(DeVecVV, DeIdV, EnVecVV, EnIdV, 0, MaxIdDiff, MaxN);
        printf("De - Es\n"); CalcSimRow(DeVecVV, DeIdV, EsVecVV, EsIdV, 1, MaxIdDiff, MaxN);
        
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
