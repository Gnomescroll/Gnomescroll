
/*
types:

uint8_t

int16_t
uint16_t

int32_t
uint32_t

float
*/

//
static inline void PACK_uint8_t

int pack_struct_uni(void* buffer, struct uni* s) {
        int n=0;
        PACK_uint8_t(s->t1, buffer, &n);
        PACK_int16_t(s->t4, buffer, &n);
        PACK_uint16_t(s->t5, buffer, &n);
        PACK_int32_t(s->t2, buffer, &n);
        PACK_uint32_t(s->t3, buffer, &n);
        PACK_int16_t(s->t4, buffer, &n);
        PACK_uint16_t(s->t5, buffer, &n);
        PACK_float(s->t6, buffer, &n);
        return n;
}

void unpack_struct_uni(void* buffer, struct uni* s) {
        int n=0;
        UNPACK_uint8_t(s->t1, buffer, &n);
        UNPACK_int16_t(s->t4, buffer, &n);
        UNPACK_uint16_t(s->t5, buffer, &n);
        UNPACK_int32_t(s->t2, buffer, &n);
        UNPACK_uint32_t(s->t3, buffer, &n);

        UNPACK_float(s->t6, buffer, &n);
        return n;
}
