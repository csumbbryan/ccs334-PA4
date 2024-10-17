#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

#include "student_code.h"

bool is_entry_valid(PageTableEntry pte) {
  // todo;
  return is_bit_set(pte, VALID_BIT);
}

bool is_read_enabled(PageTableEntry pte) {
  // todo
  // Question: Implement this function.  Hint: it can take as little as 1 line.
  return is_bit_set(pte, READ_BIT);
}

bool is_write_enabled(PageTableEntry pte) {
  // todo
  return is_bit_set(pte, WRITE_BIT);
}

bool is_execute_enabled(PageTableEntry pte) {
  // todo
  return is_bit_set(pte, EXEC_BIT);
}


PFN find_free_page(MMU m) {
  // todo
  for(int i = 0; i < NUM_FRAMES; i++) {
    if(m.page_used[i] == false) {
      return i;
    }
  }
  return -1;
}

PFN convert_PageTableEntry_to_PFN(PageTableEntry pte) {
  // todo
  //pte = pte >> ();
  pte = pte & get_mask(NUM_PFN_BITS);
  return pte;
}


/*
 * Page Table Functions
 */
PageTableEntry get_pagetableentry__MMU_pagetable(MMU m, VPN vpn) {
  // todo
  if(m.page_pointer == NULL) {
    return (PageTableEntry) NULL;
  } else {
    return m.page_pointer[vpn];
  }
}

PFN map_page__MMU_pagetable(MMU* m, VPN vpn, bool can_read, bool can_write, bool can_exec) {
  // todo
  PFN pfn = find_free_page(*m);
  PageTableEntry pte =
    (((long unsigned int)1 << (VALID_BIT-1))
      + ((long unsigned int)can_read << (READ_BIT-1))
      + ((long unsigned int)can_write << (WRITE_BIT-1))
      + ((long unsigned int)can_exec << (EXEC_BIT-1)))
    +
    ((vpn & get_mask(NUM_VPN_BITS)) << (NUM_PFN_BITS + NUM_OFFSET_BITS))
    +
    ((pfn & get_mask(NUM_PFN_BITS)));
  m->page_used[pfn] = true;
  m->page_pointer[vpn] = pte;
  return pfn;
}


/*
 * Page Directory Functions
 */

Page* get_page(MMU m, VirtualAddress va, bool for_read, bool for_write, bool for_execute) {
  // todo
  VPN vpn = va >> NUM_OFFSET_BITS;
  PageTableEntry pte = get_pagetableentry__MMU_pagetable(m, vpn);
  if(is_entry_valid(pte)) {
    if((!is_read_enabled(pte) && for_read )
     || (!is_write_enabled(pte) && for_write)
     || (!is_execute_enabled(pte) && for_execute)) {
       return -1;
     } else {
       return ((pte & get_mask(NUM_PFN_BITS)) << NUM_OFFSET_BITS) + (void*) m.physical_memory;
     }
  } else {
    return -1;
  }
}

char read_byte(MMU m, VirtualAddress va) {
  VPN vpn = va >> NUM_OFFSET_BITS;
  PageTableEntry pte = get_pagetableentry__MMU_pagetable(m, vpn);
  OFFSET offset = va & get_mask(NUM_OFFSET_BITS);
  if(is_entry_valid(pte) && is_read_enabled(pte)) {
    Page* page = get_page(m, va, true, false, false);
    return page->data[offset];
  } else {
    return -1;
  }
}

void write_byte(MMU m, VirtualAddress va, char val) {
  // todo
  VPN vpn = va >> NUM_OFFSET_BITS;
  PageTableEntry pte = get_pagetableentry__MMU_pagetable(m, vpn);
  OFFSET offset = va & get_mask(NUM_OFFSET_BITS);
  if(is_entry_valid(pte) && is_write_enabled(pte)) {
    Page* page = get_page(m, va, false, true, false);
    page->data[offset] = val;
  }
}
