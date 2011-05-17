import pyglet

win = pyglet.window.Window(fullscreen= False)
text = pyglet.font.Text(pyglet.font.load ('Arial', 16), 'Hello, World!')

text.x = 60 #x offset
text.y = 60 #y offset

'''
#this sets string
text.str = "test"
'''
while not win.has_exit:

    win.dispatch_events() #gets keyboard events
    win.clear()           #clears screen
    text.draw()           #draw text
    win.flip()            #blip
