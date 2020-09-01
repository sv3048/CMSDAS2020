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
#include "TLatex.h"

int make_plot(std::string Var, int tune){

	gStyle->SetOptStat(0);
	TH1::SetDefaultSumw2();

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
	
	bool isnoSF=(tune==0);
	bool isReShape=(tune==2);
	bool isFixed=(tune==1);

	TString plotDir="plots/";
	if(isnoSF) plotDir+="noSF/";
	if(isReShape) plotDir+="ReShape/";
	if(isFixed) plotDir+="fixedWP/";

	std::string inputDirPath="Output/";
	std::string HistoName; 
	TString fileName;

	for(UInt_t zz=0; zz<var.size(); zz++){

		if(Var!=var.at(zz)) continue;
		
		hStack_[Var]=new THStack(TString(Var),"");
		
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

	TH1D* gr=(TH1D*)h_["MuEG2017D"]->Clone();	
	gr->Divide(AllMC_[Var]);

	AllMC_[Var]->SetFillColor(kGray+2);
	AllMC_[Var]->SetLineColor(kBlack);
	AllMC_[Var]->SetFillStyle(3004);

	h_["MuEG2017D"]->SetMarkerStyle(20); h_["MuEG2017D"]->SetLineColor(kBlack); h_["MuEG2017D"]->SetLineWidth(2);
	hStack_[Var]->SetMaximum(1.5* TMath::Max(AllMC_[Var]->GetMaximum(), h_["MuEG2017D"]->GetMaximum()));	
	

	gr->SetMarkerStyle(20); gr->SetLineColor(kBlack); gr->SetLineWidth(2);

	gr->GetXaxis()->SetRangeUser(AllMC_[Var]->GetXaxis()->GetXmin(), AllMC_[Var]->GetXaxis()->GetXmax()); 
	gr->GetYaxis()->SetRangeUser(0.4,1.6); 

	gr->GetYaxis()->SetTitle("Data/MC"); gr->GetYaxis()->SetTitleSize(0.09); gr->GetYaxis()->SetTitleOffset(.55); gr->GetYaxis()->CenterTitle(1);
	gr->GetYaxis()->SetNdivisions(50000+204); gr->GetYaxis()->SetLabelSize(0.1); 

	gr->GetXaxis()->SetLabelSize(0.1);
	gr->GetXaxis()->SetLabelSize(0.1);
	gr->GetXaxis()->SetTitleOffset(1.1);
	gr->GetXaxis()->SetLabelSize(0.12);
	gr->GetXaxis()->SetLabelOffset(0.04);
	gr->GetXaxis()->SetTitleSize(0.12);

	if(Var=="nJets") gr->GetXaxis()->SetTitle("jet multiplicity"); //make y label bigger 
    if(Var=="nJets") gr->GetXaxis()->SetTitle("b-tag multiplicity");
    if(Var=="allJetsPt") gr->GetXaxis()->SetTitle("jet p_{T} [GeV]");
    if(Var=="allJetsDeepFlavB") gr->GetXaxis()->SetTitle("Deepjet score (all jets)");
    if(Var=="firstJetDeepFlavB") gr->GetXaxis()->SetTitle("Deepjet score (leading jet)");
    if(Var=="secondJetDeepFlavB") gr->GetXaxis()->SetTitle("Deepjet score (2nd leading jet)");

	TLine* myLine;
	myLine = new TLine(AllMC_[Var]->GetXaxis()->GetXmin(), 1, AllMC_[Var]->GetXaxis()->GetXmax(), 1);
	
	TString lumiinfo = "4.2 fb^{-1}(13TeV, 2017)"; 
	TLatex LumiInfoLatex(0.70, 0.91, lumiinfo);
	LumiInfoLatex.SetNDC(); LumiInfoLatex.SetTextFont(42);
	LumiInfoLatex.SetTextSize(0.04);
	
	TString cmsinfo =   "CMS";
	TLatex CMSInfoLatex(0.155, 0.91, cmsinfo);
	CMSInfoLatex.SetNDC(); CMSInfoLatex.SetTextFont(42);
	CMSInfoLatex.SetTextFont(61);
	CMSInfoLatex.SetTextSize(0.055); 

	TString publishinfo =   "Preliminary"; 
	TLatex PublishInfoLatex(0.25, 0.91, publishinfo); 
    PublishInfoLatex.SetNDC();
    PublishInfoLatex.SetTextFont(52);
	PublishInfoLatex.SetTextSize(0.045);

	TLegend* leg=new TLegend(0.14,0.78,0.9,0.88); 
	leg->SetFillColor(kWhite);
	leg->SetLineColor(kWhite);
	
	leg->SetTextFont(42);
	leg->SetTextSize(0.03);
	leg->SetNColumns(3);

	leg->AddEntry(h_["MuEG2017D"],"Data","ple1");
	leg->AddEntry(h_["TTSemi"],"t#bar{t}","f"); 
	leg->AddEntry(h_["ST_tW_top"],"#it{tW} ","f");		 
	leg->AddEntry(h_["WJets"],"W + jets","f");
	leg->AddEntry(h_["DYLowM"],"Z/#gamma* + jets","f");
	leg->AddEntry(h_["ZZ"],"VV","f");
	leg->AddEntry(AllMC_[Var],"MC stats.","f");

    TCanvas* c=new TCanvas("Can","Can",600,700); 
    gStyle->SetPadBorderMode(0);    
    Float_t small = 1.e-5;
    c->Divide(1,2,small,small);
    const float padding=1e-5; const float ydivide=0.3;
    c->GetPad(1)->SetPad( padding, ydivide + padding , 1-padding, 1-padding);
    c->GetPad(2)->SetPad( padding, padding, 1-padding, ydivide-padding);
    c->GetPad(1)->SetRightMargin(.05);
    c->GetPad(2)->SetRightMargin(.05);
    c->GetPad(1)->SetBottomMargin(.4);
    c->GetPad(2)->SetBottomMargin(.4);
    c->GetPad(1)->Modified();
    c->GetPad(2)->Modified();

	c->cd(1);
	gPad->SetBottomMargin(small);
	gPad->SetLogy();
	gPad->SetTicky(); gPad->SetTickx();
	gPad->Modified();
	hStack_[Var]->Draw("HIST");
	AllMC_[Var]->Draw("E2 SAME");
	h_["MuEG2017D"]->Draw("PE1SAME"); 
	leg->Draw(); 
	LumiInfoLatex.Draw();
	CMSInfoLatex.Draw();
	PublishInfoLatex.Draw();
	gPad->RedrawAxis();

	c->cd(2);
	gPad->SetTopMargin(small);
	gPad->SetTicky(); gPad->SetTickx();
	gr->Draw("PE1");
    myLine->Draw("SAME");
	
	c->RedrawAxis();

	c->SaveAs(plotDir+TString(Var)+".png");
	
	return 1;
}			
