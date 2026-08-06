#define lutgen_cxx
#include "lutgen.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void lutgen::Loop()
{

	const double LUTvtx_x		= 766;	//mm
	//							bar   0       1      2      
	const double LUTvtx_y[10]	= {158.175,123.025,87.875,52.725,17.575,-17.575,-52.725,-87.875,-123.025,-158.175};
		
	if (fChain == 0) return;

	double xoffset		=  860.;		// mm; UNUSED... to center prism backface in x (radial distance from beamline)
	double maxhittime	= 2500.;		// ns; plot limit
	double DZ_PRISM		=  312.;		// mm; 300mm is z-length of prism, 12mm is z-length of lenses
	double OPoriginZ	= -2729.;		// mm; 
	double OPexitZ		= -3040.074;	// mm; Z-position of hits
	double OPdZ			= OPexitZ - OPoriginZ;
	cout<<" DZ_PRISM = "<<DZ_PRISM<<"\t |OPdZ| = "<<fabs(OPdZ)<<endl;

	TString Outfilebase		= TString(Form("lutgen_bar%d",kBar));
	TString Rootfilename	= Outfilebase + TString(".root");
	TString PlotfilenameO	= Outfilebase + TString(".ps(");
	TString Plotfilename	= Outfilebase + TString(".ps");
	TString PlotfilenameC	= Outfilebase + TString(".ps]");
	TString PlotfilenameP	= Outfilebase + TString(".pdf");
	TString LUTfilename		= TString(Form("./LUT/lut_bar%d.root",kBar));
	//
	TFile *fout	= new TFile(Rootfilename.Data(),"recreate");
	fout->cd();
	//
	TH2D *hplane			= new TH2D("hplane","hplane",250,735,985,360,-180,180);
	TH2D *hplaneloc			= new TH2D("hplaneloc","hplaneloc",360,-180,180,250,735,985);
	TH2D *hplanelocrel		= new TH2D("hplanelocrel","hplane local, relative to OP gen vtx",680,-340,340,250,-32.5,217.5);
	TH2D *hplanelocrel_tcut	= new TH2D("hplanelocrel_tcut","hplane local, relative to OP gen vtx, hittime<10ns",680,-340,340,250,-32.5,217.5);
	TH1D *hhitphiglob		= new TH1D("hhitphiglob","hhitphiglob",504,-6.3,6.3);
	TH1D *hbarbox			= new TH1D("hbarbox","hbarbox",12,-0.5,11.5);
	TH1D *hhittime			= new TH1D("hhittime","hhittime (ns)",1000,0,maxhittime);
	TH1D *hhittimef			= new TH1D("hhittimef","hhittime (ns)",2000,0,200);
	//
	TH2D *hOPphithetaN	= new TH2D("hOPphithetaN","hOPphithetaN",200,M_PI/2.,M_PI+0.1,400,-M_PI,M_PI+0.01);
	TH2D *hOPphithetaH	= new TH2D("hOPphithetaH","hOPphithetaH",200,M_PI/2.,M_PI+0.1,400,-M_PI,M_PI+0.01);
	//
	TH2D* hhittimeOPtheta = new TH2D("hhittimeOPtheta","hhittimeOPtheta",100,M_PI/2.,M_PI+0.01,100,1.,6.);
//	TH2D* hthetapixOPtheta = new TH2D("hthetapixOPtheta","hthetapixOPtheta",100,M_PI/2.,M_PI,90,-0.1,0.8);
	TH2D* hthetapixOPtheta = new TH2D("hthetapixOPtheta","hthetapixOPtheta",100,2,M_PI+0.01,100,2,M_PI+0.01);
	//
	double	pixelsize	=    3.30;	//mm; length of edge of square pixel
	double	locxmin		= -178.20;	//mm;
	double	locxmax		=  178.20;	//mm; note these dimensions are wider than actual prism... 
	double	locymin		=  739.55;	//mm;
	double	locymax		=  980.45;	//mm; note these dimensions are taller than actual prism... 
 	const int NPIXX		=  108;
	const int NPIXY		=   73;
	cout<<"Number of Pixels (x,y,tot): "<<NPIXX<<" "<<NPIXY<<" "<<NPIXX*NPIXY<<endl;
	TH2D* 	hpixelplane	= new TH2D("hpixelplane","hpixelplane",NPIXX,locxmin,locxmax,NPIXY,locymin,locymax);
	TH2D* 	hpixelID	= new TH2D("hpixelID","hpixelID",NPIXX,-0.5,((double)NPIXX)-0.5,NPIXY,-0.5,((double)NPIXY)-0.5);
	//
	TH1::AddDirectory(kFALSE);			// don't save ntuples to outfile...
	TNtuple *ntPixelPaths[NPIXX][NPIXY];
	const int ntNvars	= 9;
	for (int iy=0;iy<NPIXY;iy++){
		for (int ix=0;ix<NPIXX;ix++){
			ntPixelPaths[ix][iy]	= new TNtuple(Form("ntPixelPaths_%d_%d",ix,iy),
												  Form("ntPixelPaths_%d_%d",ix,iy),
												  "alpx:alpy:alpz:locx:locy:dirx:diry:dirz:time");
			ntPixelPaths[ix][iy]->SetDirectory(nullptr);
		}
	}	
	float ntData[ntNvars]	= {0};
	TH1::AddDirectory(kTRUE);
	//
	TH2D* hNpaths_pixel		= new TH2D("hNpaths_pixel","hNpaths_pixel",NPIXX,locxmin,locxmax,NPIXY,locymin,locymax);
	TH2D* hNpaths_pixelID	= new TH2D("hNpaths_pixelID","hNpaths_pixelID",NPIXX,-0.5,((double)NPIXX)-0.5,NPIXY,-0.5,((double)NPIXY)-0.5);
	//
	//------------
	//	
	Long64_t nentries = fChain->GetEntries();
	//nentries = 1000000;
	cout<<"nentries = "<<nentries<<endl;
	//
	Long64_t nbytes = 0, nb = 0;
	for (Long64_t jentry=0; jentry<nentries;jentry++) {
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;
		// if (Cut(ientry) < 0) continue;
		//
		if (jentry%1000000==0){ cout<<"Reading thrown OP "<<jentry<<endl; }
		if (MCParticles_ != 1){ cout<<"too many MC particles... "<<endl; exit(0); }
		//
		double momx		= MCParticles_momentum_x[0];	// OP entrance
		double momy		= MCParticles_momentum_y[0];	// OP entrance
		double momz		= MCParticles_momentum_z[0];	// OP entrance
		double vtxx		= MCParticles_vertex_x[0];		// OP entrance
		double vtxy		= MCParticles_vertex_y[0];		// OP entrance
		double vtxz		= MCParticles_vertex_z[0];		// OP entrance
		double mom		= sqrt(momx*momx + momy*momy + momz*momz);
		double alpx		= momx/mom;
		double alpy		= momy/mom;
		double alpz		= momz/mom;
		double OPtheta	= acos(alpz);
		double OPphi	= atan2(alpy,alpx);
		hOPphithetaN	->Fill(OPtheta,OPphi);
		if (DIRCBarHits_){
			hOPphithetaH->Fill(OPtheta,OPphi);
		}
		//
 		//double thexz	= atan2(alpx,alpz);
 		//double theyz	= atan2(alpy,alpz);
		//
		//double tanthexz	= tan(thexz);
		//double tantheyz	= tan(theyz);
		//double tantheta	= sqrt(tanthexz*tanthexz + tantheyz*tantheyz);
		//double theta2	= atan(tantheta);
		//cout<<theta1<<" "<<theta2<<endl;
		//
		//cout<<imc<<" "<<MCParticles_<<"\t "
		//	<<MCParticles_momentum_x[imc]<<" "
		//	<<MCParticles_momentum_y[imc]<<" "
		//	<<MCParticles_momentum_z[imc]<<"\t "
		//	<<alpx<<" "<<alpy<<" "<<alpz<<"\t "
		//	<<57.3*thexz<<" "<<57.3*theyz
		//	<<endl;
		//
		for (int ihit=0;ihit<DIRCBarHits_;ihit++){	
			double hitt		= DIRCBarHits_time[ihit];					// OP time in lens assy + prism
			double hitx		= DIRCBarHits_position_x[ihit];				// OP exit
			double hity		= DIRCBarHits_position_y[ihit];				// OP exit
			double hitz		= DIRCBarHits_position_z[ihit];				// OP exit
			double momx		= DIRCBarHits_momentum_x[ihit];				// OP exit
			double momy		= DIRCBarHits_momentum_y[ihit];				// OP exit
			double momz		= DIRCBarHits_momentum_z[ihit];				// OP exit
			double momtot	= sqrt(momx*momx + momy*momy + momz*momz);
			double dirx		= DIRCBarHits_momentum_x[ihit] / momtot;	// OP exit
			double diry		= DIRCBarHits_momentum_y[ihit] / momtot;	// OP exit
			double dirz		= DIRCBarHits_momentum_z[ihit] / momtot;	// OP exit
			double phiglob	= atan2(hity,hitx);
			int id_barbox		= (phiglob + 15.*(M_PI/180.)) / (2.*M_PI/12.);	//!!! assumes 12 bar boxes !!!
			if (id_barbox<0||id_barbox>=12){ cout<<"huh?? barbox id = "<<id_barbox<<endl; exit(0); }
			double barboxphi	= id_barbox * (2.*M_PI/12.);
			double hittime		= DIRCBarHits_time[ihit];
			if (hittime>=maxhittime){ hittime = maxhittime-1.; }	// put overflows in last bin to be visible!
			//
			//	The transformation that maps the x-axis to the new y'-axis and 
			//	the y-axis to the new -x'-axis is a counterclockwise rotation of 90 degrees of the coordinate axes, 
			//	which corresponds to the algebraic mapping x' = -y and y' = x.
			//
			double xrel		= hitx-vtxx;	// position of mcp hit wrt OP vertex
			double yrel		= hity-vtxy;	// position of mcp hit wrt OP vertex
			double zrel		= hitz-vtxz;	// position of mcp hit wrt OP vertex
			double locx		= -hity;		// local x-axis is parallel to struck barplane (and points in -phi direction)
			double locy		=  hitx;		// local y-axis is perp to struck barplane, pointing outward (upward)
			double locxrel	= -yrel;		// centered on OP gen vtx, local x-axis is parallel to struck barplane
			double locyrel	=  xrel;		// centered on OP gen vtx, local y-axis is perp to struck barplane, pointing outward
			//
			int idpixx	= (locx-locxmin)/pixelsize;
			int idpixy	= (locy-locymin)/pixelsize;
			if (idpixx <      0) continue;
			if (idpixx >= NPIXX) continue;
			if (idpixy <      0) continue;
			if (idpixy >= NPIXY) continue;
			//
			hplane			->Fill(hitx,hity);
			hpixelplane		->Fill(locx,locy);
			hpixelID		->Fill(idpixx,idpixy);
			hplaneloc		->Fill(locx   ,locy   );
			hplanelocrel	->Fill(locxrel,locyrel);
			if (hittime<10){
				hplanelocrel_tcut	->Fill(locxrel,locyrel);
			}
			hhittimeOPtheta	->Fill(OPtheta,hittime);
			double thetapix	= atan2(sqrt(locxrel*locxrel+locyrel*locyrel),-DZ_PRISM);
			hthetapixOPtheta->Fill(OPtheta,thetapix);
			hhitphiglob		->Fill(phiglob);
			hbarbox			->Fill(id_barbox);
			hhittime		->Fill(hittime);
			hhittimef		->Fill(hittime);
			//
			//if (idpixx==SINGLE_IX&&idpixy==SINGLE_IY){
			//	cout<<OPtheta<<" "<<OPphi
			//		<<"\t "<<hitx<<" "<<hity<<" "<<hitz
			//		<<"\t "<<momx<<" "<<momy<<" "<<momz
			//		<<"\t "<<dirx<<" "<<diry<<" "<<dirz
			//		<<endl;
			//}
			//
			//---- 
			ntData[0]	= (float)alpx;		// OP entrance, epic coords
			ntData[1]	= (float)alpy;		// OP entrance, epic coords
			ntData[2]	= (float)alpz;		// OP entrance, epic coords
			ntData[3]	= (float)locx;		// OP exit posn, epic coords
			ntData[4]	= (float)locy;		// OP exit posn, epic coords
			ntData[5]	= (float)dirx;		// OP exit direction, epic coords
			ntData[6]	= (float)diry;		// OP exit direction, epic coords
			ntData[7]	= (float)dirz;		// OP exit direction, epic coords
			ntData[8]	= (float)hitt;		// OP time in prism
			ntPixelPaths[idpixx][idpixy]->Fill(ntData);
			//
		}
	}

	TH2D* hOPphithetaEFF	= (TH2D*)hOPphithetaH->Clone("hOPphithetaEFF");
		hOPphithetaEFF->Divide(hOPphithetaN);

	//---- deal with PixelPath data...
	//
	TProfile2D* halpzexit_pixel	= new TProfile2D("halpzexit_pixel","halpzexit_pixel",NPIXX,locxmin,locxmax,NPIXY,locymin,locymax,-1.0,1.002);
	//
//---- theta vs phi, simple th2d
//!!	TH2D* hangles_pixel	= new TH2D("hangles_pixel","hangles_pixel",100,-M_PI,M_PI+0.01,100,M_PI/2.,M_PI+0.01);	// x=phi, y=theta
//!!	TH2D* hangles_pixel_single	= new TH2D("hangles_pixel_single","hangles_pixel_single",100,-M_PI,M_PI+0.01,100,M_PI/2.,M_PI+0.01);	// x=phi, y=theta
//---- equal area binning for epic coord system
// 	TH2Poly* hangles_pixel			= EqualAreaSphereHistogram();
// 		  hangles_pixel			->SetName("hangles_pixel");
// 		  hangles_pixel			->SetTitle("hangles_pixel");
// 	TH2Poly* hangles_pixel_single	= EqualAreaSphereHistogram();
// 		  hangles_pixel_single	->SetName("hangles_pixel_single");
// 		  hangles_pixel_single	->SetTitle("hangles_pixel_single");
//---- kt vs phi (for "pol" plots)
//	TH2D* hangles_pixel			= new TH2D("hangles_pixel","hangles_pixel",100,-M_PI,M_PI+0.01,200,-1.,1.);					// x=phi, y=theta
//	TH2D* hangles_pixel_single	= new TH2D("hangles_pixel_single","hangles_pixel_single",100,-M_PI,M_PI+0.01,200,-1.,1.);	// x=phi, y=theta
//---- kt vs phi, simple th2d, save each pixel...
// 	TH2D *hangles_pixel[NPIXX][NPIXY];
// 	for (int iy=0;iy<NPIXY;iy++){
// 		for (int ix=0;ix<NPIXX;ix++){
// 			hangles_pixel[ix][iy] = new TH2D(Form("hangles_pixel_%d_%d",ix,iy),Form("hangles_pixel_%d_%d",ix,iy),100,-M_PI,M_PI+0.01,200,-1.,1.);	
// 		}
// 	}
//---- PRIMED theta vs phi, simple th2d, save each pixel...
	TH2D *hangles_pixel[NPIXX][NPIXY];
	for (int iy=0;iy<NPIXY;iy++){
		for (int ix=0;ix<NPIXX;ix++){
			hangles_pixel[ix][iy] = new TH2D(Form("hangles_pixel_%d_%d",ix,iy),Form("Primed hangles_pixel_%d_%d",ix,iy),50,-M_PI,M_PI+0.01,100,0,M_PI+0.01);	
		}
	}
	TAxis*	ax_OPthetaPrime	= hangles_pixel[0][0]->GetYaxis();
	TAxis*	ax_OPphiPrime	= hangles_pixel[0][0]->GetXaxis();
	//
	cout<<"Collecting paths across the pixel field."<<endl;
	//
	int  nfound					=   0;
	int  nfoundbiggest			=   0;
	const int maxfound			=  50;
	const int mincountseedbin	=   2;
	const int mincountclump		=   3;		// ...unused...
	//----- Define Path_OPdir[3][maxfound][NPIXX][NPIXY]...
	double (*Path_OPdir)[maxfound][NPIXX][NPIXY] = (double (*)[maxfound][NPIXX][NPIXY])calloc(3 * maxfound * NPIXX * NPIXY, sizeof(double));
    	if ( Path_OPdir == NULL){ printf("Memory allocation failed.\n"); exit(0); }
	double (*Path_OPtim)[NPIXX][NPIXY] = (double (*)[NPIXX][NPIXY])calloc(maxfound * NPIXX * NPIXY, sizeof(double));
    	if ( Path_OPtim == NULL){ printf("Memory allocation failed.\n"); exit(0); }
	double      Path_nOP[maxfound][NPIXX][NPIXY]	= {0};
	double               Pixel_nOP[NPIXX][NPIXY]	= {0};
	double     Path_Prob[maxfound][NPIXX][NPIXY]	= {0};	// this is the LUT! 
	int		           Path_nClump[NPIXX][NPIXY]	= {0};
	//
	bool DOSINGLE	= false;
	int SINGLE_IX	=  99;		// 54, 101
	int SINGLE_IY	=  69;		// 36,   7
	//
	for (int iy=0;iy<NPIXY;iy++){
		cout<<"starting iy= "<<iy<<endl;
		for (int ix=0;ix<NPIXX;ix++){
			//
			//cout<<ix<<" "<<iy<<" ---------------------"<<endl;
			//
			if (DOSINGLE){
				if (ix==SINGLE_IX&&iy==SINGLE_IY){}else{continue;}
			}
			//
 			int nOP_pixel		= ntPixelPaths[ix][iy]->GetEntries();
			//cout<<ix<<" "<<iy<<"\t nOPpixel= "<<nOP_pixel<<endl;
			for (int ient=0;ient<nOP_pixel;ient++){
				ntPixelPaths[ix][iy] -> GetEntry(ient);
				Float_t *args	= ntPixelPaths[ix][iy]->GetArgs();
				double alpx		= (double)args[0];
				double alpy		= (double)args[1];
				double alpz		= (double)args[2];
				double alpt		= sqrt(alpx*alpx + alpy*alpy);
				double OPtheta	= acos(alpz);
				double OPphi	= atan2(alpy,alpx);
				double primedx		=  alpx;
				double primedy		= -alpz;
				double primedz		=  alpy;
				double primedt		= sqrt(primedx*primedx + primedy*primedy);
				double OPthetaPrime	= acos(primedz);
				double OPphiPrime	= atan2(primedy,primedx);
				double locx		= (double)args[3];
				double locy		= (double)args[4];
				double alpxexit	= (double)args[5];
				double alpyexit	= (double)args[6];
				double alpzexit	= (double)args[7];
				//hangles_pixel		->Fill(OPphi,OPtheta);	// theta vs phi, all the OP angle pairs that hit this pixel
				//hangles_pixel[ix][iy]->Fill(OPphi,alpt);	// kt vs phi, all the OP angle pairs that hit this pixel
				hangles_pixel[ix][iy]->Fill(OPphiPrime,OPthetaPrime);	// PRIMED theta vs phi, all the OP angle pairs that hit this pixel
				//if (ix==SINGLE_IX&&iy==SINGLE_IY){
				//	//hangles_pixel_single->Fill(OPphi,OPtheta);	// all the OP angle pairs that hit this pixel
				//	//hangles_pixel_single->Fill(OPphi,alpt);			// all the OP angle pairs that hit this pixel
				//}
				halpzexit_pixel		->Fill(locx,locy,alpzexit);
			}
			//
			TH2D* hangles_pixel_W	= (TH2D*)hangles_pixel[ix][iy]->Clone("hangles_pixel_W");
			bool collecting			= true;
			nfound					= 0;
			std::vector<int> clump_binx[maxfound];
			std::vector<int> clump_biny[maxfound];
			std::vector<int> clump_nopbin[maxfound];
			//
			//----first collect everything at theta~pi into one clump (last 2 bins in theta)
			//bool foundforward	= false;
			//for (int ibx=1;ibx<=hangles_pixel_W->GetXaxis()->GetNbins();ibx++){
			//	for (int iby=hangles_pixel_W->GetYaxis()->GetNbins()-1;ibx<=hangles_pixel_W->GetYaxis()->GetNbins();ibx++){
			//		int n	= hangles_pixel_W->GetBinContent(ibx,iby);
			//		if (n>0){
			//			foundforward	= true;
			//			clump_binx[nfound].push_back(ibx);
			//			clump_biny[nfound].push_back(iby);
			//			clump_nopbin[nfound].push_back(n);					
			//			hangles_pixel_W->SetBinContent(ibx,iby,-9);
			//		}
			//	}
			//}
			//if (foundforward) nfound = 1;
			//
			//---- now collect the rest of the clumps
 			while (collecting) {
				//
				int kmaxbin	= hangles_pixel_W->GetMaximumBin();
				int ncntbin	= hangles_pixel_W->GetBinContent(kmaxbin);
				//cout<<"collecting.. kmaxbin= "<<kmaxbin<<"\t ncntbin= "<<ncntbin<<endl;
				if (ncntbin<mincountseedbin){
					collecting=false; 
					break;
				}
				int kmaxx,kmaxy,kmaxz; hangles_pixel_W->GetBinXYZ(kmaxbin, kmaxx,kmaxy,kmaxz);
				clump_binx[nfound].push_back(kmaxx);
				clump_biny[nfound].push_back(kmaxy);
				clump_nopbin[nfound].push_back(ncntbin);
				hangles_pixel_W->SetBinContent(kmaxbin,-9);
				//cout<<"collecting.. ** Add Seed Bin.. "<<nfound<<"\t"<<kmaxbin<<" "<<kmaxx<<" "<<kmaxy<<"\t"<<ncntbin<<endl;
				//
				//---- now check 8 neighbor bins and add them to this clump if nonzero
				for (int icx=-1;icx<=1;icx++){				
					for (int icy=-1;icy<=1;icy++){				
						int kcx	= kmaxx+icx;
						if (kcx<=    0) continue;
						if (kcx> NPIXX) continue;
						int kcy	= kmaxy+icy;
						if (kcy<=    0) continue;
						if (kcy> NPIXY) continue;
						int ncntbinc	= hangles_pixel_W->GetBinContent(kcx,kcy);
						if (ncntbinc>0){
							clump_binx[nfound].push_back(kcx);
							clump_biny[nfound].push_back(kcy);
							clump_nopbin[nfound].push_back(ncntbinc);
							hangles_pixel_W->SetBinContent(kcx,kcy,-9);
							//cout<<"collecting..    Add Next Bin.. "<<nfound<<"\t"<<kmaxbin<<" "<<kcx<<" "<<kcy<<"\t"<<ncntbinc<<endl;
						}
					}
				}
				//
				++nfound;
				if (nfound==maxfound){
					cout<<"Hit maxfound limit for pixel "<<ix<<" "<<iy<<endl;
					collecting = false;				
					break;
				}
			//
			}	// end collecting...
			//
			//cout<<"after collecting. nfound= "<<nfound<<endl;
			if (nfound>nfoundbiggest) nfoundbiggest = nfound;
			//hangles_pixel->Reset("");
			delete hangles_pixel_W; 
			//
			//cout<<"pixel: "<<ix<<" "<<iy<<"\t N OP angle clumps = "<<nfound<<endl;
			//
			//---- now you have nfound clumps for this pixel, loop over them.
			//---- we need one pmt-incidence plot for each clump in this pixel!
			//
			TH2D *hpmtincang[maxfound];
			double	Path_avgdirx[maxfound]		= {0};
			double	Path_avgdiry[maxfound]		= {0};
			double	Path_avgdirz[maxfound]		= {0};
			double	Path_avghitt[maxfound]		= {0};
			int 	Path_nOP_clump[maxfound]	= {0}; 
			//
			for (int ifound=0;ifound<nfound;ifound++){			// number of OP ang pair clumps in this pixel
				int nbinsclump	= clump_binx[ifound].size();	// number of OP ang pair bins in this clump
				//cout<<"clump.. nbinsclump= "<<nbinsclump<<endl;
				//
				hpmtincang[ifound]	= new TH2D(Form("hpmtincang_%d",ifound),Form("hpmtincang_%d",ifound),100,-M_PI,M_PI+0.01,100,0,M_PI+0.01);	// x=phi, y=theta
				//
				for (int iabin=0;iabin<nbinsclump;iabin++){
					int ibx		= clump_binx[ifound].at(iabin);		// OP ang pair x-bin ID (OPphi) 
					int iby		= clump_biny[ifound].at(iabin);		// OP ang pair y-bin ID (OPtheta)
					int nopbin	= clump_nopbin[ifound].at(iabin);
					//cout<<"clump.. iabin,ibx,iby,nopbin: "<<iabin<<" "<<ibx<<" "<<iby<<" "<<nopbin<<endl;
					//
					//---- now loop over *all OPs* in this pixel
					//---- but work only with the OPs in this clump
					//
					int nOP_pixel		= ntPixelPaths[ix][iy]->GetEntries();
					for (int ient=0;ient<nOP_pixel;ient++){
						//
						ntPixelPaths[ix][iy] -> GetEntry(ient);
						Float_t *args	= ntPixelPaths[ix][iy]->GetArgs();
						double alpx			= (double)args[0];
						double alpy			= (double)args[1];
						double alpz			= (double)args[2];
						double OPtheta		= acos(alpz);
						double OPphi		= atan2(alpy,alpx);
						double primedx		=  alpx;
						double primedy		= -alpz;
						double primedz		=  alpy;
						double OPthetaPrime	= acos(primedz);
						double OPphiPrime	= atan2(primedy,primedx);
						//double locx		= (double)args[3];
						//double locy		= (double)args[4];
						//double alpxexit	= (double)args[5];
						//double alpyexit	= (double)args[6];
						//double alpzexit	= (double)args[7];
						double hitt			= (double)args[8];	// time in prism
						//
						int ibxtest		= ax_OPphiPrime	 ->FindBin(OPphiPrime);
						int ibytest		= ax_OPthetaPrime->FindBin(OPthetaPrime);
						if (ibxtest!=ibx) continue;
						if (ibytest!=iby) continue;
						//
						//---- ok this OP [ient] is in the clump [ifound] in the pixel [ix][iy]
						//---- now we need to fill the pmt-incidence angle plot for this one clump in this one pixel!
						//
						TVector3 pmtincdir( (double)args[4], (double)args[5], (double)args[6] ); 
						hpmtincang[ifound]	->Fill(pmtincdir.Phi(),pmtincdir.Theta());
						//
						//---- generally there is just one pmt incidence angle for each pixel and OP angle clump.
						//---- so just average the path over all the OPs in this OP ang bin (part of this clump). 
						//
						Path_nOP_clump[ifound]	+= 1;
						Path_avgdirx[ifound]	+= alpx;	// average over every OP in this clump and bin AND all neighboring bins in this clump!
						Path_avgdiry[ifound]	+= alpy;
						Path_avgdirz[ifound]	+= alpz;
						Path_avghitt[ifound]	+= hitt;
						//
					}	// end loop over all OPs hitting this pixel (and operating just those OPs in this clump and pixel)
					//					
				}	// end loop over all the OPang bins in this clump
				//
				//cout<<"clump.. Path_nOP_clump[ifound]= "<<Path_nOP_clump[ifound]<<endl;
				if (Path_nOP_clump[ifound]){
					Path_avgdirx[ifound]	/= Path_nOP_clump[ifound];
					Path_avgdiry[ifound]	/= Path_nOP_clump[ifound];
					Path_avgdirz[ifound]	/= Path_nOP_clump[ifound];
					Path_avghitt[ifound]	/= Path_nOP_clump[ifound];
				} else {
					Path_avgdirx[ifound]	=  0.0;
					Path_avgdiry[ifound]	=  0.0;
					Path_avgdirz[ifound]	=  0.0;
					Path_avghitt[ifound]	=  0.0;
				}
 				Path_OPdir[0][ifound][ix][iy] = Path_avgdirx[ifound];
 				Path_OPdir[1][ifound][ix][iy] = Path_avgdiry[ifound];
 				Path_OPdir[2][ifound][ix][iy] = Path_avgdirz[ifound];
 				   Path_OPtim[ifound][ix][iy] = Path_avghitt[ifound];
				  Path_nOP[ifound][ix][iy]	  = Path_nOP_clump[ifound];
						 Pixel_nOP[ix][iy]	 += Path_nOP_clump[ifound];
				//
				delete hpmtincang[ifound];
				//
			}	// end loop over all clumps found for this pixel
			//
			for (int ifound=0;ifound<nfound;ifound++){			// loop over OP ang pair clumps in this pixel
				if (Pixel_nOP[ix][iy]>0){
					Path_Prob[ifound][ix][iy]	= Path_nOP[ifound][ix][iy] / Pixel_nOP[ix][iy];	
				} else {
					Path_Prob[ifound][ix][iy]	= 0.0;
				}
			}
			Path_nClump[ix][iy]	 = nfound;
			hNpaths_pixel	->SetBinContent(ix+1,iy+1,nfound);
			hNpaths_pixelID	->Fill(ix,iy,nfound);
			//
			//---- reset for next pixel...
			for (int i=0;i<nfound;i++){
				clump_binx[i].clear();
				clump_biny[i].clear();
				clump_nopbin[i].clear();
			}
			//
		}	// end pixel X loop
	}	// end pixel Y loop

	//----------------------------------
	//
	// 	Path_OPdir[ifound][ix][iy]
	// 	 Path_Prob[ifound][ix][iy]
	//
	int kx		= SINGLE_IX;
	int ky		= SINGLE_IY;
	int nclump	= Path_nClump[kx][ky];
	cout<<"ix= "<<kx<<"\t iy= "<<ky<<"\t nclump= "<<nclump<<endl;
 	for (int iclump=0;iclump<nclump;iclump++){
		int nOP	= Path_nOP[iclump][kx][ky];
 		cout<<kx<<" "<<ky<<"\t"<<iclump<<" "<<nclump<<"\t"
 			<<nOP<<"\t"
  			<<    Path_Prob[iclump][kx][ky]<<"\t  "
 			<<Path_OPdir[0][iclump][kx][ky]<<" "
  			<<Path_OPdir[1][iclump][kx][ky]<<" "
  			<<Path_OPdir[2][iclump][kx][ky]<<"\t"
  			<<   Path_OPtim[iclump][kx][ky]<<" "
 			<<endl;
 	}

	cout<<"Filling LUT for "<<LUTfilename.Data()<<endl;
	int T_ipixx;
	int T_ipixy;
	int T_npath;
	int T_ipath;
	int T_nOP;
	double T_OPprob;
	double T_OPdirx;
	double T_OPdiry;
	double T_OPdirz;
	double T_OPtime;
	TFile *fLUT	= new TFile(LUTfilename.Data(),"recreate");
		TTree *dircLUT	= new TTree("dircLUT","LUT for dirc geometric reconstruction");
		    dircLUT->Branch("ipixx", &T_ipixx, "ipixx/I");
		    dircLUT->Branch("ipixy", &T_ipixy, "ipixy/I");
		    dircLUT->Branch("npath", &T_npath, "npath/I");
		    dircLUT->Branch("ipath", &T_ipath, "ipath/I");
		    dircLUT->Branch("nOP"  , &T_nOP,   "nOP/I");
		    dircLUT->Branch("OPprob",&T_OPprob,"OPprob/D");
		    dircLUT->Branch("OPdirx",&T_OPdirx,"OPdirx/D");
		    dircLUT->Branch("OPdiry",&T_OPdiry,"OPdiry/D");
		    dircLUT->Branch("OPdirz",&T_OPdirz,"OPdirz/D");
		    dircLUT->Branch("OPtime",&T_OPtime,"OPtime/D");
	for (int ipixx=0;ipixx<NPIXX;ipixx++){
		for (int ipixy=0;ipixy<NPIXY;ipixy++){
			T_ipixx			= ipixx;
			T_ipixy			= ipixy;
			T_npath			= Path_nClump[ipixx][ipixy];
		  	for (int iclump=0;iclump<T_npath;iclump++){
				T_ipath		= iclump;
				T_nOP		=      Path_nOP[iclump][ipixx][ipixy];
				T_OPprob	=     Path_Prob[iclump][ipixx][ipixy];
				T_OPdirx	= Path_OPdir[0][iclump][ipixx][ipixy];
				T_OPdiry	= Path_OPdir[1][iclump][ipixx][ipixy];
				T_OPdirz	= Path_OPdir[2][iclump][ipixx][ipixy];
				T_OPtime	=    Path_OPtim[iclump][ipixx][ipixy];
				dircLUT->Fill();
			}
		}
	}
	cout<<"Writing LUT to  "<<LUTfilename.Data()<<endl;
	dircLUT->Write();
	fLUT->Close();

	//----- build a tgraph just for display
	TGraph* gLUTvtxloc	= new TGraph();
			gLUTvtxloc->SetMarkerStyle(53);
			gLUTvtxloc->SetMarkerColor( 1);
	for (int ip=0;ip<10;ip++){
		double LUTvtx_x_loc	= -LUTvtx_y[ip];
		double LUTvtx_y_loc	=  LUTvtx_x;
		gLUTvtxloc->SetPoint(ip,LUTvtx_x_loc,LUTvtx_y_loc);
	}

	//---- Paint setup...
	//
	gStyle->SetOptStat(11);
	gStyle->SetErrorX(0);
	gStyle->SetPadRightMargin(0.02);
	gStyle->SetPadTopMargin(0.05);
	gStyle->SetPadBottomMargin(0.05);
	gStyle->SetPadLeftMargin(0.08);
	gStyle->SetTitleX(0.5);
	gStyle->SetLabelSize(0.04,"xyz");
	gStyle->SetPadGridX(0);
	gStyle->SetPadGridY(0);
	gStyle->SetLabelFont(132,"xyz");
	gStyle->SetLegendFont(132);
	gStyle->SetStatFont(132);
	gStyle->SetTitleFont(132);
	gStyle->SetPaintTextFormat("#1.1e");
	TCanvas *ccan[1000]; int ican=-1;

	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("Page %d",ican),0,0,1300,700);
	ccan[ican]->cd(); ccan[ican]->Divide(3,3,0.0001,0.0001);
	ccan[ican]->cd(1);
		hplaneloc->Draw();
		gLUTvtxloc->Draw("P");
	ccan[ican]->cd(2);
		hplanelocrel->Draw();
	ccan[ican]->cd(3);
		//hOPphithetaEFF->Draw();
		gPad->SetRightMargin(0.12);
		hpixelID->SetStats(0);
		hpixelID->Draw("colz");
	ccan[ican]->cd(4);
		gPad->SetLogy(1);
		//hhittime->Draw();
		hhittimef->Draw();
	ccan[ican]->cd(5);
		gPad->SetRightMargin(0.12);
		//hangles_pixel_single->Draw("colz pol");		// just the selected single pixel
	ccan[ican]->cd(6);
		//hangles_pixel_single->Draw("lego2 pol");	// just the selected single pixel
	ccan[ican]->cd(7);		
		gPad->SetRightMargin(0.12);
		hNpaths_pixel->SetStats(0);
		hNpaths_pixel->Draw("colz");
		gLUTvtxloc->Draw("P");
	ccan[ican]->cd(8);
		gPad->SetRightMargin(0.12);
		hNpaths_pixelID->SetStats(0);
		hNpaths_pixelID->Draw("colz");
	ccan[ican]->cd(9);
		gPad->SetRightMargin(0.12);
		halpzexit_pixel->Draw("colz");
		gLUTvtxloc->Draw("P");
 	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(PlotfilenameO.Data());
	
		//hhittimeOPtheta->Draw();
		//hthetapixOPtheta->SetStats(0);
		//hthetapixOPtheta->Draw();

	cout<<"Closing "<<Plotfilename.Data()<<endl;
	ccan[ican]->Print(PlotfilenameC.Data());
	cout<<"Writing "<<Rootfilename.Data()<<endl;
	fout->cd();
	fout->Write();
	
}

TH2Poly* lutgen::EqualAreaSphereHistogram() {
    // 1. Create the TH2Poly object
    // We set the title and the total coordinate range: 
    // X goes from phi = 0 to 2*pi, Y goes from theta = 0 to pi.
    TH2Poly *h2p = new TH2Poly("h2p", "Equal-Area Sphere Bins; #phi (rad); #theta (rad)", 0, 2*TMath::Pi(), 0, TMath::Pi());
    // 2. Define how many divisions you want
    const Int_t nThetaBins = 100; // Number of steps from North to South pole
    const Int_t nPhiBins   = 100; // Number of steps around the equator
    // 3. Loop to build the bins
    // We step equally in cos(theta) from 1 (North Pole) down to -1 (South Pole)
    for (Int_t i = 0; i < nThetaBins; ++i) {
        // Find the cos(theta) boundaries for this band
        Double_t cos_high = 1.0 - (2.0 * i) / nThetaBins;
        Double_t cos_low  = 1.0 - (2.0 * (i + 1)) / nThetaBins;
        // Convert back to actual theta angles (in radians)
        Double_t theta_start = TMath::ACos(cos_high);
        Double_t theta_end   = TMath::ACos(cos_low);
        for (Int_t j = 0; j < nPhiBins; ++j) {
            // Find the phi boundaries for this bin
            Double_t phi_start = (2.0 * TMath::Pi() * j) / nPhiBins;
            Double_t phi_end   = (2.0 * TMath::Pi() * (j + 1)) / nPhiBins;
            // Define the 4 corners of the polygon bin clockwise or counter-clockwise
            Double_t x[4] = {phi_start, phi_end, phi_end, phi_start};
            Double_t y[4] = {theta_start, theta_start, theta_end, theta_end};
            // Add the bin to the histogram
            h2p->AddBin(4, x, y);
        }
    }
 	return h2p;
}
















// 			int NnonzeroOPbins	= 0;
// 			//---- loop over OP angles plot, from 90 deg to 180 deg
// 			for (int ibx=1;ibx<=hangles_pixel->GetXaxis()->GetNbins();ibx++){
// 				for (int iby=1;iby<=hangles_pixel->GetYaxis()->GetNbins();iby++){
// 					double bOPtheta		= hangles_pixel->GetYaxis()->GetBinCenter(iby);
// 					double bOPphi		= hangles_pixel->GetXaxis()->GetBinCenter(ibx);
// 					int	   counts		= hangles_pixel->GetBinContent(ibx,iby);
// 					if (counts <= 0) continue;
// 					//
// 					NnonzeroOPbins	+= 1;
// 					if (bOPtheta>3.14) { bOPphi	= 0.0; }
// 					//
// 					// 
// 					//
// 				}
// 			}



// 	TH2D*	hangles_pixel[175][120];
// 	for (int iy=0;iy<NPIXY;iy++){
// 		for (int ix=0;ix<NPIXX;ix++){
// 			hangles_pixel[ix][iy]	= new TH2D(Form("hangles_pixel_%d_%d",ix,iy),Form("hangles_pixel_%d_%d",ix,iy),100,M_PI/2.,M_PI,100,-M_PI,M_PI);
// 		}
// 	}


// 			if ( idpixx>=0&&idpixx<175 
// 			  && idpixy>=0&&idpixy<120 ){
// 				hangles_pixel[idpixx][idpixy]	->Fill(OPtheta,OPphi);
// 			} else {
// 				//cout<<"pixel id out of range! "<<locx<<" "<<idpixx<<"\t "<<locy<<" "<<idpixy<<endl;
// 				continue;
// 			}

//	TH2D* hExitN_pixel		= new TH2D("hExitN_pixel"    ,"hExitN_pixel"  ,NPIXX,locxmin,locxmax,NPIXY,locymin,locymax);
//	TH2D* hExitTheta_pixel	= new TH2D("hExitTheta_pixel","hExitTheta_pixel",NPIXX,locxmin,locxmax,NPIXY,locymin,locymax);
//	TH2D* hExitPhi_pixel	= new TH2D("hExitPhi_pixel"  ,"hExitPhi_pixel"  ,NPIXX,locxmin,locxmax,NPIXY,locymin,locymax);
// 	for (int iy=0;iy<NPIXY;iy++){
// 		for (int ix=0;ix<NPIXX;ix++){
// 			int kglobbin	= hangles_pixel[ix][iy]->GetMaximumBin();
// 			int kbx,kby,kbz;  hangles_pixel[ix][iy]->GetBinXYZ(kglobbin, kbx, kby, kbz);
// 			double bestN		= hangles_pixel[ix][iy]->GetBinContent(kglobbin);
// 			double bestTheta	= hangles_pixel[ix][iy]->GetXaxis()->GetBinCenter(kbx);
// 			double bestPhi		= hangles_pixel[ix][iy]->GetYaxis()->GetBinCenter(kby);
// 			hExitTheta_pixel	->SetBinContent(ix,iy,bestTheta);
// 			hExitPhi_pixel		->SetBinContent(ix,iy,bestPhi  );
// 			hExitN_pixel		->SetBinContent(ix,iy,bestN    );
// 			//if (ix==88&&iy=60){
// 			//	cout<<kglobbin<<" "<<kbx<<" "<<kby<<" "<<kbz<<" "<<bestTheta<<" "<<bestPhi
// 			//}
// 		}
// 	}


// 	ccan[ican]->cd(7);
// 		gPad->SetRightMargin(0.1);
// 		hExitTheta_pixel->SetStats(0);
// 		hExitTheta_pixel->Draw("colz");
// 	ccan[ican]->cd(8);
// 		gPad->SetRightMargin(0.1);
// 		hExitPhi_pixel->SetStats(0);
// 		hExitPhi_pixel->Draw("colz");
// 	ccan[ican]->cd(9);
// 		gPad->SetRightMargin(0.1);
// 		hExitN_pixel->SetStats(0);
// 		hExitN_pixel->Draw("colz");

