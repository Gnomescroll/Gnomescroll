class Object_handler:

    def __init__(self):
        self.objects = []
        self.world_id = None
        self.client_id = None

    def object_info(self, id, client_id, value, **msg):
        print 'object_info called.'
    
    #state change
    def object_position_change(self, id, position, **msg):
        for element in self.objects:
            if element['id'] == id:
                element['position'] = position
    
    def object_state_change(self, id, position, version ,**msg):
        for element in self.objects:
            if element['id'] == id:
                element['version'] = version

    #list
    def object_list(self, list, world_id, client_id, **msg):
        self.objects = list
        self.client_id = client_id
        self.world_id  = world_id
        
    #creation and deletion
    def object_create(self, id, position, **msg):
        self.objects.append({"position": position, "version": 0,
                             "type": msg['type'], "id": id})

    def object_delete(self, id, **msg):
        for element in self.objects:
            if element['id'] == id:
                del element