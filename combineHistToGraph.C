void combineHistToGraph(TString lowerFilename, TString higherFilename) {
  gStyle -> SetOptStat(0);
  gStyle -> SetPadRightMargin(0.07);
  gStyle -> SetPadTopMargin(0.08);
  gStyle -> SetPadBottomMargin(0.17);
  gStyle -> SetPadLeftMargin(0.13);
  gStyle -> SetTitleOffset(1.2, "x");
  gStyle -> SetTitleOffset(0.85, "y");
  gStyle -> SetTitleSize(0.06, "x");
  gStyle -> SetTitleSize(0.06, "y");
  gStyle -> SetLabelSize(0.06, "x");
  gStyle -> SetLabelSize(0.06, "y");
  gStyle -> SetTextFont(42);
  gStyle -> SetLegendTextSize(0.06);
  gStyle -> SetLegendBorderSize(0);
  TGaxis::SetMaxDigits(3);

  auto getGraph = [](TString filename, TString cvsName, TString graphName) {
    auto file = new TFile(filename);
    auto cvs = (TCanvas *) file -> Get(cvsName);
    auto hist = (TH1D *) cvs -> GetListOfPrimitives() -> At(0);
    hist -> SetName(graphName + "Hist");

    auto numBins = hist -> GetNbinsX();

//    auto graph = new TGraphErrors(hist);
    auto graph = new TGraphErrors();
    graph -> SetName(graphName + "Graph");
    graph -> GetXaxis() -> SetTitle(hist -> GetXaxis() -> GetTitle());
    graph -> GetXaxis() -> CenterTitle();
    graph -> GetYaxis() -> SetTitle(hist -> GetYaxis() -> GetTitle());
    graph -> GetYaxis() -> CenterTitle();

    for (auto iBin = 0; iBin < numBins; iBin++) {
      auto binCenter = hist -> GetBinCenter(iBin + 1);
      auto binValue = hist -> GetBinContent(iBin + 1);
      auto binError = hist -> GetBinError(iBin + 1);

      graph -> SetPoint(iBin, binCenter, binValue);
//      graph -> SetPointError(iBin, 0, binError);
    }

    cvs -> Draw("goff");

//    delete file;

    return graph;
  };

  vector<TString> fileList;
  fileList.push_back(lowerFilename);
  fileList.push_back(higherFilename);

  auto beamA = new Int_t[2];     // AMU
  auto targetA = new Int_t[2];   // AMU
  auto beamKE = new Double_t[2];  // MeV/A
  auto gammas = new Double_t[2];
  auto bs = new Int_t[2];     // fm
  auto beamSym = new TString[2];
  auto targetSym = new TString[2];

  for (Int_t iFile = 0; iFile < 2; iFile++) {
    auto array = fileList[iFile].Tokenize("_");

    auto proj = ((TObjString *) array -> At(0)) -> String();
    beamSym[iFile] = TString(proj(0, 1));
    beamSym[iFile].ToUpper();
    if (!beamSym[iFile].EqualTo("U")) {
      beamSym[iFile] += TString(proj(1, 1));
      beamA[iFile] = TString(proj(2, proj.Length() - 2)).Atoi();
    } else
      beamA[iFile] = TString(proj(1, proj.Length() - 1)).Atoi();

    auto targ = ((TObjString *) array -> At(1)) -> String();
    targetSym[iFile] = TString(targ(0, 1));
    targetSym[iFile].ToUpper();
    if (!targetSym[iFile].EqualTo("U")) {
      targetSym[iFile] += TString(targ(1, 1));
      targetA[iFile] = TString(targ(2, targ.Length() - 2)).Atoi();
    } else
      targetA[iFile] = TString(proj(1, proj.Length() - 1)).Atoi();

    auto energy = ((TObjString *) array -> At(2)) -> String();
    beamKE[iFile] = TString(energy(6, energy.Length() - 6)).Atoi();

    auto gamma = ((TObjString *) array -> At(3)) -> String();
    gammas[iFile] = TString(gamma(5, gamma.Length() - 5)).Atof();

    auto b = ((TObjString *) array -> At(4)) -> String();
    bs[iFile] = TString(b(1, b.Length() - 1)).Atoi();
  }

  auto lowerGraph = getGraph(lowerFilename, "c5", "lower");
  lowerGraph -> SetLineWidth(4);
  lowerGraph -> SetLineColor(38);
  auto higherGraph = getGraph(higherFilename, "c5", "higher");
  higherGraph -> SetLineWidth(4);
  higherGraph -> SetLineColor(46);

  lowerGraph -> Draw("AC");
  higherGraph -> Draw("C same");

  auto legend = new TLegend(0.55, 0.55, 0.8, 0.7);
  legend -> SetMargin(0.4);
  legend -> SetFillStyle(0);
  legend -> SetFillStyle(0);
//  legend -> AddEntry(lowerGraph, "soft", "l");
//  legend -> AddEntry(higherGraph, "stiff", "l");
  legend -> AddEntry(lowerGraph, Form("#gamma=%.2f", gammas[0]), "l");
  legend -> AddEntry(higherGraph, Form("#gamma=%.2f", gammas[1]), "l");
  legend -> Draw();

  auto text = new TLatex();
  text -> SetNDC();
  text -> DrawLatex(0.55, 0.73, Form("b = %d fm", bs[0]));
  text -> DrawLatex(0.55, 0.81, Form("^{%d}%s+^{%d}%s @ %.0f AMeV", beamA[0], beamSym[0].Data(), targetA[0], targetSym[0].Data(), beamKE[0]));

  lowerGraph -> GetXaxis() -> SetRangeUser(0, 200);
  lowerGraph -> GetYaxis() -> SetRangeUser(1.5, 9.5);
}
