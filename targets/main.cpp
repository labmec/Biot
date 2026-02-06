#include <iostream>
#include "TPZName.h"

// Include NeoPZ headers (example)
// #include <pz.h>

int main() {
    std::cout << "=== Default Project with NeoPZ ===" << std::endl;
    
    // Create and use TPZName class from libsource
    TPZName myObject("NeoPZ Project");
    std::string ObjectName = myObject.getName();
    
    std::cout << "Project compiled and ran successfully!" << std::endl;
    
    return 0;
}
