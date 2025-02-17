#include "TH1.h"
#include <fstream>
#include "sigfunc.C"
#include "../bkgfits/func.C"
#include "../bkgfits/helpers.C"

using namespace std;
void fitsignal(Echannel channel, int MT, std::vector<double>& means, std::vector<double>& means_err, std::vector<double>& widths, std::vector<double>& widths_err, std::vector<double>& effs, std::vector<double>& effs_err);

void sig_fit()
{
  // decide which channel to do (eEle, eMuon, eComb)
  Echannel ch = eComb;

  std::vector<double> MTs = {600, 650, 800, 900, 1000, 1100, 1200};
  std::vector<double> means;
  std::vector<double> means_err;
  std::vector<double> widths;  
  std::vector<double> widths_err;  
  std::vector<double> effs;  
  std::vector<double> effs_err;  
  std::vector<double> zeros;

  // do the fits, fill results into graph
  for (int i=0; i<MTs.size(); ++i){
    fitsignal(ch,  (int) MTs[i], means, means_err, widths, widths_err, effs, effs_err);
    zeros.push_back(0);
  }
  TGraphErrors* gmean  = new TGraphErrors(MTs.size(), MTs.data(), means.data(), zeros.data(), means_err.data());  
  TGraphErrors* gsigma = new TGraphErrors(MTs.size(), MTs.data(), widths.data(), zeros.data(), widths_err.data());    
  TGraphErrors* geff   = new TGraphErrors(MTs.size(), MTs.data(), effs.data(), zeros.data(), effs_err.data());      

  // some information text
  TString info, info2;
  if (ch==eMuon){
    info2 = "#mu+jets";
  } else if (ch==eEle) {
    info2 = "e+jets";
  } else if (ch==eComb) {
    info2 = "l+jets";
  }
  TLatex* text = new TLatex();
  text->SetTextFont(42);
  text->SetNDC();
  text->SetTextColor(kBlack);
  text->SetTextSize(0.040);  

  //-------------------------------------------------
  // Mean values with fit
  //-------------------------------------------------
  TCanvas *can = new TCanvas("mean_can","",10,10,700,700);
  gPad->SetTickx();
  gPad->SetTicky();
  can->SetLeftMargin(0.15);
  can->SetRightMargin(0.05);
  can->SetTopMargin(0.10);
  can->SetBottomMargin(0.12);

  // cosmetics
  TH1* painter = new TH1F("painter", "", 1, 550, 1250);
  painter->SetXTitle("Generated M_{T} [GeV]");
  painter->SetYTitle("#mu [GeV]");
  painter->SetTitleSize(0.045);
  painter->GetYaxis()->SetTitleSize(0.045);
  painter->GetYaxis()->SetTitleOffset(1.4);
  painter->GetXaxis()->SetTitleOffset(1.2);
  painter->SetTitle("");
  painter->GetYaxis()->SetRangeUser(550, 1250);
  painter->Draw();
  gmean->SetMarkerStyle(20);
  gmean->SetLineWidth(2);

  gStyle->SetStatX(0.63);
  gStyle->SetStatW(0.24);
  gStyle->SetStatY(0.85);  
  TF1* lin = new TF1("meanfit", "[0]+[1]*(x-600)", 550, 1250);
  lin->SetParName(0, "#mu at 600 GeV");
  lin->SetParName(1, "Slope");
  TFitResultPtr r = gmean->Fit(lin, "0");

  MTs[0] = 550;
  MTs[6] = 1250;
  TGraphErrors* fit_err_95 = new TGraphErrors(MTs.size(), MTs.data(), means.data(), zeros.data(), means_err.data());  
  TFitter* fitter = (TFitter*) TVirtualFitter::GetFitter();
  fitter->GetConfidenceIntervals(fit_err_95, 0.95);
  fit_err_95->SetFillColor(kOrange-4);
  fit_err_95->Draw("L3 same");

  TGraphErrors* fit_err_68 = new TGraphErrors(MTs.size(), MTs.data(), means.data(), zeros.data(), means_err.data());  
  fitter->GetConfidenceIntervals(fit_err_68, 0.68);
  fit_err_68->SetFillColor(kOrange+1);
  fit_err_68->Draw("L3 same");

  lin->SetLineColor(kRed+1);
  lin->Draw("same");
  gmean->Draw("P same");

  TString channel_name = "";
  if (ch==eMuon){
    channel_name = "much";
  } else if (ch==eEle) {
    channel_name = "ech";
  } else if (ch==eComb) {
    channel_name = "comb";
  }
  TString fname = "results/signal_mean_values_" + channel_name + ".pdf"; 

  // draw some info
  info = TString::Format("Signal mean values, ");
  info.Append(info2);
  text->DrawLatex(0.16, 0.92, info.Data());

  can->RedrawAxis();
  can->SaveAs(fname);


  //-------------------------------------------------
  // Widths with fit
  //-------------------------------------------------
  TCanvas *can2 = new TCanvas("widths_can","",10,10,700,700);
  gPad->SetTickx();
  gPad->SetTicky();
  can2->SetLeftMargin(0.15);
  can2->SetRightMargin(0.05);
  can2->SetTopMargin(0.10);
  can2->SetBottomMargin(0.12);

  // cosmetics
  TH1* painter2 = new TH1F("painter2", "", 1, 550, 1250);
  painter2->SetXTitle("Generated M_{T} [GeV]");
  painter2->SetYTitle("#sigma [GeV]");
  painter2->SetTitleSize(0.045);
  painter2->GetYaxis()->SetTitleSize(0.045);
  painter2->GetYaxis()->SetTitleOffset(1.4);
  painter2->GetXaxis()->SetTitleOffset(1.2);
  painter2->SetTitle("");
  painter2->GetYaxis()->SetRangeUser(0, 200);
  painter2->Draw();
  gsigma->SetMarkerStyle(20);
  gsigma->SetLineWidth(2);

  gStyle->SetStatX(0.70);
  gStyle->SetStatW(0.27);
  gStyle->SetStatY(0.85);  
  TF1* lin2 = new TF1("sigmafit", "[0]+[1]*(x-600)", 550, 1250);
  lin2->SetParName(0, "#sigma at 600 GeV");
  lin2->SetParName(1, "Slope");
  TFitResultPtr r2 = gsigma->Fit(lin2, "0");

  MTs[0] = 550;
  MTs[6] = 1250;
  TGraphErrors* fit2_err_95 = new TGraphErrors(MTs.size(), MTs.data(), widths.data(), zeros.data(), widths_err.data());  
  fitter = (TFitter*) TVirtualFitter::GetFitter();
  fitter->GetConfidenceIntervals(fit2_err_95, 0.95);
  fit2_err_95->SetFillColor(kOrange-4);
  fit2_err_95->Draw("L3 same");

  TGraphErrors* fit2_err_68 = new TGraphErrors(MTs.size(), MTs.data(), widths.data(), zeros.data(), widths_err.data());  
  fitter->GetConfidenceIntervals(fit2_err_68, 0.68);
  fit2_err_68->SetFillColor(kOrange+1);
  fit2_err_68->Draw("L3 same");

  lin2->SetLineColor(kRed+1);
  lin2->Draw("same");
  gsigma->Draw("P same");

  TString fname2 = "results/signal_sigma_values_" + channel_name + ".pdf"; 

  // draw some info
  info = TString::Format("Signal widths, ");
  info.Append(info2);
  text->DrawLatex(0.16, 0.92, info.Data());

  can2->RedrawAxis();
  can2->SaveAs(fname2);


  //-------------------------------------------------
  // Efficiencies with fit
  //-------------------------------------------------
  TCanvas *can3 = new TCanvas("eff_can","",10,10,700,700);
  gPad->SetTickx();
  gPad->SetTicky();
  can3->SetLeftMargin(0.15);
  can3->SetRightMargin(0.05);
  can3->SetTopMargin(0.10);
  can3->SetBottomMargin(0.12);

  // cosmetics
  TH1* painter3 = new TH1F("painter", "", 1, 550, 1250);
  painter3->SetXTitle("Generated M_{T} [GeV]");
  painter3->SetYTitle("#varepsilon");
  painter3->SetTitleSize(0.045);
  painter3->GetYaxis()->SetTitleSize(0.045);
  painter3->GetYaxis()->SetTitleOffset(1.4);
  painter3->GetXaxis()->SetTitleOffset(1.2);
  painter3->SetTitle("");
  painter3->GetYaxis()->SetRangeUser(0, 0.01);
  painter3->Draw();
  geff->SetMarkerStyle(20);
  geff->SetLineWidth(2);

  gStyle->SetStatX(0.63);
  gStyle->SetStatW(0.24);
  gStyle->SetStatY(0.85);  
  TF1* lin3 = new TF1("efffit", "[0]+[1]*(x-600)+[2]*(x-600)*(x-600)", 550, 1250);
  lin3->SetParameter(0, effs[0]);
  lin3->SetParameter(1, 0);  
  lin3->SetParName(0, "#varepsilon at 600 GeV");
  lin3->SetParName(1, "Slope");
  TFitResultPtr r3 = geff->Fit(lin3, "0");

  MTs[0] = 550;
  MTs[6] = 1250;
  TGraphErrors* fit3_err_95 = new TGraphErrors(MTs.size(), MTs.data(), means.data(), zeros.data(), means_err.data());  
  fitter = (TFitter*) TVirtualFitter::GetFitter();
  fitter->GetConfidenceIntervals(fit3_err_95, 0.95);
  fit3_err_95->SetFillColor(kOrange-4);
  fit3_err_95->Draw("L3 same");

  TGraphErrors* fit3_err_68 = new TGraphErrors(MTs.size(), MTs.data(), means.data(), zeros.data(), means_err.data());  
  fitter->GetConfidenceIntervals(fit3_err_68, 0.68);
  fit3_err_68->SetFillColor(kOrange+1);
  fit3_err_68->Draw("L3 same");

  lin3->SetLineColor(kRed+1);
  lin3->Draw("same");
  geff->Draw("P same");

  // draw some info
  info = TString::Format("Signal efficiencies, ");
  info.Append(info2);
  text->DrawLatex(0.16, 0.92, info.Data());

  fname = "results/signal_eff_values_" + channel_name + ".pdf"; 

  can3->RedrawAxis();
  can3->SaveAs(fname);

}

void fitsignal(Echannel channel, int MT, std::vector<double>& means, std::vector<double>& means_err, std::vector<double>& widths, std::vector<double>& widths_err, std::vector<double>& effs, std::vector<double>& effs_err)
{
  
  // set fit regions (crude estimate)
  double fit_xmin = MT-20-150.;
  double fit_xmax = MT-20+150.;  
 
  // a bit more precise 
  double mean = MT-20;
  double sigma = 61.5+0.0033*(MT-600.);
  fit_xmin = mean - 2*sigma;
  fit_xmax = mean + 2.5*sigma;

  gROOT->SetBatch(kTRUE);

  //setOptFit( pcev (default = 0111)) Probability; Chisquare/Number of degrees of freedom; errors ;values of parameters 
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0);

 
  // -----------------------------------------
  // Canvas for Background Fit without Ratios
  // -----------------------------------------
  TCanvas *can = new TCanvas("sig_c","signal fit",10,10,700,700);
  gPad->SetTickx();
  gPad->SetTicky();
  can->Clear();
  can->cd();
  can->SetLeftMargin(0.12);
  can->SetRightMargin(0.05);
  can->SetTopMargin(0.10);
  can->SetBottomMargin(0.12);

  // get data or MC
  TH1F* sigh = GetAnalysisOutputSignal(MT, channel); 

  // important: get xmin and xmax from bin edges!
  // needed for normalization, otherwise the fit quality is bad
  int Nbins = 0;
  double xmin = 1e6;
  double xmax = 0;  
  for (int i=0; i<sigh->GetNbinsX()+1; ++i){
    if (sigh->GetXaxis()->GetBinLowEdge(i)>=fit_xmin){
      if (xmin>1e5) xmin = sigh->GetXaxis()->GetBinLowEdge(i);
      if (sigh->GetXaxis()->GetBinUpEdge(i)<=fit_xmax){ 
        xmax = sigh->GetXaxis()->GetBinUpEdge(i);
        ++Nbins;
      }
    }
  }
  cout << "Nbins = " << Nbins << endl;
  cout << "xmin = " << xmin << endl;
  cout << "xmax = " << xmax << endl;

  // calculate the integral to set the normalisation
  double norm = sigh->Integral(sigh->GetXaxis()->FindBin(xmin), sigh->GetXaxis()->FindBin(xmax), "width");
  cout << "Norm from hist = " << norm << endl;

  //-----------------------------------
  // set up function for fit
  //-----------------------------------
  signalfunction_gauss sigfuncobj(xmin, xmax);
  sigfuncobj.SetNorm(norm);    

  TF1* fitmodel = new TF1("fitmodel", sigfuncobj, xmin, xmax, 2);
  fitmodel->SetParameter(0, MT-20.);  
  fitmodel->SetParameter(1, 60);
  fitmodel->SetParName(0,"#mu");
  fitmodel->SetParName(1,"#sigma");
  Int_t linecol = kRed+2; 
  Int_t col68 = kOrange; 
  Int_t col95 = kOrange+7;
  TString fdesc = "Gaussian PDF, 2 pars";
  TString ffile = TString::Format("gaus_MT%d", MT);

  cout << "Norm in function = " << fitmodel->Integral(xmin, xmax, 1e-3) << endl;

  // some cosmetics
  fitmodel->SetLineColor(linecol);
  fitmodel->SetLineStyle(kSolid);
  fitmodel->SetLineWidth(2);

  // FIT!
  TFitResultPtr r = sigh->Fit(fitmodel, "RS 0");
  TMatrixDSym covmatr = r->GetCovarianceMatrix();
  TMatrixDSym rho = r->GetCorrelationMatrix();
  covmatr.Print();
  rho.Print();

  // efficiency calculation
  double Nevents_err; 
  double Nevents = sigh->IntegralAndError(sigh->GetXaxis()->FindBin(xmin), sigh->GetXaxis()->FindBin(xmax), Nevents_err);
  double Ntot = sigh->Integral(sigh->GetXaxis()->GetXmin(), sigh->GetXaxis()->GetXmax());
  double efferr;
  double eff = CalcEff(fitmodel, Nevents, Nevents_err, Ntot, MT, efferr);

  cout << "\n" << "Total number of expected events (1pb?) = " << sigh->GetSumOfWeights() << " and within fit range: " << sigh->Integral(sigh->FindBin(xmin), sigh->FindBin(xmax)) << endl << endl;

  // store the results
  means.push_back(fitmodel->GetParameter(0));
  means_err.push_back(fitmodel->GetParError(0));
  widths.push_back(fitmodel->GetParameter(1));
  widths_err.push_back(fitmodel->GetParError(1));
  effs.push_back(eff);
  effs_err.push_back(efferr);

  // draw the result
  sigh->Draw("PZ");
  //gPad->SetLogy(true);


  // draw 95% CL
  TH1F* clhist = ComputeHistWithCL(fitmodel, r, sigh, 0.95);
  clhist->SetStats(kFALSE);
  clhist->SetLineColor(col95);
  clhist->SetFillColor(col95);
  clhist->SetMarkerColor(col95);
  clhist->Draw("e3 same");

  // draw 68% CL
  TH1F* clhist2 = ComputeHistWithCL(fitmodel, r, sigh, 0.68);
  clhist2->SetStats(kFALSE);
  clhist2->SetLineColor(col68);
  clhist2->SetFillColor(col68);
  clhist2->SetMarkerColor(col68);
  clhist2->Draw("e3 same");

  //fitmodel->DrawClone("same");
  sigh->DrawClone("PZ same");
  fitmodel->DrawClone("same");


  // some information
  TString info, info2;
  info = TString::Format("Signal with M_{T} = %d GeV", MT);
  if (channel==eMuon){
    info2 = "#mu+jets";
  } else if (channel==eEle) {
    info2 = "e+jets";
  } else if (channel==eComb) {
    info2 = "l+jets";
  }
  TString info4 = fdesc;
  TLatex* text = new TLatex();
  text->SetTextFont(42);
  text->SetNDC();
  text->SetTextColor(kBlack);
  text->SetTextSize(0.035);
  text->DrawLatex(0.13, 0.96, info.Data());
  text->DrawLatex(0.13, 0.92, info2.Data());
  //text->DrawLatex(0.13, 0.92, info3.Data());
  text->SetTextFont(62);
  text->DrawLatex(0.17, 0.83, info4.Data());
  text->SetTextFont(42);
  TString info5 = "Fit:";
  text->DrawLatex(0.17, 0.76, info5.Data());
  info5 = TString::Format("x_{min} = %3.0f GeV", xmin);
  text->DrawLatex(0.17, 0.72, info5.Data());
  info5 = TString::Format("x_{max} = %3.0f GeV", xmax);
  text->DrawLatex(0.17, 0.67, info5.Data());

  TString info6 = TString::Format("#varepsilon_{sig} = %4.2f #pm %4.2f", eff*100, efferr*100);
  info6.Append("%");
  text->DrawLatex(0.17, 0.60, info6.Data());  

  double corr = sigh->Integral(sigh->GetXaxis()->GetXmin(), sigh->GetXaxis()->GetXmax(), "width") / norm;
  cout << "corr = " << corr << endl;
  TString info7 = TString::Format("f_{norm} = %4.3f", sigfuncobj.GetFunc()->GetParameter(0)*corr );
  text->DrawLatex(0.17, 0.55, info7.Data());  

  can->RedrawAxis();

  TString folder = "results";
  TString channel_name = "";
  if (channel==eMuon){
    channel_name = "much";
  } else if (channel==eEle) {
    channel_name = "ech";
  } else if (channel==eComb) {
    channel_name = "comb";
  }

  can->Print(folder + "/signalfit_" + channel_name + "_" + ffile + ".pdf");

  // save ratios and all infos
/*  
  std::vector<TH1F*> err_hists;
  err_hists.push_back(clhist);
  err_hists.push_back(clhist2);  
  plot_ratio(back, fitmodel, err_hists, region, channel, dodata, all_bkgds, fdesc, ffile);
*/

}

