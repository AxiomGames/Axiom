#include "ThreadPool.hpp"
#include "Axiom/Math/Math.hpp"

static void AXThreadProc(ax::LockFreeRingBuffer<16>* jobs)
{
	while (!jobs->m_Done)
	{
		std::function<void()> job; 
		while (jobs->Pop(job)) {
			job();
		}
		std::this_thread::yield();
	}
}

void ax::ThreadPool::Destroy()
{
	for (int i = 0; i < m_NumThreads; ++i) {
		m_Jobs[i].m_Done = true;
	}
}

void ax::ThreadPool::PushJob(std::function<void()> job)
{
	// if thread is full try to find available thread and push to it
	while (!m_Jobs[m_CurretWorkerIndex++ % m_NumThreads].Push(job))
		std::this_thread::yield();
}

ax::ThreadPool::~ThreadPool() {

}

ax::ThreadPool::ThreadPool()
: m_NumThreads(Clamp(std::thread::hardware_concurrency(), 1u, MaxThreads))
{
	Initialize();
}

ax::ThreadPool::ThreadPool(uint32 threadCount)
: m_NumThreads(Clamp(threadCount, 1u, MaxThreads))
{
	Initialize();
}

void ax::ThreadPool::Initialize()
{
	for (uint32 i = 0; i < m_NumThreads; ++i)
	{
		std::thread worker(AXThreadProc, &m_Jobs[i]);
		ax::SetToUniqueCore(worker, i);
		worker.detach();// forget about this thread, let it do it's job in the infinite loop that we created above
	}
}