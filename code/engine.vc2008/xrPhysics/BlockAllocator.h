#ifndef BLOCK_ALLOCATOR_H
#define BLOCK_ALLOCATOR_H
template<class T, u32 block_size>
class CBlockAllocator
{
	u32 block_count;
	u32	block_position;
	T*	current_block;
	xr_vector<T*> blocks;
public:
	inline T* add()
	{
		if (block_position == block_size)
			next_block();

		++block_position;
		return &current_block[block_position - 1];
	}

	inline void empty()
	{
		block_count = 0;
		if (blocks.size())
		{
			block_position = 0;
			current_block = blocks[0];
		}
		else
		{
			block_position = block_size;
		}
	}
	CBlockAllocator()
	{
		init();
	}
	~CBlockAllocator()
	{
		clear();
	}
	inline	void init()
	{
		block_position = block_size;
		block_count = 0;
		current_block = NULL;
	}
	inline	void clear()
	{
		xr_vector<T*>::iterator i = blocks.begin(), e = blocks.end();
		for (; i != e; ++i) xr_free(*i);
		blocks.clear();
		init();
	}
private:
	/////////////////////////////////////////////////////////////////
	inline	void add_block() { blocks.push_back(xr_alloc<T>(block_size)); };

	inline void next_block()
	{
		if (block_count == blocks.size())
			add_block();

		current_block = blocks[block_count];
		++block_count;
		block_position = 0;
	}
	////////////////////////////////////////////////////////////////
public:
	template <typename _Predicate>
	inline	void for_each(const _Predicate &pred)
	{
		if (!current_block)
			return;

		xr_vector<T*>::iterator	i = blocks.begin();
		xr_vector<T*>::iterator	e = blocks.begin() + block_count;
		u32 j;
		for (; i != e; ++i)
		{
			for (j = 0; j < block_size; ++j)
				pred.operator()((*i) + j);
		}

		for (j = 0; j < block_position; ++j)
		{
			pred.operator()(current_block + j);
		}
		//for_each(blocks.begin(),block.end(),pred);
	}
private:
	inline T* pointer(u32 position)
	{
		return blocks[position / block_size] + position % block_size;
	}

	inline T& back()
	{
		return current_block[block_position - 1];
	}

	inline T& back_pointer()
	{
		return current_block + block_position - 1;
	}

	inline T& operator[](u32 position)
	{
		return *pointer(position);
	}

	inline void construct(u32 position)
	{
		xr_allocator_t <T>().construct(pointer(position));
	}

	inline void construct_back()
	{
		xr_allocator_t <T>().construct(back_pointer());
	}
};

#endif