#include <stdio.h>
#include <stdlib.h>


void chrcorr(){
	//
	const int 	NMOMN	= 3;
	const char* momn[NMOMN]			= {"3","5","7"};
	const int 	NANG	= 14;		// DEGREES
	const char* angles[NANG]		= {"23","30","50","70","86","88","90","92","94","110","125","130","150","164"};
// 	const int 	NSPE = 3;
// 	const char* species[NSPE]		= {"pi+","kaon+","proton"};
// 	const int 	speciescol[NSPE]	= {2,kOrange+4,kAzure+2};
// 	const int 	speciesmrk[NSPE]	= {20,21,33};
// 	const int 	speciesmrko[NSPE]	= {53,54,56};
	const int 	NSPE = 1;
	const char* species[NSPE]		= {"pi+"};
	const int 	speciescol[NSPE]	= {2};
	const int 	speciesmrk[NSPE]	= {20};
	const int 	speciesmrko[NSPE]	= {53};
	//
	const int 	momncol[NMOMN]		= {kOrange+4,kAzure-3,kOrange+7};
	const int 	momnmrk[NMOMN]		= {20,21,33};
	const int 	momnmrko[NMOMN]		= {53,54,56};
	//
	double fangles[NANG];
	double fanglesDeg[NANG];
	double eta[NANG];
	for (int iang=0;iang<NANG;iang++){
		fanglesDeg[iang]	=  std::atof(angles[iang]);			// DEGREES!!!
		fangles[iang]		=  fanglesDeg[iang]*(M_PI/180.);	// RADIANS!!!
		eta[iang]			= -std::log( std::tan(fangles[iang]/2.0) );
		//cout<<fangles[iang]<<" "<<eta[iang]<<endl;
	}
	//
	TH1D*	hinfo[NANG][NSPE][NMOMN];
	double	thetaCexp[NSPE][NMOMN];
	//
// 	hinfo->SetBinContent(51,hthetaC_WL1_gated->Integral());
// 	hinfo->SetBinContent(52,fchr1->GetParameter(0));
// 	hinfo->SetBinContent(53,fchr1->GetParameter(1));
// 	hinfo->SetBinContent(54,fchr1->Eval(370));
// 	hinfo->SetBinContent(55,hthetaC_WL2_gated->Integral());
// 	hinfo->SetBinContent(56,fchr2->GetParameter(0));
// 	hinfo->SetBinContent(57,fchr2->GetParameter(1));
// 	hinfo->SetBinContent(58,fchr2->Eval(370));
// 			hinfo->SetBinContent(61,INVVG_lower);
// 			hinfo->SetBinContent(62,INVVG_upper);
// 			hinfo->SetBinContent(63,thisintercept1);
// 			hinfo->SetBinContent(64,thisslope1);
// 			hinfo->SetBinContent(65,thisintercept2);
// 			hinfo->SetBinContent(66,thisslope2);

	TGraph* gchr1par0_Theta[NSPE][NMOMN];
	TGraph* gchr1par1_Theta[NSPE][NMOMN];
	TGraph* gchr2par0_Theta[NSPE][NMOMN];
	TGraph* gchr2par1_Theta[NSPE][NMOMN];
	TGraph* gChrom1par0_Theta[NSPE][NMOMN];
	TGraph* gChrom1par1_Theta[NSPE][NMOMN];
	TGraph* gChrom2par0_Theta[NSPE][NMOMN];
	TGraph* gChrom2par1_Theta[NSPE][NMOMN];
	//
	for (int imom=0;imom<NMOMN;imom++){
		//
		for (int ispe=0;ispe<NSPE;ispe++){
			//
			  gchr1par0_Theta[ispe][imom]	= new TGraph();
			  gchr1par1_Theta[ispe][imom]	= new TGraph();
			  gchr2par0_Theta[ispe][imom]	= new TGraph();
			  gchr2par1_Theta[ispe][imom]	= new TGraph();
			gChrom1par0_Theta[ispe][imom]	= new TGraph();
			gChrom1par1_Theta[ispe][imom]	= new TGraph();
			gChrom2par0_Theta[ispe][imom]	= new TGraph();
			gChrom2par1_Theta[ispe][imom]	= new TGraph();
			//
			for (int iang=0;iang<NANG;iang++){
				double thetaDeg	= atof(angles[iang]);
				int k;
				//
				TString steer  = TString(Form("%s%sGeV%sdeg",species[ispe],momn[imom],angles[iang]));
				TString infile = TString(Form("dircGR_%s.root",steer.Data()));
				TFile *f = new TFile(infile.Data(),"read");
				//
				hinfo[iang][ispe][imom]			= (TH1D*)f->Get("hinfo");
				hinfo[iang][ispe][imom]			->SetName(Form("hinfo_%d_%d_%d",iang,ispe,imom));
				hinfo[iang][ispe][imom]			->SetDirectory(0);
				double Primary_mass				= hinfo[iang][ispe][imom]->GetBinContent(1);
				double Primary_momn				= hinfo[iang][ispe][imom]->GetBinContent(2);
				double Primary_thetaCexpected	= hinfo[iang][ispe][imom]->GetBinContent(3);
				thetaCexp[ispe][imom]			= Primary_thetaCexpected;
				//
				int n1	= hinfo[iang][ispe][imom]->GetBinContent(51);
				int n2	= hinfo[iang][ispe][imom]->GetBinContent(55);
				if (n1>100){
					  gchr1par0_Theta[ispe][imom]	->SetPoint(  gchr1par0_Theta[ispe][imom]->GetN(),fanglesDeg[iang],hinfo[iang][ispe][imom]->GetBinContent(52));
					  gchr1par1_Theta[ispe][imom]	->SetPoint(  gchr1par1_Theta[ispe][imom]->GetN(),fanglesDeg[iang],hinfo[iang][ispe][imom]->GetBinContent(53));
					gChrom1par0_Theta[ispe][imom]	->SetPoint(gChrom1par0_Theta[ispe][imom]->GetN(),fanglesDeg[iang],hinfo[iang][ispe][imom]->GetBinContent(63));
					gChrom1par1_Theta[ispe][imom]	->SetPoint(gChrom1par1_Theta[ispe][imom]->GetN(),fanglesDeg[iang],hinfo[iang][ispe][imom]->GetBinContent(64));
				}
				if (n2>100){
					  gchr2par0_Theta[ispe][imom]	->SetPoint(  gchr2par0_Theta[ispe][imom]->GetN(),fanglesDeg[iang],hinfo[iang][ispe][imom]->GetBinContent(56));
					  gchr2par1_Theta[ispe][imom]	->SetPoint(  gchr2par1_Theta[ispe][imom]->GetN(),fanglesDeg[iang],hinfo[iang][ispe][imom]->GetBinContent(57));
					gChrom2par0_Theta[ispe][imom]	->SetPoint(gChrom2par0_Theta[ispe][imom]->GetN(),fanglesDeg[iang],hinfo[iang][ispe][imom]->GetBinContent(65));
					gChrom2par1_Theta[ispe][imom]	->SetPoint(gChrom2par1_Theta[ispe][imom]->GetN(),fanglesDeg[iang],hinfo[iang][ispe][imom]->GetBinContent(66));
				}
				  gchr1par0_Theta[ispe][imom]	->SetLineColor(momncol[imom]);
				  gchr1par1_Theta[ispe][imom]	->SetLineColor(momncol[imom]);
				gChrom1par0_Theta[ispe][imom]	->SetLineColor(momncol[imom]);
				gChrom1par1_Theta[ispe][imom]	->SetLineColor(momncol[imom]);
				  gchr2par0_Theta[ispe][imom]	->SetLineColor(momncol[imom]);
				  gchr2par1_Theta[ispe][imom]	->SetLineColor(momncol[imom]);
				gChrom2par0_Theta[ispe][imom]	->SetLineColor(momncol[imom]);
				gChrom2par1_Theta[ispe][imom]	->SetLineColor(momncol[imom]);
				  gchr1par0_Theta[ispe][imom]	->SetLineWidth(2);
				  gchr1par1_Theta[ispe][imom]	->SetLineWidth(2);
				gChrom1par0_Theta[ispe][imom]	->SetLineWidth(2);
				gChrom1par1_Theta[ispe][imom]	->SetLineWidth(2);
				  gchr2par0_Theta[ispe][imom]	->SetLineWidth(2);
				  gchr2par1_Theta[ispe][imom]	->SetLineWidth(2);
				gChrom2par0_Theta[ispe][imom]	->SetLineWidth(2);
				gChrom2par1_Theta[ispe][imom]	->SetLineWidth(2);
				  gchr2par0_Theta[ispe][imom]	->SetLineStyle(2);
				  gchr2par1_Theta[ispe][imom]	->SetLineStyle(2);
				gChrom2par0_Theta[ispe][imom]	->SetLineStyle(2);
				gChrom2par1_Theta[ispe][imom]	->SetLineStyle(2);
				//
				f->Close();
				//
			}	// end theta
			//
		}	// end species
		//
	}	// end momn

	cout<<"writing chrcorr_new.root"<<endl;
	TFile* fout	= new TFile("chrcorr_new.root","recreate");
	fout->cd();
	for (int imom=0;imom<NMOMN;imom++){
		for (int ispe=0;ispe<NSPE;ispe++){
			  gchr1par0_Theta[ispe][imom]	->SetTitle(Form(  "gchr1par0 %s %sGeV",species[ispe],momn[imom]));
			  gchr1par1_Theta[ispe][imom]	->SetTitle(Form(  "gchr1par1 %s %sGeV",species[ispe],momn[imom]));
			  gchr2par0_Theta[ispe][imom]	->SetTitle(Form(  "gchr2par0 %s %sGeV",species[ispe],momn[imom]));
			  gchr2par1_Theta[ispe][imom]	->SetTitle(Form(  "gchr2par1 %s %sGeV",species[ispe],momn[imom]));
			gChrom1par0_Theta[ispe][imom]	->SetTitle(Form("gChrom1par0 %s %sGeV",species[ispe],momn[imom]));
			gChrom1par1_Theta[ispe][imom]	->SetTitle(Form("gChrom1par1 %s %sGeV",species[ispe],momn[imom]));
			gChrom2par0_Theta[ispe][imom]	->SetTitle(Form("gChrom2par0 %s %sGeV",species[ispe],momn[imom]));
			gChrom2par1_Theta[ispe][imom]	->SetTitle(Form("gChrom2par1 %s %sGeV",species[ispe],momn[imom]));
			//
			  gchr1par0_Theta[ispe][imom]	->Write(Form(  "gchr1par0_Theta_%d_%d",ispe,imom));
			  gchr1par1_Theta[ispe][imom]	->Write(Form(  "gchr1par1_Theta_%d_%d",ispe,imom));
			  gchr2par0_Theta[ispe][imom]	->Write(Form(  "gchr2par0_Theta_%d_%d",ispe,imom));
			  gchr2par1_Theta[ispe][imom]	->Write(Form(  "gchr2par1_Theta_%d_%d",ispe,imom));
			gChrom1par0_Theta[ispe][imom]	->Write(Form("gChrom1par0_Theta_%d_%d",ispe,imom));
			gChrom1par1_Theta[ispe][imom]	->Write(Form("gChrom1par1_Theta_%d_%d",ispe,imom));
			gChrom2par0_Theta[ispe][imom]	->Write(Form("gChrom2par0_Theta_%d_%d",ispe,imom));
			gChrom2par1_Theta[ispe][imom]	->Write(Form("gChrom2par1_Theta_%d_%d",ispe,imom));
		}
	}
	fout->Close();	
	
	//---- Paint setup...
	//
	gStyle->SetOptStat(11);
	gStyle->SetErrorX(0);
	gStyle->SetPadRightMargin(0.01);
	gStyle->SetPadTopMargin(0.02);
	gStyle->SetPadBottomMargin(0.09);
	gStyle->SetPadLeftMargin(0.15);
	gStyle->SetTitleX(0.5);
	gStyle->SetLabelSize(0.05,"xyzt");
	gStyle->SetPadGridX(0);
	gStyle->SetPadGridY(0);
	//gStyle->SetLabelFont(41,"xyz");
	//gStyle->SetLegendFont(41);
	//gStyle->SetStatFont(41);
	//gStyle->SetTitleFont(41);
	gStyle->SetPaintTextFormat("#1.1e");
	gStyle->SetStatX(0.37);
	gStyle->SetStatY(0.94);
	TCanvas *ccan[100]; int ican=-1;
	TLatex *text[200];
	int itext	= -1;
	for (int i=0;i<200;i++){
		text[i]	= new TLatex();
		text[i]	->SetNDC(true);
		text[i]	->SetTextFont(40);
		//text[i]	->SetTextSize(0.06);
		//text[i]	->SetTextAlign(11);
	}
	int iframe = -1;
	TH1F*		frame[100];
	TLine *line[1000];
	int iline = -1;
	for (int i=0;i<1000;i++){
		line[i]	= new TLine();
		line[i]	->SetLineStyle(2);
		line[i]	->SetLineWidth(1);
	}
// 	TLegend* leg[NMOMN];
// 	for (int imom=0;imom<NMOMN;imom++){
// 		leg[imom]	= new TLegend(0.2,0.80,0.6,0.92);
// 		leg[imom]	->SetTextSize(0.05);
// 		leg[imom]	->AddEntry(ggparsP[0][0][0],Form("pi+ %sGeV"   ,momn[imom]),"LP");		// value vs theta: indexed by moment, species, momentum
// 		leg[imom]	->AddEntry(ggparsP[0][1][0],Form("kaon+ %sGeV" ,momn[imom]),"LP");
// 		leg[imom]	->AddEntry(ggparsP[0][2][0],Form("proton %sGeV",momn[imom]),"LP");
// 	}

	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("Page %d",ican),10+30*ican,30+30*ican,1400,600);
	ccan[ican]->cd(); ccan[ican]->Divide(4,2,0.0001,0.0001);
	ccan[ican]->cd(1);
		++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(15,19,175,28);
				frame[iframe]->SetTitle("; #theta (deg); chr1par0");
				frame[iframe]->GetXaxis()->SetLabelOffset(-0.01);
				frame[iframe]->GetXaxis()->SetLabelSize(0.05);	frame[iframe]->GetYaxis()->SetLabelSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleSize(0.05);	frame[iframe]->GetYaxis()->SetTitleSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleOffset(0.65);frame[iframe]->GetYaxis()->SetTitleOffset(1.6);
		for (int imom=0;imom<NMOMN;imom++){ 
			for (int ispe=0;ispe<NSPE;ispe++){
				gchr1par0_Theta[ispe][imom]	->Draw("LP");
			}
		}
	ccan[ican]->cd(2);
		++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(15,-0.1,175,0);
				frame[iframe]->SetTitle("; #theta (deg); chr1par1");
				frame[iframe]->GetXaxis()->SetLabelOffset(-0.01);
				frame[iframe]->GetXaxis()->SetLabelSize(0.05);	frame[iframe]->GetYaxis()->SetLabelSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleSize(0.05);	frame[iframe]->GetYaxis()->SetTitleSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleOffset(0.65);frame[iframe]->GetYaxis()->SetTitleOffset(1.6);
		for (int imom=0;imom<NMOMN;imom++){ 
			for (int ispe=0;ispe<NSPE;ispe++){
				gchr1par1_Theta[ispe][imom]	->Draw("LP");
			}
		}
	ccan[ican]->cd(3);
		++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(15,19,175,28);
				frame[iframe]->SetTitle("; #theta (deg); chr2par0");
				frame[iframe]->GetXaxis()->SetLabelOffset(-0.01);
				frame[iframe]->GetXaxis()->SetLabelSize(0.05);	frame[iframe]->GetYaxis()->SetLabelSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleSize(0.05);	frame[iframe]->GetYaxis()->SetTitleSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleOffset(0.65);frame[iframe]->GetYaxis()->SetTitleOffset(1.6);
		for (int imom=0;imom<NMOMN;imom++){ 
			for (int ispe=0;ispe<NSPE;ispe++){
				gchr2par0_Theta[ispe][imom]	->Draw("LP");
			}
		}
	ccan[ican]->cd(4);
		++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(15,-0.1,175,0);
				frame[iframe]->SetTitle("; #theta (deg); chr2par1");
				frame[iframe]->GetXaxis()->SetLabelOffset(-0.01);
				frame[iframe]->GetXaxis()->SetLabelSize(0.05);	frame[iframe]->GetYaxis()->SetLabelSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleSize(0.05);	frame[iframe]->GetYaxis()->SetTitleSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleOffset(0.65);frame[iframe]->GetYaxis()->SetTitleOffset(1.6);
		for (int imom=0;imom<NMOMN;imom++){ 
			for (int ispe=0;ispe<NSPE;ispe++){
				gchr2par1_Theta[ispe][imom]	->Draw("LP");
			}
		}
	ccan[ican]->cd(5);
		++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(15,-0.008,175,0.004);
				frame[iframe]->SetTitle("; #theta (deg); Chrom1par0");
				frame[iframe]->GetXaxis()->SetLabelOffset(-0.01);
				frame[iframe]->GetXaxis()->SetLabelSize(0.05);	frame[iframe]->GetYaxis()->SetLabelSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleSize(0.05);	frame[iframe]->GetYaxis()->SetTitleSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleOffset(0.65);frame[iframe]->GetYaxis()->SetTitleOffset(1.6);
		for (int imom=0;imom<NMOMN;imom++){ 
			for (int ispe=0;ispe<NSPE;ispe++){
				gChrom1par0_Theta[ispe][imom]	->Draw("LP");
			}
		}
	ccan[ican]->cd(6);
		++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(15,-1,175,80);
				frame[iframe]->SetTitle("; #theta (deg); Chrom1par1");
				frame[iframe]->GetXaxis()->SetLabelOffset(-0.01);
				frame[iframe]->GetXaxis()->SetLabelSize(0.05);	frame[iframe]->GetYaxis()->SetLabelSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleSize(0.05);	frame[iframe]->GetYaxis()->SetTitleSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleOffset(0.65);frame[iframe]->GetYaxis()->SetTitleOffset(1.6);
		for (int imom=0;imom<NMOMN;imom++){ 
			for (int ispe=0;ispe<NSPE;ispe++){
				gChrom1par1_Theta[ispe][imom]	->Draw("LP");
			}
		}
	ccan[ican]->cd(7);
		++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(15,-0.008,175,0.004);
				frame[iframe]->SetTitle("; #theta (deg); Chrom2par0");
				frame[iframe]->GetXaxis()->SetLabelOffset(-0.01);
				frame[iframe]->GetXaxis()->SetLabelSize(0.05);	frame[iframe]->GetYaxis()->SetLabelSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleSize(0.05);	frame[iframe]->GetYaxis()->SetTitleSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleOffset(0.65);frame[iframe]->GetYaxis()->SetTitleOffset(1.6);
		for (int imom=0;imom<NMOMN;imom++){ 
			for (int ispe=0;ispe<NSPE;ispe++){
				gChrom2par0_Theta[ispe][imom]	->Draw("LP");
			}
		}
	ccan[ican]->cd(8);
		++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(15,-1,175,80);
				frame[iframe]->SetTitle("; #theta (deg); Chrom2par1");
				frame[iframe]->GetXaxis()->SetLabelOffset(-0.01);
				frame[iframe]->GetXaxis()->SetLabelSize(0.05);	frame[iframe]->GetYaxis()->SetLabelSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleSize(0.05);	frame[iframe]->GetYaxis()->SetTitleSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleOffset(0.65);frame[iframe]->GetYaxis()->SetTitleOffset(1.6);
		for (int imom=0;imom<NMOMN;imom++){ 
			for (int ispe=0;ispe<NSPE;ispe++){
				gChrom2par1_Theta[ispe][imom]	->Draw("LP");
			}
		}
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print("chrcorr.ps(");


	gStyle->SetPadLeftMargin(0.06);

	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("Page %d",ican),10+30*ican,30+30*ican,1400,600);
	ccan[ican]->cd(); ccan[ican]->Divide(1,2,0.0001,0.0001);
	ccan[ican]->cd(1);
		++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(15,-0.008,175,0.004);
				frame[iframe]->SetTitle("; #theta (deg); Chrom1par0");
				frame[iframe]->GetXaxis()->SetLabelOffset(-0.01);
				frame[iframe]->GetXaxis()->SetLabelSize(0.05);	frame[iframe]->GetYaxis()->SetLabelSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleSize(0.05);	frame[iframe]->GetYaxis()->SetTitleSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleOffset(0.65);frame[iframe]->GetYaxis()->SetTitleOffset(0.6);
		for (int imom=0;imom<NMOMN;imom++){ 
			for (int ispe=0;ispe<NSPE;ispe++){
				gChrom1par0_Theta[ispe][imom]	->Draw("LP");
				gChrom2par0_Theta[ispe][imom]	->Draw("LP");
			}
		}
	ccan[ican]->cd(2);
		++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(15,-1,175,80);
				frame[iframe]->SetTitle("; #theta (deg); Chrom1par1");
				frame[iframe]->GetXaxis()->SetLabelOffset(-0.01);
				frame[iframe]->GetXaxis()->SetLabelSize(0.05);	frame[iframe]->GetYaxis()->SetLabelSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleSize(0.05);	frame[iframe]->GetYaxis()->SetTitleSize(0.05);
				frame[iframe]->GetXaxis()->SetTitleOffset(0.65);frame[iframe]->GetYaxis()->SetTitleOffset(0.6);
		for (int imom=0;imom<NMOMN;imom++){ 
			for (int ispe=0;ispe<NSPE;ispe++){
				gChrom1par1_Theta[ispe][imom]	->Draw("LP");
				gChrom2par1_Theta[ispe][imom]	->Draw("LP");
			}
		}
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print("chrcorr.ps");
			
	

// 		++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("Page %d",ican),10+30*ican,30+30*ican,1400,600);
// 		ccan[ican]->cd(); ccan[ican]->Divide(3,2,0.0001,0.0001);
// 		ccan[ican]->cd(1);
// 			++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(12.65,0,164.3,100000);
// 					frame[iframe]->SetTitle("Constant; #theta (deg); Counts");
// //			++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(-2.1,0,1.7,100000);
// //					frame[iframe]->SetTitle("Constant;Incidence #eta_{DET}; Counts");
// 					frame[iframe]->GetXaxis()->SetLabelOffset(-0.01);
// 					frame[iframe]->GetXaxis()->SetLabelSize(0.05);
// 					frame[iframe]->GetYaxis()->SetLabelSize(0.05);
// 					frame[iframe]->GetXaxis()->SetTitleSize(0.05);
// 					frame[iframe]->GetYaxis()->SetTitleSize(0.05);
// 					frame[iframe]->GetYaxis()->SetTitleOffset(1.6);
// 					frame[iframe]->GetXaxis()->SetTitleOffset(0.65);
// 			for (int imom=0;imom<NMOMN;imom++){ 
// 				for (int ispe=0;ispe<NSPE;ispe++){
// 					ggpars[0][ispe][imom]	->Draw("LP");
// 				}
// 				//leg[imom]->Draw("same");
// 			}
// 		ccan[ican]->cd(2);
// 			++iframe; 
// //			frame[iframe] = (TH1F*)gPad->DrawFrame(12.65,790,164.3,850);
// 			frame[iframe] = (TH1F*)gPad->DrawFrame(12.65,816,164.3,830);
// 					frame[iframe]->SetTitle("Mean; #theta (deg); thetaC (mrad)");
// //			frame[iframe] = (TH1F*)gPad->DrawFrame(-2.1,760,1.7,830);
// //					frame[iframe]->SetTitle("Mean;Incidence #eta_{DET}; thetaC (mrad)");
// 					frame[iframe]->GetXaxis()->SetLabelOffset(-0.01);
// 					frame[iframe]->GetXaxis()->SetLabelSize(0.05);
// 					frame[iframe]->GetYaxis()->SetLabelSize(0.05);
// 					frame[iframe]->GetXaxis()->SetTitleSize(0.05);
// 					frame[iframe]->GetYaxis()->SetTitleSize(0.05);
// 					frame[iframe]->GetYaxis()->SetTitleOffset(1.35);
// 					frame[iframe]->GetXaxis()->SetTitleOffset(0.65);
// 			for (int imom=0;imom<NMOMN;imom++){ 
// 	 			for (int ispe=0;ispe<NSPE;ispe++){
// 	 				ggpars[1][ispe][imom]	->Draw("LP");
//  				}
// 			}
// 		ccan[ican]->cd(3);
// 			++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(12.65,0,164.3,12);
// 					frame[iframe]->SetTitle("Std. Dev.; #theta (deg); Std.Dev. (mrad)");
// //			++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(-2.1,3,1.7,9);
// //					frame[iframe]->SetTitle("Std. Dev.;Incidence #eta_{DET}; Std.Dev. (mrad)");
// 					frame[iframe]->GetXaxis()->SetLabelOffset(-0.01);
// 					frame[iframe]->GetXaxis()->SetLabelSize(0.05);
// 					frame[iframe]->GetYaxis()->SetLabelSize(0.05);
// 					frame[iframe]->GetXaxis()->SetTitleSize(0.05);
// 					frame[iframe]->GetYaxis()->SetTitleSize(0.05);
// 					frame[iframe]->GetYaxis()->SetTitleOffset(1.1);
// 					frame[iframe]->GetXaxis()->SetTitleOffset(0.65);
// 			for (int imom=0;imom<NMOMN;imom++){ 
// 				for (int ispe=0;ispe<NSPE;ispe++){
// 					ggpars[2][ispe][imom]	->Draw("LP");
// 				}
// 			}
// 		ccan[ican]->cd(4);
// 			++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(12.65,0,164.3,100000);
// 					frame[iframe]->SetTitle("Constant; #theta (deg); Counts");
// //			++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(-2.1,0,1.7,100000);
// //					frame[iframe]->SetTitle("Constant;Incidence #eta_{DET}; Counts");
// 					frame[iframe]->GetXaxis()->SetLabelOffset(-0.01);
// 					frame[iframe]->GetXaxis()->SetLabelSize(0.05);
// 					frame[iframe]->GetYaxis()->SetLabelSize(0.05);
// 					frame[iframe]->GetXaxis()->SetTitleSize(0.05);
// 					frame[iframe]->GetYaxis()->SetTitleSize(0.05);
// 					frame[iframe]->GetYaxis()->SetTitleOffset(1.6);
// 					frame[iframe]->GetXaxis()->SetTitleOffset(0.65);
// 			for (int imom=0;imom<NMOMN;imom++){ 
// 				for (int ispe=0;ispe<NSPE;ispe++){
// 					ggparsP[0][ispe][imom]	->Draw("LP");
// 				}
// 				//leg[imom]->Draw("same");
// 			}
// 		ccan[ican]->cd(5);
// 			++iframe; 
// //			frame[iframe] = (TH1F*)gPad->DrawFrame(12.65,790,164.3,850);
// 			frame[iframe] = (TH1F*)gPad->DrawFrame(12.65,816,164.3,830);
// 					frame[iframe]->SetTitle("Mean; #theta (deg); thetaC (mrad)");
// //			frame[iframe] = (TH1F*)gPad->DrawFrame(-2.1,760,1.7,830);
// //					frame[iframe]->SetTitle("Mean;Incidence #eta_{DET}; thetaC (mrad)");
// 					frame[iframe]->GetXaxis()->SetLabelOffset(-0.01);
// 					frame[iframe]->GetXaxis()->SetLabelSize(0.05);
// 					frame[iframe]->GetYaxis()->SetLabelSize(0.05);
// 					frame[iframe]->GetXaxis()->SetTitleSize(0.05);
// 					frame[iframe]->GetYaxis()->SetTitleSize(0.05);
// 					frame[iframe]->GetYaxis()->SetTitleOffset(1.35);
// 					frame[iframe]->GetXaxis()->SetTitleOffset(0.65);
// 			for (int imom=0;imom<NMOMN;imom++){ 
// 	 			for (int ispe=0;ispe<NSPE;ispe++){
// 	 				ggparsP[1][ispe][imom]	->Draw("LP");
//  				}
// 			}
// 		ccan[ican]->cd(6);
// 			++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(12.65,0,164.3,12);
// 					frame[iframe]->SetTitle("Std. Dev.; #theta (deg); Std.Dev. (mrad)");
// //			++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(-2.1,3,1.7,9);
// //					frame[iframe]->SetTitle("Std. Dev.;Incidence #eta_{DET}; Std.Dev. (mrad)");
// 					frame[iframe]->GetXaxis()->SetLabelOffset(-0.01);
// 					frame[iframe]->GetXaxis()->SetLabelSize(0.05);
// 					frame[iframe]->GetYaxis()->SetLabelSize(0.05);
// 					frame[iframe]->GetXaxis()->SetTitleSize(0.05);
// 					frame[iframe]->GetYaxis()->SetTitleSize(0.05);
// 					frame[iframe]->GetYaxis()->SetTitleOffset(1.1);
// 					frame[iframe]->GetXaxis()->SetTitleOffset(0.65);
// 			for (int imom=0;imom<NMOMN;imom++){ 
// 				for (int ispe=0;ispe<NSPE;ispe++){
// 					ggparsP[2][ispe][imom]	->Draw("LP");
// 				}
// 			}
// 		ccan[ican]->cd(); ccan[ican]->Update();
// 		ccan[ican]->Print("chrcorr.ps(");
// 


// 	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("Page %d",ican),10+30*ican,30+30*ican,1400,600);
// 	ccan[ican]->cd(); ccan[ican]->Divide(NANG,NMOMN,0.0001,0.0001);
// 	ipad=0;
// 	for (int imom=0;imom<NMOMN;imom++){
// 		for (int iang=0;iang<NANG;iang++){
// 			++ipad;
// 			ccan[ican]->cd(ipad);
// 			for (int ispe=0;ispe<NSPE;ispe++){
// 				if (ispe==0){ hthetaC_gatedchrom[iang][ispe][imom] ->Draw("hist"); } else
// 				if (ispe> 0){ hthetaC_gatedchrom[iang][ispe][imom] ->Draw("hist same"); }
// 				hthetaC_gatedchrom[iang][ispe][imom]	->GetXaxis()->SetRangeUser(800.,850.);
// 				hthetaC_gated[iang][ispe][imom] 		->Draw("hist same"); 
// 				hthetaC_gatedchrom[iang][ispe][imom]	->Draw("hist same");
// 				fthetaC_gated[iang][ispe][imom]			->Draw("same");
// 				fthetaC_gatedchrom[iang][ispe][imom]	->Draw("same");
// 				++itext; text[itext]->SetTextAlign(32); text[itext]->SetTextSize(0.05); 
// 					text[itext]->DrawLatex(0.96,0.92,Form("%s %sGeV %sdeg",species[ispe],momn[imom],angles[iang]));
// 			}
// 			gPad->Update();
// 			double ymax	= gPad->GetUymax();
// 			for (int ispe=0;ispe<NSPE;ispe++){
// 				double thCexp	= thetaCexp[ispe][imom];
// 				++iline; 
// 				   line[iline]	->SetLineColor(speciescol[ispe]); 
// 				   line[iline]	->DrawLine(thCexp,0,thCexp,ymax);				
// 			}
// 			//
// 		}
// 	}
// 	ccan[ican]->cd(); ccan[ican]->Update();
// 	ccan[ican]->Print("chrcorr.ps(");
	
	
	ccan[ican]->Print("chrcorr.ps]");
	int istat = gSystem->Exec("ps2pdf chrcorr.ps chrcorr.pdf");
	if (istat==0) gSystem->Exec("rm chrcorr.ps");
	cout<<"wrote chrcorr.pdf"<<endl;
	
}




// TFile *f1 = new TFile("dircGR_pi+3GeV30deg.root","read");
// TFile *f2 = new TFile("dircGR_kaon+3GeV30deg.root","read");
// TFile *f3 = new TFile("dircGR_proton3GeV30deg.root","read");
// 
// TH1D *h1	= (TH1D*)f1->Get("hthetaC"); h1->SetName("h1");
// TH1D *h2	= (TH1D*)f2->Get("hthetaC"); h2->SetName("h2");
// TH1D *h3	= (TH1D*)f3->Get("hthetaC"); h3->SetName("h3");
// 
// h1->SetLineWidth(3); h1->SetMarkerStyle(20); 	h1->SetMarkerColor(1); 			h1->SetLineColor(1); 
// h2->SetLineWidth(3); h2->SetMarkerStyle(20); 	h2->SetMarkerColor(kGreen+2); 	h2->SetLineColor(kGreen+2); 
// h3->SetLineWidth(3); h3->SetMarkerStyle(20); 	h3->SetMarkerColor(4); 			h3->SetLineColor(4); 
// 
// TLegend *leg	= new TLegend(0.2,0.5,0.4,0.6);
// 	leg->AddEntry(h1,"pi+ 3GeV 30deg","LP");
// 	leg->AddEntry(h2,"kaon+ 3GeV 30deg","LP");
// 	leg->AddEntry(h3,"proton 3GeV 30deg","LP");
// 
// gStyle->SetOptStat(0);
// 
// h1->Draw("hist");
// h2->Draw("hist same");
// h3->Draw("hist same");
// leg->Draw();
