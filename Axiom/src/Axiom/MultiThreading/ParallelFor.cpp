#include "ParallelFor.hpp"
#include <iostream>

static void AXParallelWorkerProc(ax::ParallelThread* parallelThread)
{
	while ((parallelThread->state & ax::eParallelState_Done) == 0)
	{
		if (parallelThread->state & ax::eParallelState_Requested)
		{
			parallelThread->fun->Execute();
			parallelThread->state &= ~ax::eParallelState_Requested;
		}
		std::this_thread::yield();
	}
}

ax::ParallelFor::ParallelFor(int numThreads) : m_NumThreads(numThreads)
{
	for (int i = 0; i < numThreads; ++i)
	{
		std::thread worker(AXParallelWorkerProc, &m_Threads[i]);
		ax::SetToUniqueCore(worker, i);
		worker.detach();
	}
}

ax::ParallelFor::~ParallelFor() {
	for (int i = 0; i < m_NumThreads; ++i)
		m_Threads[i].state = eParallelState_Done;
}