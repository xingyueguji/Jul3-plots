#include <cmath>

void FADCBLOCK( Int_t nrun=1370, Int_t nevent=-1) {
    gStyle->SetOptFit(3);
    // --------------------------------------------------------------------------------
    
    TChain *t;
    TFile *f = new TFile(Form("/data1/cdaq/eel108/output/ROOTfiles/nps_eel108_%i.root", nrun));
    t = new TChain("T");
    t->Add(f->GetName(),-1);
    t->SetBranchStatus("*",false);
    t->SetBranchStatus("Ndata.NPS.cal.fly.adcCounter",true);
    t->SetBranchStatus("NPS.cal.fly.adcCounter",true);
    t->SetBranchStatus("NPS.cal.fly.adcSampPulseTime",true);
    t->SetBranchStatus("NPS.cal.fly.adcSampPulseAmp",true);
    t->SetBranchStatus("NPS.cal.fly.adcSampPed",true);
    t->SetBranchStatus("NPS.cal.fly.adcSampPulseInt",true);

    // Get some useful variables-------------------------------------------------------

    Int_t binnumber = 120;
    Int_t NadcCounter;
    t->SetBranchAddress("Ndata.NPS.cal.fly.adcCounter",&NadcCounter);
    Double_t adcCounter[2000];
    t->SetBranchAddress("NPS.cal.fly.adcCounter",&adcCounter);
    Double_t Amp[2000];
    t->SetBranchAddress("NPS.cal.fly.adcSampPulseAmp",&Amp);
    Double_t Int[2000];
    t->SetBranchAddress("NPS.cal.fly.adcSampPulseInt",&Int);
    Double_t Ped[2000];
    t->SetBranchAddress("NPS.cal.fly.adcSampPed",&Ped);
    Double_t Time[2000];
    t->SetBranchAddress("NPS.cal.fly.adcSampPulseTime",&Time);
    
    //Histogram------------------------------------------------------------------------
    TH1F *h_Amp[68]; //15 channels from vme1-4, 8 for vme5, 15*4 + 8 = 68 FADC channels
    TH1F *h_Int[68];
    TH1F *h_Time[68];
    TH1F *h_Ped[68];

    for (Int_t i=0; i<68; i++){
        h_Amp[i] = new TH1F(Form("h_Amp_Channel_%d",i),"",100,0,100);
        h_Int[i] = new TH1F(Form("h_Int_Channel_%d",i),"",100,0,100);
        h_Time[i] = new TH1F(Form("h_Time_Channel_%d",i),"",120,0,480);
        h_Ped[i] = new TH1F(Form("h_Ped_Channel_%d",i),"",100,50,150);
    }

    //Arrays, variables, ...



    //Filling Histogram
    Long64_t nentries = t->GetEntries();
    cout <<"Total Event Number is "<< nentries << endl;
    if (nevent != -1){
        nentries = nevent;
    }

    for(Int_t i=0; i<nentries; i++){
        t->GetEntry(i);
        cout << "We are at Event " << i << endl;
        // block 960 is the first one in VME5, each vme crate contains 240 blocks = 15 FADC slots
        for(Int_t j=0; j<NadcCounter; j++){
            int SlotNumber = (int(adcCounter[j]) / 16); 
            cout << "SlotNumber is " << SlotNumber << " Block Number is " << adcCounter[j] << endl;
            h_Amp[SlotNumber]->Fill(Amp[j]);
            h_Int[SlotNumber]->Fill(Int[j]);
            h_Time[SlotNumber]->Fill(Time[j]);
            h_Ped[SlotNumber]->Fill(Ped[j]);
        }
    }

    TCanvas *c_Amp = new TCanvas("c_Amp","",800,800);
    TCanvas *c_Int = new TCanvas("c_Int","",800,800);
    TCanvas *c_Time = new TCanvas("c_Time","",800,800);
    TCanvas *c_Ped = new TCanvas("c_Ped","",800,800);
    for(Int_t i=0; i<68;i++){
        c_Amp->cd();
        h_Amp[i]->Draw();
        c_Int->cd();
        h_Int[i]->Draw();
        c_Time->cd();
        h_Time[i]->Draw();
        c_Ped->cd();
        h_Ped[i]->Draw();

        c_Amp->SaveAs(Form("./Amp/Amp_run_%d_nevent_%d_slot_%d.pdf",nrun,nevent,i));
        c_Int->SaveAs(Form("./Int/Int_run_%d_nevent_%d_slot_%d.pdf",nrun,nevent,i));
        c_Time->SaveAs(Form("./Time/Time_run_%d_nevent_%d_slot_%d.pdf",nrun,nevent,i));
        c_Ped->SaveAs(Form("./Ped/Ped_run_%d_nevent_%d_slot_%d.pdf",nrun,nevent,i));

        c_Amp->Clear();
        c_Int->Clear();
        c_Ped->Clear();
        c_Time->Clear();
    }

}
