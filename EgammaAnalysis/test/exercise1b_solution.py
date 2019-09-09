#!/usr/bin/env python
from array import array
import argparse
import sys
from DataFormats.FWLite import Events, Handle
import ROOT
import itertools

if __name__ == "__main__":

    oldargv = sys.argv[:]
    sys.argv = [ '-b-' ]
    #ROOT.gROOT.SetBatch(True)
    sys.argv = oldargv
    ROOT.gSystem.Load("libFWCoreFWLite.so");
    ROOT.gSystem.Load("libDataFormatsFWLite.so");
    ROOT.FWLiteEnabler.enable()

    parser = argparse.ArgumentParser(description='prints E/gamma pat::Electrons/Photons us')
    parser.add_argument('in_filename',help='input filename')
    args = parser.parse_args()
    
    eles_handle, ele_label = Handle("std::vector<pat::Electron>"), "slimmedElectrons"
    phos_handle, pho_label = Handle("std::vector<pat::Photon>"), "slimmedPhotons"

    events = Events(args.in_filename)

    pho_sigmaietaieta_hist = ROOT.TH1D("phoSigmaIEtaIEtaHist",";#sigma_{i#etai#eta};#entries",100,0,0.05)
    ele_sigmaietaieta_hist = ROOT.TH1D("eleSigmaIEtaIEtaHist",";#sigma_{i#etai#eta};#entries",100,0,0.05)

    ele_mass_hist = ROOT.TH1D("eleMassHist",";m(ee) [GeV];#entries",70,50,120)
    pho_mass_hist = ROOT.TH1D("phoMassHist",";m(ee) [GeV];#entries",70,50,120)

    #now we'll loop over the events rather than one by one
    for event_nr,event in enumerate(events):
        event.getByLabel(ele_label,eles_handle)
        event.getByLabel(pho_label,phos_handle)

        eles = eles_handle.product()
        phos = phos_handle.product()

        for pho_nr,pho in enumerate(phos):  
            if pho.et()>=20. and pho.photonID("cutBasedPhotonID-Fall17-94X-V1-medium")==1.0:
                pho_sigmaietaieta_hist.Fill(pho.full5x5_sigmaIetaIeta())


        for ele_nr,ele in enumerate(eles):
            if ele.et()>=20. and ele.electronID("cutBasedElectronID-Fall17-94X-V2-medium")==1.0:
                ele_sigmaietaieta_hist.Fill(ele.full5x5_sigmaIetaIeta())

        for ele_pair in itertools.combinations(eles,2):
            if ele_pair[0].et()>=20. and ele_pair[0].electronID("cutBasedElectronID-Fall17-94X-V2-medium")==1.0 and ele_pair[1].et()>=20. and ele_pair[1].electronID("cutBasedElectronID-Fall17-94X-V2-medium")==1.0:
                ele_mass_hist.Fill((ele_pair[0].polarP4()+ele_pair[1].polarP4()).mag())

        for pho_pair in itertools.combinations(phos,2):
            if pho_pair[0].et()>=20. and pho_pair[0].photonID("cutBasedPhotonID-Fall17-94X-V1-medium")==1.0 and pho_pair[1].et()>=20. and pho_pair[1].photonID("cutBasedPhotonID-Fall17-94X-V1-medium")==1.0:
                pho_mass_hist.Fill((pho_pair[0].polarP4()+pho_pair[1].polarP4()).mag())

    """
    now you can access the variables of an electron/photon interactively
    
    events.to(0) #go to 1st event in the file
    events.getByLabel(ele_label,eles_handle)
    
    eles_handle.product() = vector of pat::Electrons
    
    plot try plotting some variables 
    eles = eles_handle.product()
    eles[0].energy() #energy of the first electron
    help(...) will give you useful messages
    
    """
