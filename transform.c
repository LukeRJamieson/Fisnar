#include <stdio.h>
#include <math.h>

struct point {
    long int x;
    long int y;
};


struct point transform(struct point ref, struct point target, double theta)
{
    double angle = theta * M_PI / 180;
    // Target Point translation position referred to refernce point (Fiducial 1)
    target.x -= ref.x;
    target.y -= ref.y;
    // Copy referred target points for rotation
    int x = target.x;
    int y = target.y;
    // Perform rotation
    target.x = x * cos(angle) - y * sin(angle);
    target.y = x * sin(angle) + y * cos(angle);
    // Target Point translation position referred to fisnar origin
    target.x +=  ref.x;
    target.y +=  ref.y;
    return target;
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
    struct point ref, target;
    double theta;
    printf("\nEnter reference point: ");
    enter_point(&ref);
    printf("\nEnter target point: ");
    enter_point(&target);
    printf("\nEnter theta (in degrees): ");
    scanf("%lf", &theta);
    struct point tp = transform(ref, target, theta);
    printf("\nTranformed points:\n");
    printf("x: %ld\ny: %ld\n", tp.x, tp.y);

    return 0;
}