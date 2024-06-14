#include "Private.h"
#include "xxhash.h"

std::any HashStreamStart()
{
	XXH64_state_t* state = XXH64_createState();
	XXH64_reset(state, 0);
	return state;
	
}

void HashStreamUpdate(std::any handle, const void* data, u64 size)
{
	XXH64_state_t* state = std::any_cast<XXH64_state_t*>(handle);
	XXH64_update(state, data, size);
}

u64 HashStreamEnd(std::any handle)
{
	XXH64_state_t* state = std::any_cast<XXH64_state_t*>(handle);
	XXH64_hash_t const hash = XXH64_digest(state);
	XXH64_freeState(state);
	return hash;
}