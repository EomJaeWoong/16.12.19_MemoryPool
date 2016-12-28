#include "MemoryPool.h"
#include <stdio.h>

void main()
{
	CMemoryPool<int> m(10000);
	int *i;

	while (1)
	{
		i = m.Alloc();

		*i = rand();
		printf("%d\n", *i);
		m.Free(i);

		//Sleep(50);
	}
}