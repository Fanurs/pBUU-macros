#include "v1_v2.h"

class Calc
{
  public:
    Calc() {};
    ~Calc() {};

    void Reset() ///< Reset all the values.
    { fNumValues = 0; fMean = 0; fRms = 0; }

    void Add(Double_t value) ///< With the mean and RMS, calculated before, use the **value** to calculate new **mean** and **RMS** recursively.
    {
      fMean += (value - fMean)/(Double_t)(fNumValues + 1);

      if (fNumValues > 0)
        fRms = fNumValues*fRms/(Double_t)(fNumValues + 1) + pow(value - fMean, 2)/(Double_t)(fNumValues);

      fNumValues++;
    }
    Double_t GetMean() { return fMean; };             ///< Return the calculated **mean** value.
    Double_t GetRMS()  { return TMath::Sqrt(fRms); }; ///< Return the calculated **RMS** value.
    Double_t GetRMS2() { return fRms; };              ///< Return the calculated **RMS squared** value.

  private:
       Int_t fNumValues = 0; ///< Number of values added
    Double_t fMean = 0;      ///< mean value
    Double_t fRms = 0;       ///< RMS value
};

void v1_v2_Genie_kuan_12_yongjia_new(vector<TString> fileList, Int_t drawPID = 1) {
  gStyle -> SetOptStat(0);
  gStyle -> SetPadRightMargin(0.03);
  gStyle -> SetPadTopMargin(0.03);
  gStyle -> SetPadBottomMargin(0.15);
  gStyle -> SetPadLeftMargin(0.175);
  gStyle -> SetTitleOffset(1.2, "x");
  gStyle -> SetTitleOffset(1.6, "y");
  gStyle -> SetTitleSize(0.06, "x");
  gStyle -> SetTitleSize(0.06, "y");
  gStyle -> SetLabelSize(0.06, "x");
  gStyle -> SetLabelSize(0.06, "y");
  gStyle -> SetLegendTextSize(0.06);
  gStyle -> SetLegendBorderSize(0);
  gStyle -> SetPadGridX(kTRUE);
  gStyle -> SetPadGridY(kTRUE);

//  Int_t drawPID = 1;  // 1: Proton, 2: Neutron
  Int_t numFiles = fileList.size();
  Int_t color[5] = {46, 8, 38, 41, 49};

  auto legend1 = new TLegend(0.2, 0.95, 0.55, 0.6);
  legend1 -> SetFillStyle(0);
  auto legend2 = new TLegend(0.43, 0.55, 0.78, 0.2);
  legend2 -> SetFillStyle(0);

  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  TCanvas *c2 = new TCanvas("c2","c2",800,600);

  auto aMass = (0.5*(mp + mn) - 0.008)*gev2mev; // MeV

  auto beamA = new Int_t[numFiles];     // AMU
  auto targetA = new Int_t[numFiles];   // AMU
  auto beamKE = new Double_t[numFiles];  // MeV/A
  auto gammas = new Double_t[numFiles];
  auto bs = new Int_t[numFiles];     // fm

  auto gV1 = new TGraph*[numFiles];
  auto gV2 = new TGraph*[numFiles];

  auto numBins = 22;
  auto yPoints = new Double_t[numBins];

  auto hV1 = new Calc*[numBins];
  auto hV2 = new Calc*[numBins];
  for (Int_t iBin = 0; iBin < numBins; iBin++) {
    yPoints[iBin] = -1.05 + 0.1*iBin;
    hV1[iBin] = new Calc();
    hV2[iBin] = new Calc();
  }

  auto files = new TFile*[numFiles];
  for (Int_t iFile = 0; iFile < numFiles; iFile++) {
    for (Int_t iBin = 0; iBin < numBins; iBin++) {
      hV1[iBin] -> Reset();
      hV2[iBin] -> Reset();
    }

    auto array = fileList[iFile].Tokenize("_");

    auto proj = ((TObjString *) array -> At(0)) -> String();
    beamA[iFile] = TString(proj(2, proj.Length() - 2)).Atoi();

    auto targ = ((TObjString *) array -> At(1)) -> String();
    targetA[iFile] = TString(targ(2, targ.Length() - 2)).Atoi();

    auto energy = ((TObjString *) array -> At(2)) -> String();
    beamKE[iFile] = TString(energy(6, energy.Length() - 6)).Atoi();

    auto gamma = ((TObjString *) array -> At(3)) -> String();
    gammas[iFile] = TString(gamma(5, gamma.Length() - 5)).Atof();

    auto b = ((TObjString *) array -> At(4)) -> String();
    bs[iFile] = TString(b(1, b.Length() - 1)).Atoi();

    files[iFile] = new TFile(fileList[iFile]);
    if (!files[iFile] -> IsOpen()) {
      cout << "Error: cannot open root file" << endl;

      return;
    }
    cout << "File opened: " << fileList[iFile] << endl;

    auto tree = (TTree *) files[iFile] -> Get("pure");
    tree -> SetBranchAddress("ipid", &ipid, &b_ipid);
    tree -> SetBranchAddress("ievent", &ievent, &b_ievent);
    tree -> SetBranchAddress("px", &px, &b_px);
    tree -> SetBranchAddress("py", &py, &b_py);
    tree -> SetBranchAddress("pz_lab", &pz_lab, &b_pz_lab);

    auto numEntries = tree -> GetEntries();

    // beam info LAB
    auto beamEnergy = beamKE[iFile] + aMass; // (T + U)/A beam [MeV/A]
    auto beamMomentum = TMath::Sqrt(beamEnergy*beamEnergy - aMass*aMass);  // MeV/c/A
    auto beamRapidity = TMath::Log((beamEnergy + beamMomentum)/aMass);

    // the rapidity of beam in lab
    auto beamEnergyOfCM = beamEnergy*beamA[iFile] + aMass*targetA[iFile]; // MeV
    auto beamMomentumOfCM = beamMomentum*beamA[iFile]; // MeV/c
    auto rapidityOfCM = 0.5*log((beamEnergyOfCM + beamMomentumOfCM)/(beamEnergyOfCM - beamMomentumOfCM));  //the rapidity of CM in lab
    /*
    cout << "yp = " << rapidityOfCM <<endl;
    cout << "beamRapidity = " << beamRapidity <<endl;
    cout << "beamRapidity - yp = " << beamRapidity - rapidityOfCM <<endl;
    */

    auto beta = -(beamMomentumOfCM/beamEnergyOfCM); //add by Kuan on 2016/07/14
    TVector3 vBeta(0. ,0. , beta);
    /*
    cout << "******* Beta of the system *******" << endl;
    cout << "Beta = "<< beta << endl;
    cout << "**********************************" << endl;
    */

    TLorentzVector fourVec;

    for (Int_t iEntry = 0; iEntry < numEntries; iEntry++) {
      tree -> GetEntry(iEntry);

      auto pt2 = px*px + py*py;
      if (pt2 == 0)
        continue;

      Double_t imass = 0;

      switch (ipid) {
        case    1: imass = mp;       break;
        case    2: imass = mn;       break;
        case    3: imass = md;       break;
        case    4: imass = mHe;      break;
        case    5: imass = mt;       break;
        case    6: imass = mHe4;     break;
        case    7:
        case    8:
        case    9:
        case   10: imass = mdelta;   break;
        case   11:
        case   12: imass = mbaryres; break;
        case   13:
        case   15: imass = mpi;      break;
        case   14: imass = mpi0;     break;
        case   16: imass = 0;        break;
        case -100: imass = -100;     break; // end of run output along with px=py=pz=-100
        case  999: imass = -1000;    break; // reserved for blank line
          default: cout << "Pid: " << ipid << endl; continue;
      }
      imass *= gev2mev;

      auto energyLab = TMath::Sqrt((pt2 + pz_lab*pz_lab) + imass*imass);

      fourVec.SetPxPyPzE(px, py, pz_lab, energyLab);
      fourVec.Boost(vBeta);

      auto v1 = px/TMath::Sqrt(pt2);
      auto v2 = (px*px - py*py)/pt2;

      Double_t pxCM = fourVec.Px();
      Double_t pyCM = fourVec.Py();
      Double_t pzCM = fourVec.Pz();
      Double_t energyCM = fourVec.E();
      Double_t yCM = fourVec.Rapidity();

      Double_t y0 = yCM/(beamRapidity - rapidityOfCM);    //changed the definition(y_CM/yp) with Jun by kuan at 04/19/2016 and changed back to the original definition with Jun by Kuan at 04/29/2016
      Double_t y1 = (yCM - rapidityOfCM)/beamRapidity;    //y1=ylab/yproj

      if (ipid == drawPID) {
        Int_t index = Int_t((y0 + 1.1)*10);

        if (index < 0 || index > 21)
          continue;

        hV1[index] -> Add(v1);
        hV2[index] -> Add(v2);
      }
    }

    c1 -> cd();
    gV1[iFile] = new TGraph();
    c2 -> cd();
    gV2[iFile] = new TGraph();
    for (auto iBin = 0; iBin < numBins; iBin++) {
      if (hV1[iBin] -> GetMean() != 0)
        gV1[iFile] -> SetPoint(iBin, yPoints[iBin], hV1[iBin] -> GetMean());

      if (hV2[iBin] -> GetMean() != 0)
        gV2[iFile] -> SetPoint(iBin, yPoints[iBin], hV2[iBin] -> GetMean());

//      cout << "v1: " << iBin << " " << yPoints[iBin] << " " << hV1[iBin] -> GetMean() << endl;
//      cout << "v2: " << iBin << " " << yPoints[iBin] << " " << hV2[iBin] -> GetMean() << endl;
    }
    for (auto iBin = 0; iBin < numBins; iBin++) {
      cout << "v1: " << iBin << " " << yPoints[iBin] << " " << hV1[iBin] -> GetMean() << endl;
    }
    cout << endl;
    for (auto iBin = 0; iBin < numBins; iBin++) {
      cout << "v2: " << iBin << " " << yPoints[iBin] << " " << hV2[iBin] -> GetMean() << endl;
    }

    c1 -> cd();
    gV1[iFile] -> GetXaxis() -> SetTitle("y_{CM}/y_{beam,CM}");
    gV1[iFile] -> GetXaxis() -> CenterTitle();
    gV1[iFile] -> GetYaxis() -> SetTitle("v_{1}");
    gV1[iFile] -> GetYaxis() -> CenterTitle();
    gV1[iFile] -> SetLineColor(color[iFile]);
    gV1[iFile] -> Draw((iFile == 0 ? "AC" : "Csame"));
    gV1[iFile] -> GetYaxis() -> SetRangeUser(-0.15, 0.15);

    legend1 -> AddEntry(gV1[iFile], Form("#gamma=%.2f", gammas[iFile]), "L");

    c2 -> cd();
    gV2[iFile] -> GetXaxis() -> SetTitle("y_{CM}/y_{beam,CM}");
    gV2[iFile] -> GetXaxis() -> CenterTitle();
    gV2[iFile] -> GetYaxis() -> SetTitle("v_{2}");
    gV2[iFile] -> GetYaxis() -> CenterTitle();
    gV2[iFile] -> SetLineColor(color[iFile]);
    gV2[iFile] -> Draw((iFile == 0 ? "AC" : "Csame"));
    gV2[iFile] -> GetYaxis() -> SetRangeUser(-0.03, 0.);

    legend2 -> AddEntry(gV2[iFile], Form("#gamma=%.2f", gammas[iFile]), "L");
  }

  c1 -> cd();
  legend1 -> Draw();

  c2 -> cd();
  legend2 -> Draw();

  c1 -> SaveAs(Form("%d%d-v1-b%d-%s.root", beamA[0], targetA[0], bs[0], (drawPID == 1 ? "proton" : "neutron")));
  c2 -> SaveAs(Form("%d%d-v2-b%d-%s.root", beamA[0], targetA[0], bs[0], (drawPID == 1 ? "proton" : "neutron")));
}
