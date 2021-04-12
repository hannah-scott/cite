#include "cite.c"

int main(int argc, char *argv[])
{ 
  char target[10] = { '\0' };
  char test[10] = "01234567";

  sct(target, test, 6);

  printf("%s\n", target);
}
