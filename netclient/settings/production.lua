-- server --
options.server = "174.37.26.119"
options.port = 0

-- display options -- 
options.fullscreen = false
options.width = 1280
options.height = 720
options.fov = 85

-- mouse --
options.sensitivity = 1000  -- logarithmic scale
options.camera_speed = 0.6
options.invert_mouse = false
options.mouse_bound = true

-- hud setting --
options.system_message_r = 255
options.system_message_g = 255
options.system_message_b = 50

-- sound settings --
options.sound = true
options.sfx = 50
options.music = 50
options.sound_device = ""   -- leave empty to use default

-- graphics --
options.view_distance = 128;    -- capped at 128
options.animation_level = 3;    -- valid values are 0,1,2,3; 0 is none, 1 is low, 2 is medium, 3 is high
options.placement_outline = false;  -- outline for placing blocks etc
options.mesa_shader = false;    -- use mesa shader if main shader does not work and mesa drivers are present (LINUX ONLY). required for fog

-- help --
options.show_tips = true;
