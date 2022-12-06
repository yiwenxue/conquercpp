#include <cstdio>
#include <cstring>
#include <memory>

int main() {

  char *str = new char[100];

  printf(str);

  for (int i = 0; i < 100; i++) {
    printf("0x%x\n", str[i]);
  }

  for (int i = 0; i < 100; i++) {
    str[i] = 0x000000cd;
  }

  printf(str);

  return 0;
}
