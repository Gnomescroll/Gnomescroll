def gif_maker():
    z_step = 0.0013
    freq = 9
    seed = 9090
    distance_enabled = True
    w = 256
    h = 256
    gradient = 6
    turb_enable = False
    turb_f = 1
    turb_pow = 1
    init_c_lib.VN.configure(z_step, freq, seed, distance_enabled, turb_enable, turb_f, turb_pow)
    #init_c_lib.VN.configure(z_step, freq, seed, distance_enabled, turb_f, turb_pow)
    init_c_lib.VN.init(width=w, height=h, gradient=gradient)
    print 'preparing to generate frames'
    secs = 1./30.
    #secs = 1.
    init_c_lib.VN.frames(secs)
    print 'frames generated'
    sys.exit()
