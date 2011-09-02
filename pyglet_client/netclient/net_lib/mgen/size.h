void DEBUG_net_message_size() { 
	printf("ramsize, netsize\n");
	printf("sizeof(test1)=%i, %i", sizeof(struct test1)),sizeof(uint8_t)+sizeof(float)+0);
	printf("sizeof(test2)=%i, %i", sizeof(struct test2)),sizeof(int16_t)+sizeof(int32_t)+0);
	printf("sizeof(test3)=%i, %i", sizeof(struct test3)),sizeof(uint16_t)+sizeof(uint32_t)+0);
	printf("sizeof(uni)=%i, %i", sizeof(struct uni)),sizeof(uint8_t)+sizeof(int16_t)+sizeof(uint16_t)+sizeof(int32_t)+sizeof(uint32_t)+sizeof(float)+0);
}
