#include <stdio.h>
#include <math.h>

struct point {
    long int x;
    long int y;
};


struct point transform(struct point fisnar_ref, struct point gerber_ref, struct point gerber_target, double theta)
{
    double angle = theta * M_PI / 180;
    // Target Point translation position referred to refernce point (Fiducial 1)
    gerber_target.x -= gerber_ref.x;
    gerber_target.y -= gerber_ref.y;
    // Copy referred target points for rotation
    int x = gerber_target.x;
    int y = gerber_target.y;
    // Perform rotation
    gerber_target.x = x * cos(angle) - y * sin(angle);
    gerber_target.y = x * sin(angle) + y * cos(angle);
    // Target Point translation position referred to fisnar origin
    gerber_target.x = gerber_target.x + fisnar_ref.x;
    gerber_target.y = gerber_target.y + fisnar_ref.y;
    return gerber_target;
}

void enter_point(struct point *p)
{
    printf("Enter x: ");
    scanf("%ld", &p->x);
    printf("Enter y: ");
    scanf("%ld", &p->y);
}

int main()
{
    struct point fisnar, ref, target;
    double theta;
    printf("\nEnter Fisnar reference point: ");
    enter_point(&fisnar);
    printf("\nEnter Gerber reference point: ");
    enter_point(&ref);
    printf("\nEnter Gerber target point: ");
    enter_point(&target);
    printf("\nEnter theta (in degrees): ");
    scanf("%lf", &theta);
    struct point tp = transform(fisnar, ref, target, theta);
    printf("\nTranformed points:\n");
    printf("x: %ld\ny: %ld\n", tp.x, tp.y);

    return 0;
}