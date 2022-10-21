
#include"util.h"
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>
#include <time.h>

uint64_t get_set_index(uint64_t addr) {
  uint64_t lower_bit_mask = (1<<15) - 1;
  return (addr & lower_bit_mask)>>6;
}

int main(int argc, char **argv)
{
	// Put your covert channel setup code here

	int set_to_watch = 270;
  	uint64_t eviction_set[8];


  	int assoc = 8;
 	int bl_offset_size = 64;
	int bl_offset_bits = 6;
	int set_ind_bits = 9;
	int num_sets = 512;


	char* buffer = (char *)malloc(262144*sizeof(char));

	uint64_t start_add = (uint64_t) buffer;
	printf("Finding eviction set addresses\n");
	printf("Buffer start address is 0x%lx\n", start_add);
    	int address_count = 0;
    	for(int set_index = 0; set_index < 512; set_index++) {
        	for(int way_index = 0; way_index < assoc; way_index++) {
			//printf("set index = %d, way_index = %d", set_index, way_index);
           		uint64_t  address = (uint64_t) (buffer + (set_index * assoc * bl_offset_size) + (way_index* bl_offset_size));
            		if(get_set_index(address) == set_to_watch) {
               			eviction_set[address_count] = address;
               			address_count++;
           		}
        	}
    	}
	clock_t start, curr;


	printf("Priming cache now\n");
	for(int reps = 0; reps <5000; reps++) {
		for(int way_num = 0; way_num < assoc; way_num++) {
			uint64_t *temp_ptr = eviction_set[way_num];
			uint64_t temp = *temp_ptr;
		}
	}
	

	printf("prime is done\n");

	start = clock();

	curr = start;

	while(curr - start <10000000) {
		curr = clock();
	}

	while(curr - start <10000000) {
		curr = clock();
	}
//	printf("Please press enter.\n");

//	char text_buf[2];
//	fgets(text_buf, sizeof(text_buf), stdin);

//	printf("Receiver now listening.\n");
	
	bool listening = true;
	while (listening) {
		printf("Probing the cache accesses now\n");
		// Put your covert channel code here
		for(int way_num = 0; way_num < assoc; way_num++) {
			int time = measure_one_block_access_time(eviction_set[way_num]);
			printf("Address: 0x%lx, way: %d, set: %d, time required: %d\n", eviction_set[way_num], way_num, set_to_watch, time); 
			}	
		printf("Probing finished\n");
		listening = false;
	}


	printf("Receiver finished.\n");

	return 0;
}


