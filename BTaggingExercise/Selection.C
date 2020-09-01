#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <cmath>
#include <complex>
#include <map>

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLegend.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TString.h"
#include "TRandom3.h"
#include "TLine.h"
#include "TPaveText.h"
#include "THStack.h"
#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TDirectory.h"
#include "TStyle.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TVector2.h"
#include "TStopwatch.h"


//================= Include BTagging Weight Calculator class========

#include "BTagWeight.cc"


//========================================

#ifdef __MAKECINT__
#pragma link C++ class std::vector< TLorentzVector >+; 
#endif

std::string indexToSample(int id){

	std::string sample;
	if(id==-1) sample = "MuEG2017D";
	if(id==1) sample ="ttToDiLep_small";
	if(id==2) sample ="TTSemi";
	if(id==3) sample ="ST_tW_top";
	if(id==4) sample ="ST_tW_antitop";
	if(id==5) sample ="DYLowM";
	if(id==6) sample ="DY";
	if(id==7) sample ="WJets";
	if(id==8) sample ="WW";
	if(id==9) sample ="WZ";
	if(id==10) sample ="ZZ";
	
	return sample;

}	
double getXsec(std::string sample){
	if(sample=="TTSemi") return 365.34;
	else if(sample=="ttToDiLep_small") return 88.29;
	else if(sample=="ST_tW_top") return 34.97;
	else if(sample=="ST_tW_antitop") return 34.91;
	else if(sample=="DYLowM") return 18610.0;
	else if(sample=="DY") return 6225.42;
	else if(sample=="WJets") return 61526.7;
	else if(sample=="WW") return 75.8;
	else if(sample=="WZ") return 27.6;
	else if(sample=="ZZ") return 12.14;
	else return 1.0;
}

void Selection(int index){

	
	gStyle->SetOptStat(0);
	
	TH1::SetDefaultSumw2();
//	TH1::StatOverflows();	

	std::string Sample= indexToSample(index);

	std::vector<std::string> channel;
	channel.push_back("TTSemi");
	channel.push_back("ttToDiLep_small");
	channel.push_back("ST_tW_top");
	channel.push_back("ST_tW_antitop");
	channel.push_back("DYLowM");
	channel.push_back("DY");
	channel.push_back("WJets");
	channel.push_back("WW");
	channel.push_back("WZ");
	channel.push_back("ZZ");
	channel.push_back("MuEG2017D");

	std::map<std::string, double> Xsec_;
	std::map<std::string, double> nGen_;
	std::map<std::string, double> nEntries_;
	
	
	std::map<std::string, TFile*> fIn;
	std::map<std::string, TTree*> tr;

	std::map<std::string, TH1D*> hnumJets_;
	std::map<std::string, TH1D*> hnumJets_ReShape;
	std::map<std::string, TH1D*> hnumJets_noSF;

	std::map<std::string, TH1D*> hnumTags_;
	std::map<std::string, TH1D*> hnumTags_ReShape;
	std::map<std::string, TH1D*> hnumTags_noSF;

	std::map<std::string, TH1D*> h_allJetsPt_;
	std::map<std::string, TH1D*> h_allJetsPt_ReShape;
	std::map<std::string, TH1D*> h_allJetsPt_noSF;

	std::map<std::string, TH1D*> h_allJetsDeepFlavB_;
	std::map<std::string, TH1D*> h_allJetsDeepFlavB_ReShape;
	std::map<std::string, TH1D*> h_allJetsDeepFlavB_noSF;

	std::map<std::string, TH1D*> h_firstJetDeepFlavB_;
	std::map<std::string, TH1D*> h_firstJetDeepFlavB_ReShape;
	std::map<std::string, TH1D*> h_firstJetDeepFlavB_noSF;

	std::map<std::string, TH1D*> h_secondJetDeepFlavB_;
	std::map<std::string, TH1D*> h_secondJetDeepFlavB_ReShape;
	std::map<std::string, TH1D*> h_secondJetDeepFlavB_noSF;

	std::map<std::string, TDirectory*> dir;		
	
	std::string inputDirPath="/storage/gridka-nrg/smitra/CMSDAS2020/";
	std::string outDirPath="~/workspace/CMSDAS2020/CMSSW_10_2_18/src/CMSDAS2020/BTaggingExercise/Output/";
	std::string fileName;
	std::string Channel;
	
	double Lumi = 4200; // Luminosity in 1/pb

	double Mwp = 0.2770; //Medium WP

	TStopwatch t;
	
	
/// Dummy variables to read TTree branch
	ULong64_t event;
	UInt_t run, lumiBlock;

	UInt_t nJet; 
	Float_t MET_Pt, puWeight, genWeight; 
	Float_t Jet_Pt[999], Jet_eta[999], Jet_btagDeepFlavB[999];
	Int_t Jet_jetId[999], Jet_puId[999], Jet_hadronFlavour[999], Jet_partonFlavour[999]; 
///=========================================

///=============== Output variables to store =============
	std::vector<UInt_t> jetIdx;
	std::vector<BTagWeight::JetInfo> jetinfo;
	
	int numJets, nTags;
	bool isMC;
	double evtWgt, bWeight, bWeight_ReShape;
	
	TFile* fOut= new TFile(TString(outDirPath)+"Out_"+TString(Sample)+".root","RECREATE");

	BTagWeight j;

	gROOT->cd();
	
	gROOT->SetBatch(kTRUE);
	
	for(UInt_t ll=0; ll<channel.size(); ll++){
		
		Channel = channel.at(ll);
		if(Channel!=Sample) continue;
		
		fileName = inputDirPath + "out_"+Channel+".root";
			
		fIn[Channel] = new TFile(fileName.c_str(),"Read");		
		gROOT->cd();
		nGen_[Channel] = ((TH1F*)fIn[Channel]->Get("h_total"))->GetBinContent(2);
		tr[Channel] = (TTree*)fIn[Channel]->Get("Events");
		nEntries_[Channel]=tr[Channel]->GetEntriesFast();
		
		Xsec_[Channel] = getXsec(Channel);
		std::cout<<"Sample: "<<Channel<<"\tXsec: "<<Xsec_[Channel]<<"\t # total events: "<<nGen_[Channel]<<"\t # entries: "<<nEntries_[Channel]<<std::endl;

		if(Channel!="MuEG2017D") isMC = true;
		else isMC=false;

		if(!isMC){		
			hnumJets_[Channel]=new TH1D("nJets_"+TString(Channel),"",5,-0.5,4.5);						
			hnumTags_[Channel]=new TH1D("nTags_"+TString(Channel),"",4,-0.5,3.5);			
			h_allJetsPt_[Channel]=new TH1D("allJetsPt_"+TString(Channel),"",40,0.0,400.0);
			h_allJetsDeepFlavB_[Channel]=new TH1D("allJetsDeepFlavB_"+TString(Channel),"",20,0.0,1.0);
			h_firstJetDeepFlavB_[Channel]=new TH1D("firstJetDeepFlavB_"+TString(Channel),"",20,0.0,1.0);
			h_secondJetDeepFlavB_[Channel]=new TH1D("secondJetDeepFlavB_"+TString(Channel),"",20,0.0,1.0);		
		}
		
		else{
			hnumJets_[Channel]=new TH1D("nJets_"+TString(Channel),"",5,-0.5,4.5);						
			hnumJets_ReShape[Channel]=new TH1D("nJets_ReShape_"+TString(Channel),"",5,-0.5,4.5);						
			hnumJets_noSF[Channel]=new TH1D("nJets_noSF_"+TString(Channel),"",5,-0.5,4.5);						

			hnumTags_[Channel]=new TH1D("nTags_"+TString(Channel),"",4,-0.5,3.5);			
			hnumTags_ReShape[Channel]=new TH1D("nTags_ReShape_"+TString(Channel),"",4,-0.5,3.5);			
			hnumTags_noSF[Channel]=new TH1D("nTags_noSF_"+TString(Channel),"",4,-0.5,3.5);			

			h_allJetsPt_[Channel]=new TH1D("allJetsPt_"+TString(Channel),"",40,0.0,400.0);
			h_allJetsPt_ReShape[Channel]=new TH1D("allJetsPt_ReShape_"+TString(Channel),"",40,0.0,400.0);
			h_allJetsPt_noSF[Channel]=new TH1D("allJetsPt_noSF_"+TString(Channel),"",40,0.0,400.0);

			h_allJetsDeepFlavB_[Channel]=new TH1D("allJetsDeepFlavB_"+TString(Channel),"",20,0.0,1.0);
			h_allJetsDeepFlavB_ReShape[Channel]=new TH1D("allJetsDeepFlavB_ReShape_"+TString(Channel),"",20,0.0,1.0);
			h_allJetsDeepFlavB_noSF[Channel]=new TH1D("allJetsDeepFlavB_noSF_"+TString(Channel),"",20,0.0,1.0);

			h_firstJetDeepFlavB_[Channel]=new TH1D("firstJetDeepFlavB_"+TString(Channel),"",20,0.0,1.0);
			h_firstJetDeepFlavB_ReShape[Channel]=new TH1D("firstJetDeepFlavB_ReShape_"+TString(Channel),"",20,0.0,1.0);
			h_firstJetDeepFlavB_noSF[Channel]=new TH1D("firstJetDeepFlavB_noSF_"+TString(Channel),"",20,0.0,1.0);

			h_secondJetDeepFlavB_[Channel]=new TH1D("secondJetDeepFlavB_"+TString(Channel),"",20,0.0,1.0);
			h_secondJetDeepFlavB_ReShape[Channel]=new TH1D("secondJetDeepFlavB_ReShape_"+TString(Channel),"",20,0.0,1.0);
			h_secondJetDeepFlavB_noSF[Channel]=new TH1D("secondJetDeepFlavB_noSF_"+TString(Channel),"",20,0.0,1.0);
		}			
		
		tr[Channel]->SetBranchAddress("event",&event);
		tr[Channel]->SetBranchAddress("run",&run);
		tr[Channel]->SetBranchAddress("luminosityBlock",&lumiBlock);

		tr[Channel]->SetBranchAddress("MET_Pt",&MET_Pt);
		if(isMC){
			tr[Channel]->SetBranchAddress("puWeight",&puWeight);
			tr[Channel]->SetBranchAddress("genWeight",&genWeight);
		}
		tr[Channel]->SetBranchAddress("nJet",&nJet);
		tr[Channel]->SetBranchAddress("Jet_Pt", Jet_Pt);
		tr[Channel]->SetBranchAddress("Jet_eta", Jet_eta);
		tr[Channel]->SetBranchAddress("Jet_btagDeepFlavB", Jet_btagDeepFlavB);
		tr[Channel]->SetBranchAddress("Jet_jetId",Jet_jetId);
		tr[Channel]->SetBranchAddress("Jet_puId", Jet_puId);
		if(isMC){
			tr[Channel]->SetBranchAddress("Jet_partonFlavour",Jet_partonFlavour);
			tr[Channel]->SetBranchAddress("Jet_hadronFlavour",Jet_hadronFlavour);
		}
		
		std::cout<<"Starting Event Loop for "<<Channel<<std::endl;
		t.Start();	
///========================== Event loop starts	===================================
		for(ULong64_t iEvt=0; iEvt<(ULong64_t)nEntries_[Channel]; iEvt++){
			numJets=0; nTags=0; 
			evtWgt = 1.0; bWeight=1.0; bWeight_ReShape=1.0;
			jetinfo.clear(); jetIdx.clear();

			tr[Channel]->GetEntry(iEvt);	

			if(isMC){ 
				evtWgt=(Xsec_[Channel]* Lumi)/nGen_[Channel];
				evtWgt*=(genWeight*puWeight);									
			}	
			
			for(UInt_t iJet=0; iJet<nJet; iJet++){
				if(! (Jet_Pt[iJet] > 25.0 && fabs(Jet_eta[iJet]) < 2.5 && Jet_jetId[iJet]==6 ) ) continue;
//				if(! (Jet_Pt[iJet] > 25.0 && Jet_Pt[iJet] < 50.0 && Jet_puId[iJet]==7) ) continue;
				jetIdx.push_back(iJet);
			}


			if( MET_Pt < 50.0 ) continue;
			
			if(jetIdx.size()==0) continue;

			numJets = jetIdx.size();
			
			for(UInt_t ii=0; ii<jetIdx.size(); ii++){

				if(Jet_Pt[jetIdx.at(ii)] < 20.0 || fabs(Jet_eta[jetIdx.at(ii)]) > 2.5 ) continue; /// Sanity check
				
				if(Jet_btagDeepFlavB[jetIdx.at(ii)] > Mwp) nTags++;
				
				if(!isMC) continue;
				BTagWeight::JetInfo jInfo;
				jInfo.jetPt= (double)Jet_Pt[jetIdx.at(ii)];
				jInfo.jetEta= (double)Jet_eta[jetIdx.at(ii)];
				jInfo.jetFlav = (int)Jet_hadronFlavour[jetIdx.at(ii)]; 
				jInfo.jetDiscr = (double)Jet_btagDeepFlavB[jetIdx.at(ii)];
				
				if(jInfo.jetDiscr > Mwp) jInfo.isTagged = true;
				else jInfo.isTagged = false;
				
								
				jetinfo.push_back(jInfo);				
			}
				
			if(isMC){
				bWeight = j.evalWeight("Medium","central", jetinfo, TString(Channel));
				bWeight_ReShape = j.evalWeight("ReShape","central", jetinfo, TString(Channel));
			
				hnumJets_noSF[Channel]->Fill(numJets, evtWgt);				 
				hnumJets_[Channel]->Fill(numJets,evtWgt*bWeight);
				hnumJets_ReShape[Channel]->Fill(numJets,evtWgt*bWeight_ReShape);

				hnumTags_noSF[Channel]->Fill(nTags, evtWgt);				 
				hnumTags_[Channel]->Fill(nTags,evtWgt*bWeight);
				hnumTags_ReShape[Channel]->Fill(nTags,evtWgt*bWeight_ReShape);		
					
			}	
				
			else{ 
				hnumJets_[Channel]->Fill(numJets);						
				hnumTags_[Channel]->Fill(nTags);
			}
				
			for(UInt_t ii=0; ii<jetIdx.size(); ii++){
									
				if(isMC){
					h_allJetsPt_noSF[Channel]->Fill(Jet_Pt[jetIdx.at(ii)], evtWgt);					
					h_allJetsPt_[Channel]->Fill(Jet_Pt[jetIdx.at(ii)], evtWgt*bWeight);
					h_allJetsPt_ReShape[Channel]->Fill(Jet_Pt[jetIdx.at(ii)], evtWgt*bWeight_ReShape); 

					h_allJetsDeepFlavB_noSF[Channel]->Fill(Jet_btagDeepFlavB[jetIdx.at(ii)], evtWgt);					
					h_allJetsDeepFlavB_[Channel]->Fill(Jet_btagDeepFlavB[jetIdx.at(ii)],evtWgt*bWeight);
					h_allJetsDeepFlavB_ReShape[Channel]->Fill(Jet_btagDeepFlavB[jetIdx.at(ii)], evtWgt*bWeight_ReShape); 

					if(ii==0){
						h_firstJetDeepFlavB_noSF[Channel]->Fill(Jet_btagDeepFlavB[jetIdx.at(ii)], evtWgt);
						h_firstJetDeepFlavB_[Channel]->Fill(Jet_btagDeepFlavB[jetIdx.at(ii)], evtWgt*bWeight_ReShape);						
						h_firstJetDeepFlavB_ReShape[Channel]->Fill(Jet_btagDeepFlavB[jetIdx.at(ii)], evtWgt*bWeight_ReShape);
					}
					
					if(jetIdx.size()>1 && ii==1){ 
						h_secondJetDeepFlavB_noSF[Channel]->Fill(Jet_btagDeepFlavB[jetIdx.at(ii)], evtWgt);
						h_secondJetDeepFlavB_[Channel]->Fill(Jet_btagDeepFlavB[jetIdx.at(ii)], evtWgt*bWeight_ReShape);						
						h_secondJetDeepFlavB_ReShape[Channel]->Fill(Jet_btagDeepFlavB[jetIdx.at(ii)], evtWgt*bWeight_ReShape);
					}
					
				}
					
				else{
					h_allJetsPt_[Channel]->Fill(Jet_Pt[jetIdx.at(ii)]);
					h_allJetsDeepFlavB_[Channel]->Fill(Jet_btagDeepFlavB[jetIdx.at(ii)]);
					
					if(ii==0) h_firstJetDeepFlavB_[Channel]->Fill(Jet_btagDeepFlavB[jetIdx.at(ii)]);
					
					if(jetIdx.size()>1 && ii==1) h_secondJetDeepFlavB_[Channel]->Fill(Jet_btagDeepFlavB[jetIdx.at(ii)]);

				}	

			}	
		}
/// ================== End of Event Loop ================================			

		std::cout<<"Processed "<<Channel<<std::endl;
		
		delete tr[Channel]; delete fIn[Channel]; 	

		fOut->cd();
		dir[Channel]=(TDirectory*)fOut->mkdir(TString(Channel));
		dir[Channel]->cd();

		if(!isMC){
			hnumJets_[Channel]->Write();
			hnumTags_[Channel]->Write();
			h_allJetsPt_[Channel]->Write();
			h_allJetsDeepFlavB_[Channel]->Write();
			h_firstJetDeepFlavB_[Channel]->Write();
			h_secondJetDeepFlavB_[Channel]->Write();
		}

		else{
			hnumJets_[Channel]->Write();
			hnumJets_ReShape[Channel]->Write();
			hnumJets_noSF[Channel]->Write();

			hnumTags_[Channel]->Write();
			hnumTags_ReShape[Channel]->Write();
			hnumTags_noSF[Channel]->Write();

			h_allJetsPt_[Channel]->Write();
			h_allJetsPt_ReShape[Channel]->Write();
			h_allJetsPt_noSF[Channel]->Write();

			h_allJetsDeepFlavB_[Channel]->Write();
			h_allJetsDeepFlavB_ReShape[Channel]->Write();
			h_allJetsDeepFlavB_noSF[Channel]->Write();

			h_firstJetDeepFlavB_[Channel]->Write();
			h_firstJetDeepFlavB_ReShape[Channel]->Write();
			h_firstJetDeepFlavB_noSF[Channel]->Write();

			h_secondJetDeepFlavB_[Channel]->Write();
			h_secondJetDeepFlavB_ReShape[Channel]->Write();
			h_secondJetDeepFlavB_noSF[Channel]->Write();
		}

		fOut->cd();
		dir[Channel]->Write();				
		
		gROOT->cd();
		if(!isMC){
			delete hnumJets_[Channel]; delete hnumTags_[Channel]; delete h_allJetsPt_[Channel]; delete h_allJetsDeepFlavB_[Channel]; 
			delete h_firstJetDeepFlavB_[Channel]; delete h_secondJetDeepFlavB_[Channel];
		}
		else{
			delete hnumJets_[Channel]; delete hnumJets_ReShape[Channel]; delete hnumJets_noSF[Channel]; 
			delete hnumTags_[Channel]; delete hnumTags_ReShape[Channel]; delete hnumTags_noSF[Channel];
			delete h_allJetsPt_[Channel]; delete h_allJetsPt_ReShape[Channel]; delete h_allJetsPt_noSF[Channel];
			delete h_allJetsDeepFlavB_[Channel]; delete h_allJetsDeepFlavB_ReShape[Channel]; delete h_allJetsDeepFlavB_noSF[Channel];
			delete h_firstJetDeepFlavB_[Channel]; delete h_firstJetDeepFlavB_ReShape[Channel]; delete h_firstJetDeepFlavB_noSF[Channel]; 
			delete h_secondJetDeepFlavB_[Channel]; delete h_secondJetDeepFlavB_ReShape[Channel]; delete h_secondJetDeepFlavB_noSF[Channel];
		}
		delete dir[Channel];

		t.Stop(); t.Print("u");
		t.Reset();
		std::cout<<"\n=========================================="<<std::endl;
	
	}	

	std::cout<<"Histos Written"<<std::endl;		
	fOut->Write();
	std::cout<<"File Written"<<std::endl;
	fOut->Close();
	std::cout<<"File Closed"<<std::endl;
	
	delete fOut;
		
	gDirectory->ls("-m");
	gDirectory->ls("-d");
	
}
