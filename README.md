# MineSweeper

A terminal Minesweeper in C with an AI assistant that computes the probability
of each hidden cell being a mine.

![Game with AI hints enabled](docs/screenshots/game.png)

With the AI on, hidden cells are colored by risk: **green** is guaranteed safe,
**red** is a guaranteed mine, and **gray** is uncertain.

## Build and run

You need a C compiler and the math library.

```sh
gcc -O2 -o minesweeper minesweeper.c probabilities.c -lm
./minesweeper
```

The board is 16 × 30 with 99 mines (expert). Your first reveal is always safe.

| Key | Action |
|---|---|
| `r x y` | Reveal the cell at row `x`, column `y` |
| `f x y` | Toggle a flag |
| `h` | Toggle AI hints |
| `q` | Quit |

## Comment fonctionne l'IA

Au démineur, il arrive qu'aucune case ne soit sûre à 100 %. L'IA calcule donc, pour
chaque case cachée, la probabilité qu'elle cache une mine, et l'affiche en couleur.

Chaque chiffre révélé donne une règle simple : « parmi mes voisines cachées, il y a
exactement N mines ». L'IA cherche toutes les façons de placer les mines qui
respectent toutes ces règles en même temps. Une case qui a une mine dans 3 cas sur 4
a donc 75 % de chances d'en cacher une.

Tester toutes les combinaisons une par une est beaucoup trop lent (c'est ce que fait le
prototype `proba.py`). Le programme en C utilise deux astuces pour aller plus vite :

1. **Regrouper les cases.** Deux cases cachées qui touchent exactement les mêmes chiffres
   ont forcément la même probabilité. On ne se demande plus « où sont les mines ? » mais
   « combien de mines dans chaque groupe ? », ce qui fait beaucoup moins de cas.
2. **Résoudre les règles comme un système d'équations.** Les règles sont écrites sous
   forme de matrice et simplifiées avec la méthode de Gauss-Jordan. Il ne reste alors
   que quelques inconnues à essayer.

Le [rapport](rapport.pdf) explique la méthode en détail, sur un exemple.

## Layout

- `minesweeper.c`: board, mine placement, display, and game loop.
- `probabilities.c`: the matrix-based probability solver.
- `proba.py`: a brute-force Python prototype of the solver.
- `rapport.pdf`: the project report (in French).
- `rapport/`: LaTeX sources of the report.
