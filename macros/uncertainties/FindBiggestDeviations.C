#include <TString.h>
#include <TVirtualFitter.h>
#include <iostream>
#include <TStyle.h>
#include <TFile.h>
#include <TH1.h>
#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TText.h>
#include <TPaveText.h>
#include <TGaxis.h>
#include <TFitResult.h>
#include <TMatrixD.h>
#include <TMatrixDSym.h>
#include <TMatrixDSymEigen.h>
#include <TVectorD.h>
#include <TDecompSVD.h>
#include <sstream>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include <TLegend.h>
#include <string>
#include <TROOT.h>
#include <TRint.h>
#include <TClass.h>
#include <TKey.h>


using namespace std;

void FindBiggestDeviations(){

  //Files & histograms for other processes
  TString path = "/nfs/dust/cms/user/reimersa/SingleTth/Finalselection/";
  vector<TString> processes = {"TTbar_2016v3","SingleTop_2016v3"};
  unique_ptr<TFile> f_in;

  for(unsigned int aa=0; aa< processes.size();aa++){
    TString process =  processes[aa];
    f_in.reset (new TFile(path + "NOMINAL/uhh2.AnalysisModuleRunner.MC." + process  + ".root","READ"));

    // unique_ptr<TH1F> mufmur_up, muf_up, mur_up, nominal, mufmur_down, muf_down, mur_down;
    // mufmur_up.reset(f_in.Get())

    vector<TString> histfolders = {"chi2h_2"};
    vector<TString> channel_tags = {"ech", "much"};
    vector<TString> region_tags = {"sr", "cr"};


    // unique_ptr<TFile> f_out_up, f_out_dn;
    unique_ptr<TFile> f_out;
    // f_out_up.reset(new TFile(path + "SCALE_up/" + process  + ".root","RECREATE"));
    // f_out_dn.reset(new TFile(path + "SCALE_down/" + process  + ".root","RECREATE"));
    f_out.reset(new TFile(path + "scale/" + process  + ".root","RECREATE"));

    for(unsigned int i=0; i<channel_tags.size(); i++){
      for(unsigned int j=0; j<region_tags.size(); j++){
        for(unsigned int k=0; k<histfolders.size(); k++){

          TString histfolder = histfolders[k] + "_" + channel_tags[i] + "_" + region_tags[j];

          // f_out_up->mkdir(histfolder);
          // f_out_dn->mkdir(histfolder);
          f_out->mkdir(histfolder + "_scale_up");
          f_out->mkdir(histfolder + "_scale_down");

          unique_ptr<TH1D> h_uu, h_un, h_nu, h_nd, h_dn, h_dd, h_nom;
          TString histname = "";
          f_in->cd(histfolder+"_nominal");
          TDirectory* dir = gDirectory;
          TIter iter(dir->GetListOfKeys());
          TKey *key;
          TH1D* h;
          while ((key = (TKey*)iter())) {
            TClass *cl = gROOT->GetClass(key->GetClassName());
            if (!cl->InheritsFrom("TH1")) continue;
            h = (TH1D*)key->ReadObj();
            histname = h->GetName();
            std::cout<<"histname  "<<histname<<std::endl;
            h_uu.reset((TH1D*)f_in->Get(histfolder + "_scale_upup/" + histname));
            h_un.reset((TH1D*)f_in->Get(histfolder + "_scale_upnone/" + histname));
            h_nu.reset((TH1D*)f_in->Get(histfolder + "_scale_noneup/" + histname));
            h_nd.reset((TH1D*)f_in->Get(histfolder + "_scale_nonedown/" + histname));
            h_dn.reset((TH1D*)f_in->Get(histfolder + "_scale_downnone/" + histname));
            h_dd.reset((TH1D*)f_in->Get(histfolder + "_scale_downdown/" + histname));
            h_nom.reset((TH1D*)f_in->Get(histfolder + "_nominal/" + histname));


            const int nbins = h_nom->GetNbinsX();
            vector<double> min_bins, max_bins, max_err, min_err;

            for(int ii=1; ii<h_nom->GetNbinsX()+1; ii++){
              double entries[7] = {h_uu->GetBinContent(ii),h_un->GetBinContent(ii),h_nu->GetBinContent(ii),h_nd->GetBinContent(ii),h_dn->GetBinContent(ii),h_dd->GetBinContent(ii),h_nom->GetBinContent(ii)};
              double errors[7] = {h_uu->GetBinError(ii),h_un->GetBinError(ii),h_nu->GetBinError(ii),h_nd->GetBinError(ii),h_dn->GetBinError(ii),h_dd->GetBinError(ii),h_nom->GetBinError(ii)};
              double min = 9999999;
              double max_error = 0, min_error = 0;
              double max = 0;
              for(int jj=0; jj<7; jj++){
                if(entries[jj] > max) {max = entries[jj]; max_error = errors[jj];}
                if(entries[jj] < min) {min = entries[jj]; min_error = errors[jj];}
              }

              min_bins.push_back(min);
              max_bins.push_back(max);
              min_err.push_back(min_error);
              max_err.push_back(max_error);
            }

            vector<double> bins_low;
            for(int ii=1; ii<h_nom->GetNbinsX()+2; ii++){
              bins_low.push_back(h_nom->GetBinLowEdge(ii));
            }

            unique_ptr<TH1D> hist_out_up, hist_out_dn;
            hist_out_up.reset(new TH1D(histname,"",  nbins, &bins_low[0]));
            for(int ii=1; ii<h_nom->GetNbinsX()+1; ii++){
              hist_out_up->SetBinContent(ii,max_bins[ii-1]);
              hist_out_up->SetBinError(ii, max_err[ii-1]);
            }


            hist_out_dn.reset(new TH1D(histname,"",  nbins, &bins_low[0]));
            for(int ii=1; ii<h_nom->GetNbinsX()+1; ii++){
              hist_out_dn->SetBinContent(ii,min_bins[ii-1]);
              hist_out_dn->SetBinError(ii, min_err[ii-1]);
            }


            cout << "histname: " << histname << endl;
            //Paths for other processes
            // f_out_up->cd(histfolder);
            // hist_out_up->Write();
            // f_out_dn->cd(histfolder);
            // hist_out_dn->Write();
            f_out->cd(histfolder + "_scale_up");
            hist_out_up->Write();
            f_out->cd(histfolder + "_scale_down");
            hist_out_dn->Write();
          }
        }
      }
    }

    // f_out_up->Close();
    // f_out_dn->Close();
    f_out->Close();
  }
}
