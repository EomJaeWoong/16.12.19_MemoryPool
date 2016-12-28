/*---------------------------------------------------------------

	procademy MemoryPool.

	�޸� Ǯ Ŭ����.
	Ư�� ����Ÿ(����ü,Ŭ����,����)�� ������ �Ҵ� �� ��������.

	- ����.

	procademy::CMemoryPool<DATA> MemPool(300, FALSE);
	DATA *pData = MemPool.Alloc();

	pData ���

	MemPool.Free(pData);


	!.	���� ���� ���Ǿ� �ӵ��� ������ �� �޸𸮶�� �����ڿ���
		Lock �÷��׸� �־� ����¡ ���Ϸ� ���縦 ���� �� �ִ�.
		���� �߿��� ��찡 �ƴ��̻� ��� ����.

		
		
		���ǻ��� :	�ܼ��� �޸� ������� ����Ͽ� �޸𸮸� �Ҵ��� �޸� ������ �����Ͽ� �ش�.
					Ŭ������ ����ϴ� ��� Ŭ������ ������ ȣ�� �� Ŭ�������� �Ҵ��� ���� ���Ѵ�.
					Ŭ������ �����Լ�, ��Ӱ��谡 ���� �̷����� �ʴ´�.
					VirtualAlloc ���� �޸� �Ҵ� �� memset ���� �ʱ�ȭ�� �ϹǷ� Ŭ���������� ���� ����.
		
				
----------------------------------------------------------------*/
#ifndef  __MEMORYPOOL__H__
#define  __MEMORYPOOL__H__s
#include <assert.h>
#include <new.h>
//#include <malloc.h>

#include "LockFreeStack.h"

template <class DATA>
class CMemoryPool
{
private:

	/* **************************************************************** */
	// �� ���� �տ� ���� ��� ����ü.
	/* **************************************************************** */
	struct st_BLOCK_NODE
	{
		st_BLOCK_NODE()
		{
			stpNextBlock = NULL;
		}
		st_BLOCK_NODE *stpNextBlock;
	};

public:

	//////////////////////////////////////////////////////////////////////////
	// ������, �ı���.
	//
	// Parameters:	(int) �ִ� ���� ����.
	//				(bool) �޸� Lock �÷��� - �߿��ϰ� �ӵ��� �ʿ�� �Ѵٸ� Lock.
	// Return:
	//////////////////////////////////////////////////////////////////////////
	CMemoryPool(int iBlockNum, bool bLockFlag = false)
	{
		Initial(iBlockNum, bLockFlag);
	}

	virtual	~CMemoryPool()
	{
		Release();
	}

	void Initial(int iBlockNum, bool bLockFlag = false)
	{
		st_BLOCK_NODE *pNode, *pPreNode;

		////////////////////////////////////////////////////////////////
		// �޸� Ǯ ũ�� ����
		////////////////////////////////////////////////////////////////
		m_iBlockCount = iBlockNum;

		if (iBlockNum < 0)	return;

		else if (iBlockNum == 0)
		{
			m_bStoreFlag = true;
			m_stBlockHeader = NULL;
		}

		else
		{
			m_bStoreFlag = false;

			pNode = (st_BLOCK_NODE *)malloc(sizeof(DATA) + sizeof(st_BLOCK_NODE));
			m_stBlockHeader = pNode;
			pPreNode = pNode;

			m_LockfreeStack.Push(pNode);

			for (int iCnt = 1; iCnt < iBlockNum; iCnt++)
			{
				pNode = (st_BLOCK_NODE *)malloc(sizeof(DATA) + sizeof(st_BLOCK_NODE));
				pPreNode->stpNextBlock = pNode;
				pPreNode = pNode;

				m_LockfreeStack.Push(pNode);
			}
		}
	}

	void Release()
	{
		delete[] m_stBlockHeader;
	}

	//////////////////////////////////////////////////////////////////////////
	// ���� �ϳ��� �Ҵ�޴´�.
	//
	// Parameters: ����.
	// Return: (DATA *) ����Ÿ ���� ������.
	//////////////////////////////////////////////////////////////////////////
	DATA	*Alloc(bool bPlacementNew = true)
	{
		st_BLOCK_NODE *stpBlock;

		if (m_iBlockCount < m_iAllocCount)
		{
			stpBlock = (st_BLOCK_NODE *)malloc(sizeof(DATA) + sizeof(st_BLOCK_NODE));
			InterlockedIncrement64((LONG64 *)&m_iBlockCount);
		}

		else
		{
			if (!m_LockfreeStack.Pop(&stpBlock))
				return NULL;
		}

		if (bPlacementNew)	new ((DATA *)(stpBlock + 1)) DATA;

		return (DATA *)(stpBlock + 1);
	}

	//////////////////////////////////////////////////////////////////////////
	// ������̴� ������ �����Ѵ�.
	//
	// Parameters: (DATA *) ���� ������.
	// Return: (BOOL) TRUE, FALSE.
	//////////////////////////////////////////////////////////////////////////
	bool	Free(DATA *pData)
	{
		if (!m_LockfreeStack.Push((st_BLOCK_NODE *)pData - 1))
			return false;

		InterlockedIncrement64((LONG64 *)&m_iAllocCount);
		return true;
	}


	//////////////////////////////////////////////////////////////////////////
	// ���� ������� ���� ������ ��´�.
	//
	// Parameters: ����.
	// Return: (int) ������� ���� ����.
	//////////////////////////////////////////////////////////////////////////
	int		GetAllocCount(void) { return m_iAllocCount; }

private :
	CLockfreeStack<st_BLOCK_NODE *> m_LockfreeStack;

	//////////////////////////////////////////////////////////////////////////
	// ��� ����ü ���
	//////////////////////////////////////////////////////////////////////////
	st_BLOCK_NODE *m_stBlockHeader;

	//////////////////////////////////////////////////////////////////////////
	// �޸� Lock �÷���
	//////////////////////////////////////////////////////////////////////////
	bool m_bLockFlag;

	//////////////////////////////////////////////////////////////////////////
	// �޸� ���� �÷���, true�� �ʿ��ҋ����� ���� �������� ����
	//////////////////////////////////////////////////////////////////////////
	bool m_bStoreFlag;

	//////////////////////////////////////////////////////////////////////////
	// ���� ������� ���� ����
	//////////////////////////////////////////////////////////////////////////
	int m_iAllocCount;

	//////////////////////////////////////////////////////////////////////////
	// ��ü ���� ����
	//////////////////////////////////////////////////////////////////////////
	int m_iBlockCount;
};

#endif