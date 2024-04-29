#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef char ALIGN[16];
typedef union block_header
{
  struct 
  {
    size_t size;
    int is_free;
    union block_header *next;
  } definition;
  ALIGN align;
} block_header;

block_header *head, *tail;
pthread_mutex_t global_lock;

void* get_free_block(size_t size)
{
  block_header *current_block = head;
  while (current_block){
    if (current_block->definition.is_free && current_block->definition.size >= size)
      return current_block;

    current_block = head->definition.next;
  }
  return NULL;
}

void* malloc(size_t size)
{
  unsigned int total_size = size + sizeof(block_header);
  void *block;
  block_header *header;

  if(!size)
    return NULL;

  pthread_mutex_lock(&global_lock);
  header = get_free_block(size);
  if(header){
    header->definition.is_free = 0;
    pthread_mutex_unlock(&global_lock);
    return (void *)(header + 1);
  }

  block = sbrk(total_size);
  if(block == (void*) - 1){
    pthread_mutex_unlock(&global_lock);
    return NULL;
  }

  header = block;
  header->definition.size = size;
  header->definition.is_free = 0;
  header->definition.next = NULL;

  if (!head)
    head = header;
    
  if(tail)
    tail->definition.next = header;

  tail = header;

  pthread_mutex_unlock(&global_lock);
  return (void*)(header+1);
}

void free(void* block)
{
  if(!block)
    return;

  pthread_mutex_lock(&global_malloc_lock);
  block_header *header = block - 1;

  programbreak = sbrk(0);
  if((char*)block + header->definition.size == programbreak)
    {
      if(head == tail){
	head = tail = NULL;
      }else{
	tmp = head;
	while(tmp){
	  if(tmp->definition.next == tail){
	    tmp->definition.next = null;
	    tail = tmp;
	  }
	  tmp = tmp->definition.next;
	}
      }
      sbrk(0 - sizeof(block_header) - header->definition.size);
      pthread_mutex_unlock(&global_malloc_lock);
      return;
    }
  header->definition.is_free = 1;
  pthread_mutex_unlock(&global_malloc_lock);
}

int main(int argc, char const *argv[])
{
  /* code */
  return 0;
}

