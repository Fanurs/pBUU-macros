//======================
// Definitions from BUU
//======================
//     px_lab, py_lab, pz_lab = laboratory momentum of emitted particle in MeV/c

//===============================
//     PID Definitions (IDIC)
//===============================
//     0 - EMPTY
//     1 - PROTON
//     2 - NEUTRON
//     3 - DEUTERON (IF NEGATIVE - WENT OUT OF THE COMPUTATIONAL AREA)
//     4 - HELIUM 3
//     5 - TRITON
//     6 - HELIUM 4
//     7 - DELTA ++
//     8 - DELTA +
//     9 - DELTA 0
//     10 - DELTA -
//     11 - N* +
//     12 - N* 0
//     13 - PI +
//     14 - PI 0
//     15 - PI -
//     16 - GAMMA

//gROOT->Reset();

#include <TVector3.h>
#include <TMath.h>
#include <Riostream.h>
#include <TLorentzVector.h>
#include <TRotation.h>
#include <TLorentzRotation.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH3D.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TFile.h>
#include <fstream>
#include <TString.h>
// reading an entire binary file
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <cstring>
#include <sstream>
using namespace std;

int pBUUDAT2Root_withcalc_stable_kuan(string folder = "")
{
    
    //=============
    // Definitions
    //=============
    
    // Constants
    
    Double_t mp = .938272013;
    Double_t mn = .939565346;
    Double_t md = 2.013553212;
    Double_t mHe= 2.808391383;
    Double_t mHe4 = 3.7272;
    Double_t mt= 2.808920906;
    Double_t mbaryres=1.440;
    Double_t mdelta = 1.232;
    Double_t  mpi = 0.139570;
    Double_t mpi0 = 0.134976;
    Double_t gev2mev = 1000.;
    Double_t amu2mev = 931.494028;
    Bool_t flag;
    
    FILE *inputfile;
    int header;
    string prev_datfile;
    string temp_string;
    stringstream fullpath;
    ifstream input;
    
    // filename parameters
    //string listname="LISN"+folder; // 10 files
    string listname="listDAT"; // 10 files
    //  string listname="LISN_500_"+folder; // 500 files
    //  string listname="LISN_all_"+folder; // 1479 files
    string extension = ".DAT";
    string pathheader = "./";
    
    // variables for "pure" tree
    Int_t ipid = -999;
    short int ievent = 0;
    short int nEmpty = 1;
    Float_t px = 0;
    Float_t py = 0;
    Float_t pz_lab = 0;
    short int  px_temp=0,py_temp=0,pz_lab_temp=0,ipid_temp=0;
    
    // variables for "derived" tree
    Int_t ipid_der = -999;
    Int_t  ipart = -999;
    Int_t nEntries = 0;
    Double_t imass = -999;
    Float_t pxCMS = 0;
    Float_t pyCMS = 0;
    Float_t pzCMS = 0;
    Float_t kenergy_c = 0;
    Float_t kenergy_l = 0;
    Float_t thetaC = 0;
    Float_t phiC = 0;
    Float_t thetaL = 0;
    Float_t phiL = 0;
    Float_t energy_c = 0;
    Float_t energy_l = 0;
    Float_t rap = 0;
    Float_t pt = 0; // derived tree variables
    double part[17] = {};//total number of read in particles
    
    //=====================================
    //Read parameters from folder name
    //=====================================
    
    // This block integrates old code to new labeling scheme.
    // Parameter array elements defined as
    // {A of projectile, A of target, energy in [GeV], gamma , impact parameter in [fm]}
    
    float param[5]={}; //parameter array
    string pname[5]={"au","_au","_energy","_gamma","_b"};
    int pos=0;
    int pos2=0;
    
    // Grab parameter values from file name and store in parameter array
    cout<<"Read in parameters...."<<endl;
    for(int i = 0; i < 5; i++){
        // string fname=folder;
        pos = folder.find(pname[i]);
        if(i == 4) pos2 = 0; // reads last parameter in right
        else pos2 = folder.find(pname[i+1]);
        // error for bad folder name
        if(pos2 == -1 || pos == -1){
            cout << "Bad folder name" << endl;
            return -3;
        }
        
        string temp;
        // Read from position 1 to position 2.
        // Dont forget to include length of string you found.
        temp = folder.substr(pos+pname[i].length(),pos2-pos-pname[i].length());
        param[i] = atof(temp.c_str()); // convert string to double
    }
    
    //enter beam info from parameters.
    Int_t beamA = param[0]; // [a.m.u.]
    Int_t targetA = param[1]; // [a.m.u.]
    Float_t beamE = param[2]; // [MeV/A]
    Double_t gamx100 = param[3]; //
    Float_t imp_par = param[4]; // [fm]
    cout << "Energy: "<< beamE << " - Beam A: "<< beamA << " - Target A: " << targetA << " - gamma: " << gamx100 << " - b (fm): " << imp_par << endl;
    
    //===================================
    // Creating output root file
    //===================================
    cout << "Saving tree to root file" << endl;
    // We split the large file into one containing the many nucleons and one containing the few pions. We are interested in only pions for this analysis.
    stringstream outputfile;
    outputfile << "./" << folder << "_dat_nopions.root";
    stringstream pionoutputfile;
    pionoutputfile << "./" << folder << "_dat_pion.root";
    
    TString outputFileName = outputfile.str();
    TString pionoutputFileName = pionoutputfile.str();
    
    // All pions file
    TFile *p = new TFile(pionoutputFileName,"RECREATE");
    // All other particles file
    TFile *f = new TFile(outputFileName,"RECREATE");
    
    // Definitions of the trees
    p->cd();
    TTree *pure = new TTree("pure", "DAT output");
    pure->Branch("ipid",&ipid);
    pure->Branch("ievent",&nEmpty);
    pure->Branch("px",&px);
    pure->Branch("py",&py);
    pure->Branch("pz_lab",&pz_lab);
    TTree *derived = new TTree("derived", "DAT output");
    derived->Branch("ipid_der",&ipid_der);
    derived->Branch("pxCMS",&pxCMS);
    derived->Branch("pyCMS",&pyCMS);
    derived->Branch("pzCMS",&pzCMS);
    derived->Branch("kenergy_c",&kenergy_c);
    derived->Branch("kenergy_l",&kenergy_l);
    derived->Branch("thetaC",&thetaC);
    derived->Branch("thetaL",&thetaL);
    derived->Branch("phiL",&phiL);
    derived->Branch("energy_c",&energy_c);
    derived->Branch("energy_l",&energy_l);
    derived->Branch("flag",&flag);
    derived->Branch("rap",&rap);
    derived->Branch("pt",&pt);
    
    f->cd();
    TTree *pure2 = new TTree("pure", "DAT output");
    pure2->Branch("ipid",&ipid);
    pure2->Branch("ievent",&nEmpty);
    pure2->Branch("px",&px);
    pure2->Branch("py",&py);
    pure2->Branch("pz_lab",&pz_lab);
    TTree *derived2 = new TTree("derived", "DAT output");
    derived2->Branch("ipid_der",&ipid_der);
    derived2->Branch("pxCMS",&pxCMS);
    derived2->Branch("pyCMS",&pyCMS);
    derived2->Branch("pzCMS",&pzCMS);
    derived2->Branch("kenergy_c",&kenergy_c);
    derived2->Branch("kenergy_l",&kenergy_l);
    derived2->Branch("thetaC",&thetaC);
    derived2->Branch("thetaL",&thetaL);
    derived2->Branch("phiL",&phiL);
    derived2->Branch("energy_c",&energy_c);
    derived2->Branch("energy_l",&energy_l);
    derived2->Branch("flag",&flag);
    derived2->Branch("rap",&rap);
    derived2->Branch("pt",&pt);
    
    // Calculate the transformation to the CoM frame
    Float_t beamMass = beamA*amu2mev;
    Float_t beamEnergy = beamE*beamA+beamA*amu2mev; //includes mass of proj.
    Float_t totalEnergy = beamEnergy+targetA*amu2mev; //include mass of target
    Float_t beam_momentum=sqrt(beamEnergy*beamEnergy-beamMass*beamMass);
    Float_t mBeta=-(beam_momentum/totalEnergy); //negative to go into COM!!
    cout<<" Beta:\t"<< -mBeta << endl;
    
    TVector3 beta(0. ,0. ,mBeta);
    
    TLorentzVector mLorentz;
    string temp[5];
    stringstream datfile;
    string tempstr;
    
    //====================================
    // Event Loop
    //====================================
    cout << "Event loop.... "<<endl;
    fullpath << pathheader << folder << "/" << listname;
    cout << "Looking in\t" << fullpath.str() << endl;
    input.open(fullpath.str().c_str());
    if (!input.is_open()) {
        cout<<"File could not be found"<<endl;
        return -2;
    }
    //LISN has 3 lines of headers
//    getline(input,temp_string);
//    getline(input,temp_string);
//    getline(input,temp_string);
    
    //=====================================
    // Begin loop over files in path
    //=====================================
    
    while(!input.eof()){
        if (ievent%100==0)
            cout << "Runs analyzed: " << ievent << endl;
        tempstr = "";
        datfile.str(std::string());
        datfile.clear();
        string filestring; //the x11 file name in the LISN file
        getline(input,tempstr);// reading in dat file
        
        //uncomment if you read in Jun's old LISN files or you decide to put in the columns again which are useless for this root analysis
//        tempstr=tempstr.substr(0,7);
        
        datfile<<pathheader<<"/"<<folder<<"/"<<tempstr; //<<extension;
        if (tempstr=="") break;
        prev_datfile=tempstr;
        
        //==================================
        //Open .DAT file
        //==================================
//        inputfile=fopen(datfile.str().c_str(),"rb");
        inputfile=fopen(tempstr.c_str(),"rb");
        
        if (inputfile==NULL){
            fputs("File error",stderr);
            //cout << inputfile << endl;
            //exit(1);   //change to continue by Kuan @05/13/2016
            continue;
        }
        
        //===============================================
        // begin loop to read in data from .DAT files
        //===============================================
        
        while(!feof(inputfile)){
            
            ipart = nEntries;
            nEntries++;
            
            fread(&header,sizeof(header),1,inputfile);
            ipid = 999; // reset pid so that if blank line read then stop
            fread(&ipid_temp,sizeof(ipid_temp),1,inputfile);
            ipid=int(ipid_temp);
            ipid_der = ipid;
            
            if((ipid>15 || ipid<0) && ipid!=-100 && ipid!=999){ //pid range 0-15 pid=-100 reserved for end of collision pid=999 represents a blank line was read in
                cout<<"Pid error"<<ipid<<endl;
                return(-1);
            }
            
            if(ipid==0){ // particle id 0 is empty, no momentum data
                // for some reason no trailer in binary file for pid==0
                continue;
            }
            
            fread(&px_temp,sizeof(px_temp),1,inputfile);
            px= float(px_temp);
            fread(&py_temp,sizeof(py_temp),1,inputfile);
            py=float(py_temp);
            fread(&pz_lab_temp,sizeof(pz_lab_temp),1,inputfile);
            pz_lab=float(pz_lab_temp);
            fread(&header,sizeof(header),1,inputfile);
            
            //counting the number of events buu outputs pid=px=py=pz-100 at the end of each collision/.DAT file
            if (ipid == 999)
                break;
            if (ipid==-100){ //last line in .DAT file
                ievent = nEmpty;
                nEmpty++;
                break;
            }
            
            // calculate needed variables
            
            switch(ipid) {
                case 1: imass = mp;  break;
                case 2: imass = mn;  break;
                case 3: imass = md;  break;
                case 4: imass = mHe; break;
                case 5: imass = mt;  break;
                case 6: imass = mHe4; break;
                case 7: case 8: case 9: case 10: imass = mdelta; break;
                case 11:case 12: imass = mbaryres; break;
                case 13: case 15: imass = mpi; break;
                case 14: imass = mpi0; break;
                case 16: imass =0; break;
                case -100: imass = -100;  break; // end of run output along with px=py=pz=-100
                case 999: imass = -1000;  break; // reserved for blank line
                default: cout << "Pid: " << ipid << endl; continue;
            }
            
            //////////////////////////
            //lab frame angles
            //////////////////////////
            
            //phi
            phiL = TMath::RadToDeg()* atan2(py,px);
            if (phiL<0)
                phiL = 360 + phiL;
            //theta
            Double_t r2_lab = px*px+py*py+pz_lab*pz_lab;
            thetaL = ( (r2_lab)?acos(pz_lab/sqrt(r2_lab)) :0 ); // justin this is a fancy if condition (? = then : else)
            thetaL = TMath::RadToDeg()* thetaL;
            
            //////////////////////////////////////////////////////
            // lab frame energy (( E^2 = (pc)^2 + (mc^2)^2) ))
            //////////////////////////////////////////////////////
            energy_l = sqrt((px*px+py*py+pz_lab*pz_lab)+(imass*gev2mev)*(imass*gev2mev));
            //////////////////////////////////////////////////////
            // lab frame kinetic energy
            //////////////////////////////////////////////////////
            kenergy_l = energy_l - imass*gev2mev;
            pt=sqrt(px*px+py*py);
            
            //  ROOT functions to do the lorentz transformations (roughly same results)
            mLorentz.SetPxPyPzE(px,py,pz_lab,energy_l);
            //cout<<"Lorentz pre  Px="<<mLorentz.Px()<<"  Py="<<mLorentz.Py()<<"  Pz="<<mLorentz.Pz()<<"  e="<<mLorentz.E()<<endl;
            mLorentz.Boost(beta);
            //cout<<"Lorentz post Px="<<mLorentz.Px()<<"  Py="<<mLorentz.Py()<<"  Pz="<<mLorentz.Pz()<<"  e="<<mLorentz.E()<<endl;
            //cout<<" pZ pre-post="<<pz_lab-mLorentz.Pz()<<endl;
            pxCMS=mLorentz.Px();
            pyCMS=mLorentz.Py();
            pzCMS=mLorentz.Pz();
            energy_c =mLorentz.E();
            rap = mLorentz.Rapidity();
            //  cout<<"Pz COM\t"<<pzCMS<<endl;
            
            /////////////////////////////
            // CoM frame angles
            /////////////////////////////
            //phi
            phiC = TMath::RadToDeg() * atan2(pyCMS,pxCMS);
            if (phiC<0) phiC = 360 + phiC;
            //theta
            Double_t r2_CMS = pxCMS*pxCMS+pyCMS*pyCMS+pzCMS*pzCMS;
            thetaC = ( (r2_CMS)?acos(pzCMS/sqrt(r2_CMS)):0 );//? is supposed to be there i guess
            thetaC = TMath::RadToDeg() * thetaC;
            
            ///////////////////////////////////////////
            // calculate total and kinetic energies
            ///////////////////////////////////////////
            kenergy_c = energy_c  - imass*gev2mev;
            
            flag = false; //default false
            
            if(ipid==15 || ipid==13){ // write pi+ and pi- 
                p->cd();
                pure->Fill();
                derived->Fill();
            }
            else{ // write all other particles to other file
                f->cd();
                pure2->Fill();
                derived2->Fill();
            }
            
            /////////////////////////////////////////
            //Calculate total number of particles 
            /////////////////////////////////////////
            
            for(int i=0;i<17;i++){
                if (ipid==i)
                    part[i]=part[i]+1; 
            }  
            
        } // end of .DAT loop
        
        fclose(inputfile);
        
    } // end of LISN loop
    
    // write files
    p->Write();
    f->Write();
    // close files
    p->Close();
    f->Close();
    cout << "Number of runs:\t" << ievent << endl;
    
    //////////////////////////////////
    //Check baryon conservation
    //////////////////////////////////
    
    Float_t totalnucleons = (beamA+targetA)*3000*ievent; // 3000 here is the number of test particles used in BUU calculation 
    ofstream output;
    output.open("ParticleCount.txt",ios::out|ios::ate|ios::app);  
    double totalparticlesread=part[1]+part[2]+2*part[3]+3*part[4]+3*part[5]+4*part[6]+part[7]+part[8]+part[9];
    output << folder << "\t" << "Expected total: " << totalnucleons << "\t" << "Total read in: " << totalparticlesread << endl;
    cout << folder << "\t" << "Expected total: " <<totalnucleons << "\t" << "Total read in: " << totalparticlesread << endl;
    
    output.close();
    
    return 0;
    
}


