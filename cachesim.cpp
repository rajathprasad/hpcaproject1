#include "cachesim.hpp"
#include "helper_functions.cpp"
#include <cmath>
#include <cstring>


/* global declaration */
uint64_t C, B, S, num_indices, blocks_per_set,offsets_per_block;
char F, R;
uint64_t **tag_store;
bool **valid, ***valid_eager;
bool **dirty;
int count = 0;
bool print = false;

uint64_t *mru_tag; /* most recently used tag */

/**
 * Subroutine for initializing the cache. You many add and initialize any global or heap
 * variables as needed.
 * XXX: You're responsible for completing this routine
 *
 * @c The total number of bytes for data storage is 2^C
 * @b The size of a single cache line in bytes is 2^B
 * @s The number of blocks in each set is 2^S
 * @f The fetch policy, BLOCKING or EAGER (refer to project description for details)
 * @r The replacement policy, LRU or NMRU_FIFO (refer to project description for details)
 */

void setup_cache(uint64_t c, uint64_t b, uint64_t s, char f, char r)
{
	C = c;
	B = b;
	S = s;
	F = f;
	R = r;
	num_indices = pow(2, (C-B-S));
	blocks_per_set = pow(2, S);
	offsets_per_block = pow(2,B);
	
	//printf("block per set %llu, %llu\n",blocks_per_set,S);
	/*allocate space for cache stuff*/

	
	tag_store = new uint64_t*[num_indices];
	for(uint64_t i =0; i < num_indices; i++)
	{
		tag_store[i] = new uint64_t[blocks_per_set];
	}

	dirty = new bool*[num_indices];
	for(uint64_t i =0; i < num_indices; i++)
	{
		dirty[i] = new bool[blocks_per_set];
	}

	mru_tag = new uint64_t[num_indices];
	memset(mru_tag,0,num_indices*sizeof(uint64_t));

	if(F == BLOCKING)
	{
		valid = new bool*[num_indices];
		for(uint64_t i =0; i < num_indices; i++)
		{
			valid[i] = new bool[blocks_per_set];
		}
		for(uint64_t i =0; i < num_indices; i++)
			for(uint64_t j =0; j < blocks_per_set; j++)
			{
				valid[i][j] = false;
				dirty[i][j] = false;
			}
	}

	if(F == EAGER)
	{
		valid_eager = new bool**[num_indices];
		for(uint64_t i =0; i < num_indices; i++)
		{
			valid_eager[i] = new bool*[blocks_per_set];
			for(uint64_t j =0; j < blocks_per_set; j++)
			{
				valid_eager[i][j] = new bool[offsets_per_block];
			}
		}
		for(uint64_t i =0; i < num_indices; i++) {
			for(uint64_t j =0; j < blocks_per_set; j++)
			{
				dirty[i][j] = false;
				for(uint64_t k =0; k < offsets_per_block; k++)
				{
					valid_eager[i][j][k] = false;
				}
			}
		}
	}	
}

void LRU_hit(uint64_t cache_index, uint64_t hit_block)
{
	uint64_t temp = tag_store[cache_index][hit_block];

	if(F == BLOCKING) {
		for(uint64_t j = hit_block; j > 0; j--)
		{
			tag_store[cache_index][j] = tag_store[cache_index][j-1];
			valid[cache_index][j] = valid[cache_index][j-1];
			dirty[cache_index][j] = dirty[cache_index][j-1];
		}	
		tag_store[cache_index][0] = temp;
		valid[cache_index][0] = true;	
	}

	if(F == EAGER) {
		bool *temp_valid = new bool[offsets_per_block];
		for(uint64_t k = 0; k < offsets_per_block; k++)
			temp_valid[k] = valid_eager[cache_index][hit_block][k];

		for(uint64_t j = hit_block; j > 0; j--)
		{
			tag_store[cache_index][j] = tag_store[cache_index][j-1];
			for(uint64_t k = 0; k < offsets_per_block; k++)
				valid_eager[cache_index][j][k] = valid_eager[cache_index][j-1][k];
			dirty[cache_index][j] = dirty[cache_index][j-1];
		}	
		tag_store[cache_index][0] = temp;
		for(uint64_t k = 0; k < offsets_per_block; k++)
			valid_eager[cache_index][0][k] = temp_valid[k];	
	}
	//printf("hit\n");
}


void LRU_miss(uint64_t cache_index, uint64_t tag, uint64_t offset)
{
	if(F == BLOCKING) {
		for(uint64_t j = blocks_per_set - 1; j > 0; j--)
		{
			tag_store[cache_index][j] = tag_store[cache_index][j-1];
			valid[cache_index][j] = valid[cache_index][j-1];
			dirty[cache_index][j] = dirty[cache_index][j-1];
		}
		tag_store[cache_index][0] = tag;
		valid[cache_index][0] = true;
	}

	if(F == EAGER) {
		
		for(uint64_t j = blocks_per_set - 1; j > 0; j--)
		{
			tag_store[cache_index][j] = tag_store[cache_index][j-1];
			for(uint64_t k = 0; k < offsets_per_block; k++)
				valid_eager[cache_index][j][k] = valid_eager[cache_index][j-1][k];
			dirty[cache_index][j] = dirty[cache_index][j-1];
		}	

		tag_store[cache_index][0] = tag;
		for(uint64_t k = 0; k < offset; k++)
			valid_eager[cache_index][0][k] = false;
		for(uint64_t k = offset; k < offsets_per_block; k++)
			valid_eager[cache_index][0][k] = true;	
	}
	//printf("miss\n");
}

void NMRU_miss(uint64_t cache_index, uint64_t tag, uint64_t offset)
{
	if ( F == BLOCKING ) {
	
		if ( tag_store[cache_index][blocks_per_set-1] == mru_tag[cache_index])
		{
			for(uint64_t j = blocks_per_set - 2; j > 0; j--)
			{
				tag_store[cache_index][j] = tag_store[cache_index][j-1];
				valid[cache_index][j] = valid[cache_index][j-1];
				dirty[cache_index][j] = dirty[cache_index][j-1];
			}
			tag_store[cache_index][0] = tag;
			valid[cache_index][0] = true;

		}
		else  /*kick out last in array */
		{
			for(uint64_t j = blocks_per_set - 1; j > 0; j--)
			{
				tag_store[cache_index][j] = tag_store[cache_index][j-1];
				valid[cache_index][j] = valid[cache_index][j-1];
				dirty[cache_index][j] = dirty[cache_index][j-1];
			}
			tag_store[cache_index][0] = tag;
			valid[cache_index][0] = true;
		}
	}
	if ( F == EAGER ) {
	
		if ( tag_store[cache_index][blocks_per_set-1] == mru_tag[cache_index])
		{
			//if(cache_index == 5)
			//	printf("
			for(uint64_t j = blocks_per_set - 2; j > 0; j--)
			{
				tag_store[cache_index][j] = tag_store[cache_index][j-1];
				for(uint64_t k = 0; k < offsets_per_block; k++)
					valid_eager[cache_index][j][k] = valid_eager[cache_index][j-1][k];
				dirty[cache_index][j] = dirty[cache_index][j-1];
			}
			tag_store[cache_index][0] = tag;
			for(uint64_t k = 0; k < offset; k++)
				valid_eager[cache_index][0][k] = false;
			for(uint64_t k = offset; k < offsets_per_block; k++)
				valid_eager[cache_index][0][k] = true;

		}
		else  /*kick out last in array */
		{
			for(uint64_t j = blocks_per_set - 1; j > 0; j--)
			{
				tag_store[cache_index][j] = tag_store[cache_index][j-1];
				for(uint64_t k = 0; k < offsets_per_block; k++)
					valid_eager[cache_index][j][k] = valid_eager[cache_index][j-1][k];
				dirty[cache_index][j] = dirty[cache_index][j-1];
			}
			tag_store[cache_index][0] = tag;
			for(uint64_t k = 0; k < offset; k++)
				valid_eager[cache_index][0][k] = false;
			for(uint64_t k = offset; k < offsets_per_block; k++)
				valid_eager[cache_index][0][k] = true;
		}
	}
}


/**
 * Subroutine that simulates the cache one trace event at a time.
 * XXX: You're responsible for completing this routine
 *
 * @rw The type of event. Either READ or WRITE
 * @address  The target memory address
 * @p_stats Pointer to the statistics structnt64_t ure
 */
void cache_access(char rw, uint64_t address, cache_stats_t* p_stats)
{
	uint64_t offset = get_offset( address, B );
	uint64_t cache_index = get_index( address, C, S, B);
	uint64_t tag = get_tag( address, C, S);
	
	
	//printf("address  is %c %llx %llx %llx %llx\n",rw,address, tag, cache_index, offset);
	//printf("tag  is %x\n",tag);
	//printf("index  is %x\n",cache_index);
	//printf("offset  is %x\n",offset);

	p_stats->accesses++;
	rw == READ ? p_stats->reads++: p_stats->writes++;
	bool hit = false;
	uint64_t hit_block = -1;

	//printf("bps is %llx",blocks_per_set);

	if(F == BLOCKING)
	{
		for(uint64_t j =0; j < blocks_per_set; j++)  /*check whether hit or miss*/
		{
			if((tag_store[cache_index][j] == tag) && valid[cache_index][j]== true)
			{
				hit = true;
				hit_block = j;
				break;
			}
		}
	}
	if(F == EAGER)
	{
		for(uint64_t j =0; j < blocks_per_set; j++)  /*check whether hit or miss*/
		{
			if(tag_store[cache_index][j] == tag )// && valid_eager[cache_index][j][offset]== true)
			{
				if ( valid_eager[cache_index][j][offset]== true)
				{
					hit = true;
					hit_block = j;
					break;
				}
				else  // tag present but invalid
				{
					count++;
					hit = true;
					hit_block = j;
					for(uint64_t k = 0; k < offset; k++)
						valid_eager[cache_index][j][k] = false;
					for(uint64_t k = offset; k < offsets_per_block; k++)
						valid_eager[cache_index][j][k] = true;
					
					rw == READ ? p_stats->read_hits--: p_stats->write_hits--;
					rw == READ ? p_stats->read_misses++ : p_stats->write_misses++;
					//printf(" MISS\n");
					print = true;
					break;
				}
			}
		}
	}
	
	if(hit)
	{
		//if(!print)
		//	printf(" HIT\n");
		print = false;
		if( R == LRU)
		{
			//handle cache hit lru
			LRU_hit(cache_index, hit_block);
		}
		else
		{
			//handle cache hit nmru fifo
			mru_tag[cache_index] = tag;
		}
		rw == READ ? p_stats->read_hits++: p_stats->write_hits++;
	}
	else  /*miss*/
	{
		//printf(" MISS\n");
		if( R == LRU)
		{
			//handle cache miss lru
			LRU_miss(cache_index,tag, offset);
		}
		else
		{
			//handle cache miss nmru fifo
			NMRU_miss(cache_index,tag,offset);
			mru_tag[cache_index] = tag;
		}
		rw == READ ? p_stats->read_misses++ : p_stats->write_misses++;
	}

}

/**
 * Subroutine for cleaning up any outstanding memory operations and calculating overall statistics
 * such as miss rate or average access time.
 * XXX: You're responsible for completing this routine
 *
 * @p_stats Pointer to the statistics structure
 */
void complete_cache(cache_stats_t *p_stats) {
	p_stats->storage_overhead = 0;
	p_stats->misses = p_stats->read_misses + p_stats->write_misses;
	p_stats->hit_time = 0.2 * blocks_per_set + 1;
	if ( F == BLOCKING)
	{
		p_stats->miss_penalty = p_stats->hit_time + 50 + (0.25 * offsets_per_block) ;
		p_stats->storage_overhead += num_indices * blocks_per_set;
	}
	else
	{
		p_stats->miss_penalty = p_stats->hit_time + 51 ;
		p_stats->storage_overhead += num_indices * blocks_per_set * offsets_per_block;
	}
	if ( R == LRU )
	{
		p_stats->storage_overhead += num_indices * blocks_per_set * 8;
	}
	else
	{
		p_stats->storage_overhead += num_indices * blocks_per_set * 4;
	}
	p_stats->storage_overhead += num_indices * blocks_per_set; // dirty bit
	p_stats->miss_rate = (float)p_stats->misses/(float)p_stats->accesses;
	p_stats->avg_access_time = p_stats->hit_time + (p_stats->miss_rate *p_stats->miss_penalty);
	p_stats->storage_overhead_ratio = (p_stats->storage_overhead / 8) / pow(2,C);

	



}
