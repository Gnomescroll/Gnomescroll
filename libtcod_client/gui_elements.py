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
		if self.redraw:
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
	def __init__(self, title, fore_color, back_color, has_cancel_button = True, align="center", x=0, y=0):
		self.title
		self.fore_color = fore_color
		self.back_color = back_color
		self.align = align
		self.x = x
		self.y = y
		self.lines = []
		self.hotkeys = []
		self.width = 0
		self.height = 0
		self.has_cancel_button = has_cancel_button
		self.redraw = True
		
	def add_button(button):
		lines.append(["button", button])
		if button.width > self.width:
			self.width = button.width
		self.height += 1

	def add_text(text):
		lines.append(["text", text])
		if len(text) > self.width:
			self.width = len(text)
		self.height += 1
		
	def initialize():
		if self.has_cancel_button:
			#ensure that the menu is wide enough for the cancel button and the title
			if self.width < 11:
				self.width = 1
			if self.width < len(self.title):
				self.width = len(self.title)
			self.separator = '_' * self.width
			self.height += 4 #Height increased by four, since we are adding two separators, a title, and a cancel button
			self.lines.append(["text", self.separator])
			self.lines.append(["button", Button(11, 1, "Cancel", "x", "Close this menu")])

		else:
			#ensure that the menu is wide enough for the cancel button and the title
			if self.width < len(self.title):
				self.width = len(self.title)
			self.separator = '_' * self.width
			self.height += 2 #Height increased by two, since we are adding a separator and a title

		self.lines.insert(0, ["text", self.title])
		self.lines.insert(0, ["text", self.separator])
		self.menu_con = libtcod.console_new(self.width, self.height)
		self.center_x = (int)(width/2)

	def draw():
		current_line = 0
		for line in lines:
			if line[0] == "text":
				libtcod.console_print_center(self.menu_con, self.center_x, current_line, libtcod.BKGND_SET, line[1])
				current_line += 1

			else:
				if line[1].redraw:
					button_con = line[1].draw()
					libtcod.console_blit(menu_con, 0, 0, line[1].width, line[1].height, 0, MAP_VIEWER_WIDTH, 0)
					
		redraw = False		
		


