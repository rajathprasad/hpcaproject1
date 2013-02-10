#include "cachesim.hpp"
#include "helper_functions.cpp"


/* global declaration */
uint64_t C, B, S, num_indices, blocks_per_set;
char F, R;
uint64_t **tag_store;
bool **valid;
bool **dirty;

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
	/*allocate space for cache stuff*/

	
	tag_store = new uint64_t*[num_indices];
	for(uint64_t i =0; i < num_indices; i++)
	{
		tag_store[i] = new uint64_t[blocks_per_set];
	}

	
	valid = new bool*[num_indices];
	for(uint64_t i =0; i < num_indices; i++)
	{
		valid[i] = new bool[blocks_per_set];
	}

	
	dirty = new bool*[num_indices];
	for(uint64_t i =0; i < num_indices; i++)
	{
		dirty[i] = new bool[blocks_per_set];
	}

	for(uint64_t i =0; i < num_indices; i++)
		for(uint64_t j =0; j < blocks_per_set; j++)
		{
			valid[i][j] = false;
			dirty[i][j] = false;
		}
	

}
/**
 * Subroutine that simulates the cache one trace event at a time.
 * XXX: You're responsible for completing this routine
 *
 * @rw The type of event. Either READ or WRITE
 * @address  The target memory address
 * @p_stats Pointer to the statistics structure
 */
void cache_access(char rw, uint64_t address, cache_stats_t* p_stats)
{
	uint64_t offset = get_offset( address, B );
	uint64_t cache_index = get_index( address, C, S, B);
	uint64_t tag = get_tag( address, C, S, B);
	p_stats->accesses++;
	rw == READ ? p_stats->reads++: p_stats->writes++;
	bool hit = false;

	for(uint64_t j =0; j < blocks_per_set; j++)  /*check whether hit or miss*/
	{
		if((tag_store[cache_index][j] == tag) && valid[cache_index][j]== true)
		{
			hit = true;
		}
	}
	
	if(hit)
	{
		if( R == LRU)
		{
			//handle cache hit lru
		}
		else
		{
			//handle cache hit nmru fifo
		}
	}
	else
	{
		if( R == LRU)
		{
			//handle cache miss lru
		}
		else
		{
			//handle cache miss nmru fifo
		}
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
}
