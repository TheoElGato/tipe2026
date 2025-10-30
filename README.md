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

## Note additionnel

**MP2I >> MPSI**
