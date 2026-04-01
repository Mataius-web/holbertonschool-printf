# holbertonschool-printf

> Une réimplémentation personnalisée de la fonction `printf` de la bibliothèque standard C, réalisée dans le cadre du cursus **Holberton School**.

---

## Table des matières

1. [Description du projet](#1-description-du-projet)
2. [Contexte pédagogique](#2-contexte-pédagogique)
3. [Prérequis et environnement](#3-prérequis-et-environnement)
4. [Structure des fichiers](#4-structure-des-fichiers)
5. [Le fichier d'en-tête `main.h`](#5-le-fichier-den-tête-mainh)
6. [Analyse détaillée de `_printf.c`](#6-analyse-détaillée-de-_printfc)
   - 6.1 [La fonction `print_number`](#61-la-fonction-print_number)
   - 6.2 [La fonction `_printf`](#62-la-fonction-_printf)
7. [Spécificateurs de format supportés](#7-spécificateurs-de-format-supportés)
8. [Algorithme pas à pas](#8-algorithme-pas-à-pas)
9. [Cas limites et comportements particuliers](#9-cas-limites-et-comportements-particuliers)
10. [Compilation](#10-compilation)
11. [Exemples d'utilisation](#11-exemples-dutilisation)
12. [Valeurs de retour](#12-valeurs-de-retour)
13. [Différences avec `printf` standard](#13-différences-avec-printf-standard)
14. [Auteurs](#14-auteurs)

---

## 1. Description du projet

`_printf` est une réimplémentation partielle de la fonction `printf` définie dans `<stdio.h>` de la bibliothèque standard C (libc). L'objectif est de comprendre et de reproduire le mécanisme interne de formatage et d'affichage de texte en C, **sans utiliser `printf` ni `sprintf`**.

La fonction lit une chaîne de format caractère par caractère. Lorsqu'elle rencontre un `%` suivi d'un spécificateur (comme `c`, `s`, `d`, `i` ou `%`), elle récupère l'argument correspondant dans la liste d'arguments variables (`va_list`) et l'affiche sur la sortie standard (`stdout`, descripteur de fichier 1) via l'appel système `write`.

---

## 2. Contexte pédagogique

Ce projet fait partie du cursus de **première année** de Holberton School. Il a pour buts de :

- Comprendre les **listes d'arguments variables** en C (`stdarg.h` : `va_list`, `va_start`, `va_arg`, `va_end`).
- Comprendre le fonctionnement interne de l'affichage formaté.
- Maîtriser l'appel système bas niveau `write(2)` pour écrire sur la sortie standard.
- Pratiquer la **récursivité** (utilisée pour l'affichage des entiers).
- Respecter les normes de codage imposées par Betty (style de code Holberton).

---

## 3. Prérequis et environnement

| Élément | Détail |
|---|---|
| Système d'exploitation | Ubuntu 20.04 LTS (ou toute distribution Linux/Unix) |
| Compilateur | `gcc` version 9.x ou supérieure |
| Standard C | C89/C90 (`-std=c89`) ou C99 |
| Éditeur | `vi`, `vim`, `emacs`, ou tout éditeur de texte |
| Fichiers autorisés | `_printf.c`, `main.h`, `README.md` |
| Bibliothèques externes | Aucune (la bibliothèque standard C est utilisée uniquement pour `stdarg.h`, `unistd.h` et `stddef.h`) |
| Fonction `printf` standard | **Interdite** à l'intérieur de l'implémentation |

---

## 4. Structure des fichiers

```
holbertonschool-printf/
├── _printf.c   # Contient l'implémentation de _printf et print_number
├── main.h      # Fichier d'en-tête : prototypes des fonctions et includes
└── README.md   # Documentation complète du projet
```

### `_printf.c`

C'est le cœur du projet. Ce fichier contient deux fonctions :

1. **`print_number(int n)`** — fonction auxiliaire récursive qui affiche un entier signé sur stdout.
2. **`_printf(const char *format, ...)`** — la fonction principale qui parse la chaîne de format et dispatche vers les routines d'affichage appropriées.

### `main.h`

Fichier d'en-tête qui déclare les prototypes de toutes les fonctions du projet. Il est protégé par un **include guard** (`#ifndef MAIN_H` / `#define MAIN_H` / `#endif`) pour éviter les inclusions multiples.

---

## 5. Le fichier d'en-tête `main.h`

```c
#ifndef MAIN_H
#define MAIN_H

#include <stdarg.h>
#include <stddef.h>

int _printf(const char *format, ...);
int print_number(int n);

#endif
```

### Détail ligne par ligne

| Ligne | Explication |
|---|---|
| `#ifndef MAIN_H` | Vérifie si la macro `MAIN_H` n'est pas encore définie (protection contre les inclusions multiples). |
| `#define MAIN_H` | Définit la macro `MAIN_H` pour bloquer toute inclusion suivante du même fichier. |
| `#include <stdarg.h>` | Inclut le fichier d'en-tête permettant de gérer les listes d'arguments variables (`va_list`, `va_start`, `va_arg`, `va_end`). |
| `#include <stddef.h>` | Inclut le fichier d'en-tête définissant `size_t`, `NULL` et autres types/macros fondamentaux. |
| `int _printf(const char *format, ...);` | Prototype de la fonction principale. Le `...` indique une liste d'arguments variables (variadique). |
| `int print_number(int n);` | Prototype de la fonction auxiliaire d'affichage d'entiers. |
| `#endif` | Ferme le bloc `#ifndef`. |

---

## 6. Analyse détaillée de `_printf.c`

### 6.1 La fonction `print_number`

```c
int print_number(int n)
{
    int count = 0;
    char c;

    if (n == -2147483648)
    {
        write(1, "-2147483648", 11);
        return 11;
    }

    if (n < 0)
    {
        write(1, "-", 1);
        count++;
        n = -n;
    }

    if (n / 10)
        count += print_number(n / 10);

    c = (n % 10) + '0';
    write(1, &c, 1);
    count++;

    return count;
}
```

#### But de la fonction

Cette fonction affiche un entier signé (`int`) sur la sortie standard et retourne le nombre de caractères écrits.

#### Pourquoi ne pas utiliser `sprintf` ou `itoa` ?

Ces fonctions sont interdites dans le contexte du projet ou non disponibles de façon portable. La conversion d'un entier en chaîne de caractères est donc faite **manuellement**, chiffre par chiffre, grâce à la récursivité.

#### Explication étape par étape

**Étape 1 — Cas spécial pour `INT_MIN` (-2 147 483 648)**

```c
if (n == -2147483648)
{
    write(1, "-2147483648", 11);
    return 11;
}
```

Le type `int` sur 32 bits représente des valeurs de `-2 147 483 648` à `+2 147 483 647`. La valeur `-2 147 483 648` est problématique car son opposé (`+2 147 483 648`) **ne rentre pas** dans un `int` (dépassement de capacité, undefined behavior). On traite donc ce cas séparément en écrivant directement la chaîne littérale.

**Étape 2 — Gestion du signe négatif**

```c
if (n < 0)
{
    write(1, "-", 1);
    count++;
    n = -n;
}
```

Si le nombre est négatif, on écrit le caractère `-` sur stdout, on incrémente le compteur, puis on transforme `n` en sa valeur absolue pour travailler uniquement avec un entier positif dans la suite de la fonction.

**Étape 3 — Récursivité pour l'ordre des chiffres**

```c
if (n / 10)
    count += print_number(n / 10);
```

L'idée centrale de l'algorithme : pour afficher un nombre comme `1234`, il faut afficher `1`, puis `2`, puis `3`, puis `4` (de gauche à droite). Or, quand on divise successivement par 10, on obtient les chiffres **de droite à gauche**. La récursivité permet d'inverser cet ordre :

- `print_number(1234)` appelle `print_number(123)`
- `print_number(123)` appelle `print_number(12)`
- `print_number(12)` appelle `print_number(1)`
- `print_number(1)` : `1 / 10 == 0`, donc pas de récursion → affiche `'1'`
- Retour à `print_number(12)` → affiche `'2'`
- Retour à `print_number(123)` → affiche `'3'`
- Retour à `print_number(1234)` → affiche `'4'`

Résultat final : `1234` affiché dans le bon ordre.

**Étape 4 — Affichage du chiffre courant**

```c
c = (n % 10) + '0';
write(1, &c, 1);
count++;
```

`n % 10` donne le chiffre des unités de `n` (un entier entre 0 et 9). En ajoutant `'0'` (dont la valeur ASCII est 48), on obtient le caractère ASCII correspondant. Par exemple : `3 + '0'` → `'3'` (ASCII 51). On écrit ensuite ce caractère sur stdout avec `write`.

**Étape 5 — Retour du compteur**

```c
return count;
```

La fonction retourne le nombre total de caractères écrits (y compris le signe `-` si applicable).

---

### 6.2 La fonction `_printf`

```c
int _printf(const char *format, ...)
{
    va_list args;
    int i = 0, count = 0;
    char *str;
    char c;

    if (format == NULL)
        return (-1);

    va_start(args, format);

    while (format[i])
    {
        if (format[i] == '%')
        {
            i++;
            if (format[i] == '\0')
            {
                va_end(args);
                return (-1);
            }

            if (format[i] == 'c')
            {
                c = va_arg(args, int);
                write(1, &c, 1);
                count++;
            }
            else if (format[i] == 's')
            {
                str = va_arg(args, char *);
                if (!str)
                    str = "(null)";
                while (*str)
                {
                    write(1, str, 1);
                    str++;
                    count++;
                }
            }
            else if (format[i] == '%')
            {
                write(1, "%", 1);
                count++;
            }
            else if (format[i] == 'd' || format[i] == 'i')
            {
                int num = va_arg(args, int);
                count += print_number(num);
            }
            else
            {
                write(1, "%", 1);
                write(1, &format[i], 1);
                count += 2;
            }
        }
        else
        {
            write(1, &format[i], 1);
            count++;
        }
        i++;
    }

    va_end(args);
    return (count);
}
```

#### But de la fonction

`_printf` reproduit le comportement de base de `printf` : elle lit une chaîne de format, interprète les **spécificateurs de conversion** (séquences `%x`) et affiche le résultat formaté sur la sortie standard. Elle retourne le nombre total de caractères écrits, ou `-1` en cas d'erreur.

#### Déclaration et signature

```c
int _printf(const char *format, ...)
```

- `const char *format` : pointeur vers la chaîne de format (lecture seule). Elle peut contenir du texte ordinaire et des spécificateurs de conversion.
- `...` : liste d'arguments variables. Chaque spécificateur de conversion dans `format` consomme un argument de cette liste.
- Valeur de retour : `int` — nombre de caractères affichés, ou `-1` si erreur.

#### Variables locales

| Variable | Type | Rôle |
|---|---|---|
| `args` | `va_list` | Gère la liste d'arguments variables. |
| `i` | `int` | Index courant dans la chaîne `format`. |
| `count` | `int` | Compteur du nombre total de caractères écrits. |
| `str` | `char *` | Pointeur temporaire pour parcourir une chaîne (`%s`). |
| `c` | `char` | Stocke temporairement le caractère à afficher (`%c`). |

#### Étape 1 — Validation de l'argument `format`

```c
if (format == NULL)
    return (-1);
```

Si le pointeur `format` est `NULL`, la fonction retourne immédiatement `-1` (comportement cohérent avec `printf` sur certaines implémentations). Il n'y a rien à afficher et tenter de déréférencer un pointeur `NULL` provoquerait un segfault.

#### Étape 2 — Initialisation de `va_list`

```c
va_start(args, format);
```

`va_start` initialise la variable `args` pour qu'elle pointe juste après le dernier paramètre fixe (`format`). À partir de là, on peut appeler `va_arg` pour récupérer les arguments suivants un par un.

#### Étape 3 — Boucle principale de parcours de la chaîne de format

```c
while (format[i])
{
    ...
    i++;
}
```

La boucle avance caractère par caractère dans `format`, jusqu'au caractère nul de fin de chaîne (`'\0'`).

#### Étape 4 — Branchement sur `%`

```c
if (format[i] == '%')
{
    i++;
    ...
}
```

Lorsque le caractère courant est `%`, on avance immédiatement l'index `i` pour examiner le caractère suivant (le spécificateur).

#### Étape 4a — Protection contre un `%` en fin de chaîne

```c
if (format[i] == '\0')
{
    va_end(args);
    return (-1);
}
```

Si le `%` est le **dernier caractère** de la chaîne de format (non suivi d'un spécificateur), la chaîne est invalide. La fonction libère les ressources `va_list` et retourne `-1`.

#### Étape 4b — Spécificateur `%c` (caractère)

```c
if (format[i] == 'c')
{
    c = va_arg(args, int);
    write(1, &c, 1);
    count++;
}
```

`va_arg(args, int)` récupère l'argument suivant en tant que `int` (les types entiers courts sont promus en `int` lors du passage dans une liste variadique — c'est la règle des **promotions d'arguments par défaut** du C). La valeur est castée en `char` implicitement lors de l'affectation. On écrit ce caractère unique sur stdout.

#### Étape 4c — Spécificateur `%s` (chaîne de caractères)

```c
else if (format[i] == 's')
{
    str = va_arg(args, char *);
    if (!str)
        str = "(null)";
    while (*str)
    {
        write(1, str, 1);
        str++;
        count++;
    }
}
```

On récupère un `char *`. Si le pointeur est `NULL`, on le remplace par la chaîne littérale `"(null)"` (comportement adopté par GNU libc `printf`). Ensuite, on parcourt la chaîne caractère par caractère jusqu'au `'\0'` terminal, en écrivant chaque caractère sur stdout.

#### Étape 4d — Spécificateur `%%` (caractère littéral `%`)

```c
else if (format[i] == '%')
{
    write(1, "%", 1);
    count++;
}
```

La séquence `%%` dans une chaîne de format représente un `%` littéral. On écrit simplement `%` sur stdout.

#### Étape 4e — Spécificateurs `%d` et `%i` (entiers décimaux)

```c
else if (format[i] == 'd' || format[i] == 'i')
{
    int num = va_arg(args, int);
    count += print_number(num);
}
```

On récupère un `int` et on délègue l'affichage à `print_number`. Les deux spécificateurs `%d` et `%i` sont équivalents en mode d'affichage (tous deux affichent un entier décimal signé). La différence entre eux n'existe qu'en entrée (`scanf`), où `%i` accepte les notations octale et hexadécimale.

#### Étape 4f — Spécificateur inconnu

```c
else
{
    write(1, "%", 1);
    write(1, &format[i], 1);
    count += 2;
}
```

Si le caractère qui suit `%` n'est pas reconnu, la fonction reproduit le comportement de `printf` GNU : elle affiche le `%` suivi du caractère non reconnu tel quel, et comptabilise 2 caractères.

#### Étape 5 — Caractère ordinaire (hors `%`)

```c
else
{
    write(1, &format[i], 1);
    count++;
}
```

Tout caractère qui n'est pas `%` est affiché directement sur stdout tel quel.

#### Étape 6 — Libération de `va_list` et retour

```c
va_end(args);
return (count);
```

`va_end` libère les ressources associées à `args` (obligatoire après `va_start`). La fonction retourne le nombre total de caractères écrits.

---

## 7. Spécificateurs de format supportés

| Spécificateur | Argument attendu | Description | Exemple de format | Exemple de sortie |
|---|---|---|---|---|
| `%c` | `int` (promu depuis `char`) | Affiche un seul caractère. | `_printf("%c", 'A')` | `A` |
| `%s` | `char *` | Affiche une chaîne de caractères terminée par `'\0'`. Si le pointeur est `NULL`, affiche `(null)`. | `_printf("%s", "hello")` | `hello` |
| `%d` | `int` | Affiche un entier décimal signé. | `_printf("%d", -42)` | `-42` |
| `%i` | `int` | Identique à `%d`. Affiche un entier décimal signé. | `_printf("%i", 100)` | `100` |
| `%%` | *(aucun argument)* | Affiche le caractère `%` littéral. | `_printf("100%%")` | `100%` |

---

## 8. Algorithme pas à pas

Voici le déroulé complet de l'exécution de `_printf("Hello, %s! You are %d years old.\n", "Alice", 30)` :

```
format = "Hello, %s! You are %d years old.\n"
arguments : "Alice", 30
```

| Étape | `i` | `format[i]` | Action | `count` |
|---|---|---|---|---|
| 1 | 0 | `H` | Écriture de `H` | 1 |
| 2 | 1 | `e` | Écriture de `e` | 2 |
| 3 | 2 | `l` | Écriture de `l` | 3 |
| 4 | 3 | `l` | Écriture de `l` | 4 |
| 5 | 4 | `o` | Écriture de `o` | 5 |
| 6 | 5 | `,` | Écriture de `,` | 6 |
| 7 | 6 | ` ` | Écriture de ` ` | 7 |
| 8 | 7 | `%` | Détection de `%`, avance `i` → 8 | 7 |
| 9 | 8 | `s` | Spécificateur `%s` : récupère `"Alice"`, écrit `A`, `l`, `i`, `c`, `e` | 12 |
| 10 | 9 | `!` | Écriture de `!` | 13 |
| 11 | 10 | ` ` | Écriture de ` ` | 14 |
| ... | ... | ... | ... (pareil pour `You are `) | 22 |
| 12 | 18 | `%` | Détection de `%`, avance `i` → 19 | 22 |
| 13 | 19 | `d` | Spécificateur `%d` : récupère `30`, appelle `print_number(30)`, écrit `3`, `0` | 24 |
| ... | ... | ... | ... (pareil pour ` years old.\n`) | 37 |

**Valeur retournée : 37**

---

## 9. Cas limites et comportements particuliers

### 9.1 `format` est `NULL`

```c
_printf(NULL);
// Retourne : -1
// Rien n'est affiché
```

La fonction protège contre un pointeur `NULL` dès le début et retourne `-1`.

### 9.2 `%` en fin de chaîne (sans spécificateur)

```c
_printf("Hello%");
// Retourne : -1
// Seul "Hello" aurait été affiché avant de rencontrer le % terminal
// Attention : en réalité la fonction retourne -1 sans avoir affiché "Hello"
// car la détection se fait lors du parcours ; les caractères avant % sont
// déjà écrits au moment où la condition finale est détectée.
```

Plus précisément, les caractères situés **avant** le `%` terminal ont déjà été écrits sur stdout au moment où l'erreur est détectée. La valeur de retour est `-1`.

### 9.3 Chaîne `NULL` pour `%s`

```c
_printf("%s", NULL);
// Affiche : (null)
// Retourne : 6
```

Si l'argument de `%s` est un pointeur `NULL`, la fonction affiche la chaîne `"(null)"` (6 caractères) au lieu de provoquer un segfault.

### 9.4 Entier `INT_MIN` pour `%d` / `%i`

```c
_printf("%d", -2147483648);
// Affiche : -2147483648
// Retourne : 11
```

La valeur `INT_MIN` est traitée comme un cas spécial dans `print_number` pour éviter un dépassement d'entier lors de la négation.

### 9.5 Spécificateur inconnu

```c
_printf("%x", 255);
// Affiche : %x
// Retourne : 2
```

Un spécificateur non supporté est affiché tel quel (le `%` suivi du caractère inconnu).

### 9.6 Chaîne de format vide

```c
_printf("");
// Affiche : (rien)
// Retourne : 0
```

La boucle ne s'exécute pas, `count` reste à 0.

### 9.7 `%%` (échappement du symbole `%`)

```c
_printf("100%%");
// Affiche : 100%
// Retourne : 4
```

La séquence `%%` est interprétée comme un seul `%` littéral.

---

## 10. Compilation

### Compilation standard

Pour compiler le projet avec un fichier de test `main.c` :

```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 _printf.c main.c -o test_printf
```

| Option | Signification |
|---|---|
| `-Wall` | Active tous les avertissements courants. |
| `-Werror` | Traite les avertissements comme des erreurs. |
| `-Wextra` | Active des avertissements supplémentaires. |
| `-pedantic` | Force la conformité stricte au standard C. |
| `-std=gnu89` | Utilise le standard C89 avec les extensions GNU (standard utilisé par Holberton). |

### Exemple de `main.c` pour tester

```c
#include "main.h"
#include <stdio.h>

int main(void)
{
    int len;
    int len2;

    len = _printf("Let's try to printf a simple sentence.\n");
    len2 = printf("Let's try to printf a simple sentence.\n");
    _printf("Length: [%d, %i]\n", len, len);
    printf("Length: [%d, %i]\n", len2, len2);

    _printf("Negative: [%d]\n", -762534);
    printf("Negative: [%d]\n", -762534);

    _printf("Character: [%c]\n", 'H');
    printf("Character: [%c]\n", 'H');

    _printf("String: [%s]\n", "I am a string !");
    printf("String: [%s]\n", "I am a string !");

    _printf("Percent: [%%]\n");
    printf("Percent: [%%]\n");

    _printf("Unknown: [%r]\n");
    printf("Unknown: [%r]\n");

    return (0);
}
```

### Exécution

```bash
./test_printf
```

---

## 11. Exemples d'utilisation

### Affichage d'une chaîne simple

```c
_printf("Bonjour le monde !\n");
// Sortie : Bonjour le monde !
// Retour  : 20
```

### Affichage d'un caractère

```c
_printf("Première lettre : %c\n", 'A');
// Sortie : Première lettre : A
// Retour  : 20
```

### Affichage d'une chaîne de caractères

```c
_printf("Prénom : %s\n", "Alice");
// Sortie : Prénom : Alice
// Retour  : 14
```

### Affichage d'un entier positif

```c
_printf("Valeur : %d\n", 42);
// Sortie : Valeur : 42
// Retour  : 11
```

### Affichage d'un entier négatif

```c
_printf("Température : %d degrés\n", -15);
// Sortie : Température : -15 degrés
// Retour  : 24
```

### Combinaison de plusieurs spécificateurs

```c
_printf("%s a %d ans et mesure %d cm.\n", "Bob", 25, 180);
// Sortie : Bob a 25 ans et mesure 180 cm.
// Retour  : 33
```

### Affichage du caractère `%`

```c
_printf("Taux de réussite : 87%%\n");
// Sortie : Taux de réussite : 87%
// Retour  : 24
```

### Chaîne NULL pour `%s`

```c
_printf("Valeur : %s\n", NULL);
// Sortie : Valeur : (null)
// Retour  : 14
```

### Valeur `INT_MIN`

```c
_printf("Min int : %d\n", -2147483648);
// Sortie : Min int : -2147483648
// Retour  : 23
```

---

## 12. Valeurs de retour

| Situation | Valeur retournée |
|---|---|
| Succès | Nombre de caractères écrits sur stdout (≥ 0) |
| `format == NULL` | `-1` |
| `%` suivi de `'\0'` (fin de chaîne) | `-1` (les caractères déjà écrits ne sont pas défaits) |

---

## 13. Différences avec `printf` standard

| Fonctionnalité | `printf` (libc) | `_printf` (ce projet) |
|---|---|---|
| `%c` | ✅ Supporté | ✅ Supporté |
| `%s` | ✅ Supporté | ✅ Supporté |
| `%d` | ✅ Supporté | ✅ Supporté |
| `%i` | ✅ Supporté | ✅ Supporté |
| `%%` | ✅ Supporté | ✅ Supporté |
| `%u` (unsigned int) | ✅ Supporté | ❌ Non supporté |
| `%o` (octal) | ✅ Supporté | ❌ Non supporté |
| `%x` / `%X` (hexadécimal) | ✅ Supporté | ❌ Non supporté (affiché tel quel) |
| `%f` (float) | ✅ Supporté | ❌ Non supporté |
| `%e` / `%E` (notation scientifique) | ✅ Supporté | ❌ Non supporté |
| `%p` (pointeur) | ✅ Supporté | ❌ Non supporté |
| `%n` (écriture du compteur) | ✅ Supporté | ❌ Non supporté |
| Drapeaux (`-`, `+`, `0`, ` `, `#`) | ✅ Supportés | ❌ Non supportés |
| Largeur de champ (ex. `%10d`) | ✅ Supporté | ❌ Non supporté |
| Précision (ex. `%.5s`) | ✅ Supporté | ❌ Non supporté |
| Modificateurs de longueur (`l`, `h`, etc.) | ✅ Supportés | ❌ Non supportés |
| Buffering de la sortie | ✅ (FILE * avec buffer) | ❌ (write direct, pas de buffer) |

> **Note sur le buffering** : `printf` utilise en interne un tampon (buffer) et n'écrit sur stdout que lorsque le tampon est plein ou qu'un `\n` est rencontré (en mode line-buffered). `_printf` utilise directement l'appel système `write` pour chaque caractère, ce qui est moins efficace mais plus simple à implémenter. Dans un contexte où les performances ne sont pas critiques (projet pédagogique), cela est acceptable.

---

## 14. Auteurs

Ce projet a été réalisé dans le cadre du cursus **Holberton School** par :

- **Mataius-web** — [GitHub](https://github.com/Mataius-web)

---

*Holberton School — Projet `_printf` — Tous droits réservés.*