#include <stdio.h>

int main()
{
    int months = 12;

    int youngCouples = 1;
    int adultCouples = 0;

    printf("Month\tYoung\tAdult\tTotal\n");

    for (int month = 0; month <= months; month++)
    {
        int total = youngCouples + adultCouples;

        printf("%d\t%d\t%d\t%d\n",
               month,
               youngCouples,
               adultCouples,
               total);

        /*
           Chaque jeune couple devient adulte.
           Chaque couple adulte produit un nouveau jeune couple.
        */

        int newYoungCouples = adultCouples;
        int newAdultCouples = adultCouples + youngCouples;

        youngCouples = newYoungCouples;
        adultCouples = newAdultCouples;
    }

    return 0;
}