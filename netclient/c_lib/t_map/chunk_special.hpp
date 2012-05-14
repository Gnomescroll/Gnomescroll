#pragma once


namespace t_map
{


struct inventory_block
{
	int x,y,z;
	int container_type;
	int container_id;
};

class CHUNK_ITEM_CONTAINER
{
	public:

	int iban; //inventory_block_array_num
	int ibam; //inventory_block_array_max
	struct inventory_block* iba; //inventory_block_array

	CHUNK_ITEM_CONTAINER()
	{
		iban = 0;
		ibam = 32;
		iba = (struct inventory_block*) malloc(ibam*sizeof(struct inventory_block));
	}

	//deletes all
	void _reset()
	{
		iban = 0;
	}

	void _remove(int index)
	{
		GS_ASSERT(index < ibam);
		iban--;
		iba[index] = iba[iban];
	}

	void remove (int x, int y, int z)
	{
		for(int i=0; i<iban; i++)
		{
			if(x == iba[i].x && y == iba[i].y && z == iba[i].z)
			{
				_remove(i);
				return;
			}
		}
		GS_ABORT();
	}

	void add(int x, int y, int z, int container_type, int container_id)
	{
		if(iban == ibam)
		{
			ibam *= 2;
			iba = (struct inventory_block*) realloc(iba, ibam*sizeof(struct inventory_block));
			if(iba == NULL) GS_ABORT();
		}

		iba[iban].x = x;
		iba[iban].y = y;
		iba[iban].z = z;
		iba[iban].container_type = container_type;
		iba[iban].container_id = container_id;
		iban++;
	}

	#ifdef DC_SERVER


	#endif
};


class container_block_chunk_reset_StoC
        pack_u32(&chunk_index;, buff, buff_n, pack);

class container_block_create_StoC: public MapMessagePacketToClient<container_block_create_StoC>
    uint16_t x,y,z;
    uint8_t container_type;
    uint16_t container_id;

e void handle() __attribute((always_inline));
};

class container_block_delete_StoC: public MapMessagePacketToClient<container_block_delete_StoC>

}