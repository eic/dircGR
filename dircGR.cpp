
#define dircGR_cxx
#include "dircGR.h"
#include <iomanip> 

#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "TPaletteAxis.h"
#include "TPaveStats.h"
#include "TColor.h"

// #include "tcutL0_.h"		// double tcutL0_[4562]			index = round(inc_z + 2730)
// #include "tcutL1_.h"		// double tcutL1_[4562] 		index = round(inc_z + 2730)
// #include "tcutU0_.h"		// double tcutU0_[4562]			index = round(inc_z + 2730) 
// #include "tcutU1_.h"		// double tcutU1_[4562]			index = round(inc_z + 2730) 
// #include "cosangtmin.h"	// double cosangtmin[4562]		index = round(inc_z + 2730) 
#include "cosangupper.h"	// double cosangupper[4562]		index = round(inc_z + 2730) 
#include "cosanglower.h"	// double cosanglower[4562]		index = round(inc_z + 2730) 

double FindLowestXFilled(TH1D* h);
int phi_to_barbox(double f);

TF1 *fTimeCut1U;
TF1 *fTimeCut2U;
TF1 *fTimeCut1L;	// only needed for plotting (mirror of fTimeCut1U)
TF1 *fTimeCut2L;	// only needed for plotting (mirror of fTimeCut2U)

void dircGR::Loop(){


	double	Z_RO		= -3040.074;	//mm
	double	Z_barmin	= -2729.075;	//mm
	double	Z_barmax	=  1831.025;	//mm
	double  v_bar		=   199.5;		//mm/ns
	double	pixelsize	=     3.30;		//mm; length of edge of square pixel
	double	locxmin		=  -178.20;		//mm;
	double	locxmax		=   178.20;		//mm; note these dimensions are wider than actual prism... 
	double	locymin		=   739.55;		//mm;
	double	locymax		=   980.45;		//mm; note these dimensions are taller than actual prism... 
 	const int NPIXX		=   108;		//
	const int NPIXY		=    73;		//
	const int NBAR		=    10;		//
	cout<<"Number of Pixels (x,y,tot): "<<NPIXX<<" "<<NPIXY<<" "<<NPIXX*NPIXY<<endl;


	//---- PID space setup...
	const int		NHYP				= 3;
	const char*		hyp_name[NHYP]		= {    "pi",     "K",        "p"};
	const double	hyp_mass[NHYP]		= {0.139570,0.493677,0.938272013};
	const int		hyp_pdgid[NHYP]		= {     211,     321,       2212};
	const double	hyp_minp[NHYP]		= {   0.129,   0.456,      0.867};	// e: 0.00047, mu: 0.0976
// 	const int		NHYP				= 1;
// 	const char*		hyp_name[NHYP]		= {    "pi"};
// 	const double	hyp_mass[NHYP]		= {0.139570};
//	const int		hyp_pdgid[NHYP]		= {     211};
	//
	//---- calculate some thetaC variables
	double CerenkovAngleExpected	= 0;
	if (GunSim){
		double ve		= sqrt(Primary_mass*Primary_mass + Primary_momn*Primary_momn);
		double vg		= ve/Primary_mass;
		double vb		= sqrt(1.0 - (1.0/vg/vg));
		if (vb!=0.&&fabs(1.0/1.4738/vb)<=1.0){
			CerenkovAngleExpected	= acos(1.0/1.4738/vb);	// radians!
		} else {
			CerenkovAngleExpected	= 0;
		}
		//cout<<Primary_mass<<" "<<Primary_momn<<" "<<1000.*CerenkovAngleExpected<<endl;
	}
	//
	//---- define functions for thetaC vs ptot by hypothesis
	TF1* fthCexp_ptot[NHYP];
	for (int ihyp=0;ihyp<NHYP;ihyp++){
		fthCexp_ptot[ihyp]	= new TF1(Form("fthCexp_ptot%d",ihyp),"1000.*TMath::ACos(TMath::Sqrt(x*x+[0]*[0])/(x*1.4738))", hyp_minp[ihyp], 10);
		fthCexp_ptot[ihyp]	->SetNpx(20000);
		fthCexp_ptot[ihyp]	->SetParameter(0,hyp_mass[ihyp]);
		fthCexp_ptot[ihyp]	->SetLineColor(1);
		fthCexp_ptot[ihyp]	->SetLineStyle(1);
		fthCexp_ptot[ihyp]	->SetLineWidth(1);
		cout<<"thetaC at "<<hyp_minp[ihyp]<<" GeV for "<<hyp_name[ihyp]<<" : \t"<<fthCexp_ptot[ihyp]->Eval(hyp_minp[ihyp])<<endl;
	}
	
	
	//---- set the Chromatic Correction functions for this run using info from chrcorr.(C/root)
	//
	int kkspe=0,kkmom=0;	//!!!!! default chr corr from PIONS at 3GeV !!!!!
	if (GunSim){
		// set specific chr corr?
	}
	TFile *fchrcorr	= new TFile("./chrcorr.root","READ");
		TGraph* gfchr1par0_Theta	= static_cast<TGraph*>(fchrcorr->Get(Form("gchr1par0_Theta_%d_%d"  ,kkspe,kkmom)));	// ispe, imom
		TGraph* gfchr1par1_Theta	= static_cast<TGraph*>(fchrcorr->Get(Form("gchr1par1_Theta_%d_%d"  ,kkspe,kkmom)));	// ispe, imom
		TGraph* gfchr2par0_Theta	= static_cast<TGraph*>(fchrcorr->Get(Form("gchr2par0_Theta_%d_%d"  ,kkspe,kkmom)));	// ispe, imom
		TGraph* gfchr2par1_Theta	= static_cast<TGraph*>(fchrcorr->Get(Form("gchr2par1_Theta_%d_%d"  ,kkspe,kkmom)));	// ispe, imom
		TGraph* gChrom1Inter_Theta	= static_cast<TGraph*>(fchrcorr->Get(Form("gChrom1par0_Theta_%d_%d",kkspe,kkmom)));	// ispe, imom
		TGraph* gChrom1Slope_Theta	= static_cast<TGraph*>(fchrcorr->Get(Form("gChrom1par1_Theta_%d_%d",kkspe,kkmom)));	// ispe, imom
		TGraph* gChrom2Inter_Theta	= static_cast<TGraph*>(fchrcorr->Get(Form("gChrom2par0_Theta_%d_%d",kkspe,kkmom)));	// ispe, imom
		TGraph* gChrom2Slope_Theta	= static_cast<TGraph*>(fchrcorr->Get(Form("gChrom2par1_Theta_%d_%d",kkspe,kkmom)));	// ispe, imom
	fchrcorr->Close();
	TF1 *fchr1Use	= new TF1("fchr1Use"  ,"pol1",295,655);			// parameters set for each primary separately below...
	TF1 *fchr2Use	= new TF1("fchr2Use"  ,"pol1",295,655);			// parameters set for each primary separately below...
	TF1 *fChrom1Use	= new TF1("fChrom1Use","pol1",-0.001,0.001);	// parameters set for each primary separately below...
	TF1 *fChrom2Use	= new TF1("fChrom2Use","pol1",-0.001,0.001);	// parameters set for each primary separately below...
	
	
	//---- define time cut functions. Pars are set for each incident particle separately!
	fTimeCut1U	= new TF1("fTimeCut1U","pol1",0,100);
	fTimeCut2U	= new TF1("fTimeCut2U","pol1",0,100);
	fTimeCut1L	= new TF1("fTimeCut1L","pol1",0,100);	// used for plotting only
	fTimeCut2L	= new TF1("fTimeCut2L","pol1",0,100);	// used for plotting only
	fTimeCut1U->SetLineColor(kOrange+8);	fTimeCut1L->SetLineColor(kOrange+8);
	fTimeCut2U->SetLineColor(kOrange+8);	fTimeCut2L->SetLineColor(kOrange+8);
	//
	//---- read limits from bands.C...
	//TString bandfile	= TString("./bands.root");
	//TFile* fBand		= new TFile(bandfile.Data(),"read");
	//for (int izd=0;izd<2;izd++){
	//	gtcutL[izd]	= (TGraph*)fBand->Get(Form("gtcutL_%d",izd));	// earliest time vs Zinc (as OPcr_z)
	//	gtcutL[izd]	->SetName(Form("gtcutL_%d",izd));
	//	gtcutU[izd]	= (TGraph*)fBand->Get(Form("gtcutU_%d",izd));	// latest time vs Zinc (as OPcr_z) for some MIN cos(theta)
	//	gtcutU[izd]	->SetName(Form("gtcutU_%d",izd));
	//}
	//fBand->Close();
	//delete fBand; fBand=0;


	//---- book....
	//
	double tbeg1hist	= 0;
	double tbeg2hist	= 0;
	double tbeg1OPhist	= 0;
	double tbeg2OPhist	= 0;
	//
	TFile* fout	= new TFile(OutputFile.Data(),"recreate");
	fout->cd();
	TH1D* hnhits		= new TH1D("hnhits"       ,"hnhits"      ,kMaxDIRCBarHits,0.5,((double)kMaxDIRCBarHits)+0.5);
	TH2D* hboxphi		= new TH2D("hboxphi"      ,"box vs hit phi",100,-M_PI,M_PI,13,-1.5,11.5);
	TH2D* hnhitninc		= new TH2D("hnhitninc"    ,"nhit vs ninc"    ,50,-0.5,49.5,1000,-0.5,9999.5);
	TH2D* hnhitnprimary	= new TH2D("hnhitnprimary","nhit vs nprimary",50,-0.5,49.5,1000,-0.5,9999.5);
	TH2D* hpixelplane	= new TH2D("hpixelplane"  ,"hpixelplane" ,NPIXX,locxmin,locxmax,NPIXY,locymin,locymax);
	TH2D* hpixel		= new TH2D("hpixel"       ,"hpixel"      ,NPIXX,-0.5, ((double)NPIXX)-0.5,NPIXY,-0.5,((double)NPIXY)-0.5 );
	TH2D* hpixeltime	= new TH2D("hpixeltime"   ,"hpixeltime"  ,NPIXX,-0.5, ((double)NPIXX)-0.5,NPIXY,-0.5,((double)NPIXY)-0.5 );
	TH2D* hpixel_gated	= new TH2D("hpixel_gated" ,"hpixel_gated",NPIXX,-0.5, ((double)NPIXX)-0.5,NPIXY,-0.5,((double)NPIXY)-0.5 );
	TH2D* hpixeltime_gated	= new TH2D("hpixeltime_gated"   ,"hpixeltime_gated"  ,NPIXX,-0.5, ((double)NPIXX)-0.5,NPIXY,-0.5,((double)NPIXY)-0.5 );
	TH1D* hthetaC		= new TH1D("hthetaC"      ,"hthetaC (mrad)",1000,350.,1350.);
	TH2D* hdt1texp1		= new TH2D("hdt1texp1"    ,"t_{HIT}-t_{EXP1} vs t_{EXP1}, crude-gated",400,0.,100.,80,-2.,2.);
	TH2D* hdt2texp2		= new TH2D("hdt2texp2"    ,"t_{HIT}-t_{EXP2} vs t_{EXP2}, crude-gated",400,0.,100.,80,-2.,2.);
	TH2D* hthittexp1	= new TH2D("hthittexp1"   ,"t_{HIT} vs t_{EXP1}",400,10,110,400,10,110);
	TH2D* hthittexp2	= new TH2D("hthittexp2"   ,"t_{HIT} vs t_{EXP2}",400,10,110,400,10,110);
	TH2D* hthittexp1_gated	= new TH2D("hthittexp1_gated","t_{HIT} vs t_{EXP1}, crude-gated",400,10,110,400,10,110);
	TH2D* hthittexp2_gated	= new TH2D("hthittexp2_gated","t_{HIT} vs t_{EXP2}, crude-gated",400,10,110,400,10,110);
	TH2D* hthitzdir_gated	= new TH2D("hthitzdir_gated" ,"t_{HIT} vs OP zdir, Time gated",3,-1.5,1.5,400,10,110);
	//
	TH1D* hthetaCest			= new TH1D("hthetaCest"      ,"thetaC Estimate (mrad)"          ,875,0.,875.);
		  hthetaCest			->SetLineColor(1);
	TH1D* hthetaCestPeak		= new TH1D("hthetaCestPeak"  ,"thetaC Estimate (mrad), peak-avg",875,0.,875.);
		  hthetaCestPeak		->SetLineColor(kGreen+2);	
	TH1D* hthetaCestHypMost		= new TH1D("hthetaCestHypMost","thetaC Estimate (mrad), avg over only most populous ihyp",875,0.,875.);
		  hthetaCestHypMost		->SetLineColor(kRed);	
	TH1D* hthetaCestHypMostPeak	= new TH1D("hthetaCestHypMostPeak","thetaC Estimate (mrad), avg over only peak in most populous ihyp",875,0.,875.);
		  hthetaCestHypMostPeak	->SetLineColor(kMagenta);	
	//
	TH1D* hthetaCest_W			= static_cast<TH1D*>(hthetaCest->Clone("hthetaCest_W"));
	TH1D* hthetaCestHypMost_W	= static_cast<TH1D*>(hthetaCestHypMost->Clone("hthetaCestHypMost_W"));
	//
	TH1D* hhyp_best			= new TH1D("hhyp_best"       ,"Best Hypothesis Index",3,-0.5,2.5);
	TH1D* hAMBindex			= new TH1D("hAMBindex"       ,"Best Ambiguity Index",8,-0.5,7.5);
	const char* AMBnames[8]	= {"LUTpath","flip x","flip y","flip x & y","flip z","flip x & z","flip y & z","flip all"};	// what was flipped...
	//
	TH1D* hthetaC_gated			= new TH1D("hthetaC_gated","hthetaC (mrad), Time gated",1000,350.,1350.);
		  hthetaC_gated			->SetLineColor(6);
	TH1D* hthetaC_gatedchrom	= new TH1D("hthetaC_gatedchrom","hthetaC (mrad), Time gated, Chromatic Corr",1000,350.,1350.);
		  hthetaC_gatedchrom	->SetLineColor(4);
	TH1D* hdthetaC_gated		= new TH1D("hdthetaC_gated","thetaC-exp(hyp) (mrad), Time gated",1000,-40,40);
		  hdthetaC_gated		->SetLineColor(6);
	TH1D* hdthetaC_gatedchrom	= new TH1D("hdthetaC_gatedchrom","thetaC-exp(hyp) (mrad), Time gated, Chromatic Corr",1000,-40,40);
		  hdthetaC_gatedchrom	->SetLineColor(4);
	//
	double cacutl	= 	700;	//1000.*CerenkovAngleExpected - 25;
	double cacutu	= 	900;	//1000.*CerenkovAngleExpected + 25;
	//cout<<cacutl<<" "<<cacutu<<endl;
	TH2D* hthetaC_dt1		= new TH2D("hthetaC_dt1"      ,"thetaC (mrad) vs t_{HIT}-t_{EXP1}/L_{PATH}",44,-0.00022,0.00022,200,cacutl,cacutu);
	TH2D* hthetaC_dt2		= new TH2D("hthetaC_dt2"      ,"thetaC (mrad) vs t_{HIT}-t_{EXP2}/L_{PATH}",44,-0.00022,0.00022,200,cacutl,cacutu);
	TH2D* hthetaC_dt1_gated	= new TH2D("hthetaC_dt1_gated","thetaC (mrad) vs (t_{HIT}-t_{EXP1})/L_{PATH}, Time-gated",44,-0.00022,0.00022,200,cacutl,cacutu);
	TH2D* hthetaC_dt2_gated	= new TH2D("hthetaC_dt2_gated","thetaC (mrad) vs (t_{HIT}-t_{EXP2})/L_{PATH}, Time-gated",44,-0.00022,0.00022,200,cacutl,cacutu);
	TH2D* hthetaCchr_dt1_gated	= new TH2D("hthetaCchr_dt1_gated","thetaC (mrad) vs t_{HIT}-t_{EXP1}/L_{PATH}, Time-gated, Chromatic-corr",44,-0.00022,0.00022,200,cacutl,cacutu);
	TH2D* hthetaCchr_dt2_gated	= new TH2D("hthetaCchr_dt2_gated","thetaC (mrad) vs t_{HIT}-t_{EXP2}/L_{PATH}, Time-gated, Chromatic-corr",44,-0.00022,0.00022,200,cacutl,cacutu);
	//
	TH2D* hthetaC_WL1_gated	= new TH2D("hthetaC_WL1_gated","thetaC (mrad) vs WL (nm), path1, Time-gated",36,295,655,100,cacutl,cacutu);
	TH2D* hthetaC_WL2_gated	= new TH2D("hthetaC_WL2_gated","thetaC (mrad) vs WL (nm), path2, Time-gated",36,295,655,100,cacutl,cacutu);
	TH2D* hthetaCchr_WL1_gated	= new TH2D("hthetaCchr_WL1_gated","thetaC (mrad) vs WL (nm), path1, Time-gated, Chromatic-corr",36,295,655,100,cacutl,cacutu);
	TH2D* hthetaCchr_WL2_gated	= new TH2D("hthetaCchr_WL2_gated","thetaC (mrad) vs WL (nm), path2, Time-gated, Chromatic-corr",36,295,655,100,cacutl,cacutu);
	//
	TH2D* hWL_dt1_gated		= new TH2D("hWL_dt1_gated","WL (nm) vs (t_{HIT}-t_{EXP1})/L_{PATH}, path1, Time-gated",88,-0.00022,0.00022,120,295,655);
	TH2D* hWL_dt2_gated		= new TH2D("hWL_dt2_gated","WL (nm) vs (t_{HIT}-t_{EXP2})/L_{PATH}, path2, Time-gated",88,-0.00022,0.00022,120,295,655);
	//
	TH1D* hcosanglut	= new TH1D("hcosanglut","hcosanglut",1001,-1.001,1.001);
	//
	TH1D* hinfo			= new TH1D("hinfo","hinfo",100,0.5,100.5);
	if (GunSim){
		  hinfo->SetBinContent( 1, Primary_mass);
		  hinfo->SetBinContent( 2, Primary_momn);
		  hinfo->SetBinContent( 3, 1000.*CerenkovAngleExpected);
	}
	//
	//---- PID thetaC calculation working hists/vectors... (reset every incident particle)...
	//
	TH1D *hBestHypothesis	= new TH1D("hBestHypothesis","Best Hypothesis for this incident particle",NHYP,-0.5,((double)NHYP)-0.5);
	std::vector<double> thetasHyp[NHYP];
	//
	TH2D* hthetaC_ptot	= new TH2D("hthetaC_ptot","thetaC (mrad) vs momentum",320,0,8.,875,0.,875.);
	TH2D* hthetaC_ptot_byTruth[NHYP]	= {nullptr};
	TH2D* hdthetaC_ptot_byTruth[NHYP]	= {nullptr};
	TH2D* hdthetaC_bar_byTruth[NHYP]	= {nullptr};
	TH2D* hdthetaC_eta_byTruth[NHYP]	= {nullptr};
	TH2D* hdthetaC_nhbox_byTruth[NHYP]	= {nullptr};
	for (int ihyp=0;ihyp<NHYP;ihyp++){
		hthetaC_ptot_byTruth[ihyp]	= new TH2D(Form("hthetaC_ptot_byTruth%d" ,ihyp) ,Form("thetaC (mrad) vs momentum, truth %s",hyp_name[ihyp]),320,0,8.,875,0.,875.);
		hdthetaC_ptot_byTruth[ihyp]	= new TH2D(Form("hdthetaC_ptot_byTruth%d",ihyp) ,Form("dthetaC (mrad) vs momentum, truth %s",hyp_name[ihyp]),320,0,8.,120,-30,30);
		hdthetaC_bar_byTruth[ihyp]	= new TH2D(Form("hdthetaC_bar_byTruth%d",ihyp)  ,Form("dthetaC (mrad) vs bar, truth %s",hyp_name[ihyp]),10,-0.5,9.5,120,-30,30);
		hdthetaC_eta_byTruth[ihyp]	= new TH2D(Form("hdthetaC_eta_byTruth%d",ihyp)  ,Form("dthetaC (mrad) vs #eta, truth %s",hyp_name[ihyp]),36,-2,1.6,120,-30,30);
		hdthetaC_nhbox_byTruth[ihyp]= new TH2D(Form("hdthetaC_nhbox_byTruth%d",ihyp),Form("dthetaC (mrad) vs NhitsBox, truth %s",hyp_name[ihyp]),40,0.5,40.5,120,-30,30);
	}
	TH1D* hhypParent_hit_W	= new TH1D("hhypParent_hit_W","Track working hist: Parent Hypothesis"                 ,4,-1.5,2.5);
	TH1D* hhypParent_hit	= new TH1D("hhypParent_hit"  ,"Track's Most Populous Parent Hypothesis"               ,3,-0.5,2.5);
	TH1D* hHitPurity		= new TH1D("hHitPurity"      ,"Hit Purity for Track's Most Populous Parent Hypothesis",101,0,1.01);
	//
	//
	//---- event displays
// 	TH2D *hyx0	= new TH2D("hyx0","event0, hyx",240,-1200,1200,240,-1200,1200);
// 	TH2D *hyx1	= new TH2D("hyx1","event1, hyx",240,-1200,1200,240,-1200,1200);
// 	TH2D *hyx2	= new TH2D("hyx2","event2, hyx",240,-1200,1200,240,-1200,1200);
// 	TH2D *hyx3	= new TH2D("hyx3","event3, hyx",240,-1200,1200,240,-1200,1200);
// 	TH2D *hyxr0	= new TH2D("hyxr0","event0, hyxr",240,-1200,1200,240,-1200,1200);
// 	TH2D *hyxr1	= new TH2D("hyxr1","event1, hyxr",240,-1200,1200,240,-1200,1200);
// 	TH2D *hyxr2	= new TH2D("hyxr2","event2, hyxr",240,-1200,1200,240,-1200,1200);
// 	TH2D *hyxr3	= new TH2D("hyxr3","event3, hyxr",240,-1200,1200,240,-1200,1200);
	//
	//---- END BOOKING....
	

	//---- read 10 LUT trees for 10 bars....
	//
	int	ipixx,ipixy,ipath,npath,nOPpath;
	double OPprob,OPdirx,OPdiry,OPdirz,OPdir,OPtime;
	static int                 LUT_Npaths[NPIXX][NPIXY][NBAR] = {0};
	static std::vector<double> LUT_OPprob[NPIXX][NPIXY][NBAR];
	static std::vector<double> LUT_OPdirx[NPIXX][NPIXY][NBAR];
	static std::vector<double> LUT_OPdiry[NPIXX][NPIXY][NBAR];
	static std::vector<double> LUT_OPdirz[NPIXX][NPIXY][NBAR];
	static std::vector<double> LUT_OPtime[NPIXX][NPIXY][NBAR];
	for (int ibar=0;ibar<NBAR;ibar++){
		TString lutfile	= TString(Form("./LUT/lut_bar%d.root",ibar));
		TFile* fLUT		= new TFile(lutfile.Data(),"read");
		TTree* dircLUT	= static_cast<TTree*>(fLUT->Get("dircLUT"));
			   dircLUT	->SetBranchAddress("ipixx" , &ipixx  );
			   dircLUT	->SetBranchAddress("ipixy" , &ipixy  );
			   dircLUT	->SetBranchAddress("ipath" , &ipath  );
			   dircLUT	->SetBranchAddress("npath" , &npath  );
			   dircLUT	->SetBranchAddress("nOP"   , &nOPpath);
			   dircLUT	->SetBranchAddress("OPprob", &OPprob );
			   dircLUT	->SetBranchAddress("OPdirx", &OPdirx );
			   dircLUT	->SetBranchAddress("OPdiry", &OPdiry );
			   dircLUT	->SetBranchAddress("OPdirz", &OPdirz );
			   dircLUT	->SetBranchAddress("OPtime", &OPtime );
		int maxnpaths	= 0;
		int totnpaths	= 0;
		int nentLUT 	= (Int_t)dircLUT->GetEntries();
		double minOPt	= 99999;
		for (int ie=0; ie<nentLUT; ie++){
			dircLUT->GetEntry(ie);
			  LUT_OPprob[ipixx][ipixy][ibar].push_back(OPprob);
			  LUT_OPdirx[ipixx][ipixy][ibar].push_back(OPdirx);
			  LUT_OPdiry[ipixx][ipixy][ibar].push_back(OPdiry);
			  LUT_OPdirz[ipixx][ipixy][ibar].push_back(OPdirz);
			  LUT_OPtime[ipixx][ipixy][ibar].push_back(OPtime);
			++LUT_Npaths[ipixx][ipixy][ibar];
			totnpaths	+= 1;
			if (LUT_Npaths[ipixx][ipixy][ibar]>maxnpaths) maxnpaths=LUT_Npaths[ipixx][ipixy][ibar];
			if (OPtime<minOPt){ minOPt=OPtime; }
		}
		cout<<"BAR"<<ibar<<":\t read "<<nentLUT<<" paths for "<<NPIXX<<" x "<<NPIXY<<" pixels from LUT file "<<lutfile<<endl;
		//cout<<"BAR"<<ibar<<"\t....  average number of paths per pixel = "<<totnpaths/NPIXX/NPIXY<<endl;
		//cout<<"BAR"<<ibar<<"\t....   largest npaths across all pixels = "<<maxnpaths<<endl;
		//cout<<"BAR"<<ibar<<"\t....                        min OP time = "<<minOPt<<endl;
		fLUT->Close();
		//delete fLUT; fLUT=0;
		//
	}	//---- end LUT read loop over bars [0,9]

	//---- main event loop...
	//
	if (fChainSIM     == 0){ cout<<"no sim tree!"      <<endl; exit(0); }
	if (IncidenceTree == 0){ cout<<"no incidence tree!"<<endl; exit(0); }
	Long64_t nentries	= fChainSIM->GetEntries();
	Long64_t nentinc	= IncidenceTree->GetEntries();
//nentries=10000;
	cout<<"number of entries: "<<nentries<<" edm4hep.root, "<<nentinc<<" incidence.root"<<endl;
	//---- set list of active branches...
	// 	std::vector<std::string> activeBranchNames = {
	// 		"DIRCBarHits",             
	// 		"DIRCBarHits.cellID",      
	// 		"DIRCBarHits.eDep", 		  
	// 		"DIRCBarHits.time", 		  
	// 		"DIRCBarHits.position.x",  
	// 		"DIRCBarHits.position.y",  
	// 		"DIRCBarHits.position.z",  
	// 		"DIRCBarHits.momentum.x",  
	// 		"DIRCBarHits.momentum.y",  
	// 		"DIRCBarHits.momentum.z",  
	// 		"EventHeader", 			  
	// 		"EventHeader.eventNumber"
	// 	};
	// 	//---- now get the branch pointers...
	// 	std::vector<TBranch*> activeBranches;
	// 	for (const auto& name : activeBranchNames) {
	// 		TBranch* b = fChainSIM->GetBranch(name.c_str());
	// 		if (b) activeBranches.push_back(b);
	// 		std::cout<<"\tactive branch: "<<std::setw(25)<<name<<std::endl;
	// 	}
	//
	//---- these are outside the loop for convenience in gun sims... (so available in painting at end)
	double tbeg1;				// causal calc
	double tbeg2;				// causal calc
	double tbeg1OP;				// goofy calc 
	double tbeg2OP;				// goofy calc
	double tbeg1OPband;			// from bands.C
	double tbeg2OPband;			// from bands.C
	double tend1OPband;			// from bands.C
	double tend2OPband;			// from bands.C
// 	double vtcutL1;				// from bands.C, just for drawing
// 	double vtcutU1;				// from bands.C, just for drawing
// 	double vtcutL2;				// from bands.C, just for drawing
// 	double vtcutU2;				// from bands.C, just for drawing
	double LIM_cosang_upper;	// from bands.C,  largest allowed OP cos(theta) for this Z-incidence
	double LIM_cosang_lower;	// from bands.C, smallest allowed OP cos(theta) for this Z-incidence
	//
	bool   firstcall = true; 
	ULong_t ievtsim	= -1;
	int  	ievtinc	=  0;
	bool scroll		= false;
	int  nhitmax	=  0;
	int  jentryinc	=  0;
	for (Long64_t jentry=0; jentry<nentries;jentry++) {
		//
		ClearBuffers();
		//
		//---- try to get the same event from both trees... 
		Long64_t ientry	= LoadTree(jentry);	if (ientry<0){ break; }
		Long64_t nb 	= fChainSIM->GetEntry(jentry);	// get simulation data...
		if (nb==0) break;								// break if sim stream ends...		
		//bool readError	= false;
		//for (TBranch* b : activeBranches) {
		//	if (b->GetEntry(ientry) <= 0) { 			// get simulation data...
		//		readError = true;						// Returns 0 at EOF, negative on error
		//		break;									// break branch loop if sim stream ends...		
		//	}
		//}
		//if (readError) break; 						// break event read if sim stream ends...
		ievtsim			= EventHeader_eventNumber[0];	// set sim event number...
		if (DIRCBarHits_>nhitmax){ nhitmax=DIRCBarHits_; }		// solely for diagnostics...
		if (!scroll){
			Long64_t centry	= IncidenceTree->LoadTree(jentryinc); if (centry<0){ break; }
			Long64_t nbi	= IncidenceTree->GetEntry(jentryinc);	// get incidence data...
			if (nbi==0) break;							// break if inc stream ends...
			ievtinc		= inc_evt;						// set inc event number...
			++jentryinc;
		}
		if (ievtinc<ievtsim){							// decide what tree to read next...
			cout<<"incidence tree ahead of sim tree - should not be possible!"<<endl;
			cout<<"\t jentry= "<<jentry<<"\t ievtinc= "<<ievtinc<<"\t ievtsim= "<<ievtsim<<endl;
			exit(0);
		} else if (ievtinc>ievtsim){
			scroll	= true;
			continue;
		}
		scroll	= false;	
		//---- event (2 trees, sync'd) in hand....
		//
		UpdateEDM4HepCounters();
		//
		hnhits	->Fill(DIRCBarHits_);
		if (jentryinc%5000==0)cout<<"processing "<<jentry<<" "<<jentryinc<<endl;
		//
		//cout<<jentry<<" "<<DIRCBarHits_<<" ---------------------------"<<endl;
		//if (DIRCBarHits_>10000) continue;
		//
		//---- O.K. we're going to analyze this one... 
		//---- start loop over particles incident on dirc bars from incidence tree
		//
		int numprimary		=  0;
		int nhitbox_evt[12]	= {0};
		for (int iinc=0;iinc<inc_ninc;iinc++){
			if ( inc_primary[iinc] != 0){ ++numprimary; }
			++nhitbox_evt[inc_box[iinc]];
		}
		hnhitninc		->Fill(inc_ninc,DIRCBarHits_);
		hnhitnprimary	->Fill(numprimary,DIRCBarHits_);
		//
		for (int iinc=0;iinc<inc_ninc;iinc++){
			//			
			if ( inc_primary[iinc] == 0){ continue; }
			//
			//---- get expected cerenkov angles for a track of this momentum
			double thetaCexp[NHYP]	=   {0};
			double thetaCexp_minhyp	= 99999;	// smallest thetaC amongst the hypotheses at this momentum
			double thetaCexp_maxhyp	=     0;	//  largest thetaC amongst the hypotheses at this momentum
	 		double ptot		= sqrt(inc_px[iinc]*inc_px[iinc] + inc_py[iinc]*inc_py[iinc] + inc_pz[iinc]*inc_pz[iinc]);
			for (int ihyp=0;ihyp<NHYP;ihyp++){
				double beta	= ptot/sqrt(ptot*ptot + hyp_mass[ihyp]*hyp_mass[ihyp]);
				if (beta >= 1/1.4738){ 
					thetaCexp[ihyp] = acos(1./beta/1.4738); 
				    if (thetaCexp[ihyp]<thetaCexp_minhyp){ thetaCexp_minhyp = thetaCexp[ihyp]; }
				    if (thetaCexp[ihyp]>thetaCexp_maxhyp){ thetaCexp_maxhyp = thetaCexp[ihyp]; }
				} else { 
					thetaCexp[ihyp] = 0.;
				}
			}
			//
			//---- get some useful quantities (time cuts, minpolar angle) from incidence info
			int	   kTruthPDG		= inc_pdgCode[iinc];
			int	   ihyp_TruthParent	= -1;
			for (int ihyp=0;ihyp<NHYP;ihyp++){
				if (abs(kTruthPDG)==hyp_pdgid[ihyp]){ ihyp_TruthParent = ihyp; }
			}
			double inc_ptot		= sqrt(inc_px[iinc]*inc_px[iinc] + inc_py[iinc]*inc_py[iinc] + inc_pz[iinc]*inc_pz[iinc]);
			TVector3 fPrimaryDircDir(  inc_px[iinc]/inc_ptot,      inc_py[iinc]/inc_ptot,      inc_pz[iinc]/inc_ptot    );
	 		double dist			= sqrt(inc_x[iinc]*inc_x[iinc] + inc_y[iinc]*inc_y[iinc] + inc_z[iinc]*inc_z[iinc]);
			double DetPhi		= atan2(inc_y[iinc],inc_x[iinc]);
			double DetTheta		= acos(inc_z[iinc]/dist);
			double inc_eta		= -log(tan(DetTheta/2.0));
			int    kTrackBarBox	= inc_box[iinc];
			TVector3 fPrimaryDircDirRot	= fPrimaryDircDir;
					 fPrimaryDircDirRot.RotateZ( -kTrackBarBox*30.*(M_PI/180.) );
			//
			int    kBar			= inc_bar[iinc];
			double dZ1			= inc_z[iinc] - Z_RO;							// !!!!distance to PMT!!!!
			double dZ2			= (Z_barmax - inc_z[iinc]) + (Z_barmax-Z_RO);	// distance to mirror end and back to PMT!
		 	tbeg1				= dZ1/v_bar + inc_t[iinc];						// assuming straight line path down axis of bar+prism
			tbeg2				= dZ2/v_bar + inc_t[iinc];						// assuming straight line path down axis of bar+prism
			double polarOP = 0;		// polarOP considers fact that straight line path down axis is not possible near mid-rapidity!!!
			if (DetTheta <  M_PI/2.){ polarOP = DetTheta - thetaCexp_maxhyp; } else	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			                        { polarOP = DetTheta + thetaCexp_maxhyp; } 		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if (polarOP<    0) polarOP =    0;
			if (polarOP>=M_PI) polarOP = M_PI;
			tbeg1OP				= dZ1/fabs(cos(polarOP))/v_bar + inc_t[iinc];	// considers min angle possible wrt bar axis given values of theta and thetaC...
			tbeg2OP				= dZ2/fabs(cos(polarOP))/v_bar + inc_t[iinc];	// considers min angle possible wrt bar axis given values of theta and thetaC...
			if (DetTheta*(180./M_PI)>85&&DetTheta*(180./M_PI)<95){
				tbeg1OP	-= 0.5;
				tbeg2OP	-= 0.5;
			}
			//---- define the two time cut functions for this primary...
			double x1,x2,y1,y2,slope,yinter;
			double cut1d1	= 1.0;			// time cut half-width in ns at t=tbeg1OP      or tbeg2OP
			double cut1d2	= 1.0;			// time cut half-width in ns at t=tbeg1OP+10ns or tbeg2OP+10ns
			double cut2d1	= 1.0;			// time cut half-width in ns at t=tbeg1OP      or tbeg2OP
			double cut2d2	= 1.0;			// time cut half-width in ns at t=tbeg1OP+10ns or tbeg2OP+10ns
			//---- set tight time cut functions with fixed width of +-[cutd1] ns at t=tbeg1OP and expanding with texp1...
			x1=tbeg1OP;	x2=tbeg1OP+10.;	y1=cut1d1;	y2=cut1d2;	slope=(y2-y1)/(x2-x1);	yinter= y2-slope*x2; 
			fTimeCut1U	->SetParameter(0, yinter);	fTimeCut1U	->SetParameter(1, slope);
			fTimeCut1L	->SetParameter(0,-yinter);	fTimeCut1L	->SetParameter(1,-slope);	// plotting only
			//---- set tight time cut functions with fixed width of +-[cutd2] ns at t=tbeg2OP and expanding with texp2...
			x1=tbeg2OP;	x2=tbeg2OP+10.;	y1=cut2d1;	y2=cut2d2;	slope=(y2-y1)/(x2-x1);	yinter= y2-slope*x2; 
			fTimeCut2U	->SetParameter(0, yinter);	fTimeCut2U	->SetParameter(1, slope);
			fTimeCut2L	->SetParameter(0,-yinter);	fTimeCut2L	->SetParameter(1,-slope);	// plotting only
			//---- grab some numbers from Bands cuts (useful for painting in gunsim's)
			//int kkz			= kzFromZ(inc_z[iinc]);
			//double prismtime	= 1.58;	 	// shortest time across prism!
			//tbeg1OPband		= tcutL0_[kkz] + inc_t[iinc] + prismtime;
			//tend1OPband		= tcutU0_[kkz] + inc_t[iinc] + prismtime;
			//tbeg2OPband		= tcutL1_[kkz] + inc_t[iinc] + prismtime;
			//tend2OPband		= tcutU1_[kkz] + inc_t[iinc] + prismtime;
			//double cangtmin	= cosangtmin[kkz];
			//tbeg1OPband			= dZ1/fabs(cangtmin)/v_bar + inc_t[iinc];
			//tbeg2OPband			= dZ2/fabs(cangtmin)/v_bar + inc_t[iinc];
			//tend1OPband			= tbeg1OPband + 10;		// dummy value.... plotting only
			//tend2OPband			= tbeg2OPband + 10;		// dummy value.... plotting only
			int kkz				= kzFromZ(inc_z[iinc]);
			LIM_cosang_upper	= cosangupper[kkz];		// smallest allowed OP cos(theta) for this Z value
			LIM_cosang_lower	= cosanglower[kkz];		// largest  allowed OP cos(theta) for this Z value
			//
			if (firstcall && GunSim){		// fill hinfo with first primary, GunSim only
				firstcall	= false;
				hinfo->SetBinContent(4 ,DetTheta);
				hinfo->SetBinContent(5 ,dZ1);
				hinfo->SetBinContent(6 ,dZ2);
				hinfo->SetBinContent(7 ,inc_t[iinc]);
				//hinfo->SetBinContent(8 ,tbeg1);
				//hinfo->SetBinContent(9 ,tbeg2);
				hinfo->SetBinContent(8,tbeg1OP);
				hinfo->SetBinContent(9,tbeg2OP);
			}
			//
			//---- initialize/reset variables usefule for PID thetaC estimator calculations...
			double thetaCest			= 0.;	// thetaC estimate for this track (mrad)
			int    thetaCestN			= 0;	// number of lit-pixel-paths included in thetaC estimate for this track
				  hthetaCest_W			->Reset();
			      hBestHypothesis		->Reset();
			for (int ihyp=0;ihyp<NHYP;ihyp++){ thetasHyp[ihyp].clear(); }
			double thetaCestHypMost		= 0.;
				  hthetaCestHypMost_W	->Reset();
			hhypParent_hit_W			->Reset();	// to find most populous parent hypothesis over all hits passing the time cuts for this incident particle
			//
			//
			//---- loop over all hits, and skip the ones in the wrong barbox for this incident particle...
			for (int ihit=0;ihit<DIRCBarHits_;ihit++){
				//
				//cout<<ihit<<" "<<DIRCBarHits_<<"\t "
				//	<<DIRCBarHits_position_x[ihit]<<" "
				//	<<DIRCBarHits_position_y[ihit]<<" "
				//	<<DIRCBarHits_position_z[ihit]<<endl;
				//
				//---- fill event displays for first few events for diagnostics...
				double hitt		= DIRCBarHits_time[ihit];			// OP exit
				double hitx		= DIRCBarHits_position_x[ihit];		// OP exit
				double hity		= DIRCBarHits_position_y[ihit];		// OP exit
				double hitz		= DIRCBarHits_position_z[ihit];		// OP exit
				//if (iinc==0){	// only fill hit position pattern once per event!
				//	if (jentry==0)hyx0->Fill(hitx,hity);
				//	if (jentry==1)hyx1->Fill(hitx,hity);
				//	if (jentry==2)hyx2->Fill(hitx,hity);
				//	if (jentry==3)hyx3->Fill(hitx,hity);
				//}
				//
				//---- check that barbox for this hit equals the expected barbox for the primary!!!
				double HitPhi	= atan2(hity,hitx);					// in epic coord system!
				int kHitBarBox	= phi_to_barbox(HitPhi);			// barbox ID for this hit
				hboxphi			->Fill(HitPhi,kHitBarBox);
				if (kHitBarBox != kTrackBarBox){ continue; }		// this hit is in a different barbox than the track!!
				//
				//---- this hit is in the same barbox as the track!
				double momx		= DIRCBarHits_momentum_x[ihit];				// OP exit
				double momy		= DIRCBarHits_momentum_y[ihit];				// OP exit
				double momz		= DIRCBarHits_momentum_z[ihit];				// OP exit
				double momtot	= sqrt(momx*momx + momy*momy + momz*momz);
				double dirx		= DIRCBarHits_momentum_x[ihit] / momtot;	// OP exit
				double diry		= DIRCBarHits_momentum_y[ihit] / momtot;	// OP exit
				double dirz		= DIRCBarHits_momentum_z[ihit] / momtot;	// OP exit
				double hittime	= DIRCBarHits_time[ihit];
				//---- get box-local positions...
				//double locx		= -hity;		// local x-axis is parallel to struck barplane (and points in -phi direction)
				//double locy		=  hitx;		// local y-axis is perp to struck barplane, pointing outward (upward)
				TVector3 hitpos		= TVector3(hitx,hity,hitz);
				TVector3 hitposr	= hitpos; 
				         hitposr.RotateZ(-kHitBarBox*30.*(M_PI/180.));	// now struck bar box is on x-axis (effectively), so we take the local positions from this
				double locx			= -hitposr.y();
				double locy			=  hitposr.x();
				int    idpixx	= (locx-locxmin)/pixelsize;
				int    idpixy	= (locy-locymin)/pixelsize;
				//if (iinc==0){	// only fill hit position pattern once per event!
				//	if (jentry==0)hyxr0->Fill(locx,locy);
				//	if (jentry==1)hyxr1->Fill(locx,locy);
				//	if (jentry==2)hyxr2->Fill(locx,locy);
				//	if (jentry==3)hyxr3->Fill(locx,locy);
				//}
				//
				double OPwavelen	= GetWavelength(ihit);
				// 
				int kmcPartIDTruth_hit	= GetTruthIndex(ihit);
				if (kmcPartIDTruth_hit<0){ cout<<"could not find parent to hit!! "<<endl; exit(0); }
				int Truth_pdgCode_hit	= MCParticles_PDG[kmcPartIDTruth_hit];
				int Truth_PID_hit		= -1;			// default value of hypothesis is "i have no idea"
				for (int ihyp=0;ihyp<NHYP;ihyp++){
					if (abs(Truth_pdgCode_hit)==hyp_pdgid[ihyp]){ Truth_PID_hit = ihyp; } 	// both charge signs accepted
				}
				//
				if ( idpixx>=0&&idpixx<NPIXX 
				  && idpixy>=0&&idpixy<NPIXY ){
					//
					hpixel		->Fill(idpixx,idpixy,  1.0);
					hpixeltime	->Fill(idpixx,idpixy, hitt);
					hpixelplane	->Fill(locx  ,locy  ,  1.0);
					//
					int npaths	= LUT_Npaths[idpixx][idpixy][kBar];	// indexed by bar (kBar from incidence tree)
					for (int ip=0;ip<npaths;ip++){
						//
						OPprob	= LUT_OPprob[idpixx][idpixy][kBar].at(ip);
						OPdirx	= LUT_OPdirx[idpixx][idpixy][kBar].at(ip);
						OPdiry	= LUT_OPdiry[idpixx][idpixy][kBar].at(ip);
						OPdirz	= LUT_OPdirz[idpixx][idpixy][kBar].at(ip);
						OPdir	= sqrt(OPdirx*OPdirx + OPdiry*OPdiry + OPdirz*OPdirz);
						OPtime	= LUT_OPtime[idpixx][idpixy][kBar].at(ip);	// time in prism
						//
						double OPtheta	= acos(OPdirz/OPdir);
						double dZbar1	= inc_z[iinc] - Z_barmin;							// distance to readout end of bars
						double dZbar2	= (Z_barmax - inc_z[iinc]) + (Z_barmax-Z_barmin);	// distance to mirror end and back
						double tbar1	= dZbar1/fabs(cos(OPtheta))/v_bar;			// time to bars end going short-ways
						double tbar2	= dZbar2/fabs(cos(OPtheta))/v_bar;			// time to bars end going long-ways
						double texp1	= OPtime + tbar1 + inc_t[iinc];				// time: incidence to RO end of bars
						double texp2	= OPtime + tbar2 + inc_t[iinc];				// time: incidence to mirror end and back to RO end of bars
						//
						//---- check the timing...
						bool goodgate		= false;
						bool goodgateTight	= false;
						//bool goodgateBands	= false;
						int zdir			= OPTimeCutCrude(hitt,texp1,texp2,DetTheta);					// check OP timing!
						int zdirTight		= OPTimeCutTight(hitt,texp1,texp2,DetTheta,tbeg1OP,tbeg2OP);	// check OP timing!
						//int zdirBands		= OPTimeCutBands(hitt, inc_z[iinc], inc_t[iinc], OPtime);		// check OP timing!
						if (abs(zdir)     ==1){ goodgate		= true; }
						if (abs(zdirTight)==1){ goodgateTight	= true; }
						//if (abs(zdirBands)==1){ goodgateBands	= true; }
						int  zdirUse		= zdirTight;		// -1 prism-going, +1 mirror-going...
						bool goodgateUse	= goodgateTight;	// true if zdir = +1 or -1...
						//int  zdirUse		= zdirBands;
						//bool goodgateUse	= goodgateBands;
						//
						//---- make sure OPpath theta is in allowed cos(theta) range for this Z-incidence!
 						double cosOPtheta	= cos(OPtheta);		// ...same as OPdirz/OPdir...
						bool   goodCosAng	= false;
 						if (  zdirUse    == -1			//---- prism-going at creation, use cosang cuts vs Z as is... 
 						  &&  cosOPtheta >= LIM_cosang_lower		
 						  &&  cosOPtheta <  LIM_cosang_upper ){		
							goodCosAng		= true;
 						} 
 						if (  zdirUse    ==  1			//---- mirror-going at creation, use NEGATIVE of cosang cuts vs Z... 
 						  && -cosOPtheta >= LIM_cosang_lower		
 						  && -cosOPtheta <  LIM_cosang_upper ){		
							goodCosAng		= true;
 						}
 						//
						//cout<<inc_z[iinc]<<"  "<<cosOPtheta<<"  "<<LIM_cosang_lower<<" "<<LIM_cosang_upper<<endl;
						//cout<<inc_z[iinc]<<" "<<inc_t[iinc]<<" "<<OPtime<<" \t"<<hitt<<" \t "<<zdir<<" "<<zdirTight<<" "<<zdirBands<<endl;					
						//cout<<hitt<<" "<<ip<<"\t theta="<<OPtheta<<" OPtime="<<OPtime
						//	<<" inc_z="<<inc_z[iinc]<<" dZbar="<<dZbar
						//	<<"\t tbar="<<tbar<<" texp="<<texp<<" dt="<<hitt-texp<<endl;
						//
						//
						int    indbest[NHYP]						= {-1 };
						int    indchrbest[NHYP]						= {-1 };
						double cerenkovtheta_end_AMBbest[NHYP]		= { 0.};
						double cerenkovtheta_end_AMBchrbest[NHYP]	= { 0.};
						double cerenkovtheta_end_AMBbest_sel		=   0;	// best thetaC for this hit+path
						double cerenkovtheta_end_AMBchrbest_sel		=   0; 	// best thetaC for this hit+path, w/ chromatic correction
						double Lpath								=   0;
						//
						//---- get chromatic correction slope and intercept to use in following ambiguity search
						//	note!  value of chrcorr treats BOTH the chromaticity (slope) and the offset ("per pmt correction")
						//
						double dt1		= hitt-texp1;
						double dt2		= hitt-texp2;
						double dt 		= 0;
						double chrcorr	= 0;
						fChrom1Use		->SetParameter(0,gChrom1Inter_Theta->Eval(DetTheta*(180./M_PI)));
						fChrom1Use		->SetParameter(1,gChrom1Slope_Theta->Eval(DetTheta*(180./M_PI)));
						fChrom2Use		->SetParameter(0,gChrom2Inter_Theta->Eval(DetTheta*(180./M_PI)));
						fChrom2Use		->SetParameter(1,gChrom2Slope_Theta->Eval(DetTheta*(180./M_PI)));
						if (zdirUse==-1){
							dt 		= dt1;
							Lpath	= OPtime*v_bar +  dZbar1/fabs(cos(OPtheta));
							chrcorr	= fChrom1Use->Eval( dt1/Lpath );
						} else if (zdirUse== 1){
							dt 		= dt2;
							Lpath	= OPtime*v_bar +  dZbar2/fabs(cos(OPtheta));
							chrcorr	= fChrom2Use->Eval( dt2/Lpath );
						} 
						//
						double chrcorrIdeal	= 0;
						fchr1Use->SetParameter(0, gfchr1par0_Theta->Eval( DetTheta*(180./M_PI) ) );
						fchr1Use->SetParameter(1, gfchr1par1_Theta->Eval( DetTheta*(180./M_PI) ) );
						fchr2Use->SetParameter(0, gfchr2par0_Theta->Eval( DetTheta*(180./M_PI) ) );
						fchr2Use->SetParameter(1, gfchr2par1_Theta->Eval( DetTheta*(180./M_PI) ) );
						if (zdirUse==-1){ chrcorrIdeal	= fchr1Use->Eval( OPwavelen )/1000.; } else 
						if (zdirUse== 1){ chrcorrIdeal	= fchr2Use->Eval( OPwavelen )/1000.; }
						double chrcorrUse	= 0;
						       chrcorrUse	= chrcorr;		// or chrcorrIdeal
						//if (!goodgateTight){ chrcorrUse	= 0.; }
						//
						//---- Search the Ambiguities for better thetaC values...
						TVector3	AMBdir[8];
						double		cerenkovtheta_end_AMB[8] 	= {0.};
						double		cerenkovtheta_end_AMBchr[8] = {0.};
						double 		diffbest[NHYP]				= {0};
						double 		diffchrbest[NHYP]			= {0};
						for (int ihyp=0;ihyp<NHYP;ihyp++){ diffbest[ihyp] = diffchrbest[ihyp] = 99999.; }
						for (int iamb=0;iamb<8;iamb++){
							//
							//---- The tight timing cut breaks 4 z-ambiguities...
							if (zdirUse==-1){					//---- this OP was moving towards prism at creation
								if (iamb>=4){ 									// so we must NOT flip z!!! 
 									cerenkovtheta_end_AMB[iamb]			= 0;	// so we skip iamb's that flip z....
 									cerenkovtheta_end_AMBchr[iamb]		= 0;	// so we skip iamb's that flip z....
									continue; 
								}
							} else if (zdirUse== 1){			//---- this OP was moving towards mirror at creation
								if (iamb< 4){ 									// so we MUST flip z!!! 
 									cerenkovtheta_end_AMB[iamb]			= 0;	// so we skip iamb's that don't flip z....
 									cerenkovtheta_end_AMBchr[iamb]		= 0;	// so we skip iamb's that don't flip z....
									continue; 
								}
							}
							//
							if(iamb==0){ AMBdir[iamb]	= TVector3(  OPdirx,  OPdiry,  OPdirz ); } else	// original path
							if(iamb==1){ AMBdir[iamb]	= TVector3( -OPdirx,  OPdiry,  OPdirz ); } else	// flip kx only
							if(iamb==2){ AMBdir[iamb]	= TVector3(  OPdirx, -OPdiry,  OPdirz ); } else	// flip ky only
							if(iamb==3){ AMBdir[iamb]	= TVector3( -OPdirx, -OPdiry,  OPdirz ); } else	// flip kx & ky
							if(iamb==4){ AMBdir[iamb]	= TVector3(  OPdirx,  OPdiry, -OPdirz ); } else	// flip kz only
							if(iamb==5){ AMBdir[iamb]	= TVector3( -OPdirx,  OPdiry, -OPdirz ); } else	// flip kx & kz
							if(iamb==6){ AMBdir[iamb]	= TVector3(  OPdirx, -OPdiry, -OPdirz ); } else	// flip ky & kz
							if(iamb==7){ AMBdir[iamb]	= TVector3( -OPdirx, -OPdiry, -OPdirz ); }		// flip all three
							//
							//---- loop over NHYP hypotheses and do ambiguity searching including chromatic correction
							for (int ihyp=0;ihyp<NHYP;ihyp++){
								cerenkovtheta_end_AMB[iamb]		= acos(AMBdir[iamb].Dot(fPrimaryDircDirRot));
								if ( fabs(cerenkovtheta_end_AMB[iamb]-thetaCexp[ihyp]) < diffbest[ihyp]){
									indbest[ihyp]						= iamb;
									diffbest[ihyp]						= fabs(cerenkovtheta_end_AMB[iamb]-thetaCexp[ihyp]);
									cerenkovtheta_end_AMBbest[ihyp]		= cerenkovtheta_end_AMB[iamb];
								}
								cerenkovtheta_end_AMBchr[iamb]	= acos(AMBdir[iamb].Dot(fPrimaryDircDirRot)) - chrcorrUse;
								if ( fabs(cerenkovtheta_end_AMBchr[iamb]-thetaCexp[ihyp]) < diffchrbest[ihyp]){
									indchrbest[ihyp]					= iamb;
									diffchrbest[ihyp]					= fabs(cerenkovtheta_end_AMBchr[iamb]-thetaCexp[ihyp]-chrcorrUse);
									cerenkovtheta_end_AMBchrbest[ihyp]	= cerenkovtheta_end_AMBchr[iamb];
								}
							}	// end hypothesis loop...
							//
						}	//---- end loop over 8 ambiguities...
						//
						//---- save best thetaC for each hypothesis from amongst the 8 ambiguities...
						for (int ihyp=0;ihyp<NHYP;ihyp++){
							thetasHyp[ihyp].push_back(cerenkovtheta_end_AMBchrbest[ihyp]);
						}
						//
						//---- find the hypothesis with overall smallest dthetaC
						int 	ihyp_best		=  -1;
						double	diffsmallest	= 999;
						for (int ihyp=0;ihyp<NHYP;ihyp++){
							if (diffchrbest[ihyp]<diffsmallest){ ihyp_best=ihyp; diffsmallest=diffchrbest[ihyp]; }
						}
						if (ihyp_best>-1){		// we found a best hypothesis after the ambiguity searching!
							cerenkovtheta_end_AMBbest_sel		= cerenkovtheta_end_AMBbest[ihyp_best];
							cerenkovtheta_end_AMBchrbest_sel	= cerenkovtheta_end_AMBchrbest[ihyp_best];
						} else {
							cerenkovtheta_end_AMBbest_sel		= 0;
							cerenkovtheta_end_AMBchrbest_sel	= 0;
						}
						//---- don't provide a value unless this pixel-path is time-gated!!!
						//if (!goodgateTight){ cerenkovtheta_end_AMBchr	= 0.; }
						//
						//
						hthittexp1		->Fill(texp1,hitt);												// ungated!
						hthittexp2		->Fill(texp2,hitt);												// ungated!
						hthetaC			->Fill(1000.*cerenkovtheta_end_AMBchrbest_sel,OPprob);			// ungated!
						//hthetaC_dt1	->Fill((hitt-texp1)/Lpath,1000.*cerenkovtheta_end_AMBbest_sel);	// ungated!
						//hthetaC_dt2	->Fill((hitt-texp2)/Lpath,1000.*cerenkovtheta_end_AMBbest_sel);	// ungated!
						//
						if (goodgate){			// crude timing gate just for diagnostic plots...
							if (zdir==-1) hthittexp1_gated ->Fill(texp1,hitt);
							if (zdir== 1) hthittexp2_gated ->Fill(texp2,hitt);
							hdt1texp1	->Fill(texp1,hitt-texp1);
							hdt2texp2	->Fill(texp2,hitt-texp2);
						}
						//
						if (goodgateUse){		// tight timing gate 
							//
							if (zdirUse==-1){ hcosanglut ->Fill(  cos(OPtheta)); } else
							if (zdirUse== 1){ hcosanglut ->Fill( -cos(OPtheta)); } 
							//
							if (goodCosAng){	// polar angle consistency
								//
								hhyp_best		->Fill(ihyp_best);
								hBestHypothesis	->Fill(ihyp_best);	// working histogram, reset every incident particle...
								if (ihyp_best>=0) hAMBindex	->Fill(indchrbest[ihyp_best]);
								//
								hthetaCest_W	-> Fill(1000.*cerenkovtheta_end_AMBchrbest_sel);
								thetaCest		+=      1000.*cerenkovtheta_end_AMBchrbest_sel ;
								thetaCestN		+= 1;
								//
								hpixel_gated		->Fill(idpixx,idpixy,OPprob);
								hpixeltime_gated	->Fill(idpixx,idpixy,hitt*OPprob);
								hthetaC_gated		->Fill(1000.*cerenkovtheta_end_AMBbest_sel   ,OPprob);
								hthetaC_gatedchrom	->Fill(1000.*cerenkovtheta_end_AMBchrbest_sel,OPprob);
								hdthetaC_gated		->Fill(1000.*(cerenkovtheta_end_AMBbest_sel-thetaCexp[ihyp_best])   ,OPprob);
								hdthetaC_gatedchrom	->Fill(1000.*(cerenkovtheta_end_AMBchrbest_sel-thetaCexp[ihyp_best]),OPprob);
								hthitzdir_gated	 	->Fill(zdir,hitt);
								if (zdirUse==-1){
									double invvg	= (hitt-texp1)/Lpath;
									hthetaC_dt1_gated		->Fill(invvg     ,1000.*cerenkovtheta_end_AMBbest_sel);
									hthetaC_WL1_gated		->Fill(OPwavelen ,1000.*cerenkovtheta_end_AMBbest_sel);
									hthetaCchr_dt1_gated	->Fill(invvg     ,1000.*cerenkovtheta_end_AMBchrbest_sel);
									hthetaCchr_WL1_gated	->Fill(OPwavelen ,1000.*cerenkovtheta_end_AMBchrbest_sel);
									hWL_dt1_gated			->Fill(invvg     ,OPwavelen);
								} else if (zdirUse== 1){ 
									double invvg	= (hitt-texp2)/Lpath;
									hthetaC_dt2_gated		->Fill(invvg     ,1000.*cerenkovtheta_end_AMBbest_sel);
									hthetaC_WL2_gated		->Fill(OPwavelen ,1000.*cerenkovtheta_end_AMBbest_sel);
									hthetaCchr_dt2_gated	->Fill(invvg     ,1000.*cerenkovtheta_end_AMBchrbest_sel);
									hthetaCchr_WL2_gated	->Fill(OPwavelen ,1000.*cerenkovtheta_end_AMBchrbest_sel);
									hWL_dt2_gated			->Fill(invvg     ,OPwavelen);
								}
								//
								hhypParent_hit_W->Fill(Truth_PID_hit);
								//
							}	// end good cosang
							//
						}	// end good timing gate
						//
					}	// end loop over lut paths to this pixel
					//
				} else {
					//
					//cout<<"pixel id out of range! "<<locx<<" "<<idpixx<<"\t "<<locy<<" "<<idpixy<<endl;
					continue;
					//
				}	//	end check pixel ID in range
				//
			}	// end loop over hits...
			//
			//---- now estimate what was the truth parent...
			int    ihyp_OPTruthParent	= -1 ; 	// estimated truth hypothesis for this incident track, considering all in-time hits...
			int    maxc					=  0.;
			double sumc					=  0.;
			double HitPurity			=  0.;
			for (int ibx=1;ibx<=hhypParent_hit_W->GetNbinsX();ibx++){		// remember: first bin is "unknown PID"... 
				int ihyp	= round(hhypParent_hit_W->GetBinCenter(ibx));
				if (ihyp>=0&&ihyp<NHYP){
					double nc	 = hhypParent_hit_W->GetBinContent(ibx);
					sumc		+= nc;
					if (nc>maxc){ maxc=nc; ihyp_OPTruthParent=ihyp; }
				}
			}
			if (ihyp_OPTruthParent>=0){
				 HitPurity		= hhypParent_hit_W->GetBinContent(ihyp_OPTruthParent+2)/sumc;		// remember: first bin is "unknown PID"... 
				hHitPurity		->Fill(HitPurity);
				hhypParent_hit	->Fill(ihyp_OPTruthParent);
			}
			//
			//---- now calculate PID observables for this incident particle ----------------------------------
			double  thetaC	= 0;
			double dthetaC	= 0;
			//---- PID#1: straight average over all in-time pixel-paths
			if (thetaCestN>=1){	thetaCest /= thetaCestN; } 
			             else { thetaCest  = 0.;         }
			hthetaCest	->Fill(thetaCest);
			//---- PID#2: find peak of per-photon thetaC histogram
			double thetaCestPeak	= 0;
			int kbin				= hthetaCest_W->GetMaximumBin();
			double nx=0,mx=0;
			for (int kb=1;kb<hthetaCest_W->GetXaxis()->GetNbins();kb++){
				int kbl	= kbin-20; kbl=max(1,kbl); 
				int kbu	= kbin+20; kbu=min(hthetaCest_W->GetNbinsX(),kbu); 
				if (kb>=kbl&&kb<=kbu){		// ...bins around peak
					double x	= hthetaCest_W->GetBinCenter(kb);
					double n	= hthetaCest_W->GetBinContent(kb);
					nx += n;  mx += n * x;
					//cout<<kbin<<" "<<kb<<"\t "<<x<<" "<<n<<"\t "<<nx<<" "<<mx/nx<<endl;
				}
			}	
			if (nx){ mx	/= nx; }else{ mx=0; nx=0; }
			thetaCestPeak	= mx; 
			hthetaCestPeak	->Fill(thetaCestPeak);
			hthetaCest_W	->Reset(); nx=0; mx=0;
			thetaC			= thetaCestPeak;			//!!!! HERE IT IS !!!!
			dthetaC			= -99;						
			if (ihyp_TruthParent>=0){
				dthetaC		= thetaC - fthCexp_ptot[ihyp_TruthParent]->Eval(inc_ptot);
			}
			//---- PID#3: average over all the thetaC's for the most-common best-hypothesis (from hBestHypothesis)
			int kHypMost	= hBestHypothesis->GetMaximumBin() - 1;		// bin ID to bin center 
			if (thetasHyp[kHypMost].size()>0){
				for (int ival=0;ival<thetasHyp[kHypMost].size();ival++){
					double thistheta	 = thetasHyp[kHypMost].at(ival);
					thetaCestHypMost	+= thistheta;
					hthetaCestHypMost_W	-> Fill(1000.*thistheta);
				}
				thetaCestHypMost	/= thetasHyp[kHypMost].size();
				hthetaCestHypMost	->Fill(thetaCestHypMost);
			} else {
				thetaCestHypMost	= 0;
			}
			//---- PID#4: use temp hist filled in PID#3 to average only over peak of the thetaC's for the most-common best-hypothesis (from hBestHypothesis)
			double thetaCestHypMostPeak	= 0;
			kbin					= hthetaCestHypMost_W->GetMaximumBin();
			double nnx=0,mmx=0;
			for (int kb=1;kb<hthetaCestHypMost_W->GetXaxis()->GetNbins();kb++){
				int kbl	= kbin-20; kbl=max(1,kbl); 
				int kbu	= kbin+20; kbu=min(hthetaCestHypMost_W->GetNbinsX(),kbu); 
				if (kb>=kbl&&kb<=kbu){		// ...bins around peak
					double x	= hthetaCestHypMost_W->GetBinCenter(kb);
					double n	= hthetaCestHypMost_W->GetBinContent(kb);
					nnx += n; mmx += n*x;
				}
			}	
			if (nnx){ mmx	/= nnx; }else{ mmx=0; nnx=0; }
			thetaCestHypMostPeak	= mmx; 
			hthetaCestHypMostPeak	->Fill(thetaCestHypMostPeak);
			hthetaCestHypMost_W		->Reset(); nnx=0; mmx=0;
			//---- end of overall PID algorithms...
			//
			//---- now fill plots for this incident particle -----------------------------
			//
			hthetaC_ptot	->Fill(inc_ptot,thetaC);
 			if (ihyp_TruthParent>=0){
 				 hthetaC_ptot_byTruth[ihyp_TruthParent]	->Fill(inc_ptot, thetaC);
 				hdthetaC_ptot_byTruth[ihyp_TruthParent]	->Fill(inc_ptot,dthetaC);
 				 hdthetaC_bar_byTruth[ihyp_TruthParent]	->Fill(kBar    ,dthetaC);
 				 hdthetaC_eta_byTruth[ihyp_TruthParent]	->Fill(inc_eta ,dthetaC);
 				//
 				//if (inc_ptot>1.0){
	 				hdthetaC_nhbox_byTruth[ihyp_TruthParent]->Fill(nhitbox_evt[kTrackBarBox],dthetaC);
 				//}
 				//
 			}
			//
			//
		}	// end loop over incident particles...
		//
		//
	}	// end loop over events...

	double expected	= 1000.*CerenkovAngleExpected;	// useful for gunsim only..
	
	hpixeltime			->Divide(hpixel);		// now Z-axis is the average time of the hits in each pixel!
	hpixeltime_gated	->Divide(hpixel_gated);	// now Z-axis is the average time of the hits in each pixel!
	for (int ibx=1;ibx<=hpixel->GetNbinsX();ibx++){
		for (int iby=1;iby<=hpixel->GetNbinsY();iby++){
			int nc	= hpixel->GetBinContent(ibx,iby);
			if (nc<10){
				hpixeltime			->SetBinContent(ibx,iby,0.);
				hpixeltime_gated	->SetBinContent(ibx,iby,0.);
			}
		}
	}

	//---- Extract IDEAL chromatic correction parameters... (based on wavelength)
	//
	TF1 *fchr = new TF1("fchr","pol1",295,655);
	fchr->SetParameter(0, 867.5  );		// just a ballpark value, improved here.
	fchr->SetParameter(1,  -0.064);		// from chr.C...
	TF1* fchr1	= static_cast<TF1*>(fchr->Clone("fchr1"));
	TF1* fchr2	= static_cast<TF1*>(fchr->Clone("fchr2"));
	//
	GetIdeal(expected, hthetaC_WL1_gated, fchr, fchr1);		// linear dependence of ThetaC on WL, prism-going
	GetIdeal(expected, hthetaC_WL2_gated, fchr, fchr2);		// linear dependence of ThetaC on WL, mirror-going
	//
	hinfo->SetBinContent(51,hthetaC_WL1_gated->Integral());
	hinfo->SetBinContent(52,fchr1->GetParameter(0));
	hinfo->SetBinContent(53,fchr1->GetParameter(1));
	hinfo->SetBinContent(54,fchr1->Eval(370));
	hinfo->SetBinContent(55,hthetaC_WL2_gated->Integral());
	hinfo->SetBinContent(56,fchr2->GetParameter(0));
	hinfo->SetBinContent(57,fchr2->GetParameter(1));
	hinfo->SetBinContent(58,fchr2->Eval(370));
	//
	//---- Extract APPARENT chromatic correction parameters...
	//
	TF1 *fG	= new TF1("fG", "gaus", cacutl, cacutu);	// to get thetaC distribution in each slice of (hitt-texp1)/Lpath
		 fG	->SetParameters(10, 1000.*CerenkovAngleExpected, 3.0);		
		 fG	->SetParLimits(2,0.0,5.0);					// limit std dev of fitted gaussion (for peak location!) 
	const double INVVG_lower	= -0.00005;		// -0.0001,  ...0.00007 gives same fits...
	const double INVVG_upper	=  0.0001;		//  0.0001
			hinfo->SetBinContent(61,INVVG_lower);
			hinfo->SetBinContent(62,INVVG_upper);
	//
	TString FitOptions	= TString("Q0NRB");
	if (SteerString=="pi+3GeV88deg") FitOptions += TString("L");  // use Log-Likelihood
	//
	TObjArray* arr1 		= new TObjArray();
	hthetaC_dt1_gated->FitSlicesY(fG, 0, -1, 0, FitOptions.Data(), arr1);
	TH1D*	hChrom1_constant= static_cast<TH1D*>(arr1->At(0));	if (hChrom1_constant) hChrom1_constant->SetName("hChrom1_constant");
	TH1D*	hChrom1_mean	= static_cast<TH1D*>(arr1->At(1));	if (hChrom1_mean)     hChrom1_mean->SetName("hChrom1_mean");
	TH1D*	hChrom1_sigma	= static_cast<TH1D*>(arr1->At(2));	if (hChrom1_sigma)    hChrom1_sigma->SetName("hChrom1_sigma");
	TH1D*	hChrom1_chi2	= static_cast<TH1D*>(arr1->At(3));	if (hChrom1_chi2)     hChrom1_chi2->SetName("hChrom1_chi2");
	CheckSlices(expected,hChrom1_mean);
	TF1*	fChrom1			= new TF1("fChrom1","pol1",INVVG_lower,INVVG_upper);
			fChrom1			->SetNpx(1000); 
//			fChrom1			->SetParameters(0, 60.*1000.);				// default pars...
//			fChrom1			->SetParLimits(1,20.0*1000.,75.0*1000.);	// limit slope of chromatic correction  
			hChrom1_mean	->Fit(fChrom1,"QNR");
			fChrom1			->SetLineWidth(2);
			hChrom1_mean	->SetLineWidth(3);
	double thisintercept1	= fChrom1->GetParameter(0)/1000. - expected/1000.;
	double thisslope1		= fChrom1->GetParameter(1)/1000.;
			hinfo->SetBinContent(63,thisintercept1);
			hinfo->SetBinContent(64,thisslope1);
			cout<<SteerString<<"\tChromatic1:   intercept(rad)= "<<fChrom1->GetParameter(0)/1000. - expected/1000.
				<<"\t slope= "<<thisslope1
				<<"\t "<<SteerString
				<<endl;
	//
	TObjArray* arr2			= new TObjArray();
	hthetaC_dt2_gated->FitSlicesY(fG, 0, -1, 0, FitOptions.Data(), arr2);
	TH1D*	hChrom2_constant= static_cast<TH1D*>(arr2->At(0));	if (hChrom2_constant) hChrom2_constant->SetName("hChrom2_constant");
	TH1D*	hChrom2_mean	= static_cast<TH1D*>(arr2->At(1));	if (hChrom2_mean)     hChrom2_mean->SetName("hChrom2_mean");
	TH1D*	hChrom2_sigma	= static_cast<TH1D*>(arr2->At(2));	if (hChrom2_sigma)    hChrom2_sigma->SetName("hChrom2_sigma");
	TH1D*	hChrom2_chi2	= static_cast<TH1D*>(arr2->At(3));	if (hChrom2_chi2)     hChrom2_chi2->SetName("hChrom2_chi2");
	CheckSlices(expected,hChrom2_mean);
	TF1*	fChrom2			= new TF1("fChrom2","pol1",INVVG_lower,INVVG_upper);
			fChrom2			->SetNpx(1000); 
//			fChrom2			->SetParameters(0, 60.*1000.);				// default pars...
//			fChrom2			->SetParLimits(1,20.0*1000.,75.0*1000.);	// limit slope of chromatic correction  
			hChrom2_mean	->Fit(fChrom2,"QNR");
			fChrom2			->SetLineWidth(2);
			hChrom2_mean	->SetLineWidth(3);
	double thisintercept2	= fChrom2->GetParameter(0)/1000. - expected/1000.;
	double thisslope2		= fChrom2->GetParameter(1)/1000.;
			hinfo->SetBinContent(65,thisintercept2);
			hinfo->SetBinContent(66,thisslope2);
			cout<<SteerString<<"\tChromatic2:   intercept(rad)= "<<fChrom2->GetParameter(0)/1000. - expected/1000.
				<<"\t slope= "<<thisslope2
				<<"\t "<<SteerString
				<<endl;
	//
	//---- just for plotting below:
	TGraph* gthisfit1int	= new TGraph(); gthisfit1int->SetPoint(0,Primary_theta,thisintercept1);
	TGraph* gthisfit1slo	= new TGraph(); gthisfit1slo->SetPoint(0,Primary_theta,thisslope1    );
	TGraph* gthisfit2int	= new TGraph(); gthisfit2int->SetPoint(0,Primary_theta,thisintercept2);
	TGraph* gthisfit2slo	= new TGraph(); gthisfit2slo->SetPoint(0,Primary_theta,thisslope2    );
			gthisfit1int	->SetMarkerStyle(59);	gthisfit1int	->SetMarkerSize(1.5);
			gthisfit1slo	->SetMarkerStyle(59);	gthisfit1slo	->SetMarkerSize(1.5);
			gthisfit2int	->SetMarkerStyle(55);	gthisfit2int	->SetMarkerSize(1.5);
			gthisfit2slo	->SetMarkerStyle(55);	gthisfit2slo	->SetMarkerSize(1.5);


	//---- slice-fit dthetaC vs nhbox...
	TH1D* hdthetaC_nhbox_byTruth_mean[NHYP]	= {nullptr};
	TH1D* hdthetaC_nhbox_byTruth_sigm[NHYP]	= {nullptr};
	for (int ihyp=0;ihyp<NHYP;ihyp++){
		hdthetaC_nhbox_byTruth[ihyp]		->FitSlicesY();
		TString thistit1	= hdthetaC_nhbox_byTruth[ihyp]->GetTitle() + TString(", Gaussian Mean");
		TString thistit2	= hdthetaC_nhbox_byTruth[ihyp]->GetTitle() + TString(", Gaussian Std.Dev.");
		hdthetaC_nhbox_byTruth_mean[ihyp]	= static_cast<TH1D*>(gDirectory->Get(Form("hdthetaC_nhbox_byTruth%d_1",ihyp)));
		hdthetaC_nhbox_byTruth_sigm[ihyp]	= static_cast<TH1D*>(gDirectory->Get(Form("hdthetaC_nhbox_byTruth%d_2",ihyp)));
		hdthetaC_nhbox_byTruth_mean[ihyp]	->SetLineWidth(2);		hdthetaC_nhbox_byTruth_sigm[ihyp]	->SetLineWidth(2);
		hdthetaC_nhbox_byTruth_mean[ihyp]	->SetLineColor(1);		hdthetaC_nhbox_byTruth_sigm[ihyp]	->SetLineColor(1);
		hdthetaC_nhbox_byTruth_mean[ihyp]	->SetMarkerColor(1);	hdthetaC_nhbox_byTruth_sigm[ihyp]	->SetMarkerColor(1);
		hdthetaC_nhbox_byTruth_mean[ihyp]	->SetMarkerStyle(20);	hdthetaC_nhbox_byTruth_sigm[ihyp]	->SetMarkerStyle(20);
		hdthetaC_nhbox_byTruth_mean[ihyp]	->SetTitle(thistit1);
		hdthetaC_nhbox_byTruth_sigm[ihyp]	->SetTitle(thistit2);
	}


	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//---- SPR....		ONLY MAKES SENSE IF THIS IS FOR GUNSIM!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//
	int 	  kbinmax	= hthetaC_gated->GetMaximumBin();
	double	thCbinmax	= hthetaC_gated->GetXaxis()->GetBinCenter(kbinmax);
	TF1 *fthetaC_gated	= new TF1("fthetaC_gated","gaus",thCbinmax-10,thCbinmax+10);
		 fthetaC_gated	->SetNpx(50000);
		 fthetaC_gated	->SetLineColor(kMagenta);
		 fthetaC_gated	->SetLineWidth(1);
	hthetaC_gated		->Fit(fthetaC_gated,"QRN");
	const double *gparsSPR		= fthetaC_gated->GetParameters();
	const double *gparseSPR		= fthetaC_gated->GetParErrors();
	double	chi2SPR				= fthetaC_gated->GetChisquare();
	int		ndfSPR				= fthetaC_gated->GetNDF();
	double  redchi2SPR			= 0;
	if (ndfSPR>0) redchi2SPR	= chi2SPR/ndfSPR;
	cout<<"ThetaC SPR   \t const: "<<gparsSPR[0]<<" +- "<<gparseSPR[0]<<endl;
	cout<<"ThetaC SPR   \t mean:  "<<gparsSPR[1]<<" +- "<<gparseSPR[1]<<" \t exp: "<<1000.*CerenkovAngleExpected<<endl;
	cout<<"ThetaC SPR   \t sigma: "<<gparsSPR[2]<<" +- "<<gparseSPR[2]<<endl;
	cout<<"ThetaC SPR   \t chi^2: "<<redchi2SPR<<" for "<<ndfSPR<<" DOF."<<endl;
	hinfo->SetBinContent( 11,  gparsSPR[0] );
	hinfo->SetBinContent( 12, gparseSPR[0] );
	hinfo->SetBinContent( 13,  gparsSPR[1] );
	hinfo->SetBinContent( 14, gparseSPR[1] );
	hinfo->SetBinContent( 15,  gparsSPR[2] );
	hinfo->SetBinContent( 16, gparseSPR[2] );
	hinfo->SetBinContent( 17, redchi2SPR   );
	hinfo->SetBinContent( 18,     ndfSPR   );
	//
	kbinmax		= hthetaC_gatedchrom->GetMaximumBin();
	thCbinmax	= hthetaC_gatedchrom->GetXaxis()->GetBinCenter(kbinmax);
	TF1 *fthetaC_gatedchrom	= new TF1("fthetaC_gatedchrom","gaus",thCbinmax-10,thCbinmax+10);
		 fthetaC_gatedchrom	->SetNpx(50000);
		 fthetaC_gatedchrom	->SetLineColor(4);
		 fthetaC_gatedchrom	->SetLineWidth(1);
	hthetaC_gatedchrom		->Fit(fthetaC_gatedchrom,"QRN");
	const double *gparsSPRchr		= fthetaC_gatedchrom->GetParameters();
	const double *gparseSPRchr		= fthetaC_gatedchrom->GetParErrors();
	double	chi2SPRchr				= fthetaC_gatedchrom->GetChisquare();
	int		ndfSPRchr				= fthetaC_gatedchrom->GetNDF();
	double  redchi2SPRchr			= 0;
	if (ndfSPRchr>0) redchi2SPRchr	= chi2SPRchr/ndfSPRchr;
	cout<<"ThetaC SPRchr\t const: "<<gparsSPRchr[0]<<" +- "<<gparseSPRchr[0]<<endl;
	cout<<"ThetaC SPRchr\t mean:  "<<gparsSPRchr[1]<<" +- "<<gparseSPRchr[1]<<" \t exp: "<<1000.*CerenkovAngleExpected<<endl;
	cout<<"ThetaC SPRchr\t sigma: "<<gparsSPRchr[2]<<" +- "<<gparseSPRchr[2]<<endl;
	cout<<"ThetaC SPRchr\t chi^2: "<<redchi2SPRchr<<" for "<<ndfSPRchr<<" DOF."<<endl;
	hinfo->SetBinContent( 21,  gparsSPRchr[0] );
	hinfo->SetBinContent( 22, gparseSPRchr[0] );
	hinfo->SetBinContent( 23,  gparsSPRchr[1] );
	hinfo->SetBinContent( 24, gparseSPRchr[1] );
	hinfo->SetBinContent( 25,  gparsSPRchr[2] );
	hinfo->SetBinContent( 26, gparseSPRchr[2] );
	hinfo->SetBinContent( 27, redchi2SPRchr   );
	hinfo->SetBinContent( 28,     ndfSPRchr   );

	//---- estimators...
	//
	TF1 *fthetaCest	= new TF1("fthetaCest","gaus",350.,1350.);
		 fthetaCest	->SetNpx(50000);
		 fthetaCest	->SetLineColor(4);
	hthetaCest		->Fit(fthetaCest,"QRN");
	const double *gpars		= fthetaCest->GetParameters();
	const double *gparse	= fthetaCest->GetParErrors();
	double	chi2			= fthetaCest->GetChisquare();
	int		ndf				= fthetaCest->GetNDF();
	double  redchi2			= 0;
	if (ndf>0) redchi2	= chi2/ndf;
// 	cout<<" ThetaC Estimate Performance:"<<endl;
// 	cout<<"\t const: "<<gpars[0]<<" +- "<<gparse[0]<<endl;
// 	cout<<"\t mean:  "<<gpars[1]<<" +- "<<gparse[1]<<" \t exp: "<<1000.*CerenkovAngleExpected<<endl;
// 	cout<<"\t sigma: "<<gpars[2]<<" +- "<<gparse[2]<<endl;
// 	cout<<"\t chi^2: "<<redchi2<<" for "<<ndf<<" DOF."<<endl;
	hinfo->SetBinContent(31,  gpars[0] );
	hinfo->SetBinContent(32, gparse[0] );
	hinfo->SetBinContent(33,  gpars[1] );
	hinfo->SetBinContent(34, gparse[1] );
	hinfo->SetBinContent(35,  gpars[2] );
	hinfo->SetBinContent(36, gparse[2] );
	hinfo->SetBinContent(37, redchi2   );
	hinfo->SetBinContent(38,     ndf   );
	
	  kbinmax	= hthetaCestPeak->GetMaximumBin();
	thCbinmax	= hthetaCestPeak->GetXaxis()->GetBinCenter(kbinmax);
	TF1 *fthetaCestPeak	= new TF1("fthetaCestPeak","gaus",thCbinmax-8,thCbinmax+8);
		 fthetaCestPeak	->SetNpx(50000);
		 fthetaCestPeak	->SetLineColor(kGreen+2);
	hthetaCestPeak		->Fit(fthetaCestPeak,"QRN");
	const double *gparsP	= fthetaCestPeak->GetParameters();
	const double *gparseP	= fthetaCestPeak->GetParErrors();
	double	chi2P			= fthetaCestPeak->GetChisquare();
	int		ndfP			= fthetaCestPeak->GetNDF();
	double  redchi2P		= 0;
	if (ndfP>0) redchi2P	= chi2P/ndfP;
// 	cout<<" ThetaC-PeakMean Estimate Performance:"<<endl;
// 	cout<<"\t const: "<<gparsP[0]<<" +- "<<gparseP[0]<<endl;
// 	cout<<"\t mean:  "<<gparsP[1]<<" +- "<<gparseP[1]<<" \t exp: "<<1000.*CerenkovAngleExpected<<endl;
// 	cout<<"\t sigma: "<<gparsP[2]<<" +- "<<gparseP[2]<<endl;
// 	cout<<"\t chi^2: "<<redchi2P<<" for "<<ndfP<<" DOF."<<endl;
	hinfo->SetBinContent(41,  gparsP[0] );
	hinfo->SetBinContent(42, gparseP[0] );
	hinfo->SetBinContent(43,  gparsP[1] );
	hinfo->SetBinContent(44, gparseP[1] );
	hinfo->SetBinContent(45,  gparsP[2] );
	hinfo->SetBinContent(46, gparseP[2] );
	hinfo->SetBinContent(47, redchi2P   );
	hinfo->SetBinContent(48, ndfP       );
	//
	//---- end of SPR....	ONLY MAKES SENSE IF THIS IS FOR GUNSIM!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	//---- Paint setup...
	//
	gStyle->SetStatFormat("8.1f");
	gStyle->SetOptStat(11);
	gStyle->SetErrorX(0);
	gStyle->SetPadRightMargin(0.02);
	gStyle->SetPadTopMargin(0.075);
	gStyle->SetPadBottomMargin(0.07);
	gStyle->SetPadLeftMargin(0.10);
	gStyle->SetTitleX(0.5);
	gStyle->SetLabelSize(0.07,"xyz");
	gStyle->SetTitleFontSize(0.055);
	gStyle->SetPadGridX(0);
	gStyle->SetPadGridY(0);
	//gStyle->SetLabelFont(41,"xyz");
	//gStyle->SetLegendFont(41);
	//gStyle->SetStatFont(41);
	//gStyle->SetTitleFont(41);
	gStyle->SetPaintTextFormat("#1.1e");
	gStyle->SetStatX(0.31);
	gStyle->SetStatY(0.92);
	TCanvas *ccan[1000]; int ican=-1;
	TLatex *text[1000];
	int itext	= -1;
	for (int i=0;i<1000;i++){
		text[i]	= new TLatex();
		text[i]	->SetNDC(true);
		//text[i]	->SetTextFont(40);
		//text[i]	->SetTextSize(0.06);
		//text[i]	->SetTextAlign(11);
	}
	TLine *line[1000];
	int iline = -1;
	for (int i=0;i<1000;i++){
		line[i]	= new TLine();
		line[i]	->SetLineStyle(2);
		line[i]	->SetLineWidth(1);
		line[i]	->SetLineColor(1);
	}
	int 	iframe = -1;
	TH1F*	frame[1000];

	TLine *ldiag = new TLine(10,10,110,110); 
		   ldiag->SetLineColor(15); 
		   ldiag->SetLineStyle(2); 
	TLine *ldiagu = new TLine(10,12,110,112); ldiagu->SetLineColor(1); 
	TLine *ldiagl = new TLine(12,10,112,110); ldiagl->SetLineColor(1); 
	TLine *ly0	= new TLine(0,0,80,0);
		   ly0	->SetLineStyle(2); 
		   ly0	->SetLineColor(1); 
		   ly0	->SetLineWidth(1); 
	//
	TH1D* htemp1	= static_cast<TH1D*>(hthittexp1->ProjectionX("htemp1"));
	TH1D* htemp2	= static_cast<TH1D*>(hthittexp2->ProjectionX("htemp2"));
	double valmin1	= FindLowestXFilled(htemp1);
	double valmin2	= FindLowestXFilled(htemp2);
	TH1D* htemp1y	= static_cast<TH1D*>(hthittexp1->ProjectionY("htemp1y"));
	TH1D* htemp2y	= static_cast<TH1D*>(hthittexp2->ProjectionY("htemp2y"));
	double valmin1y	= FindLowestXFilled(htemp1y);
	double valmin2y	= FindLowestXFilled(htemp2y);
	delete htemp1; delete htemp2; delete htemp1y; delete htemp2y;
	
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("Page %d",ican),10+30*ican,30+30*ican,1000,700);
	ccan[ican]->cd(); ccan[ican]->Divide(4,4,0.0001,0.0001);
	ccan[ican]->cd(1);
		gPad->SetRightMargin(0.14);
		hthittexp1->GetXaxis()->SetRangeUser(valmin1,110);
		hthittexp1->GetYaxis()->SetRangeUser(valmin1y,110);
		hthittexp1->Draw("colz");
		//ldiag->Draw("same");
		//ldiagu->Draw("same");
		//ldiagl->Draw("same");
		ldiag->DrawLine(valmin1,valmin1,110,110);
		ldiagu->DrawLine(valmin1,valmin1+2,110,110+2);
		ldiagl->DrawLine(valmin1+2,valmin1,110+2,110);
	ccan[ican]->cd(2);
		gPad->SetRightMargin(0.14);
		hthittexp2->GetXaxis()->SetRangeUser(valmin2,110);
		hthittexp2->GetYaxis()->SetRangeUser(valmin2y,110);
		hthittexp2->Draw("colz");
		//ldiag->Draw("same");
		//ldiagu->Draw("same");
		//ldiagl->Draw("same");
		ldiag->DrawLine(valmin2,valmin2,110,110);
		ldiagu->DrawLine(valmin2,valmin2+2,110,110+2);
		ldiagl->DrawLine(valmin2+2,valmin2,110+2,110);
	ccan[ican]->cd(3);
		gPad->SetRightMargin(0.14);
		hpixel->Draw("colz");
	ccan[ican]->cd(4);
// 		hthetaC_gated_norm->GetXaxis()->SetRangeUser(350.,1350.);
// 		hthetaC_gated_norm->Draw("hist");
// 		hthetaC_norm->Draw("hist same");
// 		hthetaC_gated_norm->Draw("hist same");
// 		fthetaC_gated->Draw("same");
		hthetaC_gated->GetXaxis()->SetRangeUser(350.,1350.);
		hthetaC_gatedchrom->GetXaxis()->SetRangeUser(350.,1350.);
		hthetaC_gatedchrom->Draw("hist");
		hthetaC->SetLineColor(14);
		hthetaC->Draw("hist same");
		hthetaC_gated->Draw("hist same");
		fthetaC_gated->Draw("same");
		hthetaC_gatedchrom->Draw("hist same");
		fthetaC_gatedchrom->Draw("same");
		++itext; text[itext]->SetTextColor(kMagenta); text[itext]->SetTextAlign(32); text[itext]->DrawLatex(0.95,0.89,Form("Const:  %.1f #pm %.1f",gparsSPR[0],gparseSPR[0]));
		++itext; text[itext]->SetTextColor(kMagenta); text[itext]->SetTextAlign(32); text[itext]->DrawLatex(0.95,0.84,Form("Mean:   %.1f #pm %.2f",gparsSPR[1],gparseSPR[1]));
		++itext; text[itext]->SetTextColor(kMagenta); text[itext]->SetTextAlign(32); text[itext]->DrawLatex(0.95,0.79,Form("StdDev: %.2f #pm %.2f",gparsSPR[2],gparseSPR[2]));
		//++itext; text[itext]->SetTextColor(kMagenta); text[itext]->SetTextAlign(32); text[itext]->DrawLatex(0.95,0.70,Form("#chi^{2}= %.1f, NDF= %d",redchi2SPR,ndfSPR));
		++itext; text[itext]->SetTextColor(4); text[itext]->SetTextAlign(32); text[itext]->DrawLatex(0.95,0.59,Form("Const:  %.1f #pm %.1f",gparsSPRchr[0],gparseSPRchr[0]));
		++itext; text[itext]->SetTextColor(4); text[itext]->SetTextAlign(32); text[itext]->DrawLatex(0.95,0.54,Form("Mean:   %.1f #pm %.2f",gparsSPRchr[1],gparseSPRchr[1]));
		++itext; text[itext]->SetTextColor(4); text[itext]->SetTextAlign(32); text[itext]->DrawLatex(0.95,0.49,Form("StdDev: %.2f #pm %.2f",gparsSPRchr[2],gparseSPRchr[2]));
		//++itext; text[itext]->SetTextColor(1); text[itext]->SetTextAlign(32); text[itext]->DrawLatex(0.95,0.40,Form("#chi^{2}= %.1f, NDF= %d",redchi2SPR,ndfSPR));
	ccan[ican]->cd(5);
		gPad->SetRightMargin(0.14);
		hthittexp1_gated->GetXaxis()->SetRangeUser(valmin1,valmin1+20);
		hthittexp1_gated->GetYaxis()->SetRangeUser(valmin1,valmin1+20);
		hthittexp1_gated->Draw("colz");
		//++itext; text[itext]->SetTextColor(1); text[itext]->SetTextAlign(12); text[itext]->DrawLatex(0.16,0.7,Form("#LTtbeg1OPband#GT = %.2f",tbeg1OPband));
		gPad->Update();
		double xmin	= gPad->GetUxmin();
		double xmax	= gPad->GetUxmax();
		double ymin	= gPad->GetUymin();
		double ymax	= gPad->GetUymax();
		if (GunSim){
			++itext; text[itext]->SetTextColor(1); text[itext]->SetTextAlign(12); text[itext]->DrawLatex(0.16,0.8,Form("#LTtbeg1#GT = %.2f",tbeg1));
			++itext; text[itext]->SetTextColor(1); text[itext]->SetTextAlign(12); text[itext]->DrawLatex(0.16,0.75,Form("#LTtbeg1OP#GT = %.2f",tbeg1OP));
			++iline; line[iline]->DrawLine(tbeg1,ymin,tbeg1,ymax);				
			++iline; line[iline]->DrawLine(xmin,tbeg1,xmax,tbeg1);
			++iline; line[iline]->SetLineColor(kOrange+8); line[iline]->DrawLine(tbeg1OP,ymin,tbeg1OP,ymax);
			++iline; line[iline]->SetLineColor(kOrange+8); line[iline]->DrawLine(xmin,tbeg1OP,xmax,tbeg1OP);
		}
		//ldiag->Draw("same");
		//ldiagu->Draw("same");
		//ldiagl->Draw("same");
	ccan[ican]->cd(6);
		gPad->SetRightMargin(0.14);
		hthittexp2_gated->GetXaxis()->SetRangeUser(valmin2,valmin2+20);
		hthittexp2_gated->GetYaxis()->SetRangeUser(valmin2,valmin2+20);
		hthittexp2_gated->Draw("colz");
		//++itext; text[itext]->SetTextColor(1); text[itext]->SetTextAlign(12); text[itext]->DrawLatex(0.16,0.7,Form("#LTtbeg2OPband#GT = %.2f",tbeg2OPband));
		gPad->Update();
		xmin	= gPad->GetUxmin();
		xmax	= gPad->GetUxmax();
		ymin	= gPad->GetUymin();
		ymax	= gPad->GetUymax();
		if (GunSim){
			++itext; text[itext]->SetTextColor(1); text[itext]->SetTextAlign(12); text[itext]->DrawLatex(0.16,0.8,Form("#LTtbeg2#GT = %.2f",tbeg2));
			++itext; text[itext]->SetTextColor(1); text[itext]->SetTextAlign(12); text[itext]->DrawLatex(0.16,0.75,Form("#LTtbeg2OP#GT = %.2f",tbeg2OP));
			++iline; line[iline]->DrawLine(tbeg2,ymin,tbeg2,ymax);
			++iline; line[iline]->DrawLine(xmin,tbeg2,xmax,tbeg2);
			++iline; line[iline]->SetLineColor(kOrange+8); line[iline]->DrawLine(tbeg2OP,ymin,tbeg2OP,ymax);
			++iline; line[iline]->SetLineColor(kOrange+8); line[iline]->DrawLine(xmin,tbeg2OP,xmax,tbeg2OP);
		}
		//ldiag->Draw("same");
		//ldiagu->Draw("same");
		//ldiagl->Draw("same");
	ccan[ican]->cd(7);
		//hpixel_gated->Draw("colz");
		hAMBindex	->SetMinimum(0.5);
		hAMBindex	->SetStats(0);
		hAMBindex	->SetLineColor(4);
		hAMBindex	->SetFillColor(7);
		hAMBindex	->SetLineWidth(2);
		hAMBindex	->Draw();
		gPad		->Update();
		for (int i=0;i<8;i++){
			double x	= hAMBindex->GetBinCenter(i+1);
			double y	= 0.05*gPad->GetUymax();
			++itext; 	text[itext]->SetNDC(false);  	text[itext]->SetTextColor( 1); 
						text[itext]->SetTextAlign(12);
						text[itext]->SetTextAngle(90);	text[itext]->SetTextSize(0.07); 
			text[itext]->DrawLatex(x,y,AMBnames[i]);
		}
	ccan[ican]->cd(8);
		hthetaCestPeak->GetXaxis()->SetRangeUser(700.,1000.);
		hthetaCestPeak->Draw("hist");
		fthetaCestPeak->Draw("same");
		hthetaCest->GetXaxis()->SetRangeUser(700.,1000.);
		hthetaCest->Draw("hist same");
		fthetaCest->Draw("same");
		//
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		hthetaCestHypMost->Draw("hist same");
		hthetaCestHypMostPeak->Draw("hist same");
		//
		++itext; text[itext]->SetTextColor(1); text[itext]->SetTextAlign(12); text[itext]->DrawLatex(0.16,0.77,Form("#Theta_{C}^{expected} = %.1f",1000.*CerenkovAngleExpected));
		++itext; text[itext]->SetTextColor(4); text[itext]->SetTextAlign(32); text[itext]->DrawLatex(0.95,0.89,Form("Const:  %.1f #pm %.1f",gpars[0],gparse[0]));
		++itext; text[itext]->SetTextColor(4); text[itext]->SetTextAlign(32); text[itext]->DrawLatex(0.95,0.84,Form("Mean:   %.1f #pm %.2f",gpars[1],gparse[1]));
		++itext; text[itext]->SetTextColor(4); text[itext]->SetTextAlign(32); text[itext]->DrawLatex(0.95,0.79,Form("StdDev: %.1f #pm %.2f",gpars[2],gparse[2]));
		//++itext; text[itext]->SetTextColor(4); text[itext]->SetTextAlign(32); text[itext]->DrawLatex(0.95,0.70,Form("#chi^{2}= %.1f, NDF= %d",redchi2,ndf));
		++itext; text[itext]->SetTextColor(kGreen+2); text[itext]->SetTextAlign(32); text[itext]->DrawLatex(0.95,0.59,Form("Const:  %.1f #pm %.1f",gparsP[0],gparseP[0]));
		++itext; text[itext]->SetTextColor(kGreen+2); text[itext]->SetTextAlign(32); text[itext]->DrawLatex(0.95,0.54,Form("Mean:   %.1f #pm %.2f",gparsP[1],gparseP[1]));
		++itext; text[itext]->SetTextColor(kGreen+2); text[itext]->SetTextAlign(32); text[itext]->DrawLatex(0.95,0.49,Form("StdDev: %.1f #pm %.2f",gparsP[2],gparseP[2]));
		//++itext; text[itext]->SetTextColor(kGreen+2); text[itext]->SetTextAlign(32); text[itext]->DrawLatex(0.95,0.30,Form("#chi^{2}= %.1f, NDF= %d",redchi2P,ndfP));
	ccan[ican]->cd(9);
		gPad->SetRightMargin(0.14);
		hdt1texp1->GetXaxis()->SetRangeUser(tbeg1OP-2.,tbeg1OP+30.);
		hdt1texp1->Draw("colz");
		fTimeCut1U->Draw("same");
		fTimeCut1L->Draw("same");
		gPad->Update();
		xmin	= gPad->GetUxmin();
		xmax	= gPad->GetUxmax();
		ymin	= gPad->GetUymin();
		ymax	= gPad->GetUymax();
		if (GunSim){
			++iline; line[iline]->SetLineColor(kOrange+8); line[iline]->DrawLine(tbeg1OP,ymin,tbeg1OP,ymax);
			++iline; line[iline]->SetLineWidth(1); line[iline]->DrawLine(tbeg1,ymin,tbeg1,ymax);				
			++iline; line[iline]->SetLineWidth(1); line[iline]->DrawLine(xmin,tbeg1,xmax,tbeg1);
		}
	ccan[ican]->cd(10);
		gPad->SetRightMargin(0.14);
		hdt2texp2->GetXaxis()->SetRangeUser(tbeg2OP-2.,tbeg2OP+30.);
		hdt2texp2->Draw("colz");
		fTimeCut2U->Draw("same");
		fTimeCut2L->Draw("same");
		gPad->Update();
		xmin	= gPad->GetUxmin();
		xmax	= gPad->GetUxmax();
		ymin	= gPad->GetUymin();
		ymax	= gPad->GetUymax();
		if (GunSim){
			++iline; line[iline]->SetLineColor(kOrange+8); line[iline]->DrawLine(tbeg2OP,ymin,tbeg2OP,ymax);
			++iline; line[iline]->SetLineWidth(1); line[iline]->DrawLine(tbeg2,ymin,tbeg2,ymax);
			++iline; line[iline]->SetLineWidth(1); line[iline]->DrawLine(xmin,tbeg2,xmax,tbeg2);
		}
	ccan[ican]->cd(11);
		++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(15,-0.01,175,0.01);
				  frame[iframe] ->SetTitle("Chromatic Intercept (rad) vs DetTheta (deg)");
				  frame[iframe] ->SetTitleSize(0.06,"t");
		gChrom1Inter_Theta	->SetMarkerColor(kCyan+1);
		gChrom2Inter_Theta	->SetMarkerColor(kMagenta+2);
		gChrom1Inter_Theta	->SetLineColor(kCyan+1);
		gChrom2Inter_Theta	->SetLineColor(kMagenta+2);
		gChrom1Inter_Theta	->SetMarkerStyle(23);
		gChrom2Inter_Theta	->SetMarkerStyle(22);
		gChrom1Inter_Theta	->Draw("PL");
		gChrom2Inter_Theta	->Draw("PL");
			TLegend *legdir	= new TLegend(0.62,0.7,0.97,0.9);
					 legdir ->SetTextSize(0.07);
					 legdir ->SetTextAlign(32);
					 legdir	->AddEntry(gChrom2Inter_Theta,"Mirror-going","PL");
					 legdir	->AddEntry(gChrom1Inter_Theta,"Prism-going","PL");
					 legdir	->Draw("same");
		gthisfit1int->Draw("P");
		gthisfit2int->Draw("P");
	ccan[ican]->cd(12);
		++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(15,5.,175,70.);
				  frame[iframe] ->SetTitle("Chromatic Slope (rad.ns) vs DetTheta (deg)");
				  frame[iframe] ->SetTitleSize(0.06,"t");
		gChrom1Slope_Theta	->SetMarkerColor(kCyan+1);
		gChrom2Slope_Theta	->SetMarkerColor(kMagenta+2);
		gChrom1Slope_Theta	->SetLineColor(kCyan+1);
		gChrom2Slope_Theta	->SetLineColor(kMagenta+2);
		gChrom1Slope_Theta	->SetMarkerStyle(23);
		gChrom2Slope_Theta	->SetMarkerStyle(22);
		gChrom1Slope_Theta	->Draw("PL");
		gChrom2Slope_Theta	->Draw("PL");
					 legdir	->Draw("same");
		gthisfit1slo->Draw("P");
		gthisfit2slo->Draw("P");
	ccan[ican]->cd(13);
		gPad->SetRightMargin(0.14);
//		double valf1	= fChrom1->Eval(0);
		double valf1	= expected;
		hthetaC_dt1_gated->GetYaxis()->SetRangeUser(valf1-20,valf1+20);
		hthetaC_dt1_gated->Draw("colz");
		gPad->Update();
		TPaletteAxis *palette1 = (TPaletteAxis*)hthetaC_dt1_gated->GetListOfFunctions()->FindObject("palette");
			if (palette1) { palette1->SetY1NDC(0.20); palette1->SetY2NDC(0.80); }
			gPad->Modified(); gPad->Update();
		hChrom1_mean->Draw("hist same");
		fChrom1->Draw("same");
	ccan[ican]->cd(14);
		gPad->SetRightMargin(0.14);
//		double valf2	= fChrom2->Eval(0);
		double valf2	= expected;
		hthetaC_dt2_gated->GetYaxis()->SetRangeUser(valf2-20,valf2+20);
		hthetaC_dt2_gated->Draw("colz");
		gPad->Update();
		TPaletteAxis *palette2 = (TPaletteAxis*)hthetaC_dt2_gated->GetListOfFunctions()->FindObject("palette");
			if (palette2) { palette2->SetY1NDC(0.20); palette2->SetY2NDC(0.80); }
			gPad->Modified(); gPad->Update();
		hChrom2_mean->Draw("hist same");
		fChrom2->Draw("same");
	ccan[ican]->cd(15);
		gPad->SetRightMargin(0.14);
		hthetaCchr_dt1_gated->GetYaxis()->SetRangeUser(valf1-20,valf1+20);
		hthetaCchr_dt1_gated->Draw("colz");
		gPad->Update();
		TPaletteAxis *palette3 = (TPaletteAxis*)hthetaCchr_dt1_gated->GetListOfFunctions()->FindObject("palette");
			if (palette3) { palette3->SetY1NDC(0.20); palette3->SetY2NDC(0.80); }
			gPad->Modified(); gPad->Update();
	ccan[ican]->cd(16);
		gPad->SetRightMargin(0.14);
		hthetaCchr_dt2_gated->GetYaxis()->SetRangeUser(valf2-20,valf2+20);
		hthetaCchr_dt2_gated->Draw("colz");
		gPad->Update();
		TPaletteAxis *palette4 = (TPaletteAxis*)hthetaCchr_dt2_gated->GetListOfFunctions()->FindObject("palette");
			if (palette4) { palette4->SetY1NDC(0.20); palette4->SetY2NDC(0.80); }
			gPad->Modified(); gPad->Update();
 	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(PlotFileO.Data());

	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("Page %d",ican),10+30*ican,30+30*ican,1000,700);
	ccan[ican]->cd(); ccan[ican]->Divide(4,2,0.0001,0.0001);
	ccan[ican]->cd(1);
		gPad->SetLogx(1);
		gPad->SetLogy(1);
		hnhits->Draw();
		++itext; text[itext]->SetTextColor(1); text[itext]->SetTextAlign(32); 
				 text[itext]->DrawLatex(0.9,0.85,Form("largest = %d",nhitmax));	
	ccan[ican]->cd(2);
		hhyp_best->SetMinimum(0.5);
		hhyp_best->SetFillColor(17);
		hhyp_best->Draw();
	ccan[ican]->cd(3);
		//hdthetaC_gated->GetXaxis()->SetRangeUser(-30,30);
		//hdthetaC_gatedchrom->GetXaxis()->SetRangeUser(-30,30);
		hdthetaC_gatedchrom->Draw("hist");
		hdthetaC_gated->Draw("hist same");
	ccan[ican]->cd(4);
		gPad->SetRightMargin(0.14);
		hpixeltime->Draw("colz");
	ccan[ican]->cd(5);
		gPad->SetRightMargin(0.14);
//hpixeltime_gated->SetMinimum(37);		
		hpixeltime_gated->Draw("colz");
	ccan[ican]->cd(6);
		hcosanglut->Draw();
		gPad->Update();
		ymax	= gPad->GetUymax();
		if (GunSim){
			++iline; line[iline]->DrawLine(LIM_cosang_lower,0,LIM_cosang_lower,ymax);
			++iline; line[iline]->DrawLine(LIM_cosang_upper,0,LIM_cosang_upper,ymax);
			//++iline; line[iline]->SetLineColor(kGreen+2); line[iline]->DrawLine(-LIM_cosang_lower,0,-LIM_cosang_lower,ymax);
			//++iline; line[iline]->SetLineColor(kGreen+2); line[iline]->DrawLine(-LIM_cosang_upper,0,-LIM_cosang_upper,ymax);
		}
 	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(PlotFile.Data());


	//---- page concentrating on chromatic correction
	//
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("Page %d",ican),10+30*ican,30+30*ican,1000,700);
	ccan[ican]->cd(); ccan[ican]->Divide(4,4,0.0001,0.0001);
	ccan[ican]->cd(1);
		gPad->SetRightMargin(0.14);
		hthittexp1_gated->GetXaxis()->SetRangeUser(valmin1,valmin1+20);
		hthittexp1_gated->GetYaxis()->SetRangeUser(valmin1,valmin1+20);
		hthittexp1_gated->Draw("colz");
		if (GunSim){
			++itext; text[itext]->SetTextColor(1); text[itext]->SetTextAlign(12); text[itext]->DrawLatex(0.16,0.8,Form("#LTtbeg1#GT = %.2f",tbeg1));
			++itext; text[itext]->SetTextColor(1); text[itext]->SetTextAlign(12); text[itext]->DrawLatex(0.16,0.75,Form("#LTtbeg1OP#GT = %.2f",tbeg1OP));
			//++itext; text[itext]->SetTextColor(1); text[itext]->SetTextAlign(12); text[itext]->DrawLatex(0.16,0.7,Form("#LTtbeg1OPband#GT = %.2f",tbeg1OPband));
			gPad->Update();
			xmin	= gPad->GetUxmin();
			xmax	= gPad->GetUxmax();
			ymin	= gPad->GetUymin();
			ymax	= gPad->GetUymax();
			++iline; line[iline]->DrawLine(tbeg1,ymin,tbeg1,ymax);				
			++iline; line[iline]->DrawLine(xmin,tbeg1,xmax,tbeg1);
			++iline; line[iline]->SetLineColor(kOrange+8); line[iline]->DrawLine(tbeg1OP,ymin,tbeg1OP,ymax);
			++iline; line[iline]->SetLineColor(kOrange+8); line[iline]->DrawLine(xmin,tbeg1OP,xmax,tbeg1OP);
		}
		//ldiag->Draw("same");
		//ldiagu->Draw("same");
		//ldiagl->Draw("same");
	ccan[ican]->cd(2);
		gPad->SetRightMargin(0.14);
		hthittexp2_gated->GetXaxis()->SetRangeUser(valmin2,valmin2+20);
		hthittexp2_gated->GetYaxis()->SetRangeUser(valmin2,valmin2+20);
		hthittexp2_gated->Draw("colz");
		if (GunSim){
			++itext; text[itext]->SetTextColor(1); text[itext]->SetTextAlign(12); text[itext]->DrawLatex(0.16,0.8,Form("#LTtbeg2#GT = %.2f",tbeg2));
			++itext; text[itext]->SetTextColor(1); text[itext]->SetTextAlign(12); text[itext]->DrawLatex(0.16,0.75,Form("#LTtbeg2OP#GT = %.2f",tbeg2OP));
			//++itext; text[itext]->SetTextColor(1); text[itext]->SetTextAlign(12); text[itext]->DrawLatex(0.16,0.7,Form("#LTtbeg2OPband#GT = %.2f",tbeg2OPband));
			gPad->Update();
			xmin	= gPad->GetUxmin();
			xmax	= gPad->GetUxmax();
			ymin	= gPad->GetUymin();
			ymax	= gPad->GetUymax();
			++iline; line[iline]->DrawLine(tbeg2,ymin,tbeg2,ymax);
			++iline; line[iline]->DrawLine(xmin,tbeg2,xmax,tbeg2);
			++iline; line[iline]->SetLineColor(kOrange+8); line[iline]->DrawLine(tbeg2OP,ymin,tbeg2OP,ymax);
			++iline; line[iline]->SetLineColor(kOrange+8); line[iline]->DrawLine(xmin,tbeg2OP,xmax,tbeg2OP);
		}
		//ldiag->Draw("same");
		//ldiagu->Draw("same");
		//ldiagl->Draw("same");
	ccan[ican]->cd(3);
		gPad->SetRightMargin(0.14);
		hWL_dt1_gated	->Draw("colz");
	ccan[ican]->cd(4);
		gPad->SetRightMargin(0.14);
		hWL_dt2_gated	->Draw("colz");
	ccan[ican]->cd(5);
		gPad->SetRightMargin(0.14);
		hdt1texp1->GetXaxis()->SetRangeUser(tbeg1OP-2.,tbeg1OP+30.);
		hdt1texp1->Draw("colz");
		fTimeCut1U->Draw("same");
		fTimeCut1L->Draw("same");
		if (GunSim){
			gPad->Update();
			xmin	= gPad->GetUxmin();
			xmax	= gPad->GetUxmax();
			ymin	= gPad->GetUymin();
			ymax	= gPad->GetUymax();
			++iline; line[iline]->DrawLine(tbeg1,ymin,tbeg1,ymax);				
			++iline; line[iline]->DrawLine(xmin,tbeg1,xmax,tbeg1);
			++iline; line[iline]->SetLineColor(kOrange+8); line[iline]->DrawLine(tbeg1OP,ymin,tbeg1OP,ymax);
		}
	ccan[ican]->cd(6);
		gPad->SetRightMargin(0.14);
		hdt2texp2->GetXaxis()->SetRangeUser(tbeg2OP-2.,tbeg2OP+30.);
		hdt2texp2->Draw("colz");
		fTimeCut2U->Draw("same");
		fTimeCut2L->Draw("same");
		if (GunSim){
			gPad->Update();
			xmin	= gPad->GetUxmin();
			xmax	= gPad->GetUxmax();
			ymin	= gPad->GetUymin();
			ymax	= gPad->GetUymax();
			++iline; line[iline]->DrawLine(tbeg2,ymin,tbeg2,ymax);
			++iline; line[iline]->DrawLine(xmin,tbeg2,xmax,tbeg2);
			++iline; line[iline]->SetLineColor(kOrange+8); line[iline]->DrawLine(tbeg2OP,ymin,tbeg2OP,ymax);
		}
	ccan[ican]->cd(7);
		++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(20,-0.02,170,0.02);
				  frame[iframe] ->SetTitle("Chromatic Intercept (rad) vs DetTheta (deg)");
				  frame[iframe] ->SetTitleSize(0.06,"t");
		gChrom1Inter_Theta	->SetMarkerColor(kCyan+1);
		gChrom2Inter_Theta	->SetMarkerColor(kMagenta+2);
		gChrom1Inter_Theta	->SetLineColor(kCyan+1);
		gChrom2Inter_Theta	->SetLineColor(kMagenta+2);
		gChrom1Inter_Theta	->SetMarkerStyle(23);
		gChrom2Inter_Theta	->SetMarkerStyle(22);
		gChrom1Inter_Theta	->Draw("PL");
		gChrom2Inter_Theta	->Draw("PL");
					 legdir	->Draw("same");
		gthisfit1int->Draw("P");
		gthisfit2int->Draw("P");
	ccan[ican]->cd(8);
		++iframe; frame[iframe] = (TH1F*)gPad->DrawFrame(20,20.,170,100.);
				  frame[iframe] ->SetTitle("Chromatic Slope (rad.ns) vs DetTheta (deg)");
				  frame[iframe] ->SetTitleSize(0.06,"t");
		gChrom1Slope_Theta	->SetMarkerColor(kCyan+1);
		gChrom2Slope_Theta	->SetMarkerColor(kMagenta+2);
		gChrom1Slope_Theta	->SetLineColor(kCyan+1);
		gChrom2Slope_Theta	->SetLineColor(kMagenta+2);
		gChrom1Slope_Theta	->SetMarkerStyle(23);
		gChrom2Slope_Theta	->SetMarkerStyle(22);
		gChrom1Slope_Theta	->Draw("PL");
		gChrom2Slope_Theta	->Draw("PL");
					 legdir	->Draw("same");
		gthisfit1slo->Draw("P");
		gthisfit2slo->Draw("P");
	ccan[ican]->cd(9);
		gPad->SetRightMargin(0.14);
//		valf1	= fChrom1->Eval(0);
		valf1	= expected;
		hthetaC_dt1_gated->GetYaxis()->SetRangeUser(valf1-20,valf1+20);
		hthetaC_dt1_gated->Draw("colz");
		gPad->Update();
		//TPaletteAxis *palette1 = (TPaletteAxis*)hthetaC_dt1_gated->GetListOfFunctions()->FindObject("palette");
			if (palette1) { palette1->SetY1NDC(0.20); palette1->SetY2NDC(0.80); }
			gPad->Modified(); gPad->Update();
		hChrom1_mean->Draw("hist same");
		fChrom1->Draw("same");
	ccan[ican]->cd(10);
		gPad->SetRightMargin(0.14);
//		valf2	= fChrom2->Eval(0);
		valf2	= expected;
		hthetaC_dt2_gated->GetYaxis()->SetRangeUser(valf2-20,valf2+20);
		hthetaC_dt2_gated->Draw("colz");
		gPad->Update();
		//TPaletteAxis *palette2 = (TPaletteAxis*)hthetaC_dt2_gated->GetListOfFunctions()->FindObject("palette");
			if (palette2) { palette2->SetY1NDC(0.20); palette2->SetY2NDC(0.80); }
			gPad->Modified(); gPad->Update();
		hChrom2_mean->Draw("hist same");
		fChrom2->Draw("same");
	ccan[ican]->cd(11);
		gPad->SetRightMargin(0.14);
		hthetaC_WL1_gated->GetYaxis()->SetRangeUser(valf1-20,valf1+20);
		hthetaC_WL1_gated->Draw("colz");
		gPad->Update();
	ccan[ican]->cd(12);
		gPad->SetRightMargin(0.14);
		valf2	= fChrom2->Eval(0);
		hthetaC_WL2_gated->GetYaxis()->SetRangeUser(valf2-20,valf2+20);
		hthetaC_WL2_gated->Draw("colz");
		gPad->Update();
	ccan[ican]->cd(13);
		gPad->SetRightMargin(0.14);
		hthetaCchr_dt1_gated->GetYaxis()->SetRangeUser(valf1-20,valf1+20);
		hthetaCchr_dt1_gated->Draw("colz");
		gPad->Update();
		//TPaletteAxis *palette3 = (TPaletteAxis*)hthetaCchr_dt1_gated->GetListOfFunctions()->FindObject("palette");
			if (palette3) { palette3->SetY1NDC(0.20); palette3->SetY2NDC(0.80); }
			gPad->Modified(); gPad->Update();
	ccan[ican]->cd(14);
		gPad->SetRightMargin(0.14);
		hthetaCchr_dt2_gated->GetYaxis()->SetRangeUser(valf2-20,valf2+20);
		hthetaCchr_dt2_gated->Draw("colz");
		gPad->Update();
		//TPaletteAxis *palette4 = (TPaletteAxis*)hthetaCchr_dt2_gated->GetListOfFunctions()->FindObject("palette");
			if (palette4) { palette4->SetY1NDC(0.20); palette4->SetY2NDC(0.80); }
			gPad->Modified(); gPad->Update();
	ccan[ican]->cd(15);
		gPad->SetRightMargin(0.14);
		hthetaCchr_WL1_gated->GetYaxis()->SetRangeUser(valf1-20,valf1+20);
		hthetaCchr_WL1_gated->Draw("colz");
		gPad->Update();
	ccan[ican]->cd(16);
		gPad->SetRightMargin(0.14);
		hthetaCchr_WL2_gated->GetYaxis()->SetRangeUser(valf2-20,valf2+20);
		hthetaCchr_WL2_gated->Draw("colz");
		gPad->Update();
 	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(PlotFile.Data());


	//---- pages with PID performance
	//
	//
	gStyle->SetStatX(0.84);
	gStyle->SetStatY(0.55);
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("Page %d",ican),10+30*ican,30+30*ican,1500,900);
	ccan[ican]->cd(); ccan[ican]->Divide(1,1,0.0001,0.0001);
	ccan[ican]->cd(1);
		gPad->SetRightMargin(0.14);
		gPad->SetLogz(1);
		hthetaC_ptot->SetStats(1);	
		hthetaC_ptot->Draw("colz");
		gPad->Update(); 
		TPaveStats *st = (TPaveStats*)hthetaC_ptot->GetListOfFunctions()->FindObject("stats");
		if (st) { st->SetOptStat(1000011); } 
		for (int ihyp=0;ihyp<NHYP;ihyp++){
			fthCexp_ptot[ihyp]->Draw("same");
		}
		++itext; text[itext]->SetTextAlign(32); text[itext]->SetTextFont(40); text[itext]->SetTextSize(0.03); 
				 text[itext]->DrawLatex(0.84,0.395,Form("DIRC Geometric Reconstruction"));
		++itext; text[itext]->SetTextAlign(32); text[itext]->SetTextFont(40); text[itext]->SetTextSize(0.03); 
				 text[itext]->DrawLatex(0.84,0.36,Form("%s (npsim)",InputNameBase.Data()));
		++itext; text[itext]->SetTextAlign(32); text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.03); 
				 text[itext]->DrawLatex(0.84,0.32,Form("100k events, epic.xml, -2<#eta<1.6 "));
		++itext; text[itext]->SetTextAlign(32); text[itext]->SetTextFont(40); text[itext]->SetTextSize(0.03); 
				 text[itext]->DrawLatex(0.84,0.285,Form("10 LUTs for 10 bars, 20M OP each (npsim)"));
		++itext; text[itext]->SetTextAlign(32); text[itext]->SetTextFont(40); text[itext]->SetTextSize(0.03); 
				 text[itext]->DrawLatex(0.84,0.25,Form("Chromatic-corrected "));
		++itext; text[itext]->SetTextAlign(32); text[itext]->SetTextFont(40); text[itext]->SetTextSize(0.03); 
				 text[itext]->DrawLatex(0.84,0.215,Form("3 hypotheses "));
 	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(PlotFile.Data());
	//
	gStyle->SetStatX(0.84);
	gStyle->SetStatY(0.55);
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("Page %d",ican),10+30*ican,30+30*ican,1500,900);
	ccan[ican]->cd(); ccan[ican]->Divide(1,1,0.0001,0.0001);
	ccan[ican]->cd(1);
		gPad->SetRightMargin(0.14);
		gPad->SetLogz(1);
		hthetaC_ptot->SetStats(1);	
		hthetaC_ptot->GetYaxis()->SetRangeUser(740,840);
		hthetaC_ptot->DrawCopy("colz");
		gPad->Update(); 
		st = (TPaveStats*)hthetaC_ptot->GetListOfFunctions()->FindObject("stats");
		if (st) { st->SetOptStat(1000011); } 
		for (int ihyp=0;ihyp<NHYP;ihyp++){
			fthCexp_ptot[ihyp]->Draw("same");
		}
		++itext; text[itext]->SetTextAlign(32); text[itext]->SetTextFont(40); text[itext]->SetTextSize(0.03); 
				 text[itext]->DrawLatex(0.84,0.395,Form("DIRC Geometric Reconstruction"));
		++itext; text[itext]->SetTextAlign(32); text[itext]->SetTextFont(40); text[itext]->SetTextSize(0.03); 
				 text[itext]->DrawLatex(0.84,0.36,Form("%s (npsim)",InputNameBase.Data()));
		++itext; text[itext]->SetTextAlign(32); text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.03); 
				 text[itext]->DrawLatex(0.84,0.32,Form("100k events, epic.xml, -2<#eta<1.6 "));
		++itext; text[itext]->SetTextAlign(32); text[itext]->SetTextFont(40); text[itext]->SetTextSize(0.03); 
				 text[itext]->DrawLatex(0.84,0.285,Form("10 LUTs for 10 bars, 20M OP each (npsim)"));
		++itext; text[itext]->SetTextAlign(32); text[itext]->SetTextFont(40); text[itext]->SetTextSize(0.03); 
				 text[itext]->DrawLatex(0.84,0.25,Form("Chromatic-corrected "));
		++itext; text[itext]->SetTextAlign(32); text[itext]->SetTextFont(40); text[itext]->SetTextSize(0.03); 
				 text[itext]->DrawLatex(0.84,0.215,Form("3 hypotheses "));
 	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(PlotFile.Data());
	//
	hthetaC_ptot->GetYaxis()->SetRangeUser(0,875);
	//
	//---- thetaC and dthetaC vs ptot by Truth
	gStyle->SetStatX(0.84);
	gStyle->SetStatY(0.20);
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("Page %d",ican),10+30*ican,30+30*ican,1500,900);
	ccan[ican]->cd(); ccan[ican]->Divide(NHYP,3,0.0001,0.0001);
// 	for (int ihyp=0;ihyp<NHYP;ihyp++){
// 		ccan[ican]->cd(1+ihyp);
// 			gPad->SetRightMargin(0.14);
// 			gPad->SetLogz(1);
// 			hthetaC_ptot_byTruth[ihyp]->SetStats(0);	
// 			hthetaC_ptot_byTruth[ihyp]->Draw("colz");
// 			gPad->Update(); 
// 			st = (TPaveStats*)hthetaC_ptot_byTruth[0]->GetListOfFunctions()->FindObject("stats");
// 			if (st) { st->SetOptStat(1000011); } 
// 			for (int ihyp=0;ihyp<NHYP;ihyp++){
// 				fthCexp_ptot[ihyp]->Draw("same");
// 			}
// 	}
	for (int ihyp=0;ihyp<NHYP;ihyp++){
		ccan[ican]->cd(1+ihyp);
			gPad->SetRightMargin(0.14);
			gPad->SetLogz(1);
			hdthetaC_ptot_byTruth[ihyp]->SetStats(0);	
			hdthetaC_ptot_byTruth[ihyp]->Draw("colz");
			ly0->Draw("same");
	}
	for (int ihyp=0;ihyp<NHYP;ihyp++){
		ccan[ican]->cd(1+NHYP+ihyp);
			gPad->SetRightMargin(0.14);
			gPad->SetLogz(1);
			hdthetaC_bar_byTruth[ihyp]->SetStats(0);	
			hdthetaC_bar_byTruth[ihyp]->Draw("colz");
			ly0->DrawLine(-0.5,0,9.5,0);
	}
	for (int ihyp=0;ihyp<NHYP;ihyp++){
		ccan[ican]->cd(1+2*NHYP+ihyp);
			gPad->SetRightMargin(0.14);
			gPad->SetLogz(1);
			hdthetaC_eta_byTruth[ihyp]->SetStats(0);	
			hdthetaC_eta_byTruth[ihyp]->Draw("colz");
			ly0->DrawLine(-2.0,0,1.6,0);
	}
// 	ccan[ican]->cd(9);
// 		hhypParent_hit	->SetMinimum(0.0);
// 		hhypParent_hit	->Draw();
// 	ccan[ican]->cd(10);
// 		gPad->SetLogy(1);
// 		hHitPurity		->SetFillColor(7);
// 		hHitPurity		->Draw();
 	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(PlotFile.Data());
	//
	//---- dthetaC vs nhbox by Truth
	gStyle->SetStatX(0.84);
	gStyle->SetStatY(0.20);
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("Page %d",ican),10+30*ican,30+30*ican,1500,900);
	ccan[ican]->cd(); ccan[ican]->Divide(NHYP,3,0.0001,0.0001);
	for (int ihyp=0;ihyp<NHYP;ihyp++){
		ccan[ican]->cd(1+ihyp);
			gPad->SetRightMargin(0.14);
			gPad->SetLogz(1);
			hdthetaC_nhbox_byTruth[ihyp]->SetStats(0);	
			hdthetaC_nhbox_byTruth[ihyp]->GetXaxis()->SetRangeUser(0.5,20.5);
			hdthetaC_nhbox_byTruth[ihyp]->Draw("colz");
			ly0->Draw("same");
	}
	for (int ihyp=0;ihyp<NHYP;ihyp++){
		ccan[ican]->cd(1+NHYP+ihyp);
			gPad->SetRightMargin(0.14);
			gPad->SetLogz(1);
			hdthetaC_nhbox_byTruth_mean[ihyp]->SetStats(0);	
			hdthetaC_nhbox_byTruth_mean[ihyp]->SetMinimum(-10);	
			hdthetaC_nhbox_byTruth_mean[ihyp]->SetMaximum( 10);	
			hdthetaC_nhbox_byTruth_mean[ihyp]->GetXaxis()->SetRangeUser(0.5,20.5);
			hdthetaC_nhbox_byTruth_mean[ihyp]->Draw("ep");
			//hdthetaC_nhbox_byTruth_mean[ihyp]->Draw("hist same");
	}
	for (int ihyp=0;ihyp<NHYP;ihyp++){
		ccan[ican]->cd(1+2*NHYP+ihyp);
			gPad->SetRightMargin(0.14);
			gPad->SetLogz(1);
			hdthetaC_nhbox_byTruth_sigm[ihyp]->SetStats(0);	
			hdthetaC_nhbox_byTruth_sigm[ihyp]->SetMinimum( 0);	
			hdthetaC_nhbox_byTruth_sigm[ihyp]->SetMaximum( 8);	
			hdthetaC_nhbox_byTruth_sigm[ihyp]->GetXaxis()->SetRangeUser(0.5,20.5);
			hdthetaC_nhbox_byTruth_sigm[ihyp]->Draw("ep");
			//hdthetaC_nhbox_byTruth_sigm[ihyp]->Draw("hist same");
	}
 	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(PlotFile.Data());

	//---- event displays
	//
// 	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("Page %d",ican),10+30*ican,30+30*ican,1500,900);
// 	ccan[ican]->cd(); ccan[ican]->Divide(2,2,0.0001,0.0001);
// 	ccan[ican]->cd(1);
// 		gPad->SetRightMargin(0.14);
// 		hyx0->Draw("colz");
// 	ccan[ican]->cd(2);
// 		gPad->SetRightMargin(0.14);
// 		hyx1->Draw("colz");
// 	ccan[ican]->cd(3);
// 		gPad->SetRightMargin(0.14);
// 		hyx2->Draw("colz");
// 	ccan[ican]->cd(4);
// 		gPad->SetRightMargin(0.14);
// 		hyx3->Draw("colz");
//  	ccan[ican]->cd(); ccan[ican]->Update();
// 	ccan[ican]->Print(PlotFile.Data());


	//---- close-out...
	//
	cout<<"Writing "<<OutputFile.Data()<<endl;
	fout->cd();
	fout->Write();
	//fChrom1->Write();
	//fChrom2->Write();
	//
	ccan[ican]->Print(PlotFileC.Data());
	TString Command	= TString( Form( "ps2pdf %s %s",PlotFile.Data(),PlotFileP.Data() ) );
	cout<<Command<<endl;
	int iStat = gSystem->Exec(Command);
	if (iStat==0){
		Command	= TString( Form( "rm %s",PlotFile.Data() ) );
		cout<<Command<<endl;
		iStat = gSystem->Exec(Command);
	}
	//fout->Close();
	
}
//------ END


//------------------------------------------------------------------
//	fitslices failures produce outliers in the chromatic correction fits
//	increase errors on these points to make them irrelevant to the linear fit..
//
void dircGR::CheckSlices(double expected, TH1D* h){
	if (!h) return;
	int nbx	= h->GetNbinsX();
	for (int ibx=1;ibx<=nbx;ibx++){
		double x	= h->GetBinCenter(ibx);
		double y	= h->GetBinContent(ibx);
		double ye	= h->GetBinError(ibx);
		if (y>0 && fabs(y-expected)/expected > 0.02){	// this slice fit failed... ignore it!
			ye		= 50.;		// blow up uncertainty on this point...
			h->SetBinError(ibx,ye);
		}
		//cout<<h->GetName()<<"\t "<<ibx<<" "<<x<<" "<<y<<" "<<ye<<endl;
	}
}

//------------------------------------------------------------------
//
//
void dircGR::GetIdeal(double expected, TH2D* hthetaC_WL_gated, TF1* fchr, TF1* fchrC){
	//
	int n	= hthetaC_WL_gated->Integral();
	if (n > 100){
		TString hname	= hthetaC_WL_gated->GetName();
		TH1D *htemp		= (TH1D*)hthetaC_WL_gated->ProjectionY("htemp",8,8);
		TF1  *agaus		= new TF1("agaus","gaus",700,900);
		agaus	->SetParameter(0,   10000);
		agaus	->SetParameter(1,expected);
		agaus	->SetParameter(2,       4);
		agaus	->SetParLimits(2,0,4.5);
		htemp	->Fit(agaus,"BQ0RN");
		//
		double dint		= agaus->GetParameter(1) - expected; 
		double slope	= fchr->GetParameter(1);
		double dx		= 0 - 370.;
		double dy		= slope*dx;
		double newint	= dint + dy;
		fchrC->SetParameter(0,newint);
		//
		cout<<SteerString<<" IDEAL "<<hname<<"\t"<<expected<<"\t pars: "
			<<fchrC->GetParameter(0)<<" "<<fchrC->GetParameter(1)
			<<"\t dtheta(370): "<<dint<<"\t "
			<<slope<<" "<<newint<<"\t Eval: "
			<<fchrC->Eval(300)<<" "
			<<fchrC->Eval(370)<<" "
			<<fchrC->Eval(650)<<" "
			<<endl;
		delete agaus;	
		delete htemp;	
	}
	//
}
//---- end getideal	

//--------------------------------------------------------------------
//---- find MCParticle creating this hit
//
int dircGR::GetTruthIndex(int ihit){
	int imc;
	//
	if (ihit >= _DIRCBarHits_particle_) {
		std::cout<<"Hit "<<ihit<<" has no truth-level mapping array entry."<<std::endl;
		return -1;		// no result
	}
	int mcParticleIndex = _DIRCBarHits_particle_index[ihit];
	if (mcParticleIndex >= 0 && mcParticleIndex < MCParticles_) {
		std::vector<int> path = getHitAncestryIndices(ihit);
		//
		imc	= path.back();
		//
		//int pdgCode	= MCParticles_PDG[imc];
		//cout<<ihit<<"\t "<<path[0]<<" "<<path.back()<<"\t"
		//	<<pdgCode
		//	<<endl;
		//
	}
	return imc;
}

//--------------------------------------------------------------------
//
double dircGR::GetWavelength(int ihit){
	//
	//---- find MCParticle creating this hit, when OP found: get wavelength
	// Safety Check: Avoid reading past the end of the unrolled relations lookup array
	if (ihit >= _DIRCBarHits_particle_) {
		std::cout<<"Hit "<<ihit<<" has no truth-level mapping array entry."<<std::endl;
		return 0.;		// no result
	}
	// Extract the absolute row index targeting the MCParticles collection
	int mcParticleIndex = _DIRCBarHits_particle_index[ihit];
	// Safety Range Check: Ensure the target index exists inside MCParticles
	if (mcParticleIndex >= 0 && mcParticleIndex < MCParticles_) {
		std::vector<int> path = getHitAncestryIndices(ihit);
		//
		//	 path[0] is the Optical Photon
		//	 path[1] is Gen 1 parent
		//	 path.back() is the last tracked particle before the beam
		//
		//if (!path.empty()) {
		//	cout<<ihit<<"\t OPindex: "<<mcParticleIndex<<"\t";
		//	for (int igen=0;igen<path.size();igen++){ cout<<path[igen]<<" "; }
		//	cout<<endl;
		//}
		//
		int pdgCode	= MCParticles_PDG[mcParticleIndex];
		double px	= MCParticles_momentum_x[mcParticleIndex];
		double py	= MCParticles_momentum_y[mcParticleIndex];
		double pz	= MCParticles_momentum_z[mcParticleIndex];
		double dirz	= pz/sqrt(px*px+py*py+pz*pz);
		double vtxx	= MCParticles_vertex_x[mcParticleIndex];
		double vtxy	= MCParticles_vertex_y[mcParticleIndex];
		double vtxz	= MCParticles_vertex_z[mcParticleIndex];
		double vtxr	= std::sqrt(vtxx*vtxx + vtxy*vtxy);
		double ptot	= std::sqrt(px*px + py*py + pz*pz);
		if (pdgCode==-22){
			double photonEnergyGeV		= std::sqrt(px*px + py*py + pz*pz); 
			double photonEnergyEV		= photonEnergyGeV * 1e9; 				
			double photonWavelengthNm	= 0.0;
			if (photonEnergyEV > 0.0) {
				photonWavelengthNm = 1239.84193 / photonEnergyEV;
			}
			//cout<<photonWavelengthNm<<" "<<endl;
			return photonWavelengthNm;
		}
		//
	} else {
		//std::cout<<"Hit "<<ihit<<" points to an unlinked or invalid MC index:"
		//		 <<mcParticleIndex<<std::endl;
	}
	//---- end MCParticle lookup
	//
	return 0.;		// no result
	//
}
//---- end GetWavelength()...


//----------------------------------------------------------------------------------
//
std::vector<int> dircGR::getHitAncestryIndices(int ihit) {
    std::vector<int> ancestryVector;

    // Safety Check: Avoid reading past the end of the unrolled relations lookup array
    if (ihit >= _DIRCBarHits_particle_) {
        std::cout << "Hit " << ihit << " has no truth-level mapping array entry." << std::endl;
        return ancestryVector;
    }
    
    int mcParticleIndex = _DIRCBarHits_particle_index[ihit];
    
    // Safety Range Check: Ensure the target index exists inside MCParticles
    if (mcParticleIndex >= 0 && mcParticleIndex < MCParticles_) {
        
        // Push the original particle index creating the hit (i.e. an OP) as index 0
        ancestryVector.push_back(mcParticleIndex);

        int pdgCode = MCParticles_PDG[mcParticleIndex];
        double px   = MCParticles_momentum_x[mcParticleIndex];
        double py   = MCParticles_momentum_y[mcParticleIndex];
        double pz   = MCParticles_momentum_z[mcParticleIndex];
        double vtxx = MCParticles_vertex_x[mcParticleIndex];
        double vtxy = MCParticles_vertex_y[mcParticleIndex];
        double vtxz = MCParticles_vertex_z[mcParticleIndex];
        double vtxr = std::sqrt(vtxx*vtxx + vtxy*vtxy);
        double photonWavelengthNm = 0.0;
        if (pdgCode == -22) {
            double photonEnergyGeV = std::sqrt(px*px + py*py + pz*pz); 
            double photonEnergyEV  = photonEnergyGeV * 1e9; 				
            if (photonEnergyEV > 0.0) {
                photonWavelengthNm = 1239.84193 / photonEnergyEV;
            }
        }
//!        std::cout << "Hit " << ihit << "  WL(nm)= " << photonWavelengthNm
//!                  << "\t parent mcParticleIndex= " << mcParticleIndex << " "
//!                  << "\t PDG: " << pdgCode << " -------------------------------" << std::endl;
		//
        // ---- LINEAR ANCESTRY TRAVERSAL ----
        int currentIdx = mcParticleIndex;
        int generation = 1;
        bool keepClimbing = true;
        //
        while (keepClimbing) {
            unsigned int parentBegin = MCParticles_parents_begin[currentIdx];
            unsigned int parentEnd   = MCParticles_parents_end[currentIdx];
            if (parentBegin < parentEnd) {
                unsigned int iparent = parentBegin; 						
                if (iparent >= _MCParticles_parents_) {
//!                    std::cout << "   [Gen " << generation << "] Error: Parent relation out of bounds!" << std::endl;
                    keepClimbing = false;
                    break;
                }
                int parentMCIndex = _MCParticles_parents_index[iparent]; 
                if (parentMCIndex >= 0 && parentMCIndex < MCParticles_) {
                    int parentPDG = MCParticles_PDG[parentMCIndex];
                    double parentPx = MCParticles_momentum_x[parentMCIndex];
                    double parentPy = MCParticles_momentum_y[parentMCIndex];
                    double parentPz = MCParticles_momentum_z[parentMCIndex];
                    double parentPtot = std::sqrt(parentPx*parentPx + parentPy*parentPy + parentPz*parentPz);	
                    int genStatus = MCParticles_generatorStatus[parentMCIndex];
                    int simStatus = MCParticles_simulatorStatus[parentMCIndex];	
                    if (genStatus == 4) { 		// Stop before beam particle indices get added to the vector
                        keepClimbing = false; 
                        break; 
                    } 	                    
					bool isCreatedInSim	= (simStatus & (1U << 30)) != 0;	// Bit 30 (0x40000000): Created inside the simulation
					bool isDecayedInSim	= (simStatus & (1U << 27)) != 0;	// Bit 27 (0x8000000): Decayed or interacted
					bool isStoppedInSim	= (simStatus & (1U << 24)) != 0;	// Bit 24 (0x1000000): Stopped or range-cut 
					bool isBackscatter	= (simStatus & (1U << 29)) != 0;	// moving backward w.r.t. parent
					bool leftDetector	= (simStatus & (1U << 25)) != 0;	// particle left the cave
					bool isOverlay		= (simStatus & (1U << 31)) != 0;	// particle is from an overlaid BG event
					//
//!                    std::cout << "   [Gen " << generation << "] Parent Index [" << parentMCIndex 
//!                              << "] PDG: " << parentPDG << " | P: " << parentPtot << " GeV/c"
//!                              << " | GenStatus: " << genStatus 
//!                              << " | SimStatus bits: "
//!                              <<(int)isCreatedInSim<<" "<<(int)isDecayedInSim<<" "<<(int)isStoppedInSim
//!                              <<(int)isBackscatter<<" "<<(int)leftDetector<<" "<<(int)isOverlay                              
//!                              << std::endl;
                	//
                    // Push only the integer index value
                    ancestryVector.push_back(parentMCIndex);
                    currentIdx = parentMCIndex;
                    generation++;			
                    if (generation > 50) { 
                        std::cout << "   --> Aborting: Ancestry depth exceeded 50 generations." << std::endl;
                        keepClimbing = false;
                    }
                } else {
//!                    std::cout << "   --> [Gen " << generation << "] Parent index " << parentMCIndex << " is out of bounds." << std::endl;
                    keepClimbing = false;
                }
            } else {
//!                std::cout << "   --> Ultimate Ancestor reached at Gen " << (generation - 1) << "." << std::endl;
                keepClimbing = false;
            }
        }
    }
    return ancestryVector;
}

//------------------------------------------------------------------------------------
double FindLowestXFilled(TH1D* h){
	if (!h) {return -1;}
	for (int ib=1;ib<=h->GetXaxis()->GetNbins();ib++){
		double contents	= h->GetBinContent(ib);
		if (contents>0){
			return h->GetXaxis()->GetBinLowEdge(ib-2);
		}
	}
	return h->GetXaxis()->GetBinLowEdge(1);
}

//-------------------------------------------------------------------------------------
//---- return val = -1,1	passes cut
//     return val = -9 		fails cut
//
int dircGR::OPTimeCutCrude(double hitt, double texp1, double texp2, double DetTheta){
	//
	bool skip1=false,skip2=false,pass1=false,pass2=false;
	if ( (180./M_PI)*DetTheta <= 85. ) skip1 = true;		// only mirror OPs survive...
	if ( (180./M_PI)*DetTheta >= 95. ) skip2 = true;		// only prism OPs survive...
	//
	int zdir	= -9;										// default condition is FAIL...
	double dt1	= hitt-texp1;
	double dt2	= hitt-texp2;
	if ( dt1>-2. && dt1<2. ) pass1 = true;
	if ( dt2>-2. && dt2<2. ) pass2 = true;
	if ( pass1 && !skip1 ){ zdir = -1; }
	if ( pass2 && !skip2 ){ zdir =  1; }
	return zdir;
}
//
//----------------------------------------------------------------------------------------
//---- return val = -1,0,1			passes cut
//     return val = -9,-8,-7,-6,... fails cut
//
int dircGR::OPTimeCutTight(double hitt, double texp1, double texp2, double DetTheta, double tbeg1, double tbeg2){
	//
	bool skip1=false,skip2=false,pass1=false,pass2=false;
	if ( (180./M_PI)*DetTheta <= 85. ) skip1 = true;		// only mirror OPs survive, so don't check path1 gate...
	if ( (180./M_PI)*DetTheta >= 95. ) skip2 = true;		// only prism OPs survive, so don't check path2 gate...
	//
	int zdir		= -9;									// default condition is FAIL...
	double dt1		= hitt-texp1;
	double dt2		= hitt-texp2;
	double cut1U	= fTimeCut1U->Eval(texp1);
	double cut2U	= fTimeCut2U->Eval(texp2);
	//
	if ( dt1 >= -cut1U && dt1 < cut1U ) pass1 = true;		// cuts are symmetric
	if ( dt2 >= -cut2U && dt2 < cut2U ) pass2 = true;		// cuts are symmetric
	//
 	if ( pass1 && !skip1 ){ zdir = -1; }					// PASS time cut?
 	if ( pass2 && !skip2 ){ zdir =  1; }					// PASS time cut?
	//
	if (zdir==-1 && hitt<tbeg1){ zdir = -8; return zdir; }		// FAIL time cut
	if (zdir== 1 && hitt<tbeg2){ zdir = -7; return zdir; }		// FAIL time cut
	//
	return zdir;		// PASS!
	//
}
//
//----------------------------------------------------------------------------------
int dircGR::kzFromZ(double inc_z_val){
	int kz	= std::round(inc_z_val) + 2730;
	if (kz<    0) kz=   0;
	if (kz>=4562) kz=4561;
	return kz;
}
// int dircGR::OPTimeCutBands(double hitt, double inc_z_val, double inc_t_val, double lut_t){
// 	int zdir	= -9;
// 	int kz		= kzFromZ(inc_z_val);		// kz index from inc_z
// 	double toth	= inc_t_val + lut_t;
// 	double tL0	= tcutL0_[kz] + toth;	// lower prism
// 	double tU0	= tcutU0_[kz] + toth;	// upper prism
// 	double tL1	= tcutL1_[kz] + toth;	// lower mirror
// 	double tU1	= tcutU1_[kz] + toth;	// upper mirror
// 	if ( hitt >= tL0 && hitt <= tU0 ){ zdir = -1; } else	// prism-going
// 	if ( hitt >= tL1 && hitt <= tU1 ){ zdir =  1; } 		// mirror-going
// 	return zdir;	
// }

int phi_to_barbox(double phi) {		// expects phi in radians...
    // Shift by 15deg so barbox 0 starts at 0deg, then normalize negative angles
    double phideg		= phi*(180./M_PI);
    double normalized	= fmod(phideg + 15.0, 360.);
    if (normalized < 0) normalized += 360.;
    return (int)(normalized / 30.);
}

// ------------------------
// //
// //	
// //	    double myWavelength = 532.0; // Green laser pointer light
// //	    Int_t myColorIndex = GetROOTColorFromWavelength(myWavelength);
// //		//
// //	    // Create a dummy histogram to test
// //	    TH1F *h = new TH1F("h", "Wavelength Styled Histogram", 100, -3, 3);
// //	    h->FillRandom("gaus", 10000);
// //		//
// //	    // Apply the custom color index
// //	    h->SetLineColor(myColorIndex);
// //	    h->SetFillColor(myColorIndex);
// //	    h->SetFillStyle(3001);
// //	    //
// //	    //
//
// // Function to convert wavelength (nm) to ROOT Color ID
// //
// Int_t GetROOTColorFromWavelength(double wavelength) {
//     double r = 0.0, g = 0.0, b = 0.0;
//     double factor = 0.0;
//     // 1. Map wavelength to preliminary RGB choices
//     if ((wavelength >= 380) && (wavelength < 440)) {
//         r = -(wavelength - 440) / (440 - 380);
//         g = 0.0;
//         b = 1.0;
//     } else if ((wavelength >= 440) && (wavelength < 490)) {
//         r = 0.0;
//         g = (wavelength - 440) / (490 - 440);
//         b = 1.0;
//     } else if ((wavelength >= 490) && (wavelength < 510)) {
//         r = 0.0;
//         g = 1.0;
//         b = -(wavelength - 510) / (510 - 490);
//     } else if ((wavelength >= 510) && (wavelength < 580)) {
//         r = (wavelength - 510) / (580 - 510);
//         g = 1.0;
//         b = 0.0;
//     } else if ((wavelength >= 580) && (wavelength < 645)) {
//         r = 1.0;
//         g = -(wavelength - 645) / (645 - 580);
//         b = 0.0;
//     } else if ((wavelength >= 645) && (wavelength <= 780)) {
//         r = 1.0;
//         g = 0.0;
//         b = 0.0;
//     }
//     // 2. Let the intensity fall off near the vision limits
//     if ((wavelength >= 380) && (wavelength < 420)) {
//         factor = 0.3 + 0.7 * (wavelength - 380) / (420 - 380);
//     } else if ((wavelength >= 420) && (wavelength < 701)) {
//         factor = 1.0;
//     } else if ((wavelength >= 701) && (wavelength <= 780)) {
//         factor = 0.3 + 0.7 * (780 - wavelength) / (780 - 701);
//     }
//     // 3. Apply factor and map to standard 0.0 - 1.0 float ranges
//     float red   = (r == 0.0) ? 0.0 : r * factor;
//     float green = (g == 0.0) ? 0.0 : g * factor;
//     float blue  = (b == 0.0) ? 0.0 : b * factor;
//     // 4. Return the unique ROOT color index
//     return TColor::GetColor(red, green, blue);
// }
