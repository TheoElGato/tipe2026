# tipe2026 - URSAFSIM C++

**Problématique** : *Comment optimiser le cycle de marche d'une créature à 4 pattes avec un réseau de neurones ?*


On souhaite créer une simulation à deux dimensions où une créature à 4 pattes cherche à se déplacer jusqu'à un point. La créature est contrôlée par une intelligence artificielle dotée d’un réseau de neurones.
On cherche à faire varier les paramètres variables suivants dans la simulation pour gagner du temps d'entraînement, tout en gardant  : 

**Paramètres variables :**
- Méthode/critère de sélection (distance ou temp pour une distance)
- Score & récompenses/malus
- % de sélection
- Temps de simulation par série (constant ou variable)
- Taille du réseau de neurone (+ petit -> plus rapide)(+ gros -> comportement + complexe)
- Méthode d’initialisation du réseau

**Paramètres fixes:**
- Méthode de reproduction
- Objectif/Distance totale à atteindre
- Point de départ
- Noeuds d’entrée et de sortie du réseau
- Fonction des neurones (Relu, Tangente hyperbolique)
- Créature

**Variables observables :**
- Temps de simulation total
- Méthode utilisé par la créature (allure de marche)
- Facteur aléatoire (répéter plusieur fois sur les même paramètres)


**Modules utilisé**
- SFML : https://github.com/SFML/SFML
- libtorch : https://github.com/pytorch/pytorch
- JSON for Modern C++ : https://github.com/nlohmann/json
- csv-parser : https://github.com/vincentlaucsb/csv-parser
- websocketpp : https://github.com/zaphoyd/websocketpp
- asio : https://github.com/chriskohlhoff/asio


**MP2I >> MPSI**
