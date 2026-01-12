/*
    brain.hpp

    Description:
        Header file for brain.cpp
        
    Author:
        R. Benichou
        A. Spadone
*/

#ifndef BRAIN_H
#define BRAIN_H

#include <torch/torch.h>
#include <vector>

class Brain : public torch::nn::Module {
public:
    Brain(int input_size, int output_size, std::string file, std::string device, int nb_hidden_neurones);
    torch::Tensor forward(torch::Tensor x);
    void saveFile(const std::string& file);
    void loadFile(const std::string& file);
    void mutate(float weight);
    int bid1 = 0;
    int bid2 = 0;
    Brain copy();


private:

    torch::nn::Linear fc1 = nullptr, fc2 = nullptr, fc3 = nullptr, out = nullptr;
    int input_size;
    int output_size;
    int nb_hidden_neurones;
    int links;
    void manual_init();
    torch::Device device = torch::kCPU;
};

#endif // BRAIN_H
