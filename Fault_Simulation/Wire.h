#pragma once
#include <string>

class Wire {
public:
    Wire(const std::string& name);
    ~Wire();
    
    bool getValue() const;
    void setValue(bool val);
    
    const std::string& getName() const;
    
    void setFault(bool isFaulted, bool faultValue);
    void clearFault();
    
private:
    std::string name;
    bool value;
    bool isFaulted = false;
    bool faultValue;
};