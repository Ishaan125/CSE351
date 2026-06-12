/*
 * CSE 351 Lab 1b (Manipulating Bits in C)
 *
 * Name(Ishaan Srivastava):  
 * NetID(ishaan03): 
 *
 * ----------------------------------------------------------------------------
 * Overview
 * ----------------------------------------------------------------------------
 *  This is a program to keep track of the items in a small aisle of a store.
 *
 *  A store's aisle is represented by a 64-bit long integer, which is broken
 *  into 4 16-bit sections representing one type of item each. Note that since
 *  a section is 16-bits, it fits nicely into C's short datatype.
 *
 *  Aisle Layout:
 *
 *    Within an aisle, sections are indexed starting with the least-significant
 *    section being at index 0 and continuing up until one less than the number
 *    of sections.
 *
 *    Example aisle:
 *
 *                MSB                                                       LSB
 *                  +-------------+-------------+-------------+-------------+
 *                  |  Section 3  |  Section 2  |  Section 1  |  Section 0  |
 *                  +-------------+-------------+-------------+-------------+
 *                  |             |             |             |             |
 *      bit offset: 64            48            32            16            0
 *
 *  Section Layout:
 *
 *    A section in an aisle is broken into 2 parts. The 6 most significant bits
 *    represent a unique identifier for the type of item stored in that
 *    section. The rest of the bits in a section (10 least significant)
 *    indicate individual spaces for items in that section. For each of the 10
 *    bits/spaces, a 1 indicates that an item of the section's type is stored
 *    there and a 0 indicates that the space is empty.
 *
 *    Example aisle section: 0x651A
 *
 *                MSB                               LSB
 *                  +-----------+-------------------+
 *                  |0 1 1 0 0 1|0 1 0 0 0 1 1 0 1 0|
 *                  +-----------+-------------------+
 *                  | item id   | section spaces    |
 *      bit offset: 16          10                  0
 *
 *      In this example, the item id is 0b011001, and there are currently 4
 *      items stored in the section (at bit offsets 8, 4, 3, and 1) and 6
 *      vacant spaces.
 *
 *  Written by Porter Jones (pbjones@cs.washington.edu)
 */

#include "aisle_manager.h"
#include "store_util.h"

// the number of total bits in a section
#define SECTION_SIZE 16

// The number of bits in a section used for the item spaces
#define NUM_SPACES 10

// The number of bits in a section used for the item id
#define ID_SIZE 6

// The number of sections in an aisle
#define NUM_SECTIONS 4

// TODO: Fill in these with the correct hex values

// Mask for extracting a section from the least significant bits of an aisle.
// (aisle & SECTION_MASK) should preserve a section's worth of bits at the
// lower end of the aisle and set all other bits to 0. This is essentially
// extracting section 0 from the example aisle shown above.
#define SECTION_MASK 0x000000000000FFFF

// Mask for extracting the spaces bits from a section.
// (section & SPACES_MASK) should preserve all the spaces bits in a section and
// set all non-spaces bits to 0.
#define SPACES_MASK 0x03FF

// Mask for extracting the ID bits from a section.
// (section & ID_MASK) should preserve all the id bits in a section and set all
// non-id bits to 0.
#define ID_MASK 0xFC00


/* Given a pointer to an aisle and a section index, return the section at the
 * given index of the given aisle.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
unsigned short get_section(unsigned long* aisle, int index) {
  // shift the aisle to the right by the appropriate number of bits and then keep the lower 16 bits
  return (unsigned short) (*aisle >> (index * SECTION_SIZE)) & SECTION_MASK;
}

/* Given a pointer to an aisle and a section index, return the spaces of the
 * section at the given index of the given aisle. The returned short should
 * have the least 10 significant bits set to the spaces and the 6 most
 * significant bits set to 0.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
unsigned short get_spaces(unsigned long* aisle, int index) {
  // apply the get_section method and then keep the lower 10 bits to get just the spaces bits
  return get_section(aisle, index) & SPACES_MASK;
}

/* Given a pointer to an aisle and a section index, return the id of the
 * section at the given index of the given aisle. The returned short should
 * have the least 6 significant bits set to the id and the 10 most significant
 * bits set to 0.
 *
 * Example: if the section is 0b0110010100011010, return 0b0000000000011001.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
unsigned short get_id(unsigned long* aisle, int index) {
  // shift the aisle to the right by the appropriate number of bits and then keep the upper 6 bits
  return (get_section(aisle, index) & ID_MASK) >> (NUM_SPACES);
}

/* Given a pointer to an aisle, a section index, and a short representing a new
 * bit pattern to be used for section, set the section at the given index of
 * the given aisle to the new bit pattern.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void set_section(unsigned long* aisle, int index, unsigned short new_section) {
  // apply a mask to clear the bits for the section at the given index and then set the bits for the section to new_section
  unsigned long longNew = ((unsigned long) new_section) << (index * SECTION_SIZE);
  *aisle = (*aisle & ~(((unsigned long) SECTION_MASK) << (index * SECTION_SIZE))) | longNew;
}

/* Given a pointer to an aisle, a section index, and a short representing a new
 * bit pattern to be used for the spaces of the section, set the spaces for the
 * section at the given index of the given aisle to the new bit pattern.
 * 
 * If new_spaces is invalid (if it has 1s outside of its 10 least significant
 * bits), this method should return without modifying anything.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void set_spaces(unsigned long* aisle, int index, unsigned short new_spaces) {
  // apply a mask to clear the bits for the spaces at the given index and then set the bits for the spaces to new_spaces
  if (new_spaces & ~SPACES_MASK) {
    return;
  }
  unsigned long longNew = ((unsigned long) new_spaces) << (index * SECTION_SIZE);
  *aisle = (*aisle & ~(((unsigned long) SPACES_MASK) << (index * SECTION_SIZE))) | longNew;
}

/* Given a pointer to an aisle, a section index, and a short representing a new
 * bit pattern to be used for the id of the section, set the id for the section
 * at the given index of the given aisle to the new bit pattern.
 * 
 * If new_id is invalid (if it has 1s outside of its 6 least significant
 * bits), this method should return without modifying anything.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void set_id(unsigned long* aisle, int index, unsigned short new_id) {
  // apply a mask to clear the bits for the id at the given index and then set the bits for the id to new_id
  if ((new_id >> ID_SIZE) != 0) {
    return;
  }
  unsigned long longNew = ((unsigned long) new_id) << (index * SECTION_SIZE + NUM_SPACES);
  *aisle = (*aisle & ~(((unsigned long) ID_MASK) << (index * SECTION_SIZE))) | longNew;
}

/* Given a pointer to an aisle, a section index, and a space index, toggle the
 * item in the given space index of the section at the given section index in
 * the given aisle. Toggling means that if the space was previously empty, it
 * should now be filled with an item, vice-versa.
 *
 * Can assume the section index is a valid index (0-3 inclusive).
 * Can assume the spaces index is a valid index (0-9 inclusive).
 */
void toggle_space(unsigned long* aisle, int index, int space_index) {
  // shift to the left then ^ to toggle the bit for the space at the given index
  unsigned long longNew = ((unsigned long) 1) << (space_index + (index * SECTION_SIZE));
  *aisle = *aisle ^ longNew;
}

/* Given a pointer to an aisle and a section index, return the number of items
 * in the section at the given index of the given aisle.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
unsigned short num_items(unsigned long* aisle, int index) {
  // apply the get_spaces method and then count the number of 1s in the spaces bits
  unsigned short space = get_spaces(aisle, index);
  unsigned short count = 0;
  for (int i = 0; i < NUM_SPACES; i++) {
    if ((space & 1) == 1) {
      count++;
    }
    space = space >> 1;
  }
  return count;
}

/* Given a pointer to an aisle, a section index, and the desired number of
 * items to add, add at most the given number of items to the section at the
 * given index in the given aisle. Items should be added to the least
 * significant spaces possible. If n is larger than or equal to the number of
 * empty spaces in the section, then the section should appear full after the
 * method finishes.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void add_items(unsigned long* aisle, int index, int n) {
  // apply the get_spaces method and then apply a mask to set the bits for the spaces at the given index until n items have been added or there are no more empty spaces
  unsigned short curr = 1;
  unsigned short new = get_spaces(aisle, index);
  for (int i = 0; i < NUM_SPACES && n > 0; i++) {
    if ((new & curr) == 0) {
      n--;
    }
    new = new | curr;
    curr = curr << 1;
  }
  set_spaces(aisle, index, new);
}

/* Given a pointer to an aisle, a section index, and the desired number of
 * items to remove, remove at most the given number of items from the section
 * at the given index in the given aisle. Items should be removed from the
 * least significant spaces possible. If n is larger than or equal to the
 * number of items in the section, then the section should appear empty after
 * the method finishes.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void remove_items(unsigned long* aisle, int index, int n) {
  // apply the get_spaces method and then apply a mask to clear the bits for the spaces at the given index until n items have been removed or there are no more items
  unsigned short curr = 1;
  unsigned short new = get_spaces(aisle, index);
  for (int i = 0; i < NUM_SPACES && n > 0; i++) {
    if ((new & curr) != 0) {
      n--;
    }
    new = new & ~curr;
    curr = curr << 1;
  }
  set_spaces(aisle, index, new);
}

/* Given a pointer to an aisle, a section index, and a number of slots to
 * rotate by, rotate the items in the section at the given index of the given
 * aisle to the left by the given number of slots.
 *
 * Example: if the spaces are 0b0111100001, then rotating left by 2 results
 *          in the spaces     0b1110000101
 *
 * Can assume the index is a valid index (0-3 inclusive).
 * Can NOT assume n < NUM_SPACES (hint: find an equivalent rotation).
 */
void rotate_items_left(unsigned long* aisle, int index, int n) {
  // apply a mask to rotate the bits for the spaces at the given index to the left by n, then or with the bottom bits
  n = n % NUM_SPACES;
  unsigned short top = (unsigned short) (((get_spaces(aisle, index) >> (NUM_SPACES - n)) | (get_spaces(aisle, index) << n)) & SPACES_MASK);
  set_spaces(aisle, index, top);
}

/* Given a pointer to an aisle, a section index, and a number of slots to
 * rotate by, rotate the items in the section at the given index of the given
 * aisle to the right by the given number of slots.
 *
 * Example: if the spaces are 0b1000011110, then rotating right by 2 results
 *          in the spaces     0b1010000111
 *
 * Can assume the index is a valid index (0-3 inclusive).
 * Can NOT assume n < NUM_SPACES (hint: find an equivalent rotation).
 */
void rotate_items_right(unsigned long* aisle, int index, int n) {
  // apply a mask to rotate the bits for the spaces at the given index to the right by n, then or with the top bits
  n = n % NUM_SPACES;
  unsigned short bottom = (unsigned short) (((get_spaces(aisle, index) << (NUM_SPACES - n)) | (get_spaces(aisle, index) >> n)) & SPACES_MASK);
  set_spaces(aisle, index, bottom);
}
