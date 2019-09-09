#!/usr/bin/env python
from array import array
import argparse
import sys
from DataFormats.FWLite import Events, Handle
import ROOT

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
