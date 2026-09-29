#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_RABBITS 200000

typedef enum
{
    MALE,
    FEMALE
} Sex;

typedef struct
{
    int ageMonths;          // Age en mois
    Sex sex;                // Sexe
    int maturityAge;        // Age de maturite sexuelle en mois
    int alive;              // 1 = vivant, 0 = mort

    // Utilise surtout pour les femelles
    int annualLitterTarget; // Nombre de portees prevues par an
    int littersThisYear;    // Nombre de portees deja realisees cette annee
} Rabbit;


/* -------------------------------------------------------
   Nombre entier aleatoire entre min et max
   ------------------------------------------------------- */
int randomInt(int min, int max)
{
    return min + rand() % (max - min + 1);
}


/* -------------------------------------------------------
   Nombre aleatoire entre 0 et 1
   ------------------------------------------------------- */
double randomDouble()
{
    return (double)rand() / (double)RAND_MAX;
}


/* -------------------------------------------------------
   Nombre annuel de portees

   Choix de modelisation :
   3 portees : 5 %
   4 portees : 10 %
   5 portees : 20 %
   6 portees : 30 %
   7 portees : 20 %
   8 portees : 10 %
   9 portees : 5 %
   ------------------------------------------------------- */
int generateAnnualLitterTarget()
{
    int r = randomInt(1, 100);

    if (r <= 5)
        return 3;
    else if (r <= 15)
        return 4;
    else if (r <= 35)
        return 5;
    else if (r <= 65)
        return 6;
    else if (r <= 85)
        return 7;
    else if (r <= 95)
        return 8;
    else
        return 9;
}


/* -------------------------------------------------------
   Creation d'un lapin
   ------------------------------------------------------- */
Rabbit createRabbit(int ageMonths, Sex sex)
{
    Rabbit r;

    r.ageMonths = ageMonths;
    r.sex = sex;
    r.alive = 1;

    // Maturite sexuelle entre 5 et 8 mois
    r.maturityAge = randomInt(5, 8);

    r.littersThisYear = 0;

    if (sex == FEMALE)
        r.annualLitterTarget = generateAnnualLitterTarget();
    else
        r.annualLitterTarget = 0;

    return r;
}


/* -------------------------------------------------------
   Probabilite annuelle de survie

   Jeune lapin : 35 %
   Adulte : 60 %

   A partir de 10 ans :
   10 ans -> 50 %
   11 ans -> 40 %
   12 ans -> 30 %
   13 ans -> 20 %
   14 ans -> 10 %
   15 ans -> 0 %
   ------------------------------------------------------- */
double annualSurvivalProbability(Rabbit *r)
{
    double ageYears = r->ageMonths / 12.0;

    if (ageYears >= 15.0)
        return 0.0;

    // Jeune lapin
    if (r->ageMonths < r->maturityAge)
        return 0.35;

    // Adulte de moins de 10 ans
    if (ageYears < 10.0)
        return 0.60;

    int fullYears = r->ageMonths / 12;

    double survival = 0.60 - 0.10 * (fullYears - 9);

    if (survival < 0.0)
        survival = 0.0;

    return survival;
}


/* -------------------------------------------------------
   Conversion survie annuelle -> survie mensuelle

   P_mois ^ 12 = P_an
   Donc :
   P_mois = P_an ^ (1/12)
   ------------------------------------------------------- */
double monthlySurvivalProbability(Rabbit *r)
{
    double annual = annualSurvivalProbability(r);

    if (annual <= 0.0)
        return 0.0;

    return pow(annual, 1.0 / 12.0);
}


/* -------------------------------------------------------
   Verifie s'il existe au moins un male mature vivant
   ------------------------------------------------------- */
int hasMatureMale(Rabbit rabbits[], int population)
{
    for (int i = 0; i < population; i++)
    {
        if (rabbits[i].alive &&
            rabbits[i].sex == MALE &&
            rabbits[i].ageMonths >= rabbits[i].maturityAge)
        {
            return 1;
        }
    }

    return 0;
}


/* -------------------------------------------------------
   Une simulation complete
   ------------------------------------------------------- */
int runSimulation(int years, int initialCouples, int verbose)
{
    Rabbit *rabbits =
        malloc(sizeof(Rabbit) * MAX_RABBITS);

    if (rabbits == NULL)
    {
        printf("Erreur d'allocation memoire.\n");
        exit(EXIT_FAILURE);
    }

    int population = 0;

    int totalBirths = 0;
    int totalDeaths = 0;


    /* ---------------------------------------------------
       Population initiale
       --------------------------------------------------- */
    for (int i = 0; i < initialCouples; i++)
    {
        // Male adulte
        rabbits[population] = createRabbit(8, MALE);
        rabbits[population].maturityAge = 6;
        population++;

        // Femelle adulte
        rabbits[population] = createRabbit(8, FEMALE);
        rabbits[population].maturityAge = 6;
        population++;
    }


    int totalMonths = years * 12;


    if (verbose)
    {
        printf("\n========================================\n");
        printf("Simulation de population de lapins\n");
        printf("Duree : %d ans\n", years);
        printf("Population initiale : %d couple(s)\n",
               initialCouples);
        printf("========================================\n\n");
    }


    for (int month = 1; month <= totalMonths; month++)
    {
        int currentPopulation = population;


        /* ------------------------------------------------
           1. Vieillissement + mortalite
           ------------------------------------------------ */
        for (int i = 0; i < currentPopulation; i++)
        {
            if (!rabbits[i].alive)
                continue;

            rabbits[i].ageMonths++;

            double survival =
                monthlySurvivalProbability(&rabbits[i]);

            if (randomDouble() > survival)
            {
                rabbits[i].alive = 0;
                totalDeaths++;
            }
        }


        /* ------------------------------------------------
           2. Debut d'une nouvelle annee :
              nouveau nombre annuel de portees
           ------------------------------------------------ */
        if ((month - 1) % 12 == 0)
        {
            for (int i = 0; i < currentPopulation; i++)
            {
                if (rabbits[i].alive &&
                    rabbits[i].sex == FEMALE)
                {
                    rabbits[i].annualLitterTarget =
                        generateAnnualLitterTarget();

                    rabbits[i].littersThisYear = 0;
                }
            }
        }


        /* ------------------------------------------------
           3. Reproduction
           ------------------------------------------------ */
        int matureMaleExists =
            hasMatureMale(rabbits, currentPopulation);

        if (matureMaleExists)
        {
            for (int i = 0; i < currentPopulation; i++)
            {
                Rabbit *female = &rabbits[i];

                if (!female->alive)
                    continue;

                if (female->sex != FEMALE)
                    continue;

                if (female->ageMonths < female->maturityAge)
                    continue;


                int remainingLitters =
                    female->annualLitterTarget -
                    female->littersThisYear;

                if (remainingLitters <= 0)
                    continue;


                /*
                   Correction importante :

                   On ne cherche plus a "rattraper"
                   toutes les portees restantes.

                   Si une femelle a par exemple
                   6 portees/an, elle a chaque mois
                   une probabilite 6/12 = 0.5
                   d'avoir une portee.
                */
                double reproductionProbability =
                    (double)female->annualLitterTarget / 12.0;


                if (randomDouble() < reproductionProbability)
                {
                    female->littersThisYear++;


                    /* ------------------------------------
                       Nombre de lapereaux : 3 a 6
                       equiprobables
                       ------------------------------------ */
                    int kittens = randomInt(3, 6);


                    for (int k = 0; k < kittens; k++)
                    {
                        if (population >= MAX_RABBITS)
                        {
                            printf(
                                "\nPopulation maximale atteinte (%d).\n",
                                MAX_RABBITS
                            );

                            free(rabbits);

                            return population;
                        }


                        Sex sex;

                        // 50 % male / 50 % femelle
                        if (randomDouble() < 0.5)
                            sex = MALE;
                        else
                            sex = FEMALE;


                        rabbits[population] =
                            createRabbit(0, sex);

                        population++;
                        totalBirths++;
                    }
                }
            }
        }


        /* ------------------------------------------------
           4. Statistiques
           ------------------------------------------------ */
        int alive = 0;
        int males = 0;
        int females = 0;


        for (int i = 0; i < population; i++)
        {
            if (rabbits[i].alive)
            {
                alive++;

                if (rabbits[i].sex == MALE)
                    males++;
                else
                    females++;
            }
        }


        /* Une ligne par annee */
        if (verbose && month % 12 == 0)
        {
            printf(
                "Annee %2d : population = %6d "
                "(M = %6d, F = %6d), "
                "naissances = %6d, deces = %6d\n",
                month / 12,
                alive,
                males,
                females,
                totalBirths,
                totalDeaths
            );
        }


        /* ------------------------------------------------
           Extinction
           ------------------------------------------------ */
        if (alive == 0)
        {
            if (verbose)
            {
                printf(
                    "\nPopulation eteinte au mois %d.\n",
                    month
                );
            }

            free(rabbits);

            return 0;
        }
    }


    /* ---------------------------------------------------
       Population finale
       --------------------------------------------------- */
    int alive = 0;


    for (int i = 0; i < population; i++)
    {
        if (rabbits[i].alive)
            alive++;
    }


    if (verbose)
    {
        printf("\n----------------------------------------\n");
        printf("Population finale : %d\n", alive);
        printf("Naissances totales : %d\n", totalBirths);
        printf("Deces totaux : %d\n", totalDeaths);
        printf("----------------------------------------\n");
    }


    free(rabbits);

    return alive;
}


/* -------------------------------------------------------
   Programme principal
   ------------------------------------------------------- */
int main()
{
    srand((unsigned int)time(NULL));


    /* Parametres */
    int years = 10;
    int initialCouples = 1;


    /* ===================================================
       1. Une simulation detaillee
       =================================================== */
    runSimulation(
        years,
        initialCouples,
        1
    );


    /* ===================================================
       2. 40 experiences independantes
       =================================================== */

    clock_t startTime = clock();
    int experiments = 40;

    int extinctions = 0;

    long long totalFinalPopulation = 0;


    printf("\n\n========================================\n");
    printf("40 experiences independantes\n");
    printf("========================================\n");


    for (int i = 1; i <= experiments; i++)
    {
        int result =
            runSimulation(
                years,
                initialCouples,
                0
            );


        printf(
            "Experience %2d : population finale = %d\n",
            i,
            result
        );


        if (result == 0)
            extinctions++;


        totalFinalPopulation += result;
    }


    /* ===================================================
       Resultats statistiques
       =================================================== */
    double meanPopulation =
        (double)totalFinalPopulation /
        experiments;


    double extinctionProbability =
        (double)extinctions /
        experiments;


    printf("\n========================================\n");

    printf(
        "Population finale moyenne : %.2f\n",
        meanPopulation
    );

    printf(
        "Nombre d'extinctions : %d / %d\n",
        extinctions,
        experiments
    );

    printf(
        "Probabilite experimentale d'extinction : %.2f %%\n",
        extinctionProbability * 100.0
    );

    printf("========================================\n");

    clock_t endTime = clock();

    double executionTime =
        (double)(endTime - startTime) / CLOCKS_PER_SEC;

    printf("Temps d'execution : %.3f secondes\n",
        executionTime);

    return 0;
}