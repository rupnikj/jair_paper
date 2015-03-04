#include <base.h>
#include <mine.h>

void MakeVec(const TStr& Text, TUnicodeVSM::TGlibUBow& Bow, TFOut& FOut, const int& Count) {
    TUStr UText(Text);
    TIntKdV Vec = Bow.TextToVec(UText);
    for (int VecN = 0; VecN < Vec.Len(); VecN++) {
        FOut.PutStrFmtLn("%d %d %d", Count, Vec[VecN].Key.Val + 1, Vec[VecN].Dat.Val);
    }
}

int main(int argc, char* argv[]) {
	TUnicodeDef::Load("./UnicodeDef.Bin");
	
	TStr FPath = "/home/blazf/Projects/datasets/cca_exp/";
	
//	{
//		// sort articles
//		TIntStrKdV IdTextV;
//		TFIn AFIn(FPath + "Articles.json");
//		TStr LnStr; int Count = 0;
//		while (AFIn.GetNextLn(LnStr)) {
//			if (Count % 1000 == 0) { printf("R%d\r", Count); } Count++;
//			PJsonVal Val = TJsonVal::GetValFromStr(LnStr);
//			IdTextV.Add(TIntStrKd(Val->GetObjInt("id"), LnStr));
//		}
//		IdTextV.Sort();
//		
//		TFOut AFOut(FPath + "Articles.sorted.json");
//		for (int i = 0; i < IdTextV.Len(); i++) {
//			if (Count % 1000 == 0) { printf("W%d\r", Count); } Count++;
//			AFOut.PutStrLn(IdTextV[i].Dat);
//		}
//	}

	{
		TFIn AFIn(FPath + "Articles.sorted.json");
		TFOut AFOut(FPath + "id_uri.json");
		TStr LnStr; int Count = 0;
		while (AFIn.GetNextLn(LnStr)) {
			if (Count % 1000 == 0) { printf("R%d\r", Count); } Count++;
			PJsonVal AVal = TJsonVal::GetValFromStr(LnStr);
			PJsonVal Val = TJsonVal::NewObj();
			Val->AddToObj("id", AVal->GetObjInt("id"));
			Val->AddToObj("uri", AVal->GetObjStr("uri"));
			AFOut.PutStrLn(Val->SaveStr());
		}
		
	}
	
	try {
		printf("Loading UBows\n");
		TUnicodeVSM::TGlibUBow EnBow, EsBow, DeBow;
		TFIn EnFIn(FPath + "en.ubow"); EnBow.LoadBin(EnFIn);
		TFIn EsFIn(FPath + "es.ubow"); EsBow.LoadBin(EsFIn);
		TFIn DeFIn(FPath + "de.ubow"); DeBow.LoadBin(DeFIn);
        
        printf("Loading articles\n");
        TFIn AFIn(FPath + "Articles.sorted.json");
        TFOut EnMatFOut(FPath + "en.spmat"), EnIdFOut(FPath + "en.idmat");
        TFOut EsMatFOut(FPath + "es.spmat"), EsIdFOut(FPath + "es.idmat");
        TFOut DeMatFOut(FPath + "de.spmat"), DeIdFOut(FPath + "de.idmat");
		TStr LnStr; int Count = 0, CountEn = 1, CountEs = 1, CountDe = 1;
		while (AFIn.GetNextLn(LnStr)) {
			if (Count % 1000 == 0) {
                printf("%d %d %d %d\n", Count, CountEn, CountEs, CountDe);
            }
            Count++;
            // parse document
			PJsonVal Val = TJsonVal::GetValFromStr(LnStr);
            const int Id = Val->GetObjInt("id");
            const TStr Lang = Val->GetObjStr("lang");
            const TStr Text = Val->GetObjStr("title") + "\n" + Val->GetObjStr("body");
            // create sparse vector
            if (Lang == "eng") {
                EnIdFOut.PutStrFmtLn("%d %d", CountEn, Id);
                MakeVec(Text, EnBow, EnMatFOut, CountEn);
                CountEn++;
            } else if (Lang == "spa") {
                EsIdFOut.PutStrFmtLn("%d %d", CountEs, Id);
                MakeVec(Text, EsBow, EsMatFOut, CountEs);
                CountEs++;
            } else if (Lang == "deu") {
                DeIdFOut.PutStrFmtLn("%d %d", CountDe, Id);
                MakeVec(Text, DeBow, DeMatFOut, CountDe);
                CountDe++;
            } else {
                printf("Unkown language %s\n", Lang.CStr());
            }
		}
        
	} catch (PExcept Except) {
		printf("Error: %s\n", Except->GetMsgStr().CStr());
	}
	
	printf("Done\n");
	return 0;
}
