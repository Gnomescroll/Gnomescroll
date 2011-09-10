./sheet_maker.py blocks ./block/ block_sprite_conf $@
cd block_gen
python gen_block_dat.py
cd .. 
