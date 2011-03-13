import libtcodpy as libtcod
import textwrap

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
		self.msg = self.hotkey + ": " + self.text
		self.center_x = (int)(width/2)
		self.center_y = (int)(height/2)
			

	def draw(self):
		#If the user is hovering over the button, swap foreground and background colors
		if self.mouse_is_hovering:
			libtcod.console_set_default_background(self.button_con, self.fore_color)
			libtcod.console_set_default_foreground(self.button_con, self.back_color)
			libtcod.console_rect(self.button_con, 0, 0, self.width, self.height, True, libtcod.BKGND_SET)
		else:
			libtcod.console_set_default_background(self.button_con, self.back_color)
			libtcod.console_set_default_foreground(self.button_con, self.fore_color)
			libtcod.console_rect(self.button_con, 0, 0, self.width, self.height, True, libtcod.BKGND_SET)

		#Print the button to the button_con, and return that
		libtcod.console_set_alignment(self.button_con, libtcod.LEFT)
		libtcod.console_set_background_flag(self.button_con, libtcod.BKGND_SET)
		libtcod.console_print(self.button_con, 0, self.center_y, self.msg)

		return self.button_con

##Usage:
#test = Menu("Test Menu", has_cancel_button = True)
#test.add_button(Button(11, 1, "Button 1", 'b', "Test button 1 tooltip"))
#test.add_button(Button(5, 1, "a", 'a', "Second button tooptip"))
#test.add_text("Test message")
#test.add_button(Button(10, 1, "Button 2", 'b', "Test 3"))
#test.add_text("Test 2")
#test.initialize()
#test.set_position("center", 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)
#####
#then, you can call this in the main game loop every update:
#test.update()
#in the main key handler, check if there is a menu open, and send the keypress to that menu's key handler:
#current_menu.handle_key(char)
#Then to draw:
#if test.redraw:
#       menu_con = test.draw()
#       libtcod.console_blit(menu_con, 0, 0, test.width, test.height, 0, test.x, test.y)

class Menu:
	#If you set x and y here, you still need to do it later with set_position(), just use alignment = "exact"
	def __init__(self, title, fore_color=libtcod.white, back_color=libtcod.black, has_cancel_button = True, x=0, y=0):
		self.title = title
		self.fore_color = fore_color
		self.back_color = back_color
		self.x = x
		self.y = y
		self.lines = []
		self.hotkeys = {}
		self.width = 0
		self.height = 0
		self.has_cancel_button = has_cancel_button
		self.redraw = True
		self.close = False
		self.latest_hover_index = None
		self.button_pressed = None

	#This should be called after initialize()
	#current alignment options: center, exact. Still to come: north, south, east, west, northwest, southwest, northeast, southeast
	#padding: the minimum number of tiles that the menu must be from an edge. It is ignored for "center" alignment
	#window_start_x, window_start_y, window_width, and window_height describe the window the menu is going to be placed in
	def set_position(self, align, padding, window_start_x, window_start_y, window_width, window_height):
		self.align = align
		if align == "center":
			self.x = (int)(window_width/2)-(int)(self.width/2) + window_start_x
			self.y = (int)(window_height/2)-(int)(self.height/2) + window_start_y

	def add_button(self, button):
		self.lines.append(["button", button])
		if button.width > self.width:
			self.width = button.width
		self.hotkeys[button.hotkey] = self.height		
		self.height += 1
		

	def add_text(self, text, align = "left"):
		self.lines.append([align, text])
		if len(text) > self.width:
			self.width = len(text)
		self.height += 1
		
	def initialize(self):
		if self.has_cancel_button:
			#ensure that the menu is wide enough for the cancel button and the title
			if self.width < 11:
				self.width = 1
			if self.width < len(self.title):
				self.width = len(self.title)
			self.separator = '_' * self.width
			self.height += 4 #Height increased by four, since we are adding two separators, a title, and a cancel button
			self.lines.append(["left", self.separator])
			self.lines.append(["button", Button(11, 1, "Cancel", "x", "Close this menu")])

		else:
			#ensure that the menu is wide enough for the title
			if self.width < len(self.title):
				self.width = len(self.title)
			self.separator = '_' * self.width
			self.height += 2 #Height increased by two, since we are adding a separator and a title

		self.lines.insert(0, ["left", self.separator])
		self.lines.insert(0, ["center", self.title])
		self.menu_con = libtcod.console_new(self.width, self.height)
		self.center_x = (int)(self.width/2)
		libtcod.console_set_default_background(self.menu_con, self.back_color)
		libtcod.console_set_default_foreground(self.menu_con, self.fore_color)
		libtcod.console_rect(self.menu_con, 0, 0, self.width, self.height, True, libtcod.BKGND_SET)

	def draw(self):
		current_line = 0
		if self.redraw:
			for line in self.lines:
				libtcod.console_set_background_flag(self.menu_con, libtcod.BKGND_SET)
				if line[0] == "center":
					libtcod.console_set_alignment(self.menu_con, libtcod.CENTER)
					libtcod.console_print(self.menu_con, self.center_x, current_line, line[1])
					current_line += 1
				elif line[0] == "left":
					libtcod.console_set_alignment(self.menu_con, libtcod.LEFT)
					libtcod.console_print(self.menu_con, 0, current_line, line[1])
					current_line += 1
				else:
					button_con = line[1].draw()
					libtcod.console_blit(button_con, 0, 0, line[1].width, line[1].height, self.menu_con, 0, current_line)
					current_line += 1
		self.redraw = False
		return self.menu_con #returns regardless of redraw status, but still try not to call draw() if redraw is False. More efficient

	#because checking keys in different functions simultaneously doesn't work, this only checks the mouse
	#in the main key handler, check if there is a menu open, and send the keypress to that menu's key handler
	def update(self, mouse):
		#clear out the last button
		if self.latest_hover_index:
			self.lines[self.latest_hover_index][1].mouse_is_hovering = False
			self.lines[self.latest_hover_index][1].redraw = True
			self.latest_hover_index = None
			self.redraw = True
		if mouse.cx >= self.x and mouse.cx < (self.x + self.width) and mouse.cy >= self.y and mouse.cy < (self.y + self.height):
			index = mouse.cy - self.y
			if self.lines[index][0] == "button":
				self.lines[index][1].mouse_is_hovering = True
				self.lines[index][1].redraw = True
				self.latest_hover_index = index
				self.redraw = True
				if mouse.lbutton_pressed:
					self.button_pressed = index
				
		if mouse.rbutton_pressed:
			self.close = True

	def handle_key(self, char):
		if char in self.hotkeys:
			self.button_pressed = (self.hotkeys[char])+1 #they pressed a hotkey, so mark the corresponding button as pressed 


class Message_Log:
	def __init__(self, width, height, back_color = libtcod.darker_blue):
		self.game_msgs = []
		self.redraw = True
		self.width = width
		self.height = height
		self.message_con = libtcod.console_new(width, height)
		self.back_color = back_color

	def add(self, new_msg, color = libtcod.white):
		self.redraw = True
		#split the message if necessary, among multiple lines
		new_msg_lines = textwrap.wrap(new_msg, self.width)

		for line in new_msg_lines:
			#if the buffer is full, remove the first line to make room for the new one
			if len(self.game_msgs) == self.height:
			    del self.game_msgs[0]

			#add the new line as a tuple, with the text and the color
			self.game_msgs.append((line, color))

	def draw(self):
		libtcod.console_set_default_background(self.message_con, self.back_color)
		libtcod.console_clear(self.message_con)
		y = 1
		for (line, color) in self.game_msgs:
			libtcod.console_set_default_foreground(self.message_con, color)
			libtcod.console_print(self.message_con, 1, y, line)
			y += 1
		redraw = False;
		return self.message_con



