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


