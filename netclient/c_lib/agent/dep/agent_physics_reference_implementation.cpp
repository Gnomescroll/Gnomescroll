
/*

REFERENCE

*/

//tick stuff

/*
    int index = (cs_seq+1) % 128;

    int _tc =0;
    while(cs[(cs_seq+1) % 128].seq == (cs_seq+1)% 256) {
        _tc++;
        cs_seq = (cs_seq+1)%256;

        s.theta = cs[index].theta;
        s.phi = cs[index].phi;


        int a_cs = cs[index].cs;

        //set control state variables
        //printf("cs= %i \n", a_cs);
        bool forward     = a_cs & 1? 1 :0;
        bool backwards   = a_cs & 2? 1 :0;
        bool left        = a_cs & 4? 1 :0;
        bool right       = a_cs & 8? 1 :0;
        bool jetpack     = a_cs & 16? 1 :0;


        const float tr = 10.0;    //tick rate
        const float tr2 = tr*tr;

        const float xy_speed = 2.00 / tr;
        const float z_jetpack = 0.80 / tr2;
        const float z_gravity = -.40 / tr2;

        const float ground_distance = 0.02;
        const float z_bounce = 0.65;
        const float z_bounce_v_threshold = 0.35 / tr;

        const float pi = 3.14159265;

        //box properties
        float b_height = 2.5;  //agent collision box height
        float box_r = 0.4;

//### Collisions on X axis


    //  integer cordinates used for checking voxels for collision

        int bx_pos_current = s.x+box_r;          //floor
        int bx_neg_current = s.x-box_r;

        int by_pos_current = s.y+box_r;
        int by_neg_current = s.y-box_r;

        int bz_pos_current = s.z+b_height;
        int bz_neg_current = s.z;           //bottom of agent is just s.z

        //loop setup


    // Max and min integer boundraries the cubes agent collision boxes occupies

        int bx_min = s.x - box_r;
        int bx_max = s.x + box_r + 1.0;

        int by_min = s.y - box_r;
        int by_max = s.y + box_r + 1.0;

        int bz_min = s.z;
        int bz_max = s.z + b_height + 1.0;

        //state variables


        //projected positions in absence of collision detection

        // projected z position depends on whether agent is on ground 

        float cs_vx =0 ;
        float cs_vy =0 ;

        if(forward)
        {
                cs_vx += xy_speed*cos( s.theta * pi);
                cs_vy += xy_speed*sin( s.theta * pi);
        }
        if(backwards)
        {
                cs_vx += -xy_speed*cos( s.theta * pi);
                cs_vy += -xy_speed*sin( s.theta * pi);
        }
        if(left) 
        {
                cs_vx += xy_speed*cos( s.theta * pi + pi/2);
                cs_vy += xy_speed*sin( s.theta * pi + pi/2);
        }
        if(right) 
        {
                cs_vx += -xy_speed*cos( s.theta * pi + pi/2);
                cs_vy += -xy_speed*sin( s.theta * pi + pi/2);
        }

        int bx_pos_projected = s.x+s.vx+cs_vx+box_r;    //floor
        int bx_neg_projected = s.x+s.vx+cs_vy-box_r;

        int by_pos_projected = s.y+s.vy+cs_vx+box_r;
        int by_neg_projected = s.y+s.vy+cs_vy-box_r; 
                  
        int bz_pos_projected = s.z+s.vz+cs_vx+b_height;
        int bz_neg_projected = s.z+s.vz;

    //  Float to int is floor
    // may be slow or could be optimized
    
    //cast to ints are implicit floors

    //floor, float to int
        


        int bx,by,bz;

        int xc_pos_current = 0;
        int xc_pos_projected = 0;

        int xc_neg_current = 0;
        int xc_neg_projected = 0;

        //x collisions
        for(bz = bz_min; bz<bz_max; bz++) { //less than equal
        for(by = by_min; by<by_max; by++) {
            //#x+
                if(_collision_check(bx_pos_current,by,bz))  xc_pos_current +=1;
                if(_collision_check(bx_pos_projected,by,bz)) xc_pos_projected +=1;
            //#x-
                if(_collision_check(bx_neg_current,by,bz))  xc_neg_current +=1;
                if(_collision_check(bx_neg_projected,by,bz)) xc_neg_projected +=1;
        }}

        //y collisions
        int yc_pos_current = 0;
        int yc_pos_projected = 0;

        int yc_neg_current = 0;
        int yc_neg_projected = 0;

        for(bz = bz_min; bz < bz_max; bz++){
        for(bx = bx_min; bx < bx_max; bx++){
            //#y+
                if(_collision_check(bx,by_pos_current,bz))   yc_pos_current +=1;
                if(_collision_check(bx,by_pos_projected,bz)) yc_pos_projected +=1;
            //#y-
                if(_collision_check(bx,by_neg_current,bz))   yc_neg_current +=1;
                if(_collision_check(bx,by_neg_projected,bz)) yc_neg_projected +=1;
        }}

        //z collisions

        int zc_pos_current = 0;
        int zc_pos_projected = 0;

        int zc_neg_current = 0;
        int zc_neg_projected = 0;
                
        int zc_ground_position = s.z - ground_distance; //checks if player is in contact with ground

        int on_ground = 0;

        for(bx = bx_min; bx < bx_max; bx++){
        for(by = by_min; by < by_max; by++){
            //ground contact
                if(_collision_check(bx,by, zc_ground_position)) on_ground += 1;
            //#z+
                if(_collision_check(bx,by,bz_pos_current))   zc_pos_current +=1;
                if(_collision_check(bx,by,bz_pos_projected)) zc_pos_projected +=1;
            //#z-
                if(_collision_check(bx,by,bz_neg_current))   zc_neg_current +=1;
                if(_collision_check(bx,by,bz_neg_projected)) zc_neg_projected +=1;
        }}

        //compute state variables

        //collision handling
        int total_current_collisions = xc_pos_current + xc_neg_current + yc_pos_current + yc_neg_current + zc_pos_current + zc_neg_current;
        if(total_current_collisions > 0) {
            printf("%i current collisions this frame\n", total_current_collisions);
            //push agent out of block
        }

        //handle x collisions
        if(xc_pos_current == 0 && xc_neg_current == 0)
        {
            if(xc_pos_projected != 0) s.vx = 0.0;
            if(xc_neg_projected != 0) s.vx = 0.0;
        }

        //handle y collisions

        if(yc_pos_current ==0 && yc_neg_current ==0)
        {
            if(yc_pos_projected != 0) s.vy = 0;
            if(yc_neg_projected != 0) s.vy = 0;
        }

        //handle z collision

        //#Hard collision predicted and not inside of something already
        if(zc_neg_projected != 0 && zc_neg_current == 0)
        {
            if(s.vz < 0)
            {
                if(s.vz < -z_bounce_v_threshold)
                {
                    s.vz *= -1 *z_bounce;
                }
                else
                {
                    s.vz = 0;
                }
            }
        }

        if(zc_neg_current != 0) // #if agent is inside of block, float them out the top
        {
            s.z += 0.50 / tr;
        }

        if(zc_pos_current != 0) // #if agent is inside of block, float them out the top
        {
            s.z += -0.50 / tr;
        }

        //jetpack handling
        if(!on_ground) {
            if(s.z>0)
            {
                s.vz += z_gravity;
            } 
            else 
            {
                s.vz -= z_gravity;
            }    
        }
        if(jetpack) {
            s.vz += z_jetpack;
        }
        
        //newton intergrate positions
        s.x += s.vx + cs_vx;    //use better intergrator
        s.y += s.vy + cs_vy;
        s.z += s.vz;
    
    //    printf("cs_vx= %f, cs_vy= %f \n", cs_vx, cs_vy);
    //    printf("dx= %f, dy= %f, dz= %f \n", s.vx + cs_vx, s.vy + cs_vy, s.vz);
    //   printf("cs_seq= %i, x= %f, y= %f, z= %f, vx= %f, vy= %f, vz= %f \n",cs_seq, s.x, s.y, s.z, s.vx, s.vy, s.vz);
    
        //printf("vx= %f, vy= %f, vz= %f \n", s.vx, s.vy, s.vz);
    } //end physics loop
    //printf("_tick: processed %i agent ticks\n", _tc);

}
*/

/*
## determine if agent is on ground and if they are colliding with anything at current position
        zc_current = 0
        zc_ground = 0
        zc_neg_projected = 0
        bz_current = float(z - b_height)
        bz_ground = floor(z - b_height - z_margin)
        bz_neg_projected = floor(z+vz-b_height)

        for bx in range(int(floor(x-box_r)), int(floor(x+box_r)+1)):
            for by in range(int(floor(y-box_r)), int(floor(y+box_r)+1)):
                if self.collisionDetection(bx,by,bz_current):
                    zc_current +=1
                if self.collisionDetection(bx,by,bz_ground):
                     zc_ground += 1
                if self.collisionDetection(bx,by,bz_neg_projected):
                    zc_neg_projected +=1
*/
    /*
        bx_pos_current = floor(x+box_r)
        bx_pos_projected = floor(x+vx+box_r)

        bx_neg_current = floor(x-box_r)
        bx_neg_projected = floor(x+vx-box_r)

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for bx in range(int(floor(x-box_r)), int(floor(x+box_r)+1)):
            #x+
                if self.collisionDetection(bx,by_pos_current,bz):
                    yc_pos_current +=1
                if self.collisionDetection(bx,by_pos_projected,bz):
                    yc_pos_projected +=1
            #x-
                if self.collisionDetection(bx,by_neg_current,bz):
                    yc_neg_current +=1
                if self.collisionDetection(bx,by_neg_projected,bz):
                    yc_neg_projected +=1
    */


/*
        x,y,z, vx,vy,vz, ax,ay,az = self.state
        ax,ay,az = (0,0,0)
        vx,vy = (0,0)
        #constants
        tr = 10. #tick rate
        tr2 = tr**2 #tick rate squared
        xy_brake = math.pow(.50, 1/(float(tr))) #in percent per second
        xy_speed = 2. / tr
        if GameStateGlobal.apply_gravity:
            z_gravity = -.40 / tr2
        else:
            z_gravity = 0
        z_jetpack = 0.80 / tr2
        #gravity
    #TODO: should turn gravity off if agent is in contact with ground
        #velocity from acceleration and inputs

        vx += ax + self.v_x*xy_speed
        vy += ay + self.v_y*xy_speed

        #print str((vx,vy))
        if self.brake != 0:
            if GameStateGlobal.apply_gravity:
                vx *= xy_brake
                vy *= xy_brake
                vz *= xy_brake
            else:
                vz += -0.40 / tr2

    #constants for collision box
        b_height = self.b_height
        t_height = self.t_height
        box_r = self.box_r

### Collisions on X axis collision ###

        xc_pos_current = 0
        xc_pos_projected = 0

        xc_neg_current = 0
        xc_neg_projected = 0

        bx_pos_current = floor(x+box_r)
        bx_pos_projected = floor(x+vx+box_r)

        bx_neg_current = floor(x-box_r)
        bx_neg_projected = floor(x+vx-box_r)

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for by in range(int(floor(y-box_r)), int(floor(y+box_r)+1)):
            #x+
                if self.collisionDetection(bx_pos_current,by,bz):
                    xc_pos_current +=1
                if self.collisionDetection(bx_pos_projected,by,bz):
                    xc_pos_projected +=1
            #x-
                if self.collisionDetection(bx_neg_current,by,bz):
                    xc_neg_current +=1
                if self.collisionDetection(bx_neg_projected,by,bz):
                    xc_neg_projected +=1

### Collision on Y axis ###

        yc_pos_current = 0
        yc_pos_projected = 0

        yc_neg_current = 0
        yc_neg_projected = 0

        by_pos_current = floor(y+box_r)
        by_pos_projected = floor(y+vy+box_r)

        by_neg_current = floor(y-box_r)
        by_neg_projected = floor(y+vy-box_r)

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for bx in range(int(floor(x-box_r)), int(floor(x+box_r)+1)):
            #x+
                if self.collisionDetection(bx,by_pos_current,bz):
                    yc_pos_current +=1
                if self.collisionDetection(bx,by_pos_projected,bz):
                    yc_pos_projected +=1
            #x-
                if self.collisionDetection(bx,by_neg_current,bz):
                    yc_neg_current +=1
                if self.collisionDetection(bx,by_neg_projected,bz):
                    yc_neg_projected +=1

### XY Collision ###

        xyc_projected = 0

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for by in range(int(floor(y+vy-box_r)), int(floor(y+vy+box_r)+1)):
                for bx in range(int(floor(x+vx-box_r+vx)), int(floor(x+vx+box_r)+1)):
                    if self.collisionDetection(bx,by,bz):
                        xyc_projected += 1

        xyc_current = 0

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for by in range(int(floor(y-box_r)), int(floor(y+box_r)+1)):
                for bx in range(int(floor(x-box_r+vx)), int(floor(x+box_r)+1)):
                    if self.collisionDetection(bx,by,bz):
                        xyc_current += 1

        #dont do this right now

        if False and xyc_projected != 0:
            # print "Projected XY collision!"
            vx =0
            vy =0

### XY collision constants
        xy_bounce = .65
        xy_bounce_v_threshold = 0.35 / tr

    ## handle x collisions
        #xc_pos_current, xc_pos_projected , xc_neg_current, xc_neg_projected

        if xc_pos_current == 0 and xc_neg_current == 0:
            if xc_pos_projected != 0:
                vx = 0
            if xc_neg_projected != 0:
                vx = 0
        else:
            pass
            #print "X collision error!!"

    ## handle y collisions
        #yc_pos_current, yc_pos_projected , yc_neg_current, yc_neg_projected

        if yc_pos_current ==0 and yc_neg_current ==0:
            if yc_pos_projected != 0:
                vy = 0
            if yc_neg_projected != 0:
                vy = 0
        else:
            pass
            #print "Y collision error!!"


        #environmental and input controls
        on_ground = self.on_ground #environmental state
        jetpack = self.jetpack #control state

        if on_ground == 0: #agent on ground
            az += (z_gravity) if z>0 else (-z_gravity)
        #jetpack adjustment to gravity
        if jetpack != 0: az += z_jetpack
    ## calculate velocity from acceleration inputs
        vz += az

##parameters for collision
        z_margin = .01
        z_bounce = .65
        z_bounce_v_threshold = 0.35 / tr

## determine if agent is on ground and if they are colliding with anything at current position
        zc_current = 0
        zc_ground = 0
        zc_neg_projected = 0
        bz_current = float(z - b_height)
        bz_ground = floor(z - b_height - z_margin)
        bz_neg_projected = floor(z+vz-b_height)

        for bx in range(int(floor(x-box_r)), int(floor(x+box_r)+1)):
            for by in range(int(floor(y-box_r)), int(floor(y+box_r)+1)):
                if self.collisionDetection(bx,by,bz_current):
                    zc_current +=1
                if self.collisionDetection(bx,by,bz_ground):
                     zc_ground += 1
                if self.collisionDetection(bx,by,bz_neg_projected):
                    zc_neg_projected +=1
    ##  calculate environmental state
        #agent ground state
        if zc_ground != 0:
            if self.on_ground != 1:
                self.on_ground = 1
                #print "On ground!"
        else:
            if self.on_ground == 1:
                #print "Off ground!"
                self.on_ground = 0

    ## apply velocity
        #Hard collision predicted and not inside of something already
        if zc_neg_projected != 0 and zc_current == 0:
            if vz < 0:
                if vz < -z_bounce_v_threshold: #vertical velocity bounce treshold
                    vz *= -1 *z_bounce
                else:
                    vz = 0

        if zc_current != 0: #if agent is inside of block, float them out the top
            z += .50 / tr

        if zc_neg_projected != 0:
            #print "Predicted neg Z-Collision!"
            pass
        if zc_current != 0:
            pass
            #print "Hard current neg Z-Collision!"

## Position Change ##
        z += vz
        x += vx
        y += vy
*/
