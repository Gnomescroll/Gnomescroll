#ifndef net_lib_mgen_size_h 
#define net_lib_mgen_size_h 


{ 
	struct test1 s;
	s.testbyte = x; // uint8_t 
	s.testfloat = x; // float 
	pack_struct_test1(buff, &s, &n); 
 } 

{ 
	struct test2 s;
	s.testshort = x; // int16_t 
	s.testint = x; // int32_t 
	pack_struct_test2(buff, &s, &n); 
 } 

{ 
	struct test3 s;
	s.testushort = x; // uint16_t 
	s.testuint = x; // uint32_t 
	pack_struct_test3(buff, &s, &n); 
 } 

{ 
	struct uni s;
	s.t1 = x; // uint8_t 
	s.t4 = x; // int16_t 
	s.t5 = x; // uint16_t 
	s.t2 = x; // int32_t 
	s.t3 = x; // uint32_t 
	s.t6 = x; // float 
	pack_struct_uni(buff, &s, &n); 
 } 


#endif
