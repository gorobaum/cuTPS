#!/bin/bash

for i in {1..10};
  do ./tps ../3dconfigurations/config3Dcpu.txt >> resultscpu.txt;
done

for i in {1..10};
  do ./tps ../3dconfigurations/config3Dcpuinter.txt >> resultscpuInter.txt;
done

for i in {1..10};
  do ./tps ../3dconfigurations/config3DallOff.txt >> resultsAllOff.txt;
done

for i in {1..10};
  do ./tps ../3dconfigurations/config3Dparallel.txt >> resultsParallel.txt;
done

for i in {1..10};
  do ./tps ../3dconfigurations/config3Dtexture.txt >> resultsTexture.txt;
done

for i in {1..10};
  do ./tps ../3dconfigurations/config3Doccupancy.txt >> resultsOccupancy.txt;
done

for i in {1..10};
  do ./tps ../3dconfigurations/config3DoccAndPll.txt >> resultsOccAndPll.txt;
done

for i in {1..10};
  do ./tps ../3dconfigurations/config3DoccAndTex.txt >> resultsOccAndTex.txt;
done

for i in {1..10};
  do ./tps ../3dconfigurations/config3DpllAndTex.txt >> resultspllAndTex.txt;
done

for i in {1..10};
  do ./tps ../3dconfigurations/config3DallOn.txt >> resultsAllOn.txt;
done
