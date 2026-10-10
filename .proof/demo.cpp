#include "telemetry.hpp"
#include <cassert>
#include <iostream>
#include <limits>
int main() {
 SignalNormalizer normalizer({0,1,0,3e38f,"bar"});
 bool rejected=false;
 try {normalizer.normalize("dev","sensor",1,1,2);} catch(...) {rejected=true;}
 assert(rejected);
 auto good=normalizer.normalize("dev","sensor",1,2,0.5f); assert(good.quality==SignalQuality::Good);
 good.value=std::numeric_limits<float>::infinity(); rejected=false;
 try {serialize(good);} catch(...) {rejected=true;}
 assert(rejected);
std::cout << "calibration overflow rejected; finite serialization enforced\n";
}
