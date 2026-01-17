/*
    brain.cpp

    Description:
        This file is responsible for all neural networking work.
        In particular, this uses CPP implementation of Pythorch : libtorch
        to deal with neural network.

        A Brain class is created to hold a neural network, initialise it,
        saving and loading it, mutate it and get data from it.

    Author:
        R. Benichou
        A. Spadone
*/

#include "brain.hpp"

/*
 * Constructor for the Brain class
 * @param input_size The size of the input layer
 * @param output_size The size of the output layer
 * @param file If not a empty string, the brain file to load
 * @param nb_hidden_layer The number of hidden layer for a brain
 */
Brain::Brain(int input_size, int output_size, std::string file, std::string device, int nb_hidden_neurones) {

    // Define useful variable
    this->input_size = input_size;
    this->output_size = output_size;
    this->nb_hidden_neurones = nb_hidden_neurones;
    this->links = (input_size * nb_hidden_neurones) + (nb_hidden_neurones * nb_hidden_neurones) + (nb_hidden_neurones * nb_hidden_neurones) + (nb_hidden_neurones * output_size);

    // Define the layers
    this->fc1 = register_module("fc1", torch::nn::Linear(input_size, nb_hidden_neurones));
    this->fc2 = register_module("fc2", torch::nn::Linear(nb_hidden_neurones, nb_hidden_neurones));
    this->fc3 = register_module("fc3", torch::nn::Linear(nb_hidden_neurones, nb_hidden_neurones));
    this->out = register_module("out", torch::nn::Linear(nb_hidden_neurones, output_size));

    // Set device (CPU or CUDA)
    if (device == "cuda" && torch::cuda::is_available()) {
        this->device = torch::kCUDA;
        this->to(this->device);
    } else {
        this->device = torch::kCPU;
        this->to(this->device);
    }

    // Manuel seed and initial randomness
    auto seed = static_cast<unsigned>(
        std::chrono::high_resolution_clock::now()
            .time_since_epoch()
            .count()
        + reinterpret_cast<uintptr_t>(this)
    );
    torch::manual_seed(seed);
    if (this->device.is_cuda()) {
        torch::cuda::manual_seed_all(seed);
    }

    manual_init();

    // Load model from file if provided
    if (file!="") {
        try {
            this->loadFile(file);
            //this->nb_hidden_neurones = this->fc1->weight.size(0);
        } catch (const c10::Error& e) {
            std::cerr << "Error loading the model from " << file << ": " << e.what() << std::endl;
        }
    }
}

/*
 * Save the weight and bias using to the designated file
 * @param file
 */
void Brain::saveFile(const std::string& file) {
    torch::serialize::OutputArchive archive;
    this->save(archive);
    archive.save_to(file);
}

/*
 * Load the weight and bias using the provided file
 * @param file
 */
void Brain::loadFile(const std::string& file) {
    torch::serialize::InputArchive archive;
    archive.load_from(file);
    this->load(archive);
}

/*
 * Calculate the ouput of the brain with the provided tensor.
 * It use the softsing activation function
 * @param x The tensor input
 * @return x The ouput of the brain as a tensor
 */
torch::Tensor Brain::forward(torch::Tensor x) {
    x = x.to(this->device);
    x = torch::nn::functional::softsign(this->fc1->forward(x));
    x = torch::nn::functional::softsign(this->fc2->forward(x));
    x = torch::nn::functional::softsign(this->fc3->forward(x));
    x = torch::nn::functional::softsign(this->out->forward(x));
    return x;
}

/*
 * Change randomly the weight and bias of the brain.
 * @param weight of the modification
 */
void Brain::mutate(float weight) {
    torch::NoGradGuard no_grad;
    this->fc1->weight += weight * torch::randn_like(this->fc1->weight);
    this->fc1->bias += weight * torch::randn_like(this->fc1->bias);
    this->fc2->weight += weight * torch::randn_like(this->fc2->weight);
    this->fc2->bias += weight * torch::randn_like(this->fc2->bias);
    this->fc3->weight += weight * torch::randn_like(this->fc3->weight);
    this->fc3->bias += weight * torch::randn_like(this->fc3->bias);
    this->out->weight += weight * torch::randn_like(this->out->weight);
    this->out->bias += weight * torch::randn_like(this->out->bias);
}

/*
 * This copy the associated brain.
 * This allow to not have referencing issues
 * @return new_brain the copy of the brain
 */
Brain Brain::copy() {
    std::string device;
    if (this->device == torch::kCUDA){
        device = "cuda";
    } else {
        device = "cpu";
    }

    Brain new_brain(this->input_size, this->output_size, "", device, this->nb_hidden_neurones);
    torch::NoGradGuard no_grad;
    new_brain.fc1->weight.copy_(this->fc1->weight.detach().clone());
    new_brain.fc1->bias.copy_(this->fc1->bias.detach().clone());
    new_brain.fc2->weight.copy_(this->fc2->weight.detach().clone());
    new_brain.fc2->bias.copy_(this->fc2->bias.detach().clone());
    new_brain.fc3->weight.copy_(this->fc3->weight.detach().clone());
    new_brain.fc3->bias.copy_(this->fc3->bias.detach().clone());
    new_brain.out->weight.copy_(this->out->weight.detach().clone());
    new_brain.out->bias.copy_(this->out->bias.detach().clone());
    return new_brain;
}

/*
 * Do a manual random initialization for all layers
 */
void Brain::manual_init() {
    torch::NoGradGuard no_grad;

    fc1->weight.copy_(torch::randn_like(fc1->weight));
    fc1->bias.copy_(torch::randn_like(fc1->bias));

    fc2->weight.copy_(torch::randn_like(fc2->weight));
    fc2->bias.copy_(torch::randn_like(fc2->bias));

    fc3->weight.copy_(torch::randn_like(fc3->weight));
    fc3->bias.copy_(torch::randn_like(fc3->bias));

    out->weight.copy_(torch::randn_like(out->weight));
    out->bias.copy_(torch::randn_like(out->bias));
}
