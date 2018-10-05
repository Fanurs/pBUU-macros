void drawYield() {
  gStyle -> SetOptStat(0);
  gStyle -> SetPalette(55);
  gStyle -> SetPadRightMargin(0.05);
  gStyle -> SetPadTopMargin(0.05);
  gStyle -> SetPadBottomMargin(0.15);
  gStyle -> SetPadLeftMargin(0.15);
  gStyle -> SetTitleOffset(1.15, "x");
  gStyle -> SetTitleOffset(1.20, "y");
  gStyle -> SetTitleSize(0.06, "x");
  gStyle -> SetTitleSize(0.06, "y");
  gStyle -> SetTitleSize(0.06, "z");
  gStyle -> SetLabelSize(0.06, "x");
  gStyle -> SetLabelSize(0.06, "y");
  gStyle -> SetLabelSize(0.06, "z");
  gStyle -> SetLegendTextSize(0.06);

  auto timeStep = 3.;
  auto maxTime = 165.;
  auto numTStep = Int_t(maxTime/timeStep) + 1;

  auto vpim = new vector<Double_t>[numTStep];
  auto vpiz = new vector<Double_t>[numTStep];
  auto vpip = new vector<Double_t>[numTStep];
  auto vdm = new vector<Double_t>[numTStep];
  auto vdz = new vector<Double_t>[numTStep];
  auto vdp = new vector<Double_t>[numTStep];
  auto vdpp = new vector<Double_t>[numTStep];

  Double_t tim, dpp, dp, dz, dm, pim, piz, pip;
  auto time = new Double_t[numTStep];
  auto pdpp = new Double_t[numTStep];
  auto pdp = new Double_t[numTStep];
  auto pdz = new Double_t[numTStep];
  auto pdm =  new Double_t[numTStep];
  auto ppim = new Double_t[numTStep];
  auto ppiz = new Double_t[numTStep];
  auto ppip = new Double_t[numTStep];
  Int_t pNumZero = 0;
  auto ppiratio = new Double_t[numTStep];

  ifstream openFile;
  for (auto iFile = 0; iFile < 3000; iFile++) {
    openFile.open(Form("%03d/piDeltaYield", iFile));
    
    while (true) {
      openFile >> tim >> dpp >> dp >> dz >> dm >> pip >> piz >> pim;

      if (openFile.eof())
        break;

      vdm[Int_t(tim/timeStep)].push_back(dm);
      vdz[Int_t(tim/timeStep)].push_back(dz);
      vdp[Int_t(tim/timeStep)].push_back(dp);
      vdpp[Int_t(tim/timeStep)].push_back(dpp);
      vpim[Int_t(tim/timeStep)].push_back(pim);
      vpiz[Int_t(tim/timeStep)].push_back(piz);
      vpip[Int_t(tim/timeStep)].push_back(pip);
    }

    openFile.close();
  }

  for (auto iStep = 0; iStep < numTStep; iStep++) {
    time[iStep] = timeStep*iStep;

    pdm[iStep] = TMath::Mean(vdm[iStep].begin(), vdm[iStep].end());
    pdz[iStep] = TMath::Mean(vdz[iStep].begin(), vdz[iStep].end());
    pdp[iStep] = TMath::Mean(vdp[iStep].begin(), vdp[iStep].end());
    pdpp[iStep] = TMath::Mean(vdpp[iStep].begin(), vdpp[iStep].end());
    ppim[iStep] = TMath::Mean(vpim[iStep].begin(), vpim[iStep].end());
    ppiz[iStep] = TMath::Mean(vpiz[iStep].begin(), vpiz[iStep].end());
    ppip[iStep] = TMath::Mean(vpip[iStep].begin(), vpip[iStep].end());
    ppiratio[iStep] = ppim[iStep]/ppip[iStep];
    if (std::isnan(ppiratio[iStep]) || std::isinf(ppiratio[iStep])) {
      ppiratio[iStep] = 0;
      pNumZero++;
    }
  }

/*
  auto c1 = new TCanvas("c1", "", 1400, 500);
  c1 -> Divide(3, 1);

  c1 -> cd(1);
  hpim -> Draw("colz");
  c1 -> cd(2);
  hpiz -> Draw("colz");
  c1 -> cd(3);
  hpip -> Draw("colz");
*/
  
  auto c2 = new TCanvas("c2", "", 800, 600);
  c2 -> SetGrid();

  auto gpim = new TGraph(numTStep, time, ppim);
  gpim -> SetTitle(0);
  gpim -> GetXaxis() -> SetTitle("time (fm/c)");
  gpim -> GetXaxis() -> CenterTitle();
  gpim -> GetXaxis() -> SetRangeUser(0, 90);
  gpim -> GetYaxis() -> SetTitle("Yield");
  gpim -> GetYaxis() -> CenterTitle();
  gpim -> SetLineColor(4);
  gpim -> SetLineWidth(2);
  gpim -> Draw("AL");

  auto gpiz = new TGraph(numTStep, time, ppiz);
  gpiz -> SetLineColor(1);
  gpiz -> SetLineWidth(2);
  gpiz -> Draw("L same");

  auto gpip = new TGraph(numTStep, time, ppip);
  gpip -> SetLineColor(2);
  gpip -> SetLineWidth(2);
  gpip -> Draw("L same");

  auto gpiratio = new TGraph(numTStep - pNumZero, time + pNumZero, ppiratio + pNumZero);
  gpiratio -> Draw("P same");

  auto legend = new TLegend(0.5, 0.5, 0.7, 0.7);
  legend -> AddEntry(gpim, "#pi^{-}", "l");
  legend -> AddEntry(gpiz, "#pi^{0}", "l");
  legend -> AddEntry(gpip, "#pi^{+}", "l");
  legend -> SetMargin(0.35);
  legend -> SetFillStyle(0);
  legend -> SetLineWidth(0);
  legend -> Draw("same");

  auto gdm = new TGraph(numTStep, time, pdm);
  gdm -> SetLineColor(6);
  gdm -> SetLineWidth(2);
  gdm -> SetLineStyle(9);
  gdm -> Draw("C same");

  auto gdz = new TGraph(numTStep, time, pdz);
  gdz -> SetLineColor(7);
  gdz -> SetLineWidth(2);
  gdz -> SetLineStyle(9);
  gdz -> Draw("C same");

  auto gdp = new TGraph(numTStep, time, pdp);
  gdp -> SetLineColor(8);
  gdp -> SetLineWidth(2);
  gdp -> SetLineStyle(9);
  gdp -> Draw("C same");

  auto gdpp = new TGraph(numTStep, time, pdpp);
  gdpp -> SetLineColor(38);
  gdpp -> SetLineWidth(2);
  gdpp -> SetLineStyle(9);
  gdpp -> Draw("C same");

  auto legend2 = new TLegend(0.5, 0.5, 0.7, 0.7);
  legend2 -> AddEntry(gdm, "#Delta^{-}", "l");
  legend2 -> AddEntry(gdz, "#Delta^{0}", "l");
  legend2 -> AddEntry(gdp, "#Delta^{+}", "l");
  legend2 -> AddEntry(gdpp, "#Delta^{++}", "l");
  legend2 -> SetMargin(0.35);
  legend2 -> SetFillStyle(0);
  legend2 -> SetLineWidth(0);
  legend2 -> Draw("same");
}
