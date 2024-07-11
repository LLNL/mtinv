/*** PROGRAM TO TEST GETPAR LIB
TWO EXAMPLES 

>gettest x=1.1 y=2 s="hello gettest" a u=1,2,3,4,5
x=1.1 y=2 s=hello gettest a=1

>gettest x=1.1 y=2 s="hello gettest" noa u=1,2,3,4,5
x=1.1 y=2 s=hello gettest a=0 

***/

#include <stdio.h>

main(ac,av)
{
	int i;
	float x;
	int y;
	char s[128];
	int a;
	float u[5];

	setpar(ac,av);
	mstpar("x","f",&x);
	mstpar("y","d",&y);
	mstpar("s","s",&s);
	mstpar("a","b",&a);
	mstpar("u","vf[5]",&u);
	endpar();

	fprintf(stdout,"x=%g y=%d s=%s a=%d\n",x,y,s,a);
	for(i=0;i<5;i++)
	{
		fprintf(stdout,"u[%d]=%g\n",i,u[i]);
	}
}


