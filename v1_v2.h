// Constants

Double_t mp = .938272013;
Double_t mn = .939565346;
Double_t md = 2.013553212;
Double_t mHe= 2.808391383;
Double_t mHe4 = 3.7272;
Double_t mt= 2.808920906;
Double_t mbaryres=1.440;
Double_t mdelta = 1.232;
Double_t mpi = 0.139570;
Double_t mpi0 = 0.134976;
Double_t gev2mev = 1000.;
Double_t amu2mev = 931.494028;

 // Counters
  Int_t cntr = 0;
  Int_t cntr1 = 0;
  Int_t cntr2 = 0;
  Int_t cntr3 = 0;
  Int_t cntr4 = 0;
  Int_t cntr5 = 0;
  Int_t cntr6 = 0;
  Int_t cntr7 = 0;
  Int_t cntr8 = 0;
  Int_t cntr9 = 0;

vector<double> proton_v2;
vector<double> proton_v1;
vector<double> proton_pt;
vector<double> proton_y0;


  // Declaration of leaf types
  Int_t           ipid;
  Short_t         ievent;
  Float_t         px;
  Float_t         py;
  Float_t         pz_lab;

  Int_t           ipid_der;
  Float_t         pxCMS;
  Float_t         pyCMS;
  Float_t         pzCMS;
  Float_t         kenergy_c;
  Float_t         kenergy_l;
  Float_t         thetaC;
  Float_t         thetaL;
  Float_t         phiL;
  Float_t         energy_c;
  Float_t         energy_l;
  Bool_t          flag;
  Float_t         rap;
  Float_t         pt;

// List of branches
  TBranch        *b_ipid;   //!
  TBranch        *b_ievent;   //!
  TBranch        *b_px;   //!
  TBranch        *b_py;   //!
  TBranch        *b_pz_lab;   //!

  TBranch        *b_ipid_der;   //!
  TBranch        *b_pxCMS;   //!
  TBranch        *b_pyCMS;   //!
  TBranch        *b_pzCMS;   //!
  TBranch        *b_kenergy_c;   //!
  TBranch        *b_kenergy_l;   //!
  TBranch        *b_thetaC;   //!
  TBranch        *b_thetaL;   //!
  TBranch        *b_phiL;   //!
  TBranch        *b_energy_c;   //!
  TBranch        *b_energy_l;   //!
  TBranch        *b_flag;   //!
  TBranch        *b_rap;   //!
  TBranch        *b_pt;   //!
