#!/bin/sh
../mefa small_fdr_power2.tif small_fac_power2.tif
../mefa -e taudem small_fdr_taudem.tif small_fac_taudem.tif
../mefa -e 45degree small_fdr_45degree.tif small_fac_45degree.tif
../mefa -e degree small_fdr_degree_int.tif small_fac_degree_int.tif
../mefa -e degree small_fdr_degree_double.tif small_fac_degree_double.tif
../mefa -e 1,8,7,6,5,4,3,2 small_fdr_taudem.tif small_fac_taudem_custom.tif
echo
if [ $(md5sum small_fac_*.tif | sed 's/ .*//' | uniq | wc -l) -eq 1 ]; then
	echo "PASSED!"
else
	echo "FAILED..."
fi
rm -f small_fac_*.tif
