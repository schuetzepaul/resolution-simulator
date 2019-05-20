// Simon Spannagel (DESY) January 2016

#include "TCanvas.h"
#include "TProfile.h"
#include "TString.h"
#include "TFile.h"

#include "assembly.h"
#include "propagate.h"
#include "materials.h"
#include "constants.h"
#include "log.h"

using namespace std;
using namespace gblsim;
using namespace unilog;

int main(int argc, char* argv[]) {

  /*
   * Telescope resolution simulation for the DATURA telescope at the DESY TB21 beam line
   * Six MIMOSA26 planes with 150mm spacing, intrinsic sensor resolution 3.4um
   * DUT with variable thickness (scan)
   */

  
  Log::ReportingLevel() = Log::FromString("INFO");

  for (int i = 1; i < argc; i++) {
    // Setting verbosity:
    if (std::string(argv[i]) == "-v") { 
      Log::ReportingLevel() = Log::FromString(std::string(argv[++i]));
      continue;
    } 
  }

  TFile * out = TFile::Open("datura-resolution.root","RECREATE");
  gDirectory->pwd();

  TCanvas *c1 = new TCanvas("c1","resolution",700,700);
  TProfile *resolution = new TProfile("resolution","Res Vs Energy",40,0.,8.);

  //----------------------------------------------------------------------------
  // Preparation of the telescope and beam properties:

  // MIMOSA26 telescope planes consist of 50um silicon plus 2x25um Kapton foil only:
  double MIM26 = 55e-3 / X0_Si + 50e-3 / X0_Kapton;
  // The intrinsic resolution has been measured to be around 3.25um:
  double RES = 3.24e-3;

  // M26  M26  M26      DUT      M26  M26  M26
  //  |    |    |        |        |    |    |
  //  |    |    |        |        |    |    |
  //  |<-->|    |<------>|<------>|    |    |
  //   DIST      DUT_DIST DUT_DIST
  
  // Distance between telescope planes in mm:
  double DIST = 20;
  // Distance of telescope arms and DUT assembly:
  double DUT_DIST = 7.5;

  // Beam energy 5 GeV electrons/positrons at DESY:
  double BEAM = 5.0;


  //----------------------------------------------------------------------------
  // Build the trajectory through the telescope device:

  // Build a vector of all telescope planes:
  std::vector<plane> datura;
  double position = 0;
  
  // Upstream telescope arm:
  for(int i = 0; i < 3; i++) {
    position = i*DIST;
    datura.push_back(plane(position,MIM26,true,RES));
  }

  // Downstream telescope arm:
  double positionOffset = 2*DIST + 2*DUT_DIST;
  for(int i = 1; i < 3; i++) {
    position = positionOffset + i*DIST;
    datura.push_back(plane(position,MIM26,true,RES));
  }

  for(BEAM = 1.; BEAM <= 7.; BEAM += 0.2) {

    // Prepare the DUT (no measurement, just scatterer
    plane dut(positionOffset, MIM26, false);
    //plane dut(positionOffset, MIM26, true, RES);

    // Duplicate the planes vector and add the current DUT:
    std::vector<plane> planes = datura;
    planes.push_back(dut);

    // Build the telescope:
    telescope mytel(planes, BEAM);

    // Get the resolution at plane-vector position (x):
    LOG(logRESULT) << "Track resolution at DUT with a beam energy of " << BEAM << " GeV: " << mytel.getResolution(3);
    resolution->Fill(BEAM,mytel.getResolution(3),1);
  }

  c1->cd();
  resolution->SetTitle("DATURA Track Resolution at DUT;Beam Energy [GeV];resolution at DUT #left[#mum#right]");
  //resolution->GetYaxis()->SetRangeUser(1.5,4);
  resolution->SetMarkerStyle(0);
  resolution->SetLineColor(kRed+1);
  resolution->SetLineWidth(2);
  resolution->SetMarkerColor(kRed+1);
  
  resolution->Draw();
  c1->Write();

  // Write result to file
  out->Write();
  return 0;
}
