void drawDensity(vector<TString> fileList) {
  gStyle -> SetOptStat(0);
  gStyle -> SetPadRightMargin(0.03);
  gStyle -> SetPadTopMargin(0.03);
  gStyle -> SetPadBottomMargin(0.15);
  gStyle -> SetPadLeftMargin(0.13);
  gStyle -> SetTitleOffset(1.2, "x");
  gStyle -> SetTitleOffset(1.05, "y");
  gStyle -> SetTitleSize(0.06, "x");
  gStyle -> SetTitleSize(0.06, "y");
  gStyle -> SetLabelSize(0.06, "x");
  gStyle -> SetLabelSize(0.06, "y");
  gStyle -> SetTextFont(42);
  gStyle -> SetLegendTextSize(0.06);
  gStyle -> SetLegendBorderSize(0);
  gStyle -> SetPadGridX(kTRUE);
  gStyle -> SetPadGridY(kTRUE);

  auto maxTime = 270; // fm/c
  auto binTime = 5;  // fm/c
  auto numBins = maxTime/binTime;
  auto rho0 = 0.16; // fm-3
  auto reducedRho = new Double_t[numBins];

  Int_t numFiles = fileList.size();
  Int_t color[5] = {46, 8, 38, 41, 49};

  auto beamA = new Int_t[numFiles];     // AMU
  auto targetA = new Int_t[numFiles];   // AMU
  auto beamKE = new Double_t[numFiles];  // MeV/A
  auto gammas = new Double_t[numFiles];
  auto bs = new Int_t[numFiles];     // fm

  TString beamSym;
  TString targetSym;

  auto legend = new TLegend(0.75, 0.92, 0.95, 0.57);
  legend -> SetFillStyle(0);

  auto cvs = new TCanvas("cvs", "", 800, 600);

  auto gRho = new TGraph*[numFiles];

  auto tPoints = new Double_t[numBins];
  for (Int_t iBin = 0; iBin < numBins; iBin++)
    tPoints[iBin] = binTime/2. + iBin*binTime;

  Float_t ttime, rho;
  Int_t pid;
  for (Int_t iFile = 0; iFile < numFiles; iFile++) {
    memset(reducedRho, 0, sizeof(Double_t)*numBins);
    
    auto array = fileList[iFile].Tokenize("_");

    auto proj = ((TObjString *) array -> At(0)) -> String();
    beamA[iFile] = TString(proj(2, proj.Length() - 2)).Atoi();
    beamSym = TString(proj(0, 1));
    beamSym.ToUpper();
    beamSym += TString(proj(1, 1));

    auto targ = ((TObjString *) array -> At(1)) -> String();
    targetA[iFile] = TString(targ(2, targ.Length() - 2)).Atoi();
    targetSym = TString(targ(0, 1));
    targetSym.ToUpper();
    targetSym += TString(targ(1, 1));

    auto energy = ((TObjString *) array -> At(2)) -> String();
    beamKE[iFile] = TString(energy(6, energy.Length() - 6)).Atoi();

    auto gamma = ((TObjString *) array -> At(3)) -> String();
    gammas[iFile] = TString(gamma(5, gamma.Length() - 5)).Atof();

    auto b = ((TObjString *) array -> At(4)) -> String();
    bs[iFile] = TString(b(1, b.Length() - 1)).Atoi();

    auto pure = new TChain("pure");
    pure -> AddFile(fileList[iFile]);
    cout << "File opened: " << fileList[iFile] << endl;

//    pure -> AddFile(fileList[iFile].ReplaceAll("nopions", "pion"));
//    cout << "Added to chain: " << fileList[iFile] << endl;

    pure -> SetBranchAddress("ipid", &pid);
    pure -> SetBranchAddress("time", &ttime);
    pure -> SetBranchAddress("rho", &rho);

    auto numEntries = pure -> GetEntries();
    for (auto iEntry = 0; iEntry < numEntries; iEntry++) {
      pure -> GetEntry(iEntry);

      if (pid != 1)
        continue;

      ttime /= 20;
      rho = rho/rho0/1E4;

      auto index = Int_t(ttime/binTime);
      reducedRho[index] = max(reducedRho[index], Double_t(rho));
    }

    gRho[iFile] = new TGraph();
    for (auto iBin = 0; iBin < numBins; iBin++)
      gRho[iFile] -> SetPoint(iBin, tPoints[iBin], reducedRho[iBin]);

    cvs -> cd();

    gRho[iFile] -> GetXaxis() -> SetTitle("time (fm/c)");
    gRho[iFile] -> GetXaxis() -> CenterTitle();
    gRho[iFile] -> GetYaxis() -> SetTitle("#rho_{max}/#rho_{0} at the last interaction");
    gRho[iFile] -> GetYaxis() -> CenterTitle();
    gRho[iFile] -> SetLineColor(color[iFile]);
    gRho[iFile] -> Draw((iFile == 0 ? "AC" : "Csame"));
    gRho[iFile] -> GetXaxis() -> SetRangeUser(0, 200);
    gRho[iFile] -> GetYaxis() -> SetRangeUser(0, 2.3);

    legend -> AddEntry(gRho[iFile], Form("#gamma=%.2f", gammas[iFile]), "L");
  }

  cvs -> cd();
  legend -> SetMargin(legend -> GetMargin() + 0.07);
  legend -> Draw();

  auto text = new TLatex();
  text -> DrawLatex(60, 2.1, Form("^{%d}%s+^{%d}%s", beamA[0], beamSym.Data(), targetA[0], targetSym.Data()));
  text -> DrawLatex(60, 1.9, Form("%d AMeV", Int_t(beamKE[0])));
  text -> DrawLatex(60, 1.7, Form("b = %d fm", bs[0]));
  text -> DrawLatex(60, 1.5, "Only with protons");
}
