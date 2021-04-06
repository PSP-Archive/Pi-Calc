
// Pi Calculator example by unknown author (public domain), ported to PSP by Art.


#include <pspdisplay.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <stdio.h>
#include <pspctrl.h>

/* Define the module info section */
PSP_MODULE_INFO("Pi_Calc",0, 1, 1);

# define MAXSIZE 1050
int printed = 0;
int ffls;
int MAXDIGITS = 10000; // default number of digits

/* MAXSIZE is the accuracy of the calculation in 32-bit words.
   MAXDIGITS is the number of decimals that will be printed.
   MAXSIZE should be a little more than MAXDIGITS divided by the
   base-10 logarithm of 2 to the 32th power, or a little bit more 
   than 0.1038 times MAXDIGITS. For an explanation of this fact,
   see, e.g., "The Mathematical Theory of Communication" by Claude Shannon*/

typedef union
{
  unsigned long long L;
  struct
  {
    unsigned int lo;
    unsigned int hi;
  }
  w;
}
dword;

typedef unsigned int bignum[MAXSIZE];
/* This program represents "big numbers" (numbers with many digits) as
   arrays of unsigned ints. The first element (with index zero)
   in such an array represents the integer part of the number, the other
   elements represent the part after the decimal point.  */


void copybig (bignum dest, bignum source);
void multbig (bignum number, unsigned int x);
void printbig (bignum number);
int divbig (bignum number, unsigned int x);
void addbig (bignum dest, bignum source);
void subbig (bignum dest, bignum source);
void setbig (bignum number, int intpart, unsigned int decpart);
void atanbig (bignum A, unsigned int x);
void make_pi (void);


void
copybig (bignum dest, bignum source)
/*Copies a big number from source to dest*/
{
  int j;
  for (j = 0; j < MAXSIZE; j++)
    dest[j] = source[j];
}


void
multbig (bignum number, unsigned int x)
/*multiplies a big number by an unsigned integer*/
{
  int j = MAXSIZE - 1;
  dword result;
  unsigned int remember = 0;
  while (number[j] == 0 && j >= 0)
    j--;
  while (j >= 0)
    {
      result.L = (long long) number[j] * x + remember;
      number[j] = result.w.lo;
      remember = result.w.hi;
      j--;
    }
}

void
printbig (bignum number)
/*prints a big number as a decimal float number, by repeatedly multiplying
  by 100000 and printing the integer part */
{
  bignum num;
  int count = 0, line = 0, ndigits = 0;
  copybig (num, number);
  pspDebugScreenPrintf (" %d.", num[0]);
  while (ndigits < MAXDIGITS)
    {
      num[0] = 0;
      multbig (num, 100000);
      pspDebugScreenPrintf ("%05d", num[0]);
      ndigits += 5;
      count++;
      if (count == 2)
	{
	  pspDebugScreenPrintf (" ");
	  line++;


if (MAXDIGITS > 1120) {
	for(ffls=0; ffls<2; ffls++) {sceDisplayWaitVblankStart();}
}


	  count = 0;
	}
      if (line == 4)
	{
	pspDebugScreenSetTextColor(0x00EE1111);
	pspDebugScreenSetBackColor(0x00000000);
	pspDebugScreenPrintf ("  %5d\n       ", ndigits);
	pspDebugScreenSetTextColor(0x00EFDDCA);
	pspDebugScreenSetBackColor(0x00000000);

	  line = 0;
	}
    }
}


int
divbig (bignum number, unsigned int x)
/*divides a big number by an unsigned integer, returns zero if result zero,
  otherwise returns 1 */
{
  dword result;
  int j = 0;
  unsigned int rest = 0;
  while (number[j] == 0 && j < MAXSIZE)
    j++;
  if (j == MAXSIZE)
    return (0);
  while (j < MAXSIZE)
    {
      result.w.lo = number[j];
      result.w.hi = rest;
      number[j] = result.L / x;
      rest = result.L % x;
      j++;
    }
  return (1);
}


void
addbig (bignum dest, bignum source)
/*adds "source" to "dest". "addbig" and "subbig" assume that "dest" and
  "source" are both positive and that the result of a subtraction will also
  always be positive. */
{
  int j = MAXSIZE - 1;
  dword result;
  unsigned int remember = 0;
  while (source[j] == 0 && j >= 0)
    j--;
  while (j >= 0)
    {
      result.L = (long long) dest[j] + source[j] + remember;
      dest[j] = result.w.lo;
      remember = result.w.hi;
      j--;
    }
}


void
subbig (bignum dest, bignum source)
/*subtracts source from dest*/
{
  int j = MAXSIZE - 1;
  unsigned int borrow = 0;
  dword result;
  while (source[j] == 0 && j >= 0)
    j--;
  while (j >= 0)
    {
      result.L = (long long) dest[j] - source[j] + (int) borrow;
      dest[j] = result.w.lo;
      borrow = result.w.hi;
      j--;
    }
}


void
setbig (bignum number, int intpart, unsigned int decpart)
/*fill a big number with some numbers*/
{
  int j;
  number[0] = intpart;
  for (j = 1; j < MAXSIZE; j++)
    number[j] = decpart;
}
 

void
atanbig (bignum A, unsigned int x)
/*Calculates the arc tangent of (1/x) by series summation.
  Result is stored in big number A */
{
  bignum X, Y;
  unsigned int n = 1;
  setbig (X, 1, 0);
  divbig (X, x);
  copybig (A, X);
  x *= x;
  while (1)
    {
      n += 2;
      divbig (X, x);
      copybig (Y, X);
      if (!divbig (Y, n))
	break;
      if (n & 2)
	subbig (A, Y);
      else
	addbig (A, Y);
    }
}


void
make_pi (void)
/*Calculates and prints pi using Machin's formula:
     pi = 16 * atan(1/5)  - 4 * atan (1/239)  */
{
  bignum A, B;
  atanbig (A, 5);
  atanbig (B, 239);
  multbig (A, 16);
  multbig (B, 4);
  subbig (A, B);
  pspDebugScreenPrintf ("\nPI= ");
  printbig (A);
  pspDebugScreenPrintf ("\n");
}


main () {

	SceCtrlData pad;
	pspDebugScreenInit();
	pspDebugScreenSetTextColor(0x00EFDDCA);
	pspDebugScreenSetBackColor(0x00000000);

while (1) {

while (1){

if (printed == 0) {
  pspDebugScreenPrintf ("\n Pi Calc V1 - Art '07\n\n");
  pspDebugScreenPrintf (" Press:\n");
  pspDebugScreenPrintf ("   Circle to calculate Pi to 100 decimal places,\n");
  pspDebugScreenPrintf ("    Cross to calculate Pi to 1120 decimal places,\n");
  pspDebugScreenPrintf ("   Square to calculate Pi to 10000 decimal places,\n");
  pspDebugScreenPrintf (" Triangle to calculate Pi to 30000 decimal places,\n");
  pspDebugScreenPrintf ("\n");
  pspDebugScreenPrintf (" PSP unit clock speed is not altered by this program.\n");

printed = 1;

} // printed

  			sceCtrlSetSamplingMode(1); 
			sceCtrlPeekBufferPositive(&pad, 1);

			if(pad.Buttons & PSP_CTRL_CROSS) {
			MAXDIGITS = 1120;
			pspDebugScreenClear();
			break;
			}
			if(pad.Buttons & PSP_CTRL_CIRCLE) {
			MAXDIGITS = 100;
			pspDebugScreenClear();
			break;
			}
			if(pad.Buttons & PSP_CTRL_TRIANGLE) {
			MAXDIGITS = 30000;
			pspDebugScreenClear();
			break;
			}
			if(pad.Buttons & PSP_CTRL_SQUARE) {
			MAXDIGITS = 10000;
			pspDebugScreenClear();
			break;
			}

			if(pad.Buttons & PSP_CTRL_HOME) {
			pspDebugScreenClear();
			_sw(0, 0);		// cause exception to crash PSP unit (1)
			asm("break\n");		// cause exception to crash PSP unit (2)
			sceKernelExitGame();
			return 0;
			}

} // while




  int N = MAXDIGITS;
  pspDebugScreenPrintf ("\n CALCULATING PI TO %d DECIMAL PLACES...\n", N);
  make_pi ();
  pspDebugScreenPrintf ("\n DONE! PRESS CROSS TO CONTINUE.");

while (1){
  			sceCtrlSetSamplingMode(1); 
			sceCtrlPeekBufferPositive(&pad, 1);

			if(pad.Buttons & PSP_CTRL_CROSS) {
			printed = 0;
			pspDebugScreenClear();
			break;
			}
} // wait for button push
	for(ffls=0; ffls<110; ffls++) {sceDisplayWaitVblankStart();}







} // while
        sceKernelExitGame();
	return 0;
}





