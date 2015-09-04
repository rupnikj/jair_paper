#include <base.h>
#include <mine.h>


void PrintVec(TPair<TIntV, TFltV>& SparseVec, TUnicodeVSM::TGlibUBow& Ubow){
    Ubow.TextToVec(TUStr("test the cars"), SparseVec);
    int n_words = Ubow.GetNumberOfWords();
for (int i = 0; i < SparseVec.Val1.Len(); i++){
	int widx = SparseVec.Val1[i];
	int freq = SparseVec.Val2[i];
	printf("Beseda: %s\t%d\t%d\n", Ubow.GetWord(widx).GetStr().CStr(), widx, freq);
}
}


void PrintVec(TIntKdV& SparseVec, TUnicodeVSM::TGlibUBow& Ubow){
	int n_words = Ubow.GetNumberOfWords();
	for (int i = 0; i < SparseVec.Len(); i++){
		int widx = SparseVec[i].Key;
		int freq = SparseVec[i].Dat;
		printf("Beseda: %s\t%d\t%d\n", Ubow.GetWord(widx).GetStr().CStr(), widx, freq);
	}
}

void MakeVirtual(TUnicodeVSM::TGlibUBow& Bow, TFOut& FOut){
	FOut.PutStrFmtLn("%d %d %d", Bow.GetNumberOfTokens(), 1, 0);
}
void MakeVec(TStr& Text, TUnicodeVSM::TGlibUBow& Bow, TFOut& FOut, TFOut& FOutText, const int& Count) {
    TUStr UText(Text);
	Text.ChangeChAll('\n', '\t');
	FOutText.PutStrFmtLn("%s", Text.CStr());
    TIntKdV Vec = Bow.TextToVec(UText);
    for (int VecN = 0; VecN < Vec.Len(); VecN++) {
		FOut.PutStrFmtLn("%d %d %d", Vec[VecN].Key.Val + 1, Count, Vec[VecN].Dat.Val);
    }
	/*printf("Doc: %s\n", Text.CStr());
	PrintVec(Vec, Bow);
	char c;
	scanf("%c", &c);
	*/
}

int main(int argc, char* argv[]) {
	TUnicodeDef::Load("./UnicodeDef.Bin");
	
	TStr FPath = "D:/Code/qminer/src/textgarden/UBow/";
	
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
		TFIn EnFIn(FPath + "enBow.bin"); EnBow.LoadBin(EnFIn);
		TIntV EnIndV; EnIndV.Load(TFIn("en.idx"));
		printf("EnBow Words %d\n", EnBow.GetNumberOfWords());
		EnBow.CompactVocabulary(EnIndV);


		TFIn EsFIn(FPath + "esBow.bin"); EsBow.LoadBin(EsFIn);
		TIntV EsIndV; EsIndV.Load(TFIn("es.idx"));
		printf("EsBow Words %d\n", EsBow.GetNumberOfWords());
		EsBow.CompactVocabulary(EsIndV);

		TFIn DeFIn(FPath + "deBow.bin"); DeBow.LoadBin(DeFIn);
		TIntV DeIndV; DeIndV.Load(TFIn("de.idx"));
		DeBow.CompactVocabulary(DeIndV);
		printf("DeBow Words %d\n", DeBow.GetNumberOfWords());
	
        printf("Loading articles\n");
        TFIn AFIn(FPath + "Articles.sorted.json");
		TFOut EnMatFOut(FPath + "en.spmat"), EnTextFOut(FPath + "en.txt"), EnIdFOut(FPath + "en.idmat");
		TFOut EsMatFOut(FPath + "es.spmat"), EsTextFOut(FPath + "es.txt"), EsIdFOut(FPath + "es.idmat");
		TFOut DeMatFOut(FPath + "de.spmat"), DeTextFOut(FPath + "de.txt"), DeIdFOut(FPath + "de.idmat");
		TStr LnStr; int Count = 0, CountEn = 1, CountEs = 1, CountDe = 1;
		MakeVirtual(EnBow, EnMatFOut);
		MakeVirtual(EsBow, EsMatFOut);
		MakeVirtual(DeBow, DeMatFOut);
		while (AFIn.GetNextLn(LnStr)) {
			if (Count % 1000 == 0) {
                printf("%d %d %d %d\n", Count, CountEn, CountEs, CountDe);
            }
            Count++;
            // parse document
			PJsonVal Val = TJsonVal::GetValFromStr(LnStr);
            const int Id = Val->GetObjInt("id");
            const TStr Lang = Val->GetObjStr("lang");
            TStr Text = Val->GetObjStr("title") + "\n" + Val->GetObjStr("body");
            // create sparse vector
            if (Lang == "eng") {
                EnIdFOut.PutStrFmtLn("%d %d", CountEn, Id);
				MakeVec(Text, EnBow, EnMatFOut, EnTextFOut, CountEn);
                CountEn++;
            } else if (Lang == "spa") {
                EsIdFOut.PutStrFmtLn("%d %d", CountEs, Id);
				MakeVec(Text, EsBow, EsMatFOut, EsTextFOut, CountEs);
                CountEs++;
            } else if (Lang == "deu") {
                DeIdFOut.PutStrFmtLn("%d %d", CountDe, Id);
				MakeVec(Text, DeBow, DeMatFOut, DeTextFOut, CountDe);
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
