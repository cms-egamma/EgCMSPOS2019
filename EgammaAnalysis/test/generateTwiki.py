#!/usr/bin/env python

import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='prints E/gamma pat::Electrons/Photons us')
#    parser.add_argument('body',help='body filename')
    parser.add_argument('--ex1',help='ex1 filename')
    parser.add_argument('--ex2',help='ex2 filename')
    parser.add_argument('--ex3',help='ex3 filename')
    parser.add_argument('--ex4',help='ex4 filename')
    parser.add_argument('--ex5',help='ex5 filename')
    parser.add_argument('--ex6',help='ex6 filename')
    args = parser.parse_args()

    with open(args.ex1) as f:
        ex1 = f.read()
    with open(args.ex2) as f:
        ex2 = f.read()
    with open(args.ex3) as f:
        ex3 = f.read()
    if args.ex4:
        with open(args.ex4) as f:
            ex4 = f.read()
    else:
        ex4 = ""
    if args.ex5:
        with open(args.ex5) as f:
            ex5 = f.read()
    else:
        ex5 = ""

    if args.ex6:
        with open(args.ex6) as f:
            ex6 = f.read()
    else:
        ex6 = ""


    body_txt="""
---+ CMS Physics Object School 2019 in Aachen: E/gamma long exercise
<noautolink> 

Tutors: Sam James Harper and Swagata Mukherjee

Electrons and photons play a crucial role in the success of CMS. They were vital in the discovery of the Higgs boson and play an important role in searches for new physics, including many models of supersymmetry. The design of the CMS detector, featuring a high-resolution electromagnetic calorimeter (ECAL) and an excellent tracker, provide clean identification and good energy and momentum resolution of these physics objects. In this exercise, we will examine electron and photon identification and study how these objects are used in physics analyses.

The attendees are assumed to have basic knowledge of ROOT, C++, Python.

Electrons and photons are particles which release most of their energy in the electromagnetic calorimeter (ECAL), so the main detector involved in their reconstruction is ECAL. However because of the tracker material in front of ECAL, electrons start radiating (bremsstrahlung) in many cases before reaching ECAL so special algorithms are involved to reconstruct electrons which radiates photons (electron tracks change curvature moving through the tracker layers) or photons which convert.

Exercises are below:

{ex1}

{ex2}

{ex3}

{ex4}

{ex5}

{ex6}

 </noautolink>

""".format(ex1=ex1,ex2=ex2,ex3=ex3,ex4=ex4,ex5=ex5,ex6=ex6)

    print body_txt
