#include "brain.h"

Brain::Brain(int input_size, int output_size, std::string file, std::string device, int nb_hidden_neurones)
{

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

void Brain::saveFile(const std::string& file) {
    torch::serialize::OutputArchive archive;
    this->save(archive);
    archive.save_to(file);
}

void Brain::loadFile(const std::string& file) {
    torch::serialize::InputArchive archive;
    archive.load_from(file);
    this->load(archive);
}

torch::Tensor Brain::forward(torch::Tensor x) {
    x = x.to(this->device);
    x = torch::nn::functional::softsign(this->fc1->forward(x));
    x = torch::nn::functional::softsign(this->fc2->forward(x));
    x = torch::nn::functional::softsign(this->fc3->forward(x));
    x = torch::nn::functional::softsign(this->out->forward(x));
    return x;
}

void Brain::mutate(float valeur) {
    torch::NoGradGuard no_grad;
    this->fc1->weight += valeur * torch::randn_like(this->fc1->weight);
    this->fc1->bias += valeur * torch::randn_like(this->fc1->bias);
    this->fc2->weight += valeur * torch::randn_like(this->fc2->weight);
    this->fc2->bias += valeur * torch::randn_like(this->fc2->bias);
    this->fc3->weight += valeur * torch::randn_like(this->fc3->weight);
    this->fc3->bias += valeur * torch::randn_like(this->fc3->bias);
    this->out->weight += valeur * torch::randn_like(this->out->weight);
    this->out->bias += valeur * torch::randn_like(this->out->bias);
}

Brain Brain::copy() {
    Brain new_brain(this->input_size, this->output_size, "", "cpu", this->nb_hidden_neurones);
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

void Brain::manual_init() {
    // Manual random initialization for all layers
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

