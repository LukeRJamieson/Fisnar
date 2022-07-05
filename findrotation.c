#include <stdio.h>
#include <math.h>


struct point {
    int x;
    int y;
};


struct point FindRotation(struct point Fisnar_Fid1, struct point gerber_Fid1,struct point gerber_Fid2, double *theta)
{
    gerber_Fid2.x -= gerber_Fid1.x;
    gerber_Fid2.y -= gerber_Fid1.y;

    int x = gerber_Fid2.x;
    int y = gerber_Fid2.y;
    
    gerber_Fid2.x = x*cos(*theta) - y*sin(*theta);
    gerber_Fid2.y = x*sin(*theta) + y*cos(*theta);
    
    gerber_Fid2.x = gerber_Fid2.x + Fisnar_Fid1.x;
    gerber_Fid2.y = gerber_Fid2.y + Fisnar_Fid1.y;

    return gerber_Fid2;
}

int main()
{
    struct point Fisnar_Fid1, Fisnar_Fid2 ,gerber_Fid1, gerber_Fid2;
    double theta = 0;
    char locating = 1;
    char answer = 'y';

    // Testing only ///////////
    Fisnar_Fid1.x = 300000;
    Fisnar_Fid1.y = 300000;
    gerber_Fid1.x = 0;
    gerber_Fid1.y = 1000;
    gerber_Fid2.x = 1000;
    gerber_Fid2.y = 0;
    ////////////////////////////
    printf("\nFor this example Fiducial 1 is located at (%d,%d) on the Fisnar plate\n", Fisnar_Fid1.x, Fisnar_Fid1.y);
    printf("Fiducial 1 is located at (%d,%d) in the gerber file\n", gerber_Fid1.x, gerber_Fid1.y);
    printf("Fiducial 2 is located at (%d,%d) in the gerber file\n", gerber_Fid2.x, gerber_Fid2.y);

    while(locating)
    {
        Fisnar_Fid2 = FindRotation(Fisnar_Fid1, gerber_Fid1, gerber_Fid2, &theta);
        // Correct position of fiducial 2
        printf("\nIs the fiducial 2 position correct? (y/n): ");
        scanf(" %c", &answer);
        while ((getchar()) != '\n');
        if(answer == 'y')
        {
            locating = 0;
        }
        else
        {
            printf("\nIncrement (+) or (-) the rotation: ");
            scanf(" %c", &answer);
            while ((getchar()) != '\n');
            if(answer == '+')
            {
                theta += 0.01;
            }
            else if(answer == '-')
            {
                theta -= 0.01;
            }   
        }
    }
    printf("The offset angle is %lf degrees\n", theta * 180 / M_PI);
    printf("The Fiducial 2 position is located at (%d, %d) on the Fisnar plate.\n", Fisnar_Fid2.x, Fisnar_Fid2.y);

    return 0;
}
