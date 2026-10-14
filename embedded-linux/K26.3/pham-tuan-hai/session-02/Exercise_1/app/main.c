#include <stdio.h>
#include "mathutils.h"

int main(){
   int x,y,n;

   printf("Nhap x va y: ");
   scanf("%d %d", &x, &y);
   printf("%d + %d = %d\n", x , y, math_add(x,y));
   printf("%d - %d = %d\n", x , y, math_sub(x,y));

	printf("Nhap n tinh giai thua:");
	scanf("%d", &n);
	
	printf("%d! = %d\n", n, math_factorial(n));
	return 0;
}
