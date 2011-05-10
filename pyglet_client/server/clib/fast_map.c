

extern int test(void);

int test(void)
{
  return 5;
}


extern int hash_cord(int);

inline int hash_cord(int input)
{
    return input >> 4;
}

