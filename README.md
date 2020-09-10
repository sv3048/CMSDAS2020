# CMSDAS2020
Repository for BTaggingExercise for CMSDAS2020

## Installation
```
$export SCRAM_ARCH=slc7_amd64_gcc700

$cmsrel CMSSW_1CMSSW_10_6_16/
$cd CMSSW_10_6_16/src/
$cmsenv

$git clone https://github.com/Soureek89/CMSDAS2020 CMSDAS2020/

$scram b -j9

$cd CMSDAS2020/BTaggingExercise/
```
## Running
The b-tagging calibration SF is calculated via the `BTagWeight` class defined in `BTagWeight.cc` using the efficiency histograms from `EffHistos/`

`Selection.C` runs selection criteria on the input samples. Before running on all the samples, it is required to create a library using ROOT as follows.
```
$root -l -b
root [0] .L Selection.C+g
root [1] .q
$
```
This creates a shared object `Selection_C.so` locally and related dictionaries. To run on all processes now type the following command.
```
$source process_all.sh &>master.log &
```
It takes around 20-25 mins for the jobs to complete. For information regarding the overall progress refer to the `master.log`, while individual job logs are stored in `logs/` directory.
  
The input sample files can be found in `/afs/cern.ch/work/s/spmondal/public/BTV_CMSDAS2020/samples/`

`make_plot.C` script can be used to check Data-MC comparison plots with two different b-tagging calibration methods, namely 1a) fixed WP-based (index =1) and 2b) discriminant reshaping (index=2) from https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagSFMethods . Also the plots w/o  applying the SFs can be obtained by switching to index=0.     

```
root -l "make_plot.C+(\"allJetsDeepFlavB\",1)"
```
One can replace the variable and the index in the above line to get plots for other observables and calibration methods. Compare the DeepJet score for the leading (first) jet using the SFs due to the two calibration methods mentioned above and also w/o any calibration SFs.

Please fill up the following feedback form after you complete the exercise.

https://docs.google.com/forms/d/e/1FAIpQLSeRQ1tZGhfS8pnWr0at-uaDeYwbLBuGmgk9dSUzUJIaM-GvlQ/viewform
