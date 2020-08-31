# CMSDAS2020
Repository for BTaggingExercise for CMSDAS2020 
The exercise shows the usage of BTagCalibrationReader standalone.
## Installation
```
$export SCRAM_ARCH=slc7_amd64_gcc700

$cmsrel CMSSW_10_2_18
$CMSSW_10_2_18/src/
$cmsenv

$git clone https://github.com/Soureek89/CMSDAS2020 CMSDAS2020/

$scram b -j9

$cd CMSDAS2020/BTaggingExercise/
```
## Running

The b-tagging calibration weight is calculated via the `BTagWeight` class defined in `BTagWeight.cc` using the efficiency histograms from `EffHistos/`

`Selection.C` runs selection criteria on the input samples and it is run using

```
$source process_all.sh 
```
The job logs are stored in `jobs/` directory.

The input sample Files can be found in `root://cms-xrd-global.cern.ch//store/user/smitra/CMSDAS2020/`

`make_plot.C` script can be used to check Data-MC comparison plots with 2 different b-tagging calibration methods, discriminant reshaping (2b) and fixed WP-based (1a) calibration from `https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagSFMethods`

```
root -l make_plot.C(<variable-string-to-plot>,<calibration-tuning-index>)
```
