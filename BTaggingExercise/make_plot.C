#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <cmath>

#include "TMath.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLegend.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2D.h"
#include "TString.h"
#include "TRandom3.h"
#include "TLine.h"
#include "TPaveText.h"
#include "THStack.h"
#include "TROOT.h"
#include "TApplication.h"
#include "TStyle.h"
#include "TDirectory.h"
#include "TGraphAsymmErrors.h"
#include "TGaxis.h"
#include "TPad.h"

int make_plot(std::string Var, int tune){

	gStyle->SetOptStat(0);

	std::vector<std::string> channel;
	channel.push_back("ZZ");
	channel.push_back("WZ");
	channel.push_back("WW");
	channel.push_back("DYLowM");
	channel.push_back("DY");
	channel.push_back("WJets");
	channel.push_back("ST_tW_top");
	channel.push_back("ST_tW_antitop");
	channel.push_back("TTSemi");
	channel.push_back("ttToDiLep_small");
	channel.push_back("MuEG2017D");

	std::vector<std::string> var;
	var.push_back("nJets");
	var.push_back("nTags");
	var.push_back("allJetsPt");
	var.push_back("allJetsDeepFlavB");
	var.push_back("firstJetDeepFlavB");
	var.push_back("secondJetDeepFlavB");
	

	std::string Channel; 
	bool isMC;
	
	std::map<std::string, TFile*> fHisto;
	
	std::map<std::string, TH1D*> h_;
	
	std::map<std::string, TH1D*> AllMC_;
	std::map<std::string, THStack*> hStack_;
	
	TGraphAsymmErrors* gr;

	bool isnoSF=(tune==0);
	bool isReShape=(tune==2);
	
	TString plotDir="plots/";
	

	std::string inputDirPath="Output/";
	std::string HistoName; 
	TString fileName;

	for(UInt_t zz=0; zz<var.size(); zz++){

		if(Var!=var.at(zz)) continue;
		
		hStack_[Var]=new THStack(TString(Var),TString(Var));
		

		for(UInt_t ll=0; ll<channel.size(); ll++){
		
			Channel = channel.at(ll);
			fileName = TString(inputDirPath)+"Out_"+TString(Channel)+".root";

			isMC=(Channel!="MuEG2017D");
			
			fHisto[Channel] = new TFile(fileName,"Read");		
			gROOT->cd();
			
			HistoName=Channel+"/"+Var+"_"+Channel;
			
			if(isReShape && isMC) HistoName=Channel+"/"+Var+"_ReShape_"+Channel;
			if(isnoSF && isMC) HistoName=Channel+"/"+Var+"_noSF_"+Channel;
			
			h_[Channel] = (TH1D*)fHisto[Channel]->Get(TString(HistoName));

			if(Channel=="WZ" || Channel=="WW") h_["ZZ"]->Add(h_[Channel]);	
			if(Channel=="DY") h_["DYLowM"]->Add(h_[Channel]);
			if(Channel=="ST_tW_antitop")  h_["ST_tW_top"]->Add(h_[Channel]);
			if(Channel=="ttToDiLep_small") h_["TTSemi"]->Add(h_[Channel]); 

		}
		
		h_["ZZ"]->SetFillColor(kViolet); h_["ZZ"]->SetLineColor(kBlack); hStack_[Var]->Add(h_["ZZ"]);  AllMC_[Var]=(TH1D*)h_["ZZ"]->Clone();
		h_["DYLowM"]->SetFillColor(kBlue); h_["DYLowM"]->SetLineColor(kBlack);  hStack_[Var]->Add(h_["DYLowM"]); AllMC_[Var]->Add(h_["DYLowM"]);
		h_["WJets"]->SetFillColor(kGreen-2); h_["WJets"]->SetLineColor(kBlack); hStack_[Var]->Add(h_["WJets"]); AllMC_[Var]->Add(h_["WJets"]);
		h_["ST_tW_top"]->SetFillColor(kYellow); h_["ST_tW_top"]->SetLineColor(kBlack); hStack_[Var]->Add(h_["ST_tW_top"]); AllMC_[Var]->Add(h_["ST_tW_top"]);
		h_["TTSemi"]->SetFillColor(kOrange-1); h_["TTSemi"]->SetLineColor(kBlack); hStack_[Var]->Add(h_["TTSemi"]); AllMC_[Var]->Add(h_["TTSemi"]);
	}
	
	AllMC_[Var]->SetFillColor(kGray+2);
	AllMC_[Var]->SetLineColor(kBlack);
	AllMC_[Var]->SetFillStyle(3004);

	h_["MuEG2017D"]->SetMarkerStyle(20); h_["MuEG2017D"]->SetLineColor(kBlack); h_["MuEG2017D"]->SetLineWidth(2);
	

	TCanvas* c=new TCanvas("Can","Can"); 
	TLegend* leg=new TLegend(0.6,0.8,0.75,0.95); leg->SetFillColor(kWhite);
	leg->AddEntry(h_["MuEG2017D"],"Data","ple1");
	leg->AddEntry(h_["TTSemi"],"t#bar{t}","f"); 
	leg->AddEntry(h_["ST_tW_top"],"#it{tW} ","f");		 
	leg->AddEntry(h_["WJets"],"W + jets","f");
	leg->AddEntry(h_["DYLowM"],"Z/#gamma^{*} + jets","f");
	leg->AddEntry(h_["ZZ"],"VV","f");
	leg->AddEntry(AllMC_[Var],"Total Unc.","f");
	
	c->cd(1); 
	gPad->SetTicky(); gPad->SetTickx();	
	TGaxis::SetMaxDigits(3);
	hStack_[Var]->Draw("HIST");
	AllMC_[Var]->Draw("E2 SAME");
	h_["MuEG2017D"]->Draw("PE1SAME"); 
	leg->Draw("SAME"); 
	
	c->SaveAs(plotDir+TString(Var)+".png");
	
	return 1;
}			
