#include <stdio.h>
#include <stddef.h>
#include <locale.h>
 
int main(void) {
	wchar_t Board[3];
	setlocale(LC_ALL, "");
	Board[0] = L'★';
	Board[1] = L'☆';
	Board[2] = L'+';
	printf("%lc", Board[0]);
	printf("%lc", Board[1]);
	printf("%lc", Board[2]);
}
