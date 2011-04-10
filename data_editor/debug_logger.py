import pprint

def pp_print_data(message, data): #pretty prints data to console
	print "---"
	print message
	pp = pprint.PrettyPrinter(indent=4)
	pp.pprint(output_dict)	
