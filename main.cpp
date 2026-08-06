// cd ...eic/dircGR
// mkdir -p build && cd build
// cmake ..
// cmake --build .

#include <iostream>
#include "TString.h"
#include "dircGR.h"

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cerr<<"Usage: "<<argv[0]<<" <steerstr> <geostr>"<<std::endl;
        return 1;
    }

    TString steerstr	= argv[1];
    TString   geostr	= argv[2];
    dircGR* analyzer	= new dircGR(steerstr.Data(),geostr.Data());
		    analyzer	->Loop();

    return 0;
}
