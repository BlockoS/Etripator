#include "config.h"
#include "random.h"

static unsigned long Q[4096], c=362436;

/** \brief Initialize CMWC496 RNG */
void SetupCMWC4096(void)
{
	int i;

	for(i=0; i<4096; i++)
	{
		Q[i] = rand();
	}
}

/** \brief George Marsaglia's random number generator.
 *  Its name stands for Complimentary-Multiply-With-Carry
 *  4096 is the initial random value.
 *
 *  Reference : 
 *		* http://groups.google.com/group/comp.lang.c/browse_thread/thread/a9915080a4424068/?pli=1
 *		* http://groups.google.com/group/sci.crypt/browse_thread/thread/305c507efbe85be4?pli=1
 */
unsigned long CMWC4096(void)
{
	unsigned long long t, a=18782LL,b=4294967295LL;
	static unsigned long i=4095;
	unsigned long x,r=(b-1);
	i=(i+1)&4095;
	t=a*Q[i]+c;
	c=(t>>32); t=(t&b)+c;
	if(t>r) {c++; t=t-b;}
    return (Q[i]=r-t);
} 