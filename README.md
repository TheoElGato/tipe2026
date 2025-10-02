# tipe2026 - URSAFSIM C++

**Problématique** : *Comment optimiser le cycle de marche d'une créature à 4 pattes avec un réseau de neurones ?*


On souhaite créer une simulation de deux dimensions ou une créature à 4 pattes cherche à parcourir un point à une distance de 10m. La créature est contrôlée par une intelligence artificielle dotée d’un réseau de neurones.
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


**Module utilisé**
- SFML
- libtorch


**MP2I >> MPSI**
