#include "src/student_code.h"
#include <criterion/criterion.h>
#include <stdio.h>

#define NUMBER_OF_REPEATS 10

void write_string(MMU m, VirtualAddress va, char* str) {
  int i;
  for (i = 0; str[i] != '\0'; i++) {
    write_byte(m, va+i, str[i]);
  }
    write_byte(m, va+i, '\0');
}

int main() {

  // See the random number
  printf("VALID BIT: %ld\n", VALID_BIT);
  printf("READ BIT: %ld\n", READ_BIT);
  printf("Frame size: %d\n", PHYSICAL_MEMORY_SIZE / NUM_FRAMES);

  MMU m = new__MMU_pagetable();

  for (VPN vpn = 0; vpn < NUM_PAGES ; vpn++) {
    if (vpn == NUM_PAGES - 1) {
      m.map_page(&m, vpn, true, true, true);
    } else {
      m.map_page(&m, vpn, vpn % 2 == 0, vpn % 3 == 0, vpn % 5 == 0);
    }
  }

  VirtualAddress va = ((0b0 << NUM_VPN_BITS)  << NUM_OFFSET_BITS) | 0b0;

  char* str;

  str = "hello";
  write_string(m, va, str);
  cr_assert_str_eq(
    (char*)get_page(m, va, true, false, false),
    str
  );


  str = "hello";
  write_string(m, va, str);
  cr_assert_str_eq(
    (char*)get_page(m, va, true, false, false),
    str
  );
  for (int i = 0; i < strlen(str); i++) {
    cr_assert_eq(
      read_byte(m, va+i),
      str[i]
    );
  }

  va = (((0b1<<NUM_VPN_BITS)-1)<< NUM_OFFSET_BITS) | 0b0;
  str = "bye";
  write_string(m, va, str);
  cr_assert_str_eq(
    (char*)get_page(m, va, true, false, false),
    str
  );
  for (int i = 0; i < strlen(str); i++) {
    cr_assert_eq(
      read_byte(m, va+i),
      str[i]
    );
  }
}
