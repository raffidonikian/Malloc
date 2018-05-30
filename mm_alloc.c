#include "mm_alloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


size_t blocksize = sizeof(struct block);
struct block* head;

struct block* find(size_t size) {
  struct block *temp = head->next;
  while(temp!= NULL) {
    if (temp->free  && temp->size >= size) {
      return temp;
    }
    temp = temp->next;
  }
  return NULL;
}

void *mm_malloc(size_t size) {
    if (head == NULL) {
      head = sbrk(blocksize);
      head->next = NULL;
    }
    if (size == 0) {
      return NULL;
    }
    //HERE TRY TO FIT MALLOC INTO EXISITING STUFF
    struct block* try = find(size);
    if (try) {
      memset(try->startofmem, 0, try->size);
      return try->startofmem;
    }
    //creating new block at end of heap
    struct block* temp = head;
    struct block* lastvalid = head;
    while (temp != NULL) {
      lastvalid = temp;
      temp = temp->next;
    }
    struct block* create = (struct block*) sbrk(blocksize + size);
    if (create == (void*) -1) {
      return NULL;
    }
    create->size = size;
    create->free = 0;
    create->startofmem = (void *) create + blocksize;
    memset(create->startofmem, 0, create->size);
    lastvalid->next = create;
    create->prev = lastvalid;
    create->next = NULL;
    return create->startofmem;
}

void *mm_realloc(void *ptr, size_t size) {
    if (ptr == NULL) {
        return mm_malloc(size);
    }
    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }
    struct block *temp = ptr - blocksize;
    size_t sizeofdata;
    if (size < temp->size) {
      sizeofdata = size;
    }
    else {
      sizeofdata = temp->size;
    }
    void *returnee = mm_malloc(size);
    if (returnee == NULL) {
      return NULL;
    }
    memcpy(returnee, temp->startofmem, sizeofdata);
    mm_free(ptr);
    return returnee;
}

void mm_free(void *ptr) {
    if (ptr == NULL) {
      return;
    }
    struct block* tofree = ptr - blocksize;
    tofree->free = 1;
    struct block* lastfree = tofree;
    while (lastfree->next != NULL && lastfree->next->free) {
      lastfree = lastfree->next;
    }
    tofree->size = (lastfree->startofmem + lastfree->size - tofree->startofmem);
    if (lastfree->next == NULL) {
      tofree->next = NULL;
    }
    else {
      tofree->next = lastfree->next;
    }
}
