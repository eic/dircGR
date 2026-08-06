//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat May 16 21:06:14 2026 by ROOT version 6.38.00
// from TTree events/events data tree
// found on file: luttest_1M.edm4hep.root
//////////////////////////////////////////////////////////

#ifndef lutgen_h
#define lutgen_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
//#include "podio/ObjectID.h"

class lutgen {
public :

   lutgen(int kb=0,TTree *tree=0);
   virtual ~lutgen();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual bool     Notify();
   virtual void     Show(Long64_t entry = -1);

   virtual TH2Poly* EqualAreaSphereHistogram();

   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.
   static constexpr Int_t kMaxDIRCBarHits = 1000;
   static constexpr Int_t kMax_DIRCBarHits_particle = 1000;
   static constexpr Int_t kMaxEventHeader = 1000;
   static constexpr Int_t kMaxMCParticles = 10;
   static constexpr Int_t kMax_MCParticles_parents = 100;
   static constexpr Int_t kMax_MCParticles_daughters = 100;

	int kBar;

   // Declaration of leaf types
   Int_t           DIRCBarHits_;
   ULong_t         DIRCBarHits_cellID[kMaxDIRCBarHits];   //[DIRCBarHits_]
   Float_t         DIRCBarHits_eDep[kMaxDIRCBarHits];   //[DIRCBarHits_]
   Float_t         DIRCBarHits_time[kMaxDIRCBarHits];   //[DIRCBarHits_]
   Float_t         DIRCBarHits_pathLength[kMaxDIRCBarHits];   //[DIRCBarHits_]
   Int_t           DIRCBarHits_quality[kMaxDIRCBarHits];   //[DIRCBarHits_]
   Double_t        DIRCBarHits_position_x[kMaxDIRCBarHits];   //[DIRCBarHits_]
   Double_t        DIRCBarHits_position_y[kMaxDIRCBarHits];   //[DIRCBarHits_]
   Double_t        DIRCBarHits_position_z[kMaxDIRCBarHits];   //[DIRCBarHits_]
   Float_t         DIRCBarHits_momentum_x[kMaxDIRCBarHits];   //[DIRCBarHits_]
   Float_t         DIRCBarHits_momentum_y[kMaxDIRCBarHits];   //[DIRCBarHits_]
   Float_t         DIRCBarHits_momentum_z[kMaxDIRCBarHits];   //[DIRCBarHits_]
   Int_t           _DIRCBarHits_particle_;
   Int_t           _DIRCBarHits_particle_index[kMax_DIRCBarHits_particle];   //[_DIRCBarHits_particle_]
   UInt_t          _DIRCBarHits_particle_collectionID[kMax_DIRCBarHits_particle];   //[_DIRCBarHits_particle_]
   Int_t           EventHeader_;
   ULong_t         EventHeader_eventNumber[kMaxEventHeader];   //[EventHeader_]
   UInt_t          EventHeader_runNumber[kMaxEventHeader];   //[EventHeader_]
   ULong_t         EventHeader_timeStamp[kMaxEventHeader];   //[EventHeader_]
   Double_t        EventHeader_weight[kMaxEventHeader];   //[EventHeader_]
   UInt_t          EventHeader_weights_begin[kMaxEventHeader];   //[EventHeader_]
   UInt_t          EventHeader_weights_end[kMaxEventHeader];   //[EventHeader_]
   vector<double>  *_EventHeader_weights;
   Int_t           MCParticles_;
   Int_t           MCParticles_PDG[kMaxMCParticles];   //[MCParticles_]
   Int_t           MCParticles_generatorStatus[kMaxMCParticles];   //[MCParticles_]
   Int_t           MCParticles_simulatorStatus[kMaxMCParticles];   //[MCParticles_]
   Float_t         MCParticles_charge[kMaxMCParticles];   //[MCParticles_]
   Float_t         MCParticles_time[kMaxMCParticles];   //[MCParticles_]
   Double_t        MCParticles_mass[kMaxMCParticles];   //[MCParticles_]
   Double_t        MCParticles_vertex_x[kMaxMCParticles];   //[MCParticles_]
   Double_t        MCParticles_vertex_y[kMaxMCParticles];   //[MCParticles_]
   Double_t        MCParticles_vertex_z[kMaxMCParticles];   //[MCParticles_]
   Double_t        MCParticles_endpoint_x[kMaxMCParticles];   //[MCParticles_]
   Double_t        MCParticles_endpoint_y[kMaxMCParticles];   //[MCParticles_]
   Double_t        MCParticles_endpoint_z[kMaxMCParticles];   //[MCParticles_]
   Double_t        MCParticles_momentum_x[kMaxMCParticles];   //[MCParticles_]
   Double_t        MCParticles_momentum_y[kMaxMCParticles];   //[MCParticles_]
   Double_t        MCParticles_momentum_z[kMaxMCParticles];   //[MCParticles_]
   Double_t        MCParticles_momentumAtEndpoint_x[kMaxMCParticles];   //[MCParticles_]
   Double_t        MCParticles_momentumAtEndpoint_y[kMaxMCParticles];   //[MCParticles_]
   Double_t        MCParticles_momentumAtEndpoint_z[kMaxMCParticles];   //[MCParticles_]
   Int_t           MCParticles_helicity[kMaxMCParticles];   //[MCParticles_]
   UInt_t          MCParticles_parents_begin[kMaxMCParticles];   //[MCParticles_]
   UInt_t          MCParticles_parents_end[kMaxMCParticles];   //[MCParticles_]
   UInt_t          MCParticles_daughters_begin[kMaxMCParticles];   //[MCParticles_]
   UInt_t          MCParticles_daughters_end[kMaxMCParticles];   //[MCParticles_]
   Int_t           _MCParticles_parents_;
   Int_t           _MCParticles_parents_index[kMax_MCParticles_parents];   //[_MCParticles_parents_]
   UInt_t          _MCParticles_parents_collectionID[kMax_MCParticles_parents];   //[_MCParticles_parents_]
   Int_t           _MCParticles_daughters_;
   Int_t           _MCParticles_daughters_index[kMax_MCParticles_daughters];   //[_MCParticles_daughters_]
   UInt_t          _MCParticles_daughters_collectionID[kMax_MCParticles_daughters];   //[_MCParticles_daughters_]
   vector<string>  *GPIntKeys;
   vector<vector<int> > *GPIntValues;
   vector<string>  *GPFloatKeys;
   vector<vector<float> > *GPFloatValues;
   vector<string>  *GPDoubleKeys;
   vector<vector<double> > *GPDoubleValues;
   vector<string>  *GPStringKeys;
   vector<vector<string> > *GPStringValues;

   // List of branches
   TBranch        *b_DIRCBarHits_;   //!
   TBranch        *b_DIRCBarHits_cellID;   //!
   TBranch        *b_DIRCBarHits_eDep;   //!
   TBranch        *b_DIRCBarHits_time;   //!
   TBranch        *b_DIRCBarHits_pathLength;   //!
   TBranch        *b_DIRCBarHits_quality;   //!
   TBranch        *b_DIRCBarHits_position_x;   //!
   TBranch        *b_DIRCBarHits_position_y;   //!
   TBranch        *b_DIRCBarHits_position_z;   //!
   TBranch        *b_DIRCBarHits_momentum_x;   //!
   TBranch        *b_DIRCBarHits_momentum_y;   //!
   TBranch        *b_DIRCBarHits_momentum_z;   //!
   TBranch        *b__DIRCBarHits_particle_;   //!
   TBranch        *b__DIRCBarHits_particle_index;   //!
   TBranch        *b__DIRCBarHits_particle_collectionID;   //!
   TBranch        *b_EventHeader_;   //!
   TBranch        *b_EventHeader_eventNumber;   //!
   TBranch        *b_EventHeader_runNumber;   //!
   TBranch        *b_EventHeader_timeStamp;   //!
   TBranch        *b_EventHeader_weight;   //!
   TBranch        *b_EventHeader_weights_begin;   //!
   TBranch        *b_EventHeader_weights_end;   //!
   TBranch        *b__EventHeader_weights;   //!
   TBranch        *b_MCParticles_;   //!
   TBranch        *b_MCParticles_PDG;   //!
   TBranch        *b_MCParticles_generatorStatus;   //!
   TBranch        *b_MCParticles_simulatorStatus;   //!
   TBranch        *b_MCParticles_charge;   //!
   TBranch        *b_MCParticles_time;   //!
   TBranch        *b_MCParticles_mass;   //!
   TBranch        *b_MCParticles_vertex_x;   //!
   TBranch        *b_MCParticles_vertex_y;   //!
   TBranch        *b_MCParticles_vertex_z;   //!
   TBranch        *b_MCParticles_endpoint_x;   //!
   TBranch        *b_MCParticles_endpoint_y;   //!
   TBranch        *b_MCParticles_endpoint_z;   //!
   TBranch        *b_MCParticles_momentum_x;   //!
   TBranch        *b_MCParticles_momentum_y;   //!
   TBranch        *b_MCParticles_momentum_z;   //!
   TBranch        *b_MCParticles_momentumAtEndpoint_x;   //!
   TBranch        *b_MCParticles_momentumAtEndpoint_y;   //!
   TBranch        *b_MCParticles_momentumAtEndpoint_z;   //!
   TBranch        *b_MCParticles_helicity;   //!
   TBranch        *b_MCParticles_parents_begin;   //!
   TBranch        *b_MCParticles_parents_end;   //!
   TBranch        *b_MCParticles_daughters_begin;   //!
   TBranch        *b_MCParticles_daughters_end;   //!
   TBranch        *b__MCParticles_parents_;   //!
   TBranch        *b__MCParticles_parents_index;   //!
   TBranch        *b__MCParticles_parents_collectionID;   //!
   TBranch        *b__MCParticles_daughters_;   //!
   TBranch        *b__MCParticles_daughters_index;   //!
   TBranch        *b__MCParticles_daughters_collectionID;   //!
   TBranch        *b_GPIntKeys;   //!
   TBranch        *b_GPIntValues;   //!
   TBranch        *b_GPFloatKeys;   //!
   TBranch        *b_GPFloatValues;   //!
   TBranch        *b_GPDoubleKeys;   //!
   TBranch        *b_GPDoubleValues;   //!
   TBranch        *b_GPStringKeys;   //!
   TBranch        *b_GPStringValues;   //!

};

#endif

#ifdef lutgen_cxx
lutgen::lutgen(int kb, TTree *tree) : fChain(0) 
{
	//
	kBar	= kb;
	cout<<"Using Bar "<<kBar<<" .........................."<<endl;
	//
	TString	rfile	= TString(Form("./LUTsim/lut_bar%d_20M.edm4hep.root",kBar));
	TFile *f 		= new TFile(rfile.Data());
	f->GetObject("events",tree);
	Init(tree);
	//
}

lutgen::~lutgen()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t lutgen::GetEntry(Long64_t entry){
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t lutgen::LoadTree(Long64_t entry){
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void lutgen::Init(TTree *tree)
{
   _EventHeader_weights = 0;
   GPIntKeys = 0;
   GPIntValues = 0;
   GPFloatKeys = 0;
   GPFloatValues = 0;
   GPDoubleKeys = 0;
   GPDoubleValues = 0;
   GPStringKeys = 0;
   GPStringValues = 0;
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("DIRCBarHits", &DIRCBarHits_, &b_DIRCBarHits_);
   fChain->SetBranchAddress("DIRCBarHits.cellID", DIRCBarHits_cellID, &b_DIRCBarHits_cellID);
   fChain->SetBranchAddress("DIRCBarHits.eDep", DIRCBarHits_eDep, &b_DIRCBarHits_eDep);
   fChain->SetBranchAddress("DIRCBarHits.time", DIRCBarHits_time, &b_DIRCBarHits_time);
   fChain->SetBranchAddress("DIRCBarHits.pathLength", DIRCBarHits_pathLength, &b_DIRCBarHits_pathLength);
   fChain->SetBranchAddress("DIRCBarHits.quality", DIRCBarHits_quality, &b_DIRCBarHits_quality);
   fChain->SetBranchAddress("DIRCBarHits.position.x", DIRCBarHits_position_x, &b_DIRCBarHits_position_x);
   fChain->SetBranchAddress("DIRCBarHits.position.y", DIRCBarHits_position_y, &b_DIRCBarHits_position_y);
   fChain->SetBranchAddress("DIRCBarHits.position.z", DIRCBarHits_position_z, &b_DIRCBarHits_position_z);
   fChain->SetBranchAddress("DIRCBarHits.momentum.x", DIRCBarHits_momentum_x, &b_DIRCBarHits_momentum_x);
   fChain->SetBranchAddress("DIRCBarHits.momentum.y", DIRCBarHits_momentum_y, &b_DIRCBarHits_momentum_y);
   fChain->SetBranchAddress("DIRCBarHits.momentum.z", DIRCBarHits_momentum_z, &b_DIRCBarHits_momentum_z);
   fChain->SetBranchAddress("_DIRCBarHits_particle", &_DIRCBarHits_particle_, &b__DIRCBarHits_particle_);
   fChain->SetBranchAddress("_DIRCBarHits_particle.index", _DIRCBarHits_particle_index, &b__DIRCBarHits_particle_index);
   fChain->SetBranchAddress("_DIRCBarHits_particle.collectionID", _DIRCBarHits_particle_collectionID, &b__DIRCBarHits_particle_collectionID);
   fChain->SetBranchAddress("EventHeader", &EventHeader_, &b_EventHeader_);
   fChain->SetBranchAddress("EventHeader.eventNumber", EventHeader_eventNumber, &b_EventHeader_eventNumber);
   fChain->SetBranchAddress("EventHeader.runNumber", EventHeader_runNumber, &b_EventHeader_runNumber);
   fChain->SetBranchAddress("EventHeader.timeStamp", EventHeader_timeStamp, &b_EventHeader_timeStamp);
   fChain->SetBranchAddress("EventHeader.weight", EventHeader_weight, &b_EventHeader_weight);
   fChain->SetBranchAddress("EventHeader.weights_begin", EventHeader_weights_begin, &b_EventHeader_weights_begin);
   fChain->SetBranchAddress("EventHeader.weights_end", EventHeader_weights_end, &b_EventHeader_weights_end);
   fChain->SetBranchAddress("_EventHeader_weights", &_EventHeader_weights, &b__EventHeader_weights);
   fChain->SetBranchAddress("MCParticles", &MCParticles_, &b_MCParticles_);
   fChain->SetBranchAddress("MCParticles.PDG", MCParticles_PDG, &b_MCParticles_PDG);
   fChain->SetBranchAddress("MCParticles.generatorStatus", MCParticles_generatorStatus, &b_MCParticles_generatorStatus);
   fChain->SetBranchAddress("MCParticles.simulatorStatus", MCParticles_simulatorStatus, &b_MCParticles_simulatorStatus);
   fChain->SetBranchAddress("MCParticles.charge", MCParticles_charge, &b_MCParticles_charge);
   fChain->SetBranchAddress("MCParticles.time", MCParticles_time, &b_MCParticles_time);
   fChain->SetBranchAddress("MCParticles.mass", MCParticles_mass, &b_MCParticles_mass);
   fChain->SetBranchAddress("MCParticles.vertex.x", MCParticles_vertex_x, &b_MCParticles_vertex_x);
   fChain->SetBranchAddress("MCParticles.vertex.y", MCParticles_vertex_y, &b_MCParticles_vertex_y);
   fChain->SetBranchAddress("MCParticles.vertex.z", MCParticles_vertex_z, &b_MCParticles_vertex_z);
   fChain->SetBranchAddress("MCParticles.endpoint.x", MCParticles_endpoint_x, &b_MCParticles_endpoint_x);
   fChain->SetBranchAddress("MCParticles.endpoint.y", MCParticles_endpoint_y, &b_MCParticles_endpoint_y);
   fChain->SetBranchAddress("MCParticles.endpoint.z", MCParticles_endpoint_z, &b_MCParticles_endpoint_z);
   fChain->SetBranchAddress("MCParticles.momentum.x", MCParticles_momentum_x, &b_MCParticles_momentum_x);
   fChain->SetBranchAddress("MCParticles.momentum.y", MCParticles_momentum_y, &b_MCParticles_momentum_y);
   fChain->SetBranchAddress("MCParticles.momentum.z", MCParticles_momentum_z, &b_MCParticles_momentum_z);
   fChain->SetBranchAddress("MCParticles.momentumAtEndpoint.x", MCParticles_momentumAtEndpoint_x, &b_MCParticles_momentumAtEndpoint_x);
   fChain->SetBranchAddress("MCParticles.momentumAtEndpoint.y", MCParticles_momentumAtEndpoint_y, &b_MCParticles_momentumAtEndpoint_y);
   fChain->SetBranchAddress("MCParticles.momentumAtEndpoint.z", MCParticles_momentumAtEndpoint_z, &b_MCParticles_momentumAtEndpoint_z);
   fChain->SetBranchAddress("MCParticles.helicity", MCParticles_helicity, &b_MCParticles_helicity);
   fChain->SetBranchAddress("MCParticles.parents_begin", MCParticles_parents_begin, &b_MCParticles_parents_begin);
   fChain->SetBranchAddress("MCParticles.parents_end", MCParticles_parents_end, &b_MCParticles_parents_end);
   fChain->SetBranchAddress("MCParticles.daughters_begin", MCParticles_daughters_begin, &b_MCParticles_daughters_begin);
   fChain->SetBranchAddress("MCParticles.daughters_end", MCParticles_daughters_end, &b_MCParticles_daughters_end);
   fChain->SetBranchAddress("_MCParticles_parents", &_MCParticles_parents_, &b__MCParticles_parents_);
   fChain->SetBranchAddress("_MCParticles_parents.index", &_MCParticles_parents_index, &b__MCParticles_parents_index);
   fChain->SetBranchAddress("_MCParticles_parents.collectionID", &_MCParticles_parents_collectionID, &b__MCParticles_parents_collectionID);
   fChain->SetBranchAddress("_MCParticles_daughters", &_MCParticles_daughters_, &b__MCParticles_daughters_);
   fChain->SetBranchAddress("_MCParticles_daughters.index", &_MCParticles_daughters_index, &b__MCParticles_daughters_index);
   fChain->SetBranchAddress("_MCParticles_daughters.collectionID", &_MCParticles_daughters_collectionID, &b__MCParticles_daughters_collectionID);
   fChain->SetBranchAddress("GPIntKeys", &GPIntKeys, &b_GPIntKeys);
   fChain->SetBranchAddress("GPIntValues", &GPIntValues, &b_GPIntValues);
   fChain->SetBranchAddress("GPFloatKeys", &GPFloatKeys, &b_GPFloatKeys);
   fChain->SetBranchAddress("GPFloatValues", &GPFloatValues, &b_GPFloatValues);
   fChain->SetBranchAddress("GPDoubleKeys", &GPDoubleKeys, &b_GPDoubleKeys);
   fChain->SetBranchAddress("GPDoubleValues", &GPDoubleValues, &b_GPDoubleValues);
   fChain->SetBranchAddress("GPStringKeys", &GPStringKeys, &b_GPStringKeys);
   fChain->SetBranchAddress("GPStringValues", &GPStringValues, &b_GPStringValues);
   Notify();
}

bool lutgen::Notify(){ return true; }

void lutgen::Show(Long64_t entry)
{
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t lutgen::Cut(Long64_t entry){ return 1; }
#endif // #ifdef lutgen_cxx
