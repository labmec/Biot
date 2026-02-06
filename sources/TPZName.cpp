#include "TPZName.h"
#include <iostream>

TPZName::TPZName() : m_name("Unnamed") {}

TPZName::TPZName(const std::string& name) : m_name(name) {}

void TPZName::setName(const std::string& name) {
    m_name = name;
}

std::string TPZName::getName() const {
    return m_name;
}

void TPZName::printName() const {
    std::cout << "Name: " << m_name << std::endl;
}
