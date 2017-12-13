class ChopDetector{
public:
    int predict(double features[]);
private:
    double intercepts[1] = {0.605390303176};
    int weights[2] = {1318, 487};
    #include "model_properties"
};