/*
 * CSE 351 Lab 1b (Manipulating Bits in C)
 *
 * Name(Ishaan Srivastava):  
 * NetID(ishaan03): 
 *
 * This is a file for managing a store of various aisles, represented by an
 * array of 64-bit integers. See aisle_manager.c for details on the aisle
 * layout and descriptions of the aisle functions that you may call here.
 *
 * Written by Porter Jones (pbjones@cs.washington.edu)
 */

#include <stddef.h>  // To be able to use NULL
#include "aisle_manager.h"
#include "store_client.h"
#include "store_util.h"

// Number of aisles in the store
#define NUM_AISLES 10

// Number of sections per aisle
#define SECTIONS_PER_AISLE 4

// Number of items in the stockroom (2^6 different id combinations)
#define NUM_ITEMS 64

// Global array of aisles in this store. Each unsigned long in the array
// represents one aisle.
unsigned long aisles[NUM_AISLES];

// Array used to stock items that can be used for later. The index of the array
// corresponds to the item id and the value at an index indicates how many of
// that particular item are in the stockroom.
int stockroom[NUM_ITEMS];


/* Starting from the first aisle, refill as many sections as possible using
 * items from the stockroom. A section can only be filled with items that match
 * the section's item id. Prioritizes and fills sections with lower addresses
 * first. Sections with lower addresses should be fully filled (if possible)
 * before moving onto the next section.
 */
void refill_from_stockroom() {
  // iterate through the aisles and sections and apply the add_items method, removing the added items from the stockroom
  for (int i = 0; i < NUM_AISLES; i++) {
    for (int j = 0; j < SECTIONS_PER_AISLE; j++) {
      unsigned short id = get_id(&aisles[i], j);
      if (stockroom[id] > 0) {
        int before = num_items(&aisles[i], j);
        add_items(&aisles[i], j, stockroom[id]);
        int added = num_items(&aisles[i], j) - before;
        stockroom[id] -= added;
      }
    }
  }
}

/* Remove at most num items from sections with the given item id, starting with
 * sections with lower addresses, and return the total number of items removed.
 * Multiple sections can store items of the same item id. If there are not
 * enough items with the given item id in the aisles, first remove all the
 * items from the aisles possible and then use items in the stockroom of the
 * given item id to finish fulfilling an order. If the stockroom runs out of
 * items, you should remove as many items as possible.
 */
int fulfill_order(unsigned short id, int num) {
  // iterate through the aisles and sections and remove items with the given id. If there are not enough items in the aisles, remove items from the stockroom until num items have been removed or the stockroom runs out of items.
  // keep track of the total number of items removed to return at the end
  int total = 0;
  for (int i = 0; i < NUM_AISLES && num > 0; i++) {
    for (int j = 0; j < SECTIONS_PER_AISLE && num > 0; j++) {
      if (id == get_id(&aisles[i], j)) {
        int items = num_items(&aisles[i], j);
        int to_remove = 0;
        if (items < num) {
          to_remove = items;
        } 
        else {
          to_remove = num;
        }
        remove_items(&aisles[i], j, to_remove);
        total += to_remove;
        num -= to_remove;
      }
    }
  }

  if (num > 0) {
    int from_stock = 0;
    if (stockroom[id] < num) {
      from_stock = stockroom[id];
    }
    else {
      from_stock = num;
    }
    total += from_stock;
    stockroom[id] -= from_stock;
  }
  return total;
}

/* Return a pointer to the first section in the aisles with the given item id
 * that has no items in it or NULL if no such section exists. Only consider
 * items stored in sections in the aisles (i.e., ignore anything in the
 * stockroom). Break ties by returning the section with the lowest address.
 */
unsigned short* empty_section_with_id(unsigned short id) {
  // iterate through the aisles and sections and apply the get_id and num_items methods to find the first section with the given id that has no items in it
  for (int i = 0; i < NUM_AISLES; i++) {
    for (int j = 0; j < SECTIONS_PER_AISLE; j++) {
      if (id == get_id(&aisles[i], j) && num_items(&aisles[i], j) == 0) {
        return (unsigned short*)&aisles[i] + j;
      }
    }
  }
  return NULL;
}

/* Return a pointer to the section with the most items in the store. Only
 * consider items stored in sections in the aisles (i.e., ignore anything in
 * the stockroom). Break ties by returning the section with the lowest address.
 */
unsigned short* section_with_most_items() {
  // iterate through the aisles and sections and keep track of the max items and the section with the max items
  unsigned short* max = NULL;
  int maxItems = -1;
  for (int i = 0; i < NUM_AISLES; i++) {
    for (int j = 0; j < SECTIONS_PER_AISLE; j++) {
      if (num_items(&aisles[i], j) > maxItems) {
        maxItems = num_items(&aisles[i], j);
        max = (unsigned short*)&aisles[i] + j;
      }
    }
  }
  return max;
}
