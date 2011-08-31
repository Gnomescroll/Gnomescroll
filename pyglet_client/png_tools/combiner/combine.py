
from combine_lib import blend

png1 = "stone.png"
png2 = "ore.png"

out_png = "test5.png"

#function combines the rgba values
combine_type = 2

ore = "ore.png"
blend("stone.png", ore, "stage1.png", 1)
blend("stage1.png", ore, "stage2.png", 1)
blend("stage2.png", ore, "stage3.png", 1)




blend("stone.png", ore, "test1.png", 1)

blend("stone.png", ore, "test2.png", 2)
blend("test2.png", ore, "test3.png", 2)
blend("test3.png", ore, "test4.png", 2)
blend("test4.png", ore, "test5.png", 2)
blend("test5.png", ore, "test6.png", 2)
