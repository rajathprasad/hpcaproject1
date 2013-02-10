#include <cstdio>
#include <cmath>
#define ADDRESS_SIZE 64

/*function declarations */
uint64_t get_offset( uint64_t address,uint64_t B);
uint64_t get_index( uint64_t address,uint64_t C ,uint64_t S, uint64_t B);
uint64_t get_tag( uint64_t address,uint64_t C,uint64_t B,uint64_t S );


uint64_t get_offset( uint64_t address,uint64_t B)
{
	//uint64_t block_size = pow(2,B);
	uint64_t offset;
	//printf("B is %ld\n",B);
	uint64_t shift = ADDRESS_SIZE - B;
	//printf("shift is %ld\n",shift);
	offset = address << shift;
	offset = offset >> shift;
	//printf("offset is %x\n",offset);
	return offset;

}

uint64_t get_index( uint64_t address,uint64_t C ,uint64_t S, uint64_t B)
{
	uint64_t cache_index;
	//printf("address  is %x\n",address);
	uint64_t shift = ADDRESS_SIZE - ( C - S );
	//printf("shift is %ld\n", shift);
	cache_index = address << shift;
	//printf("index  is %x\n",cache_index);
	cache_index = cache_index >> ADDRESS_SIZE - ( C - S ) + B;

	//printf("index  is %x\n",cache_index);
	return cache_index;
}

uint64_t get_tag( uint64_t address,uint64_t C,uint64_t B,uint64_t S )
{
	uint64_t tag;
	uint64_t shift = C - S;
	//printf("address  is %llx\n",address);	
	//printf("shift is %lld\n", shift);
	tag = address >> shift;
	//printf("tag  is %llx\n",tag);
	//tag = tag & ((1 << shift) - 1);
	//printf("tag  is %llx\n",tag);
	return tag;

}



