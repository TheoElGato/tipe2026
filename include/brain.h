#ifndef BRAIN_H
#define BRAIN_H

#include <torch/torch.h>
#include <vector>

class Brain {
public:
    Brain(int input_size, int hidden_size, int output_size);
    void test();
    //torch::Tensor forward(torch::Tensor x);
    //void train(const std::vector<torch::Tensor>& inputs, const std::vector<torch::Tensor>& targets);
private:
    //torch::nn::Linear fc1, fc2;
    //torch::nn::ReLU relu;
};

#endif // BRAIN_H