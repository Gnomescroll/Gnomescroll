class Globals:

	def __init__(self, world_id):
		#server globals
		self.world_id = world_id
		self.unique_id = itertools.count(1) #will be replaced by a unique-id pool


	#!!! FIX !!!
	#in future, will have pool/list of global unique_ids	
	def get_unique_id()
		next(self.unique_id)
