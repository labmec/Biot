#ifndef TPZNAME_H
#define TPZNAME_H

#include <string>

class TPZName {
public:
    TPZName();
    TPZName(const std::string& name);
    
    void setName(const std::string& name);
    std::string getName() const;
    
private:
    std::string m_name;
};

#endif 
