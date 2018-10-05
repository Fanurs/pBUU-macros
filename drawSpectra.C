void drawSpectra(TString filename, Double_t totalStat = 900000.) {
  gStyle -> SetOptStat(0);
  gStyle -> SetPadRightMargin(0.07);
  gStyle -> SetPadTopMargin(0.08);
  gStyle -> SetPadBottomMargin(0.17);
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
  TGaxis::SetMaxDigits(3);
//  gStyle -> SetPadGridX(kTRUE);
//  gStyle -> SetPadGridY(kTRUE);

  auto pRangeMin = 0.;
  auto pRangeMax = 600.;
  auto pNumBin = 30;
  auto pBinSize = (pRangeMax - pRangeMin)/pNumBin;

  auto keRangeMin = 0.;
  auto keRangeMax = 1200.;
  auto keNumBin = 24;
  auto keBinSize = (keRangeMax - keRangeMin)/keNumBin;

  auto keHRangeMin = 0.;
  auto keHRangeMax = 1200.;
  auto keHNumBin = 24;
  auto keHBinSize = (keHRangeMax - keHRangeMin)/keHNumBin;

  auto pure = new TChain("pure");
  pure -> AddFile(filename);

  auto derived = new TChain("derived");
  derived -> AddFile(filename);

  pure -> AddFriend(derived);

  pure -> AddFile(filename.ReplaceAll("nopions", "pion"));
  derived -> AddFile(filename);
  cout << "Added to chain: " << filename << endl;

  Short_t eventid;
  Int_t pid;
  Float_t px, py, pzLab, kenergy_c, rap, thetaC;
  pure -> SetBranchAddress("ievent", &eventid);
  pure -> SetBranchAddress("ipid", &pid);
  pure -> SetBranchAddress("px", &px);
  pure -> SetBranchAddress("py", &py);
  pure -> SetBranchAddress("pz_lab", &pzLab);

  derived -> SetBranchAddress("kenergy_c", &kenergy_c);
  derived -> SetBranchAddress("thetaC", &thetaC);
  derived -> SetBranchAddress("rap", &rap);

  pure -> GetEntry(0);
  Int_t oldeventid = eventid;

  auto hpim = new TH1D("hpim", ";#it{p}_{LAB} (MeV/c);dM/d#it{p} (MeV^{-1}c)", pNumBin, pRangeMin, pRangeMax);
  auto hpip = new TH1D("hpip", ";#it{p}_{LAB} (MeV/c);dM/d#it{p} (MeV^{-1}c)", pNumBin, pRangeMin, pRangeMax);

  hpim -> GetXaxis() -> CenterTitle();
  hpim -> GetYaxis() -> CenterTitle();

  hpim -> Sumw2();
  hpip -> Sumw2();

  auto hpimKE = new TH1D("hpimKE", ";E_{k,CM} (MeV);dM/dE_{k} (MeV^{-1})", keNumBin, keRangeMin, keRangeMax);
  auto hpipKE = new TH1D("hpipKE", ";E_{k,CM} (MeV);dM/dE_{k} (MeV^{-1})", keNumBin, keRangeMin, keRangeMax);

  hpimKE -> GetXaxis() -> CenterTitle();
  hpimKE -> GetYaxis() -> CenterTitle();

  hpimKE -> Sumw2();
  hpipKE -> Sumw2();

  auto hpimKEMid = new TH1D("hpimKEMid", ";E_{k,CM} (MeV);dM/dE_{k}/d#Omega (MeV^{-1})", keNumBin, keRangeMin, keRangeMax);
  auto hpipKEMid = new TH1D("hpipKEMid", ";E_{k,CM} (MeV);dM/dE_{k}/d#Omega (MeV^{-1})", keNumBin, keRangeMin, keRangeMax);

  hpimKEMid -> GetXaxis() -> CenterTitle();
  hpimKEMid -> GetYaxis() -> CenterTitle();

  hpimKEMid -> Sumw2();
  hpipKEMid -> Sumw2();

  auto hnKE = new TH1D("hnKE", ";E_{k,CM} (MeV);dM/dE_{k} (MeV^{-1})", keHNumBin, keHRangeMin, keHRangeMax);
  auto hpKE = new TH1D("hpKE", ";E_{k,CM} (MeV);dM/dE_{k} (MeV^{-1})", keHNumBin, keHRangeMin, keHRangeMax);

  hnKE -> GetXaxis() -> CenterTitle();
  hnKE -> GetYaxis() -> CenterTitle();

  hnKE -> Sumw2();
  hpKE -> Sumw2();

  auto hnKEMid = new TH1D("hnKEMid", ";E_{k,CM} (MeV);dM/dE_{k}/d#Omega (MeV^{-1})", keHNumBin, keHRangeMin, keHRangeMax);
  auto hpKEMid = new TH1D("hpKEMid", ";E_{k,CM} (MeV);dM/dE_{k}/d#Omega (MeV^{-1})", keHNumBin, keHRangeMin, keHRangeMax);

  hnKEMid -> GetXaxis() -> CenterTitle();
  hnKEMid -> GetYaxis() -> CenterTitle();

  hnKEMid -> Sumw2();
  hpKEMid -> Sumw2();

  auto htKE = new TH1D("htKE", ";E_{k,CM} (MeV);dM/dE_{k} (MeV^{-1})", keHNumBin, keHRangeMin, keHRangeMax);
  auto hHe3KE = new TH1D("hHe3KE", ";E_{k,CM} (MeV);dM/dE_{k} (MeV^{-1})", keHNumBin, keHRangeMin, keHRangeMax);

  htKE -> GetXaxis() -> CenterTitle();
  htKE -> GetYaxis() -> CenterTitle();

  htKE -> Sumw2();
  hHe3KE -> Sumw2();

  auto htKEMid = new TH1D("htKEMid", ";E_{k,CM} (MeV);dM/dE_{k}/d#Omega (MeV^{-1})", keHNumBin, keHRangeMin, keHRangeMax);
  auto hHe3KEMid = new TH1D("hHe3KEMid", ";E_{k,CM} (MeV);dM/dE_{k}/d#Omega (MeV^{-1})", keHNumBin, keHRangeMin, keHRangeMax);

  htKEMid -> GetXaxis() -> CenterTitle();
  htKEMid -> GetYaxis() -> CenterTitle();

  htKEMid -> Sumw2();
  hHe3KEMid -> Sumw2();

  TVector3 mom;

  Double_t thetaCMin = 60;
  Double_t thetaCMax = 120;

  auto numEntries = pure -> GetEntries();
  for (auto iEntry = 0; iEntry < numEntries; iEntry++) {
    pure -> GetEntry(iEntry);

    if (oldeventid != eventid) {
      oldeventid = eventid;

      // something to do with numPartices;

    }

    switch (pid) {
      case 1:
        hpKE -> Fill(kenergy_c);
        if (thetaC >= thetaCMin && thetaC <= thetaCMax)
          hpKEMid -> Fill(kenergy_c);
        break;

      case 2:
        hnKE -> Fill(kenergy_c);
        if (thetaC >= thetaCMin && thetaC <= thetaCMax)
          hnKEMid -> Fill(kenergy_c);
        break;

      case 4:
        hHe3KE -> Fill(kenergy_c);
        if (thetaC >= thetaCMin && thetaC <= thetaCMax)
          hHe3KEMid -> Fill(kenergy_c);
        break;

      case 5:
        htKE -> Fill(kenergy_c);
        if (thetaC >= thetaCMin && thetaC <= thetaCMax)
          htKEMid -> Fill(kenergy_c);
        break;

      case 13:
        mom.SetXYZ(px, py, pzLab);

        hpip -> Fill(mom.Mag());
        hpipKE -> Fill(kenergy_c);
        if (thetaC >= thetaCMin && thetaC <= thetaCMax)
          hpipKEMid -> Fill(kenergy_c);
        break;

      case 15:
        mom.SetXYZ(px, py, pzLab);

        hpim -> Fill(mom.Mag());
        hpimKE -> Fill(kenergy_c);
        if (thetaC >= thetaCMin && thetaC <= thetaCMax)
          hpimKEMid -> Fill(kenergy_c);
        break;

      default:
        break;
    }
  }

//  cout << "pi- multiplicity/collision = " << hpim -> GetEntries()/totalStat << endl;
//  cout << "pi+ multiplicity/collision = " << hpip -> GetEntries()/totalStat << endl;

  hpim -> Scale(1./totalStat/pBinSize);
  hpip -> Scale(1./totalStat/pBinSize);

  hpim -> Sumw2(kFALSE);
  hpip -> Sumw2(kFALSE);

  auto hpiRatioKE = (TH1D *) hpimKE -> Clone("hpiRatioKE");
  hpiRatioKE -> GetYaxis() -> SetTitle("Y(#pi^{-})/Y(#pi^{+})");
  hpiRatioKE -> Divide(hpipKE);

  hpimKE -> Scale(1./totalStat/keBinSize);
  hpipKE -> Scale(1./totalStat/keBinSize);

  hpimKE -> Sumw2(kFALSE);
  hpipKE -> Sumw2(kFALSE);

  auto hnpRatioKE = (TH1D *) hnKE -> Clone("hnpRatioKE");
  hnpRatioKE -> GetYaxis() -> SetTitle("Y(n)/Y(p)");
  hnpRatioKE -> Divide(hpKE);

  auto hnpRatioKEMid = (TH1D *) hnKEMid -> Clone("hnpRatioKEMid");
  hnpRatioKEMid -> GetYaxis() -> SetTitle("Y(n)/Y(p)");
  hnpRatioKEMid -> Divide(hpKEMid);

  auto htHe3RatioKE = (TH1D *) htKE -> Clone("htHe3RatioKE");
  htHe3RatioKE -> GetYaxis() -> SetTitle("Y(t)/Y(^{3}He)");
  htHe3RatioKE -> Divide(hHe3KE);

  auto c1 = new TCanvas("c1");
  hpim -> Draw();
  hpip -> SetLineColor(2);
  hpip -> Draw("same");

  auto legend = new TLegend(0.6, 0.6, 0.9, 0.8);
  legend -> AddEntry(hpim, "#pi^{-}", "l");
  legend -> AddEntry(hpip, "#pi^{+}", "l");
  legend -> Draw();

  c1 -> SetWindowPosition(0, 0);

  auto c2 = new TCanvas("c2");
  hpimKE -> Draw();
  hpipKE -> SetLineColor(2);
  hpipKE -> Draw("same");

  legend -> Draw("same");

  c2 -> SetWindowPosition(0, 0);

  auto c3 = new TCanvas("c3");
  hpiRatioKE -> Draw();

  c3 -> SetWindowPosition(0, 0);

  auto c4 = new TCanvas("c4");
  hnpRatioKE -> Draw();

  c4 -> SetWindowPosition(0, 0);

  auto c5 = new TCanvas("c5");
  hnpRatioKEMid -> Draw();

  c5 -> SetWindowPosition(0, 0);

  auto c6 = new TCanvas("c6");
  htHe3RatioKE -> Draw();

  c6 -> SetWindowPosition(0, 0);

  auto angleNorm = 2*TMath::Pi()*(-TMath::Cos(thetaCMax*TMath::DegToRad()) + TMath::Cos(thetaCMin*Tmath::DegTORad()));
  hpimKEMid -> Scale(1./totalStat/keBinSize/angleNorm);
  hpipKEMid -> Scale(1./totalStat/keBinSize/angleNorm);

  hpimKEMid -> Sumw2(kFALSE);
  hpipKEMid -> Sumw2(kFALSE);

  hpKEMid -> Scale(1./totalStat/keBinSize/angleNorm);
  hnKEMid -> Scale(1./totalStat/keBinSize/angleNorm);

  hpKEMid -> Sumw2(kFALSE);
  hnKEMid -> Sumw2(kFALSE);

  htKEMid -> Scale(1./totalStat/keBinSize/angleNorm);
  hHe3KEMid -> Scale(1./totalStat/keBinSize/angleNorm);

  htKEMid -> Sumw2(kFALSE);
  hHe3KEMid -> Sumw2(kFALSE);

  auto c7 = new TCanvas("c7");
  c7 -> SetLogy();
  hnKEMid -> Draw();
  hpKEMid -> Draw("same");
  htKEMid -> Draw("same");
  hHe3KEMid -> Draw("same");
  hpimKEMid -> Draw("same");
  hpipKEMid -> Draw("same");

  c7 -> SetWindowPosition(0, 0);

  auto writeFile = new TFile(filename.ReplaceAll("pion", "hist"), "recreate");
  c1 -> Write();
  c2 -> Write();
  c3 -> Write();
  c4 -> Write();
  c5 -> Write();
  c6 -> Write();
  c7 -> Write();
}
