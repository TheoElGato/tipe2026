# tipe2026 - URSAFSIM C++

## Problématique : *Comment optimiser le cycle de marche d'une créature à 4 pattes avec un réseau de neurones ?*


On souhaite créer une simulation à deux dimensions où une créature à 4 pattes cherche à se déplacer jusqu'à un point. La créature est contrôlée par une intelligence artificielle dotée d’un réseau de neurones.
On cherche à faire varier les paramètres variables suivants dans la simulation pour gagner du temps d'entraînement, tout en gardant  : 

##### Paramètres variables :
- Méthode/critère de sélection (distance ou temp pour une distance)
- Score & récompenses/malus
- % de sélection
- Temps de simulation par série (constant ou variable)
- Taille du réseau de neurone (+ petit -> plus rapide)(+ gros -> comportement + complexe)
- Méthode d’initialisation du réseau

##### Paramètres fixes:
- Méthode de reproduction
- Objectif/Distance totale à atteindre
- Point de départ
- Noeuds d’entrée et de sortie du réseau
- Fonction des neurones (Relu, Tangente hyperbolique)
- Créature

##### Variables observables :
- Temps de simulation total
- Méthode utilisé par la créature (allure de marche)
- Facteur aléatoire (répéter plusieur fois sur les même paramètres)

##### Pourquoi URSAF ???
Bonne question. En réalité, URSAF est un acronyme pour Unité de Recherche de Simulation d'Apprentissage de la Foulée.
Je suis sûr que c'est la réponse que vous attendiez.

## Comment build
- Installer cmake.
- Installer toute les dépendances de SFML 2.5.1
- Installer libtorch (cuda est supporté) et donner le chemin à cmake avec `-DCMAKE_PREFIX_PATH=/absolute/path/to/libtorch`
- Utiliser cmake pour build, par exemple :
  `cmake -S . -B build`
  `cmake --build build`
  
Vous pouvez maintenant éxecuter le programme.

> **_NOTE:_** Le répertoire de travail courant doit contenir le dossier assets, un dossier save (Vous devez le créer), et task.json (Utilisez le template, taskDefault.json) 

### Création des json
- Créer un fichier `data.csv` avec les différents paramètres (donner dans le `csv_to_json.py`)
- Lancer `csv_to_json.py` pour obtenir un fichier json utilisable par le projet

### Exploitation des résultats
- Dans tools créer 2 fichiers "saves" et "rapports"
- Mettre les résultats à traiter dans "saves"
- Lancer `rapport.py` pour obtenir un pdf par simulation

## Modules utilisés
- SFML : https://github.com/SFML/SFML
- libtorch : https://github.com/pytorch/pytorch
- JSON for Modern C++ : https://github.com/nlohmann/json
- csv-parser : https://github.com/vincentlaucsb/csv-parser
- websocketpp : https://github.com/zaphoyd/websocketpp
- asio : https://github.com/chriskohlhoff/asio

## Licence et attributions tierces

Ce projet est sous licence Apache, version 2.0. Consultez le fichier « LICENSE » pour obtenir le texte complet.

Copyright :
- elycraft (2025)
- TheoElGato (2025)

Les modules tierces utilisées par ce projet et le texte complet de leurs licences sont rassemblés dans le fichier « THIRD_PARTY_LICENSES ».
Il est également inclut tous les fichiers NOTICE requis par ces modules (NOTICE de PyTorch/libtorch). 

## Note additionnelle

**MP2I >> MPSI**


# English section
## Problematic : *How can to optimize the walking cycle of a four-legged creature using a neural network?*


We want to create a two-dimensional simulation in which a four-legged creature tries to move to a specific point. The creature is controlled by artificial intelligence equipped with a neural network.
We want to vary the following parameters in the simulation to save training time, while keeping:

##### Variable :
- Selection method/criterion (distance or time for a distance)
- Score & rewards/penalties
- Selection percentage
- Simulation time per series (constant or variable)
- Neural network size (+ smaller -> faster)(+ larger -> more complex behavior)
- Network initialization method

##### Fixed parameters:
- Reproduction method
- Goal/Total distance to reach
- Starting point
- Network input and output nodes
- Neuron function (Relu, Hyperbolic tangent)
- Creature

##### Observable variables :
- Total simulated time
- Method used by the creature (walking speed)
- Random factor (repeat several times using the same parameters)

##### Why URSAF ???
Good question. In reality, URSAF is an acronyme for Stride Learning Simulation Research Unit (in french: Unité de Recherche de Simulation d'Apprentissage de la Foulée).
I'm sure this was you're expected answer.

## How to build
- Install cmake.
- Install every dependency of SFML 2.5.1
- Install libtorch (cuda is supported) and give the path to cmake with `-DCMAKE_PREFIX_PATH=/absolute/path/to/libtorch`
- Use cmake to build, for example :
  `cmake -S . -B build`
  `cmake --build build`
  
You can now run the program.

> **_NOTE:_** The current working directory must contain the assets folder, a save folder (you must create it), and task.json (use the template, taskDefault.json). 

### Creation of the json
- Create a `data.csv` file with the various parameters (specified in `csv_to_json.py`)
- Run `csv_to_json.py` to obtain a json file that can be used by the project

### Using the results
- In tools, create two files: “saves” and “reports.”
- Put the results to be processed in “saves.”
- Run `report.py` to obtain a PDF for each simulation.

## Used module
- SFML : https://github.com/SFML/SFML
- libtorch : https://github.com/pytorch/pytorch
- JSON for Modern C++ : https://github.com/nlohmann/json
- csv-parser : https://github.com/vincentlaucsb/csv-parser
- websocketpp : https://github.com/zaphoyd/websocketpp
- asio : https://github.com/chriskohlhoff/asio

## License and third-party attributions

This project is licensed under Apache License, Version 2.0. See the « LICENSE » file for the full text.

Copyright :
- elycraft (2025)
- TheoElGato (2025)

The third-party modules used by this project and the full text of their licenses are collected in the file “THIRD_PARTY_LICENSES.”
It also includes all NOTICE files required by these modules (NOTICE from PyTorch/libtorch). 

