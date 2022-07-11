#include <bootloader/utility.h>
#include <string.h>

int Utility::versionSum(const char * version, int length) {
  int sum = 0;
  for (int i  = 0; i < length; i++) {
    sum += version[i] * (strlen(version) * 100 - i * 10);
  }
  return sum;
}

int Utility::atoi(const char * str) {
  int result = 0;
  for (int i = 0; str[i] != '\0'; i++) {
    result = result * 10 + str[i] - '0';
  }
  return result;
}
