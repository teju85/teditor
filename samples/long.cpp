#include <stdio.h>
#include <stdlib.h>

int factorial(int n) {
  if (n == 1) return 1;
  return n * factorial(n - 1);
}

int main(int argc, char** argv) {
  int n = 1;
  if (argc == 2) n = atoi(argv[i]);
  printf("factorial(%d) = %d\n", factorial(n));
  return 0;
}
