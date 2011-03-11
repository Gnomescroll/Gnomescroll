from server_API import *
admin = Admin_commands()
agent = Agent_commands()
info = Info_commands()

z = 0
width = 16
height = 16

for x in range(0, width):
	for y in range(0, height):
		admin.set_map(x, y, 5, z)
		z = z+1
