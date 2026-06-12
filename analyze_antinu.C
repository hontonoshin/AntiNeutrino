#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TROOT.h>



void SetPublicationStyle()
{
    gStyle->Reset();
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    gStyle->SetPadLeftMargin(0.14);
    gStyle->SetPadRightMargin(0.05);
    gStyle->SetPadTopMargin(0.07);
    gStyle->SetPadBottomMargin(0.13);

    gStyle->SetTitleFont(42, "XYZ");
    gStyle->SetLabelFont(42, "XYZ");
    gStyle->SetTextFont(42);

    gStyle->SetTitleSize(0.045, "XYZ");
    gStyle->SetLabelSize(0.04, "XYZ");

    gStyle->SetTitleOffset(1.3, "Y");
    gStyle->SetTitleOffset(1.1, "X");

    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);
    gStyle->SetFrameLineWidth(1);
    gStyle->SetHistLineWidth(2);

    gStyle->SetCanvasColor(kWhite);
    gStyle->SetPadColor(kWhite);
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetPadBorderMode(0);

    gStyle->SetPalette(kBird);
}

void DrawAndSave(TH1D* h, const char* xTitle, const char* yTitle,
                  const char* outName, int lineColor = kAzure+2,
                  bool logy = false)
{
    TCanvas* c = new TCanvas(Form("c_%s", outName), "", 1000, 800);
    c->SetLogy(logy);

    h->SetLineColor(lineColor);
    h->SetLineWidth(2);
    h->SetFillColorAlpha(lineColor, 0.25);
    h->GetXaxis()->SetTitle(xTitle);
    h->GetYaxis()->SetTitle(yTitle);
    h->GetXaxis()->SetTitleSize(0.045);
    h->GetYaxis()->SetTitleSize(0.045);
    h->Draw("HIST");

    c->SaveAs(Form("%s.png", outName));
    //c->SaveAs(Form("%s.svg", outName));

    delete c;
}

void analyze_antinu()
{
    SetPublicationStyle();

    TFile* file = TFile::Open("antinu_output.root", "READ");
    if (!file || file->IsZombie()) {
        printf("ERROR: could not open antinu_output.root\n");
        return;
    }

    TTree* tree = (TTree*)file->Get("antinu");
    if (!tree) {
        printf("ERROR: ntuple 'antinu' not found in file\n");
        return;
    }

    Double_t promptEdep, delayedEdep, captureTime, captureDR, eNuTrue, vertexR;
Int_t capturedOnGd;

tree->SetBranchAddress("promptEdep",   &promptEdep);
tree->SetBranchAddress("delayedEdep",  &delayedEdep);
tree->SetBranchAddress("captureTime",  &captureTime);
tree->SetBranchAddress("captureDR",    &captureDR);
tree->SetBranchAddress("capturedOnGd", &capturedOnGd);
tree->SetBranchAddress("eNuTrue",      &eNuTrue);
tree->SetBranchAddress("vertexR",      &vertexR);

    Long64_t nEntries = tree->GetEntries();
    printf("Total events: %lld\n", nEntries);

    // ── Histograms ───────────────────────────────────────────────────────
    TH1D* hPrompt  = new TH1D("hPrompt",  "", 100, 0.0, 10.0);
    TH1D* hDelayed = new TH1D("hDelayed", "", 100, 0.0, 10.0);
    TH1D* hCapTime = new TH1D("hCapTime", "", 100, 0.0, 300.0);
    TH1D* hCapDR   = new TH1D("hCapDR",   "", 100, 0.0, 50.0);
    TH1D* hENuTrue = new TH1D("hENuTrue", "", 100, 0.0, 10.0);
    TH1D* hGdFrac  = new TH1D("hGdFrac",  "", 2, -0.5, 1.5); 
    TH1D* hDelayedGd = new TH1D("hDelayedGd", "", 100, 0.0, 10.0);
    TH1D* hDelayedH  = new TH1D("hDelayedH",  "", 100, 0.0, 10.0);
    TH1D* hVertexR  = new TH1D("hVertexR",  "", 100, 0.0, 60.0);  // cm
    TH1D* hENuReco  = new TH1D("hENuReco",  "", 100, 0.0, 10.0);
    TH2D* hENuCorr  = new TH2D("hENuCorr",  "", 80, 0.0, 10.0, 80, 0.0, 10.0);

    // Delayed-coincidence selected spectra
    TH1D* hPromptSel  = new TH1D("hPromptSel",  "", 100, 0.0, 10.0);
    TH1D* hDelayedSel = new TH1D("hDelayedSel", "", 100, 0.0, 10.0);

    Long64_t nCaptured = 0;

    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);

        hPrompt->Fill(promptEdep);
        hENuTrue->Fill(eNuTrue);

        if (capturedOnGd >= 0) {
    ++nCaptured;
    hDelayed->Fill(delayedEdep);
    hCapTime->Fill(captureTime);
    if (captureDR >= 0) hCapDR->Fill(captureDR);
    hGdFrac->Fill(capturedOnGd);

    if (capturedOnGd == 1) {
        hDelayedGd->Fill(delayedEdep);
    } else if (capturedOnGd == 0) {
        hDelayedH->Fill(delayedEdep);
    }
}

hVertexR->Fill(vertexR);

// Reconstructed neutrino energy from prompt deposit
double eNuReco = promptEdep + 0.78; // MeV, inverse-IBD approx
hENuReco->Fill(eNuReco);
hENuCorr->Fill(eNuTrue, eNuReco);

// Delayed-coincidence selection: capture occurred, Delta t in [1,200] us,
// and a sensible delayed-energy window for either H (1.9-2.5 MeV) or
// Gd (4-8 MeV) capture
if (capturedOnGd >= 0 && captureTime >= 1.0 && captureTime <= 300.0) {
    bool hWindow  = (delayedEdep >= 1.9 && delayedEdep <= 2.5);
    bool gdWindow = (delayedEdep >= 4.0 && delayedEdep <= 8.0);
    if (hWindow || gdWindow) {
        hPromptSel->Fill(promptEdep);
        hDelayedSel->Fill(delayedEdep);
    }
}


}

    printf("Events with neutron capture: %lld (%.1f%%)\n",
           nCaptured, 100.0 * nCaptured / nEntries);

    // ── Plot 1: True reactor antineutrino energy spectrum ──────────────────
    DrawAndSave(hENuTrue,
                "True E_{#bar{#nu}_{e}} [MeV]",
                "Events / bin",
                "enu_true_spectrum",
                kAzure+2);

    // ── Plot 2: Prompt energy spectrum (e+ + annihilation gammas) ──────────
    DrawAndSave(hPrompt,
                "Prompt energy deposit [MeV]",
                "Events / bin",
                "prompt_energy_spectrum",
                kOrange+7);

    // ── Plot 3: Delayed energy spectrum (neutron capture gammas) ───────────
    DrawAndSave(hDelayed,
                "Delayed energy deposit [MeV]",
                "Events / bin",
                "delayed_energy_spectrum",
                kGreen+2);
    // ── Plot 3b: Delayed energy spectrum, Gd captures only ─────────────────
    DrawAndSave(hDelayedGd,
            "Delayed energy deposit [MeV]",
            "Events / bin",
            "delayed_energy_spectrum_gd",
            kGreen+3);

    // ── Plot 3c: Delayed energy spectrum, H captures only ───────────────────
    DrawAndSave(hDelayedH,
            "Delayed energy deposit [MeV]",
            "Events / bin",
            "delayed_energy_spectrum_h",
            kTeal+1);

    // ── Plot 4: Capture time distribution (delayed coincidence Delta-t) ───
    DrawAndSave(hCapTime,
                "Capture time #Delta t [#mus]",
                "Events / bin",
                "capture_time_distribution",
                kViolet+1);

    // ── Plot 5: Capture distance from IBD vertex (Delta-R) ─────────────────
    DrawAndSave(hCapDR,
                "Capture distance #Delta R [cm]",
                "Events / bin",
                "capture_distance_distribution",
                kRed+1);

    // ── Plot 6: Capture isotope fraction (H vs Gd) ──────────────────────────
    {
        TCanvas* c = new TCanvas("c_capture_isotope", "", 1000, 800);

        hGdFrac->SetLineColor(kBlack);
        hGdFrac->SetLineWidth(2);
        hGdFrac->SetFillColorAlpha(kCyan+1, 0.4);
        hGdFrac->GetXaxis()->SetTitle("Capture isotope (0 = H, 1 = Gd)");
        hGdFrac->GetYaxis()->SetTitle("Events");
        hGdFrac->GetXaxis()->SetNdivisions(2);
        hGdFrac->GetXaxis()->SetTitleSize(0.045);
        hGdFrac->GetYaxis()->SetTitleSize(0.045);
        hGdFrac->Draw("HIST");

        c->SaveAs("capture_isotope_fraction.png");
        //c->SaveAs("capture_isotope_fraction.svg");
        delete c;
    }

// ── Plot 7: IBD vertex radius distribution ──────────────────────────────
DrawAndSave(hVertexR,
            "Vertex radius R [cm]",
            "Events / bin",
            "vertex_radius_distribution",
            kMagenta+1);

// ── Plot 8: Reconstructed antineutrino energy ───────────────────────────
DrawAndSave(hENuReco,
            "Reconstructed E_{#bar{#nu}_{e}} [MeV]",
            "Events / bin",
            "enu_reco_spectrum",
            kBlue+2);

// ── Plot 9: True vs reconstructed E_nu (2D correlation) ─────────────────
{
    TCanvas* c = new TCanvas("c_enu_correlation", "", 1000, 800);
    hENuCorr->GetXaxis()->SetTitle("True E_{#bar{#nu}_{e}} [MeV]");
    hENuCorr->GetYaxis()->SetTitle("Reconstructed E_{#bar{#nu}_{e}} [MeV]");
    hENuCorr->GetXaxis()->SetTitleSize(0.045);
    hENuCorr->GetYaxis()->SetTitleSize(0.045);
    hENuCorr->Draw("COLZ");
    c->SaveAs("enu_true_vs_reco.png");
    //c->SaveAs("enu_true_vs_reco.svg");
    delete c;
}

// ── Plot 10: Prompt spectrum after delayed-coincidence selection ────────
DrawAndSave(hPromptSel,
            "Prompt energy deposit [MeV]",
            "Events / bin",
            "prompt_energy_selected",
            kOrange+7);

// ── Plot 11: Delayed spectrum after delayed-coincidence selection ───────
DrawAndSave(hDelayedSel,
            "Delayed energy deposit [MeV]",
            "Events / bin",
            "delayed_energy_selected",
            kGreen+2);



    
    Long64_t nSelected = hPromptSel->GetEntries();
      printf("Delayed-coincidence selected events: %lld (%.1f%% of total)\n",
       nSelected, 100.0 * nSelected / nEntries);
       printf("Events with neutron capture: %lld (%.1f%%)\n", nCaptured, 100.0 * nCaptured / nEntries);
      printf("hCapTime entries=%lld, mean=%.2f, min content check...\n", (Long64_t)hCapTime->GetEntries(), hCapTime->GetMean());
    
    printf("\033[1;32m\u2714\033[0m Done!\n");
    file->Close();
}
