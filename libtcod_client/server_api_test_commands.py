from server_API import *
admin = Admin_commands()
agent = Agent_commands()
info = Info_commands()

admin.create_agent(1,1,1)

#agent.move_0(1, 1,1)

info.get_agent_list()

admin.set_map(2,2,0,1)


admin.create_object(1,1,1,'crop')
admin.create_object(1,3,1,'crop', 'generic_crop')
admin.create_object(1,1,1,'crop', 'generic_dead_crop')
admin.create_object(1,2,1,'crop', 'generic_mature_crop')

admin.create_agent(1,1,1)
admin.create_object(1,1,1,'crop')
agent.harvest_crop(1,2)

admin.create_agent(1,1,1)
admin.create_object(0,0,1, 'workshop', 'oven') 
admin.create_object(1,1,1,'item', 'generic_food')
agent.craft_item(1, 'bake bread')

admin.create_agent(1,1,1)
admin.set_map(2,1,1,5)
agent.dig(1,2,1,1)

admin.create_agent(1,1,1)
admin.create_agent(1,2,1)
agent.attack(1,2)

