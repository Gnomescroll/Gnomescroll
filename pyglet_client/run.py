#! python -O

from pyglet import app, clock
from pyglet.window import key, Window
from rts_camera import RTS_Camera
from mouse import Mouse
from keyboard import Keyboard
from renderer import Renderer

#@window.event
#def on_mouse_drag(x, y, dx, dy, buttons, modifiers):
#	global camera_x_angle, camera_y_angle
#	print 'dy, dy = ' +  str(dx) + ' ' + str(dy)
#	camera_x_angle = camera_x_angle + dx/100
#	camera_z_angle = camera_y_angle + dx/100
	
def main():
    win = Window(fullscreen=True, visible=False)
    camera = RTS_Camera(win.width, win.height, (0, 0, 0))
    renderer = Renderer()
    keyboard = Keyboard()
    mouse = Mouse(camera)
    keyboard.key_handlers[key.ESCAPE] = win.close
    keyboard.key_handlers.update(camera.key_handlers) #use to add handlers
    #clock.schedule(maze.update)
    win.on_draw = lambda: renderer.on_draw(camera, win.width, win.height)
    win.on_mouse_drag = mouse.on_mouse_drag
    win.on_key_press = keyboard.on_key_press
    keyboard.print_handlers()
    win.set_visible()
    
    

    app.run()

if __name__ == "__main__":
    main()
