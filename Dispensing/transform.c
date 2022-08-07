#include <stdio.h>
#include <math.h>


struct point {
    int x;
    int y;
};


struct point FindLocation(struct point Fisnar_Fid1, struct point gerber_Fid1, struct point working, double *theta)
{
    working.x -= gerber_Fid1.x;
    working.y -= gerber_Fid1.y;

    int x = working.x;
    int y = working.y;
    
    working.x = x*cos(*theta) - y*sin(*theta);
    working.y = x*sin(*theta) + y*cos(*theta);
    
    working.x = working.x + Fisnar_Fid1.x;
    working.y = working.y + Fisnar_Fid1.y;

    return working;
}

int main()
{
    struct point Fisnar_Fid1, Fisnar_Fid2 ,gerber_Fid1, gerber_Fid2, translation, test_point;
    double theta = 0;
    char locating = 1;
    char answer = 'y';

    printf("This is a an example algorithm to find the translation and rotation of a PCB.\n");
    printf("Enter the coordinates of the Gerber fiducial 1: (x ENTER y ENTER) ");
    scanf("%d %d", &gerber_Fid1.x, &gerber_Fid1.y);
    printf("Enter the coordinates of the Gerber fiducial 2: (x ENTER y ENTER) ");
    scanf("%d %d", &gerber_Fid2.x, &gerber_Fid2.y);
    printf("Enter the coordinates of the fiducial 1 according to the Fisnar: (x ENTER y ENTER) ");
    scanf("%d %d", &Fisnar_Fid1.x, &Fisnar_Fid1.y);

    printf("Using the coordinates you entered, the tranlation transform performed on Fiducial 1 is: \n");
    translation.x = Fisnar_Fid1.x - gerber_Fid1.x;
    translation.y = Fisnar_Fid1.y - gerber_Fid1.y;
    printf("(%d, %d)\n", translation.x, translation.y);

    printf("Imagine that you are probing Fiducial 2 on the PCB.\n");
    while(locating)
    {
        Fisnar_Fid2 = FindLocation(Fisnar_Fid1, gerber_Fid1, gerber_Fid2, &theta);
        // Correct position of fiducial 2
        printf("The probe is located at: (%d, %d)\n", Fisnar_Fid2.x, Fisnar_Fid2.y);
        printf("\nIs the fiducial 2 position correct? (Y) or (Z) to rotate CW, or (X) to rotate CCW: ");
        scanf(" %c", &answer);
        while ((getchar()) != '\n');
        if(answer == 'y')
        {
            locating = 0;
        }
        else if(answer == 'z')
        {
            theta += 0.05;
        }
        else if(answer == 'x')
        {
            theta -= 0.05;
        }
        printf("Is Fiducial 2 position (%d, %d) on the Fisnar plate correct?\n", Fisnar_Fid2.x, Fisnar_Fid2.y);   
    }
    
    printf("The offset angle is %lf degrees\n", theta * 180 / M_PI);

    printf("\nEnter a third point (Gerber) to test the rotation: (x ENTER y ENTER) ");
    scanf("%d %d", &test_point.x, &test_point.y);
    printf("The point is located at (%d, %d) on the Fisnar plate.\n", FindLocation(Fisnar_Fid1, gerber_Fid1, test_point, &theta).x, FindLocation(Fisnar_Fid1, gerber_Fid1, test_point, &theta).y);

    return 0;
}
