import libtcodpy as libtcod

##A simple button GUI element framework. 
##Simple usage example:
##	button1 = Button(10, 1, "ab", 'A', "Tooltip text")
##	if button1.redraw:
##		button_console = button1.draw()

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
		libtcod.console_print_left(self.button_con, 0, 0, libtcod.BKGND_SET, self.hotkey)
		libtcod.console_print_left(self.button_con, 1, 0, libtcod.BKGND_SET, ": ")
		libtcod.console_print_left(self.button_con, 3, 0, libtcod.BKGND_SET, self.text)

		self.redraw = False
		return self.button_con
