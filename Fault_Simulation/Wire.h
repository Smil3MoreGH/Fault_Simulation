#pragma once
#include <string>

class Wire {
public:
    Wire(const std::string& name); // Constructor
    ~Wire(); // Destructor

    // Getter and Setter for the value
    bool getValue() const;
    void setValue(bool val);

    // Getter for the name
    const std::string& getName() const;
    
    void setFault(bool isFaulted, bool faultValue);
    void clearFault();
    
private:
    std::string name;
    bool value;
    bool isFaulted = false;
    bool faultValue;
};