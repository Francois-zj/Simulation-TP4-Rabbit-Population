# Simulation TP4 – Croissance d'une population de lapins

Ce dépôt contient le code source du TP4 de simulation portant sur l'évolution d'une population de lapins.

Le travail est divisé en deux parties :

1. un modèle simple basé sur la suite de Fibonacci ;
2. un modèle stochastique plus réaliste basé sur des individus.

## Fichiers

### `fibonacci_rabbits.c`

Ce programme simule le modèle simple de Fibonacci.

Hypothèses principales :

- le pas de temps est d'un mois ;
- l'unité est un couple de lapins ;
- un jeune couple devient adulte après un mois ;
- un couple adulte produit un nouveau jeune couple ;
- aucune mortalité n'est prise en compte.

Le nombre total de couples suit alors la suite :

`1, 1, 2, 3, 5, 8, 13, ...`

---

### `rabbit_population.c`

Ce programme simule une population de lapins avec une approche stochastique basée sur les individus.

Chaque lapin possède notamment :

- un âge ;
- un sexe ;
- un âge de maturité sexuelle ;
- un état vivant ou mort.

Le modèle prend en compte :

- la reproduction ;
- la taille des portées ;
- le sexe des nouveaux lapins ;
- la maturité sexuelle ;
- la mortalité en fonction de l'âge ;
- plusieurs expériences indépendantes ;
- le risque d'extinction de la population.

Certaines règles probabilistes correspondent à des choix de modélisation réalisés dans le cadre du TP.

## Choix de modélisation principaux

- pas de temps : 1 mois ;
- sexe à la naissance : 50 % mâle / 50 % femelle ;
- taille d'une portée : entre 3 et 6 lapereaux ;
- maturité sexuelle : entre 5 et 8 mois ;
- nombre de portées par an : entre 3 et 9, avec une probabilité plus élevée pour 5, 6 et 7 portées ;
- survie annuelle des jeunes : 35 % ;
- survie annuelle des adultes : 60 % ;
- diminution progressive du taux de survie à partir de 10 ans ;
- survie nulle à 15 ans.

Les probabilités annuelles de survie sont converties en probabilités mensuelles afin de conserver un pas de temps d'un mois.

## Compilation

Avec GCC :

```bash
gcc fibonacci_rabbits.c -o fibonacci_rabbits
gcc rabbit_population.c -o rabbit_population -lm
