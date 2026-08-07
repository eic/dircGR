#include "TF1.h"
#include "TCanvas.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>


TH2D*	hOPcr_costhetaz;
TF1*	f_upper;
TF1*	f_lower;
void	makebands();
void	drawbands();
void	makegates();
void	drawgates();
void	drawbandsb();
void	writebands();
TGraph*	gtcutL[2];		// L=earlier time,  [0]=prism, [1]=mirror
TGraph*	gtcutU[2];		// U=later time,    [0]=prism, [1]=mirror
TGraph*	gangtcutL[2];	// L=earlier time,  [0]=prism, [1]=mirror
TGraph*	gangtcutU[2];	// U=later time,    [0]=prism, [1]=mirror
TGraph* makeregion(TGraph* g1, TGraph* g2);
TGraph*	gRegion_prism;	
TGraph*	gRegion_mirror;	
TGraph* gangtmin;
double CherenkovBoundary(double *x, double *p);

void exportArrayToHeader(const std::string& filename, const std::string& arrayName, const double* data, int size);

const double Z_barmin	= -2729.0;		// z position of bar end, @ prism (mm)
const double Z_barmax	=  1831.025;	// z position of bar end, @ mirror (mm)
const double v_group	=   199.5;		// fixed group velocity in mm/ns
const double tts_pad	=     0.5;		// fixed 500 ps sensor TTS padding
const double Rpar		=   774.5;		// 765.875 + 17.25/2.;
const double Zpar		=     0.0;		// Zvertex!  (maybe -Zvertex...)
const double n 			=     1.4738;	// 

//const double COSMIN	= 0.6785;		// cos(theta).ge.1.0003/1.4738 -> MIN = 0.6785
const double COSMIN		= 0.5;   		// 

int itext	= -1;
TLatex *text[100];

//----------------------------------------------------------------------------
void bands() {

	TFile*	f		= new TFile("./test.incidence.root","read");
//	TFile*	f		= new TFile("./root/DIS/dis_eicBeam_hiDiv_18x275_1to10.incidence.root","read");
	hOPcr_costhetaz	= (TH2D*)f->Get("hOPcr_costhetaz");
	hOPcr_costhetaz	->SetDirectory(0);
	f->Close();
	
	makebands();
	drawbands();

	for (int i=0;i<2;i++){	// directions...
		gtcutL[i]		= new TGraph();
		gtcutU[i]		= new TGraph();
		gangtcutL[i]	= new TGraph();
		gangtcutU[i]	= new TGraph();
	}
	gangtmin	= new TGraph();
	makegates();
	drawgates();

	TFile* fout	= new TFile("bands.root","recreate");
		fout->cd();
		for (int i=0;i<2;i++){	// directions...
			gtcutL[i]	->Write(Form("gtcutL_%d",i));
			gtcutU[i]	->Write(Form("gtcutU_%d",i));
			gangtcutL[i]->Write(Form("gangtcutL_%d",i));
			gangtcutU[i]->Write(Form("gangtcutU_%d",i));
		}
	fout->Close();
	writebands();

//	drawbandsb();
	
}

//----------------------------------------------------------------------------
void writebands(){
	double tL[2][4600] = {0};	// 1832-(-2730) = 4562
	double tU[2][4600] = {0};
	double cosangtmin[4600] 	= {0};
	double cosangupper[4600]	= {0};
	double cosanglower[4600]	= {0};
	for (int iz=-2730;iz<=1832;iz++){
		double	z	= iz;
		int 	kz	= iz + 2730;			// given Zinc -> iZinc -> kz = iZinc + 2730
		for (int izd=0;izd<2;izd++){
			tL[izd][kz]	= gtcutL[izd]->Eval(z);
			tU[izd][kz]	= gtcutU[izd]->Eval(z);
		}
		cosangtmin[kz]	= gangtmin->Eval(z);
		cosangupper[kz]	= f_upper->Eval(z);
		cosanglower[kz]	= f_lower->Eval(z);
	}
// 	exportArrayToHeader("./newheaders/tcutL0_.h", "tcutL0_", tL[0], 4562);
// 	exportArrayToHeader("./newheaders/tcutU0_.h", "tcutU0_", tU[0], 4562);
// 	exportArrayToHeader("./newheaders/tcutL1_.h", "tcutL1_", tL[1], 4562);
// 	exportArrayToHeader("./newheaders/tcutU1_.h", "tcutU1_", tU[1], 4562);
// 	//
// 	exportArrayToHeader("./newheaders/cosangtmin.h", "cosangtmin", cosangtmin, 4562);
	//
	exportArrayToHeader("./newheaders/cosangupper.h", "cosangupper", cosangupper, 4562);
	exportArrayToHeader("./newheaders/cosanglower.h", "cosanglower", cosanglower, 4562);
	//
}

//----------------------------------------------------------------------------
// Helper function to export any double array to a formatted include file
void exportArrayToHeader(const std::string& filename, const std::string& arrayName, const double* data, int size) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    outFile << std::fixed << std::setprecision(4);
    outFile << "double " << arrayName << "[" << size << "] = {\n  ";
    for (int i = 0; i < size; ++i) {
        outFile << data[i];
        if (i < size - 1) {
            outFile << ", ";
            if ((i + 1) % 16 == 0) {
                outFile << "\n  ";
            }
        }
    }
    outFile << "\n};\n";
    outFile.close();
    cout<<"wrote "<<filename<<endl;
}

//----------------------------------------------------------------------------
void makegates() {
    // --- Constants & Configuration ---
    
    // Number of angular steps to sample inside the band at each Z
    const int n_ang_samples = 10000; 

    for (int iz = 0; iz <= 4700; iz++) {
        double z_track = ((double)iz) - 2800.;
        //---- Get the boundary cosines from your analytical curves
        double cos_B = f_lower->Eval(z_track); // Lower bound
        double cos_A = f_upper->Eval(z_track); // Upper bound
        //---- Ensure proper ordering for the loop
        double cos_min = std::min(cos_B, cos_A);
        double cos_max = std::max(cos_B, cos_A);
        double t_min_overall = 999999.0;
        double ang_t_min_overall;
        double t_min_calc[2];
        double t_max_calc[2];
        double ang_t_min[2];
        double ang_t_max[2];
        for (int i=0;i<2;i++){ 
        	t_min_calc[i] =  999999.0;
        	t_max_calc[i] = -999999.0;
        }
        bool found_valid_photon = false;
        //---- Helper lambda to calculate 3D path length
        auto get_path_length = [=](double z_init, double cos_val) {
            double abs_cos = std::fabs(cos_val);
            if (cos_val < 0) {		// Direct Path
                return std::fabs(z_init - Z_barmin) / abs_cos;
            } else {				// Reflected Path
                double to_mirror = std::fabs(Z_barmax - z_init);
                double mirror_to_sensor = std::fabs(Z_barmax - Z_barmin);
                return (to_mirror + mirror_to_sensor) / abs_cos;
            }
        };
        //---- Scan across the entire angular band to find true t_min and t_max
        double step = (cos_max - cos_min) / double(n_ang_samples);
        for (int i = 0; i <= n_ang_samples; i++) {
            double cos_val = cos_min + i * step;
            // Protect against division by zero near cos(theta) ~ 0
            if (std::fabs(cos_val) < COSMIN) continue; 
	        int indzdir;
            if (cos_val < 0){ indzdir=0; }else{ indzdir=1; }
            double path = get_path_length(z_track, cos_val);
            double t_path = path / v_group;
            if (t_path < t_min_overall      ){ t_min_overall       = t_path; ang_t_min_overall  = cos_val; }
            if (t_path < t_min_calc[indzdir]){ t_min_calc[indzdir] = t_path; ang_t_min[indzdir] = cos_val; }
            if (t_path > t_max_calc[indzdir]){ t_max_calc[indzdir] = t_path; ang_t_max[indzdir] = cos_val; }
            found_valid_photon = true;
        }
        //---- Apply timing padding and save the point if a valid path was found
        if (found_valid_photon) {
        	for (int i=0;i<2;i++){
				double t_min = t_min_calc[i] - tts_pad;
				double t_max = t_max_calc[i] + tts_pad;
				if (t_min> 1000) t_min = 0;
				if (t_max<-1000) t_max = 0;
				gtcutL[i]		->SetPoint(gtcutL[i]->GetN()   , z_track,     t_min   );
				gtcutU[i]		->SetPoint(gtcutU[i]->GetN()   , z_track,     t_max   );
				gangtcutL[i]	->SetPoint(gangtcutL[i]->GetN(), z_track, ang_t_min[i]);
				gangtcutU[i]	->SetPoint(gangtcutU[i]->GetN(), z_track, ang_t_max[i]);
			}	// directions...
        	//
        	gangtmin	->SetPoint(gangtmin->GetN(),z_track,ang_t_min_overall);
			//
        }
    } // end Z loop
}

//----------------------------------------------------------------------------
void makebands() {
    const double theta_C	= std::acos(1.0 / n);	// ~0.8242 radians @ beta=1
    const double cos_tC		= std::cos(theta_C);
    const double sin_tC		= std::sin(theta_C);
	f_upper = new TF1("f_upper", CherenkovBoundary, -2800, 2000, 5); 
	f_lower = new TF1("f_lower", CherenkovBoundary, -2800, 2000, 5); 
	f_upper	->SetParameters(Rpar, Zpar, cos_tC, sin_tC,  1.0);		// sign= 1 upper
	f_lower	->SetParameters(Rpar, Zpar, cos_tC, sin_tC, -1.0);		// sign=-1 lower
	f_upper	->SetParNames("Rpar", "Zpar", "cos_tC", "sin_tC", "sign");
	f_lower	->SetParNames("Rpar", "Zpar", "cos_tC", "sin_tC", "sign");
	f_upper->FixParameter(2, cos_tC);	f_upper->FixParameter(3, sin_tC);	f_upper->FixParameter(4,  1.0);
	f_lower->FixParameter(2, cos_tC);	f_lower->FixParameter(3, sin_tC);	f_lower->FixParameter(4, -1.0);
}
double CherenkovBoundary(double *x, double *p) {
    double z 			= x[0];		// value for incident particle...
    double R 			= p[0];		// Rpar: 774.5
    double z_offset		= p[1];		// Zpar:   0.0
    double cos_tC		= p[2];		// =1/n  (assumes beta=1)
    double sin_tC		= p[3];		// 
    double sign			= p[4]; 	// 1.0 for upper, -1.0 for lower
    double dz			= z - z_offset;
    double hypotenuse	= std::sqrt(dz*dz + R*R);
    if (hypotenuse == 0) return 0.0;		// Prevent division by zero if evaluating exactly at the vertex
    double cos_theta_p	= dz / hypotenuse;
    double sin_theta_p	= R / hypotenuse;
    double widen		= sign*0.005;
    return (cos_theta_p*cos_tC) + (sign*sin_theta_p*sin_tC) + widen;	// (+) for upper band and (-) for lower band
}

//----------------------------------------------------------------------------
void drawbands() {
	gStyle->SetOptStat(0);
	gStyle->SetErrorX(0);
	gStyle->SetPadRightMargin(0.03);
	gStyle->SetPadTopMargin(0.055);
	gStyle->SetPadBottomMargin(0.075);
	gStyle->SetPadLeftMargin(0.06);
	gStyle->SetTitleX(0.5);
	gStyle->SetLabelSize(0.04,"xyz");
	gStyle->SetTitleSize(0.04,"t");
	gStyle->SetPadGridX(0);
	gStyle->SetPadGridY(0);
	gStyle->SetPaintTextFormat("#1.1e");
	gStyle->SetStatX(0.5);
	gStyle->SetStatY(0.94);
	for (int i=0;i<100;i++){
		text[i]	= new TLatex();
		text[i]	->SetNDC(true);
		text[i]	->SetTextFont(41);
		text[i]	->SetTextSize(0.03);
		text[i]	->SetTextAlign(32);
	}
    f_upper->SetLineColor(kGreen);
    f_upper->SetLineStyle(1);
    f_upper->SetLineWidth(4);
    f_lower->SetLineColor(kGreen);
    f_lower->SetLineStyle(1);
    f_lower->SetLineWidth(4);
	//
	TCanvas *can0 = new TCanvas("can0","bands0",0,30,1400,600);
	can0->cd();
	can0->cd(1);
		hOPcr_costhetaz->GetXaxis()->SetRangeUser(-2900,2000);
//!!		hOPcr_costhetaz->SetMaximum(1000);
		hOPcr_costhetaz->Draw();
	can0->cd();
	can0->Print("bands0.png");
	//
	TCanvas *cana = new TCanvas("cana","bands1",30,60,1400,600);
	cana->cd();
	cana->cd(1);
		hOPcr_costhetaz->GetXaxis()->SetRangeUser(-2900,2000);
		hOPcr_costhetaz->Draw();
		f_upper->Draw("same");
		f_lower->Draw("same");
	cana->cd();
	cana->Print("bands1.png");

}
//----------------------------------------------------------------------------
void drawgates() {
	//
	gStyle->SetPadLeftMargin(0.08);
	gStyle->SetPadRightMargin(0.02);
	for (int i=0;i<2;i++){		// directions...
		gtcutL[i]->SetLineColor(4);
		gtcutL[i]->SetLineStyle(1+i);
		gtcutL[i]->SetLineWidth(3);
		gtcutU[i]->SetLineColor(6);
		gtcutU[i]->SetLineStyle(1+i);
		gtcutU[i]->SetLineWidth(3);
	}
	
// 	TCanvas *can = new TCanvas("can","cuts",60,90,1400,600);
// 	can->cd(); can->Divide(2,1,0.0001,0.0001);
// 	can->cd(1);
// 		TH1F* frame1	= (TH1F*)gPad->DrawFrame(-2900,-5,200,55);	
// 			  frame1	->SetTitle("OP bar travel time cuts (ns) vs OP creation Z");
// 			  frame1	->GetXaxis()->SetNdivisions(6);
// 		gtcutU[0]->Draw("same");   
// 		gtcutL[0]->Draw("same");
// 		//
// 		++itext; text[itext]->SetTextSize(0.04); text[itext]->DrawLatex(0.77,0.9,"Prism-going");
// 		++itext; text[itext]->DrawLatex(0.45,0.90,Form("Z_barmin = %.1f",Z_barmin));
// 		++itext; text[itext]->DrawLatex(0.45,0.86,Form("Z_barmax = %.1f",Z_barmax));
// 		++itext; text[itext]->DrawLatex(0.45,0.82,Form("v_group = %.1f",v_group ));
// 		++itext; text[itext]->DrawLatex(0.45,0.78,Form("tts_pad = %.3f",tts_pad ));
// 		++itext; text[itext]->DrawLatex(0.45,0.74,Form("Rpar = %.1f",Rpar    ));
// 		++itext; text[itext]->DrawLatex(0.45,0.70,Form("Zpar = %.1f",Zpar    ));
// 		++itext; text[itext]->DrawLatex(0.45,0.66,Form("n = %.4f",n 	    ));
// 		++itext; text[itext]->DrawLatex(0.45,0.58,Form("OP min cos(theta) = %.4f",COSMIN   ));
// 		gPad->Update();
// 		//
// 	can->cd(2);
// 		TH1F* frame2	= (TH1F*)gPad->DrawFrame(-200,-5,2000,55);	
// 			  frame2	->SetTitle("OP bar travel time cuts (ns) vs OP creation Z");
// 			  frame2	->GetXaxis()->SetNdivisions(6);
// 		gtcutU[1]->Draw("same");   
// 		gtcutL[1]->Draw("same");
// 		//
// 		++itext; text[itext]->SetTextSize(0.04); text[itext]->DrawLatex(0.77,0.9,"Mirror-going");
// 		gPad->Update();
// 		//
// 	can->cd();
// 	can->Print("bands2.png");

	gRegion_prism	= makeregion(gtcutL[0],gtcutU[0]);
	gRegion_mirror	= makeregion(gtcutL[1],gtcutU[1]);
		gRegion_prism ->SetFillColor(kGreen);
		gRegion_mirror->SetFillColor(kGreen);
		gRegion_prism ->SetFillStyle(3001);
		gRegion_mirror->SetFillStyle(3001);
	TLine* lvert	= new TLine();
		lvert		->SetLineWidth(2);
		lvert		->SetLineColor(16);
		
	TCanvas *cand = new TCanvas("cand","bands2",60,90,1400,600);
	cand->cd(); cand->Divide(2,1,0.0001,0.0001);
	cand->cd(1);
		TH1F* frame1	= (TH1F*)gPad->DrawFrame(-2900,-2,800,70);	
			  frame1	->SetTitle("OP bar travel time cuts (ns) vs OP creation Z");
			  frame1	->GetXaxis()->SetNdivisions(6);
		lvert->DrawLine(0,-2,0,70);					// theta=90 deg
		lvert->DrawLine(Z_barmin,-2,Z_barmin,70);	// end of bar
		gRegion_prism->Draw("F same");
		gtcutU[0]->Draw("same");   
		gtcutL[0]->Draw("same");
		//
		++itext; text[itext]->SetTextSize(0.04); text[itext]->DrawLatex(0.77,0.9,"Prism-going");
		++itext; text[itext]->DrawLatex(0.45,0.90,Form("Z_barmin = %.1f",Z_barmin));
		++itext; text[itext]->DrawLatex(0.45,0.86,Form("Z_barmax = %.1f",Z_barmax));
		++itext; text[itext]->DrawLatex(0.45,0.82,Form("v_group = %.1f",v_group ));
		++itext; text[itext]->DrawLatex(0.45,0.78,Form("tts_pad = %.3f",tts_pad ));
		++itext; text[itext]->DrawLatex(0.45,0.74,Form("Rpar = %.1f",Rpar    ));
		++itext; text[itext]->DrawLatex(0.45,0.70,Form("Zpar = %.1f",Zpar    ));
		++itext; text[itext]->DrawLatex(0.45,0.66,Form("n = %.4f",n 	    ));
		++itext; text[itext]->DrawLatex(0.45,0.58,Form("OP min cos(theta) = %.4f",COSMIN   ));
		gPad->Update();
		//
	cand->cd(2);
		TH1F* frame2	= (TH1F*)gPad->DrawFrame(-800,-2,2000,70);	
			  frame2	->SetTitle("OP bar travel time cuts (ns) vs OP creation Z");
			  frame2	->GetXaxis()->SetNdivisions(6);
		lvert->DrawLine(0,-2,0,70);
		lvert->DrawLine(Z_barmax,-2,Z_barmax,70);	// end of bar
		gRegion_mirror->Draw("F same");
		gtcutU[1]->Draw("same");   
		gtcutL[1]->Draw("same");
		//
		++itext; text[itext]->SetTextSize(0.04); text[itext]->DrawLatex(0.77,0.9,"Mirror-going");
		gPad->Update();
		//
	cand->cd();
	cand->Print("bands2.png");

	TCanvas *cane = new TCanvas("cane","bands2b",90,120,1400,600);
	cane->cd(); cane->Divide(1,1,0.0001,0.0001);
	cane->cd(1);
		TH1F* frame1b	= (TH1F*)gPad->DrawFrame(-2900,-2,2000,70);	
			  frame1b	->SetTitle("OP bar travel time cuts (ns) vs OP creation Z");
			  frame1b	->GetXaxis()->SetNdivisions(6);
		gRegion_prism->Draw("F");
		gtcutU[0]->Draw("L");   
		gtcutL[0]->Draw("L");
		gRegion_mirror->Draw("F");
		gtcutU[1]->Draw("L");   
		gtcutL[1]->Draw("L");
 		gPad->Update();
		//
// 		++itext; text[itext]->SetTextSize(0.04); text[itext]->DrawLatex(0.77,0.9,"Prism-going");
// 		++itext; text[itext]->DrawLatex(0.45,0.90,Form("Z_barmin = %.1f",Z_barmin));
// 		++itext; text[itext]->DrawLatex(0.45,0.86,Form("Z_barmax = %.1f",Z_barmax));
// 		++itext; text[itext]->DrawLatex(0.45,0.82,Form("v_group = %.1f",v_group ));
// 		++itext; text[itext]->DrawLatex(0.45,0.78,Form("tts_pad = %.3f",tts_pad ));
// 		++itext; text[itext]->DrawLatex(0.45,0.74,Form("Rpar = %.1f",Rpar    ));
// 		++itext; text[itext]->DrawLatex(0.45,0.70,Form("Zpar = %.1f",Zpar    ));
// 		++itext; text[itext]->DrawLatex(0.45,0.66,Form("n = %.4f",n 	    ));
// 		++itext; text[itext]->DrawLatex(0.45,0.58,Form("OP min cos(theta) = %.4f",COSMIN   ));
// 		gPad->Update();
// 		//
// 	cane->cd(2);
// 		TH1F* frame2	= (TH1F*)gPad->DrawFrame(-800,-5,2000,200);	
// 			  frame2	->SetTitle("OP bar travel time cuts (ns) vs OP creation Z");
// 			  frame2	->GetXaxis()->SetNdivisions(6);
// 		//
// 		++itext; text[itext]->SetTextSize(0.04); text[itext]->DrawLatex(0.77,0.9,"Mirror-going");
// 		gPad->Update();
		//
	cane->cd();
	cane->Print("bands2b.png");


}
//----------------------------------------------------------------------------
TGraph* makeregion(TGraph* g1, TGraph* g2){
	if (!g1 || !g2) return nullptr;	
	int n1 = g1->GetN();
	int n2 = g2->GetN();
	TGraph* gFilled = new TGraph(n1 + n2);
	for (int i = 0; i < n1; i++) {
		gFilled->SetPoint(i, g1->GetPointX(i), g1->GetPointY(i));
	}
	for (int i = 0; i < n2; i++) {
		gFilled->SetPoint(n1 + i, g2->GetPointX(n2 - 1 - i), g2->GetPointY(n2 - 1 - i));
	}
	return gFilled;
}



//----------------------------------------------------------------------------
void drawbandsb() {
	gStyle->SetPadLeftMargin(0.05);
	gStyle->SetPadRightMargin(0.02);
	for (int i=0;i<2;i++){		// directions...
		gangtcutL[i]->SetLineColor(1);
		gangtcutL[i]->SetLineStyle(1);
		gangtcutL[i]->SetLineWidth(4);
		gangtcutU[i]->SetLineColor(2);
		gangtcutU[i]->SetLineStyle(2);
		gangtcutU[i]->SetLineWidth(4);
	}
	TCanvas *canb = new TCanvas("canb","cuts",120,150,1200,600);
	canb->cd(); //canb->Divide(2,1,0.0001,0.0001);
	canb->cd(1);
		hOPcr_costhetaz->GetXaxis()->SetRangeUser(-2900,2000);
		hOPcr_costhetaz->Draw();
		f_upper->Draw("same");
		f_lower->Draw("same");
		for (int i=0;i<2;i++){		// directions...
			gangtcutL[i]->Draw("L");
			gangtcutU[i]->Draw("L");
		}
	canb->cd();
	canb->Print("bands3.png");
	TLegend *leg	= new TLegend(0.12,0.76,0.3,0.9);
		leg->AddEntry(gangtcutL[0],"Earliest Time","L");
		leg->AddEntry(gangtcutU[0],"Latest Time","L");
	leg->Draw("same");
}

