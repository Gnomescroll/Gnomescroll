import libtcodpy as libtcod

##A simple button GUI element framework. 
##Simple usage example:
##	button1 = Button(10, 1, "ab", 'A', "Tooltip text")
##	if button1.redraw:
##		button_console = button1.draw()
##	libtcod.console_blit(button_console, 0, 0, 10, 1, 0, 0, 0, 1)

class Button:
	def __init__(self, width, height, text, hotkey, tooltip, fore_color = libtcod.white, back_color=libtcod.blue):
		self.width = width		
		self.height = height
		self.text = text
		self.hotkey = hotkey
		self.tooltip = tooltip
		self.fore_color = fore_color
		self.back_color = back_color
		self.mouse_is_hovering = False
		self.button_con = libtcod.console_new(width, height)
		self.redraw = True #True initially for first draw. After that, it should only be set to True if it is being hovered/clicked
		self.msg = self.hotkey + ": " + self.text
		self.center_x = (int)(width/2)
		self.center_y = (int)(height/2)
			

	def draw(self):
		#If the user is hovering over the button, swap foreground and background colors
		if self.mouse_is_hovering:
			libtcod.console_set_background_color(self.button_con, self.fore_color)
			libtcod.console_set_foreground_color(self.button_con, self.back_color)
			libtcod.console_rect(self.button_con, 0, 0, self.width, self.height, True, libtcod.BKGND_SET)
		else:
			libtcod.console_set_background_color(self.button_con, self.back_color)
			libtcod.console_set_foreground_color(self.button_con, self.fore_color)
			libtcod.console_rect(self.button_con, 0, 0, self.width, self.height, True, libtcod.BKGND_SET)

		#Print the button to the button_con, and return that
		print self.center_x, self.center_y
		libtcod.console_print_center(self.button_con, self.center_x, self.center_y, libtcod.BKGND_SET, self.msg)

		self.redraw = False
		return self.button_con

class Menu:
	def __init__(self, title, fore_color, back_color, align="center", x=0, y=0):
		self.title
		self.fore_color = fore_color
		self.back_color = back_color
		self.align = align
		self.x = x
		self.y = y
		self.lines = []
		self.keys = []
		
	def add_button():
		pass
		

	def initialize():
		#todo add a cancel button at the end of all menus
		self.menu_con = libtcod.console_new(self.width, self.height)
		self.separator = '_' * self.width
		
		


