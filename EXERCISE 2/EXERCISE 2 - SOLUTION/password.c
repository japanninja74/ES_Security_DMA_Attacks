#include "password.h"

bool passwordcheck(char *val) {
  if (val[0] == '0') {
    if (val[1] == '0') {
      if (val[2] == '4') {
        if (val[3] == '2') {
          return true;
        }
      }
    }
  }
  return false;
}