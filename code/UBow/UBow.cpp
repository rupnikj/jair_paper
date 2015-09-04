// UBow.cpp : Defines the entry point for the console application.
//

#include "base.h"
#include "mine.h"
//#include "net.h"
//#include "stdafx.h"ž

template<class TVal, class TSizeTy>
void PrintVec(const TVec<TVal, TSizeTy>& Vector, const int& clip = 10){
	int n = MIN(Vector.Len(), clip);
	TFlt test;
	for (int i = 0; i < n; i++){
		printf("%s, ", Vector[i].GetStr().CStr());
	}
	printf("%s\n", Vector[n - 1].GetStr().CStr());
}

template<class TVal, class TSizeTy>
void PrintVVec(const TVVec<TVal, TSizeTy>& Matrix, const int& clip = 10){
	int m = TMath::Min(Matrix.GetRows(), clip);
	int n = Matrix.GetCols();
	for (int i = 0; i < m; i++){
		for (int j = 0; j < n; j++){
			printf("%s, ", Matrix(i, j).GetStr());
		}
		printf("%s\n", Matrix(i, n).GetStr());
	}

}

//Linux style paths!!!
void ConvertOldToNew(TStrV& FileStr, TStr& OutDirFull, TStr& OutDirCompact, const TBool& old = true){
	int n = FileStr.Len();
	for (int i = 0; i < n; i++){
		TTmStopWatch time;
		TStr FileName = FileStr[i];
		TFIn FileOld(FileName);
		TStr Name, Dir; FileName.SplitOnLastCh(Dir, '/', Name);
		printf("Processing Bow %s\n", FileName.CStr());
		time.Start();
		TUnicodeVSM::TGlibUBow Ubow;
		if (old){
			printf("Testing Old format!\n");
			Ubow.LoadOldBin(FileOld);
		}
		else{
			printf("Testing New format!\n");
			Ubow.LoadBin(FileOld);
		}
		time.Stop("Loading takes: ");
		Ubow.CompactMatrix(TIntV::GetV(0, 1, 2));
		time.Reset(false);
		time.Start();
		Ubow.ComputeDocFreq();
		time.Stop("InvDoc takes: ");
		TFOut FileNewFull(OutDirFull + Name);
		printf("Saving New Bow %s\n", (OutDirFull + Name).CStr());
		time.Reset(false);
		time.Start();
		Ubow.SaveBin(FileNewFull);
		time.Stop("Saving NewBow takes: ");
		Ubow.DelMatrix();
		TFOut FileNewCompact(OutDirCompact + Name);
		time.Reset(false);
		time.Start();
		printf("Saving Truncated Bow %s\n", (OutDirCompact + Name).CStr());
		Ubow.SaveBin(FileNewCompact);
		time.Stop("Saving ComNewBow takes: ");
		printf("Ndocs is %d\n", Ubow.GetNumberOfDocs());
		const TIntV& InvDoc = Ubow.GetInvDoc();
		printf("InvDoc len is %d\n", InvDoc.Len());
	}
	//	PrintVec(InvDoc);
	//	TFltV inv;
	//	TFIn test("D:/invdoc1.bin");
	//	inv.Load(test);
	//	PrintVec(inv);
	//	double N = (double)(Ubow.GetNumberOfDocs());
	//	for (int i = 0; i < 10; i++){
	//		TFlt weight = TMath::Log(N / (InvDoc[i]+1));
	//		p*/rintf("%g\t", weight.Val);
	//	}
	//}
}

void Test(){
	TStr UnicodeDefFile = "D:/Code/qminer/src/textgarden/UBow/x64/Debug/UnicodeDef.Bin";
	TUnicodeDef::Load(UnicodeDefFile);
	TFIn File("D:/Code/qminer/src/textgarden/UBow/x64/Debug/en.ubow");
	TUnicodeVSM::TGlibUBow EnUbow;
	TFIn FileTest("D:/enBow.Bin");
	EnUbow.LoadOldBin(FileTest);
	printf(" Words Len: %d\n %d\n", EnUbow.GetNumberOfWords(), EnUbow.IsStemmerSupported().Val);
	TFOut FileOut("D:/new/enBow.Bin");
	EnUbow.SaveBin(FileOut);
	EnUbow.DelMatrix();

	EnUbow.LoadBin(File);
	TPair<TIntV, TFltV> SparseVec;
	TUStr Ustr1 = TUStr("test the cars car");
	TUStr Ustr2 = TUStr("great success");
	TUStrV Docs; Docs.Add(Ustr1); Docs.Add(Ustr2);
	EnUbow.TextToVec(TUStr("test the cars"), SparseVec);
	int n_words = EnUbow.GetNumberOfWords();
	for (int i = 0; i < SparseVec.Val1.Len(); i++){
		int widx = SparseVec.Val1[i];
		int freq = SparseVec.Val2[i];
		printf("Beseda: %s\t%d\t%d\n", EnUbow.GetWord(widx).GetStr().CStr(), widx, freq);
	}

	TTriple<TIntV, TIntV, TFltV> DocMatrix;
	TFltV Inv;
	TFIn FileV("D:/Code/qminer/src/textgarden/UBow/x64/Debug/invdoc1.bin");
	Inv.Load(FileV);
	TFIn FileV1("D:/Code/qminer/src/textgarden/UBow/x64/Debug/invdoc11.bin");
	TFltV Inv1; Inv1.Load(FileV1);
	if (Inv == Inv1){
		printf("Inverse indexing OK!\n");
	}
	else{
		printf("Inverse indexing fail!\n");
	}
	printf("InvDoc Len: %d, Words Len: %d\n", Inv.Len(), EnUbow.GetNumberOfWords());
	EnUbow.TextToVec(Docs, DocMatrix, Inv);

	TIntV& WordIdxV = DocMatrix.Val1;
	TIntV& DocIdxV = DocMatrix.Val2;
	TFltV& ValV = DocMatrix.Val3;

	int nval = WordIdxV.Len();
	for (int i = 0; i < nval; i++){
		printf("Doc %d Beseda %d %s Utez %g\n", DocIdxV[i].Val, WordIdxV[i].Val, EnUbow.GetWord(WordIdxV[i].Val).GetStr().CStr(), ValV[i].Val);
	}

}

int main(int argc, char* argv[])
{
	Env = TEnv(argc, argv, TNotify::StdNotify);
	Env.PrepArgs("UbowConverter", 0);
	TStr Mode = Env.GetIfArgPrefixStr("-mode:", "catserver", "Mode: catserver, catsimserver");
	TStr ConfigFile = Env.GetIfArgPrefixStr("-configfile:", "./config.txt", "ConfigFile");
	TStr UnicodeDefFile = Env.GetIfArgPrefixStr("-unicodefile:", "./UnicodeDef.Bin", "UnicodeDef.Bin file location");
	TStr ChineseDefDirectory = Env.GetIfArgPrefixStr("-chinese:", "./StemmingTokenization", "Chinese proprietary directory location");
	TStr OutDirFull = Env.GetIfArgPrefixStr("-fulldir:", "./full/", "Full Matrix Dir");
	TStr OutDirCompact = Env.GetIfArgPrefixStr("-compactdir:", "./compact/", "Compact Dir");
	TStr Old = Env.GetIfArgPrefixStr("-old:", "true", "Compact Dir");

	TUnicodeDef::Load(UnicodeDefFile);
	if (!ChineseTokenizer::init(ChineseDefDirectory)){
		printf("Chinese tokenizer is not supported, beware");
		return 0;
	}
	else{
		TStr Tokenized;
		ChineseTokenizer::tokenize("This is a test", Tokenized);
		printf("%s", Tokenized.CStr());
	}

	TStrV Files;
	TStr Text = TStr::LoadTxt(ConfigFile);
	Text.SplitOnAllAnyCh("\n\r\t", Files);
	ConvertOldToNew(Files, OutDirFull, OutDirCompact, Old == "true");

	return 0;
}

 