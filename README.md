# 🌍 Planet Simulator (C++)

Un simulateur de système planétaire écrit en **C++**, basé sur la gravitation newtonienne.  
Le projet permet de simuler l’évolution dynamique de plusieurs corps soumis à leurs interactions gravitationnelles.

---

## 🚀 Objectif

L’objectif du projet est de :

- modéliser un système N-corps
- implémenter une gravitation réaliste
- observer la stabilité (ou l’instabilité) des orbites
- expérimenter différents paramètres physiques et numériques

Le simulateur vise la **compréhension du comportement des systèmes gravitationnels**, pas la reproduction exacte du système solaire réel.

---

## ⚙️ Fonctionnalités

- Simulation gravitationnelle N-corps
- Lois de Newton
- Mise à jour des positions et vitesses
- Pas de temps discret
- Simulation déterministe
- Architecture simple et lisible

---

## 🧠 Modèle physique

Chaque planète est modélisée par :

- une masse
- une position
- une vitesse

La force exercée entre deux corps est donnée par :

\[
\vec{F}_{ij} = G \frac{m_i m_j}{r_{ij}^2} \hat{u}_{ij}
\]

où :
- \(G\) est la constante gravitationnelle
- \(r_{ij}\) la distance entre les deux corps

L’accélération est ensuite intégrée numériquement à chaque pas de temps.

---

## 🧮 Intégration numérique

Le simulateur utilise un schéma d’intégration discret :

- calcul des forces
- calcul des accélérations
- mise à jour des vitesses
- mise à jour des positions

> Le choix d’un pas de temps trop grand peut entraîner une instabilité des orbites.

---

## 📁 Structure du projet

```txt
.
├── src/            # code source
├── include/        # headers
├── main.cpp
└── README.md
