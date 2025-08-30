#include "brain.h"

Brain::Brain(int input_size, int hidden_size, int output_size) {
    test();
}


void Brain::test() {
  torch::Tensor tensor = torch::rand({2, 3});
  std::cout << tensor << std::endl;
}