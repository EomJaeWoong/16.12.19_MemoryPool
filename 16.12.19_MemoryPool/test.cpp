#include <stdio.h>
#include <process.h>
#include <Windows.h>

#include "MemoryPool.h"

#define dfTHREAD_MAX 10

CMemoryPool<int> g_mem(0);


unsigned __stdcall StackThread(void *pParam);

void main()
{
	HANDLE hThread[dfTHREAD_MAX];
	DWORD dwThreadID;



	for (int iCnt = 0; iCnt < dfTHREAD_MAX; iCnt++)
	{
		hThread[iCnt] = (HANDLE)_beginthreadex(
			NULL,
			0,
			StackThread,
			(LPVOID)1000,
			0,
			(unsigned int *)&dwThreadID
			);
	}

	while (1)
	{
		Sleep(999);
	}
}

/*------------------------------------------------------------------*/
// 0. �� �����忡�� st_QUEUE_DATA �����͸� ���� ��ġ (10000��) ����		
// 0. ������ ����(Ȯ��)
// 1. iData = 0x0000000055555555 ����
// 1. lCount = 0 ����
// 2. ���ÿ� ����

// 3. �ణ���  Sleep (0 ~ 3)
// 4. ���� ���� ������ �� ��ŭ ���� 
// 4. - �̶� �����°� ���� ���� �������� ����, �ٸ� �����尡 ���� �������� ���� ����
// 5. ���� ��ü �����Ͱ� �ʱⰪ�� �´��� Ȯ��. (�����͸� ���� ����ϴ��� Ȯ��)
// 6. ���� ��ü �����Ϳ� ���� lCount Interlock + 1
// 6. ���� ��ü �����Ϳ� ���� iData Interlock + 1
// 7. �ణ���
// 8. + 1 �� �����Ͱ� ��ȿ���� Ȯ�� (���� �����͸� ���� ����ϴ��� Ȯ��)
// 9. ������ �ʱ�ȭ (0x0000000055555555, 0)
// 10. ���� �� ��ŭ ���ÿ� �ٽ� ����
//  3�� ���� �ݺ�.
/*------------------------------------------------------------------*/
unsigned __stdcall StackThread(void *pParam)
{
	int *i;

	while (1)
	{
		i = g_mem.Alloc();

		*i = rand();
		printf("%d\n", *i);
		g_mem.Free(i);

		Sleep(3);
	}
}