#ifndef net_lib_mgen_size_h 
#define net_lib_mgen_size_h 


{ 
	struct NET_agent_snapshot s;
	s.id = x; // int16_t 
	s.tick = x; // int16_t 
	s.x = x; // float 
	s.y = x; // float 
	s.z = x; // float 
	s.vx = x; // float 
	s.vy = x; // float 
	s.vz = x; // float 
	pack_struct_NET_agent_snapshot(buff, &s, &n); 
 } 

{ 
	struct NET_agent_control_state s;
	s.id = x; // int16_t 
	s.tick = x; // int16_t 
	s.seq = x; // uint8_t 
	s.theta = x; // uint8_t 
	s.phi = x; // uint8_t 
	s.cs = x; // uint32_t 
	pack_struct_NET_agent_control_state(buff, &s, &n); 
 } 


#endif
