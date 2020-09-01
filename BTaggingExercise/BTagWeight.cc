#ifndef BTagWeight_cc
#define BTagWeight_cc

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <cmath>
#include <complex>
#include <stdio.h>

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

//Include BTagCalibrationStandalone class === Incomplete ===
#include "BTagCalibrationStandalone.h"
#include "BTagCalibrationStandalone.cpp"
//========End of solution section of code (1 of 3)=========


class BTagWeight
{
	public:
		BTagWeight();
		~BTagWeight();

		BTagCalibration calib;
		BTagCalibrationReader* reader;
		BTagCalibrationReader* readerReShape;
			
		struct JetInfo{
			double jetPt;
			double jetEta;
			int jetFlav;
			double jetDiscr;
			bool isTagged;
		};
			
		double evalWeight(TString WP, std::string syst, std::vector<BTagWeight::JetInfo> jets, TString channel);
				
	private:
		unsigned int nJets;
		double MCTagEfficiency(TString WP, int flavor, double pt, double eta, TString Sample);
		double TagScaleFactor(TString WP, std::string syst, int flavor, double pt, double eta, double discr);

};

BTagWeight::BTagWeight(){

///========================= Initialize BTagCalibration and BTagCalibrationReader ==========
//
//
//==========================
}

BTagWeight::~BTagWeight()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

	delete reader; delete readerReShape;
	
}
	
double BTagWeight::MCTagEfficiency(TString WP, int flavor, double pt, double eta, TString Sample){  

	double eff=1.0;
	
	TH2D*  bEff;
	TH2D*  cEff;
	TH2D*  lEff;
 	
	int binX, binY;
	TString sample;
	
	if(Sample.Contains(TString("ttToDiLep"))) sample= "ttDiLep";
	else if(Sample.Contains(TString("DY"))) sample= "DY";
	else sample= Sample;
	
	TFile* fBTagEff=new TFile("EffHistos/"+sample+".root","Read");
		
	bEff=(TH2D*)fBTagEff->Get("Histograms/h_eff_bjets_m_deepjet");
	cEff=(TH2D*)fBTagEff->Get("Histograms/h_eff_cjets_m_deepjet");
	lEff=(TH2D*)fBTagEff->Get("Histograms/h_eff_udsgjets_m_deepjet");
	
	gROOT->cd();		 
			
	if(pt < 20.0){ 
		std::cout<<"Caution!!! Jet Pt below 20 GeV. Something wrong in selection. Setting eff. to zero"<<std::endl;
		eff=0.0;
	}
	
	else{
		if (flavor==5){
			binX=bEff->GetXaxis()->FindBin(eta); binY=bEff->GetYaxis()->FindBin(pt);
			eff=(double) bEff->GetBinContent(binX, binY);
		}			
		if (flavor==4){
			binX=cEff->GetXaxis()->FindBin(eta); binY=cEff->GetYaxis()->FindBin(pt);
			eff=(double) cEff->GetBinContent(binX, binY);
		}

		if (flavor!=4 && flavor!=5){
			binX=lEff->GetXaxis()->FindBin(eta); binY=lEff->GetYaxis()->FindBin(pt);
			eff=(double) lEff->GetBinContent(binX, binY);
		}
	}	

	delete bEff; delete cEff; delete lEff; 		
	delete fBTagEff;
		
	return eff;  
}

double BTagWeight::TagScaleFactor(TString WP, std::string syst, int flavor, double pt, double eta, double discr){
	
	if(WP == "Medium"){
		if(flavor==5){return reader->eval_auto_bounds(syst,BTagEntry::FLAV_B, eta, pt);      }
		if(flavor==4){return reader->eval_auto_bounds(syst,BTagEntry::FLAV_C, eta, pt);      }
		if(flavor!=5 && flavor!=4){return reader->eval_auto_bounds(syst,BTagEntry::FLAV_UDSG, eta, pt);      }		
	}
	
	if(WP == "ReShape"){
		if(flavor==5){ return readerReShape->eval_auto_bounds(syst, BTagEntry::FLAV_B, eta, pt, discr); 	}
		if(flavor==4){ return readerReShape->eval_auto_bounds(syst, BTagEntry::FLAV_C, eta, pt, discr); 	}
		if(flavor!=5 && flavor!=4){ return readerReShape->eval_auto_bounds(syst, BTagEntry::FLAV_UDSG, eta, pt, discr); 	}
	}		
	
	return 1.0;
}

double BTagWeight::evalWeight(TString WP, std::string syst, std::vector<BTagWeight::JetInfo> jets, TString channel){
	
	nJets = jets.size();
	
	double eff; double sf;
	
	double pMC=1.0;
	double pData=1.0;
	
	double weight=1.0;
		
	for(unsigned int aa=0; aa<jets.size(); aa++){
		
		sf 	= TagScaleFactor(WP, syst, jets.at(aa).jetFlav, jets.at(aa).jetPt, jets.at(aa).jetEta, jets.at(aa).jetDiscr);

		if(WP!="ReShape"){
			eff = MCTagEfficiency(WP, jets.at(aa).jetFlav, jets.at(aa).jetPt, jets.at(aa).jetEta, channel);
			if(jets.at(aa).isTagged){ 
				pMC=pMC*eff;
				pData=pData*(sf*eff);
			}
			else{
				pMC=pMC*(1.0 - eff);
				pData=pData*(1.0 - sf*eff);
			}
		}
		
		else{
			weight=weight*sf;
		}		  
	}
	
	if(WP!="ReShape"){
		if(pMC!=0.0) weight = pData/pMC;		
	}	
	
	return weight;	
}	 		


#endif
