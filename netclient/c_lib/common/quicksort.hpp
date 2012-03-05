#pragma once

void swap(int *a, int *b)
{
  int t=*a; *a=*b; *b=t;
}
void quicksort_int(int* points, int beg, int end)
{
  if (end > beg + 1)
  {
    int piv = points[beg];
    int l = beg + 1, r = end;
    while (l < r)
    {
      if (points[l] <= piv)
        l++;
      else
        swap(&points[l], &points[--r]);
    }
    swap(&points[--l], &points[beg]);
    quicksort_int(points, beg, l);
    quicksort_int(points, r, end);
  }
}

