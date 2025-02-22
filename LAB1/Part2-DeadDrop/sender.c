
#include"util.h"
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>

// TODO: define your own buffer size
#define BUFF_SIZE (1<<21)
//#define BUFF_SIZE [TODO]

uint64_t get_set_index(uint64_t addr) {
  uint64_t lower_bit_mask = (1<<15) - 1;
  return (addr & lower_bit_mask)>>6;
}

int main(int argc, char **argv)
{
  // Allocate a buffer using huge page
  // See the handout for details about hugepage management
  void *buf= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
  
  if (buf == (void*) - 1) {
     perror("mmap() error\n");
     exit(EXIT_FAILURE);
  }
  // The first access to a page triggers overhead associated with
  // page allocation, TLB insertion, etc.
  // Thus, we use a dummy write here to trigger page allocation
  // so later access will not suffer from such overhead.
  //*((char *)buf) = 1; // dummy write to trigger page allocation
  

  int sets_to_watch[8] = {10, 45, 121, 180, 235, 312, 397, 447};
  uint64_t eviction_set[8][8];

  
  int assoc = 8;
  int bl_offset_size = 64;
  int bl_offset_bits = 6;
  int set_ind_bits = 9;
  int num_sets = 512;

  char* buffer = (char *)malloc(262144*sizeof(char));

  uint64_t start_add = (uint64_t) buffer;
  printf("Buffer start address is 0x%lx\n", start_add);
  for(int set_id=0; set_id<8; set_id++) {
      int address_count = 0;
      for(int set_index = 0; set_index < 512; set_index++) {
	  for(int way_index = 0; way_index < assoc; way_index++) {
	      uint64_t  address = (uint64_t) (buffer + (set_index * assoc * bl_offset_size) + (way_index* bl_offset_size));
	      if(get_set_index(address) == sets_to_watch[set_id]) {
		  eviction_set[set_id][address_count] = address;
		  address_count++;
	      }
	  }
      }
  }

  printf("Buffer is allocated %c\n", *((char *)buf)); 
  // TODO:
  // Put your covert channel setup code here


  printf("Please type a message.\n");

  bool sending = true;
  while (sending) {
      char text_buf[128];
      fgets(text_buf, sizeof(text_buf), stdin);
      sending=false;
      // TODO:
      // Put your covert channel code here
      char *msg = string_to_binary(text_buf);
      printf("MSG is %s\n",msg);
      char *debug_msg = binary_to_string(msg);
      printf("Debug msg is %s\n", debug_msg);
      int i = 0;

      int len = 8;
      for(int j = 0; j < len; j++) {
	 if(msg[j] == '1'){
	    printf("Sender is evicting for set[%d]\n",sets_to_watch[j]);
	    for(int reps= 0; reps < 10000; reps++) {
		for(int way_num = 0; way_num < assoc; way_num++) {
	    	    uint64_t *ptr = (uint64_t *) eviction_set[j][way_num];
		    volatile char tmp = *ptr;
		}
	    }  
	 }
	 else {;}
      }
  }

  printf("Sender finished.\n");

  free(buffer);
  return 0;
}


