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
    void mutate(float a);


    //void train(const std::vector<torch::Tensor>& inputs, const std::vector<torch::Tensor>& targets);
private:
    void save(torch::serialize::OutputArchive& archive) const override;
    void load(torch::serialize::InputArchive& archive) override;

    torch::nn::Linear fc1 = nullptr, fc2 = nullptr, fc3 = nullptr, out = nullptr;
    //torch::nn::ReLU relu;
    int input_size;
    int output_size;
    int nb_hidden_neurones;
    int links;
    torch::Device device = torch::kCPU;
};

#endif // BRAIN_H