#include "depch.h"
#include "TaskQueue.h"

namespace Engine
{
    TaskQueue::TaskQueue(int NumThreads) : bIsAcceptingTasks(true), bIsShuttingDown(false)
    {
        // Create the worker threads
        for (int i = 0; i < NumThreads; i++) {
            Threads.emplace_back(new std::thread(BIND_CLASS_FN(WorkerThread)));
        }
    }

    TaskQueue::~TaskQueue()
    {
        Shutdown();
    }

    void TaskQueue::PushTask(std::function<void()> TaskFunction)
    {
        if (!bIsAcceptingTasks) {
            throw std::runtime_error("Cannot add tasks to a shutdown queue");
        }

        while (!QueueMutex.try_lock());
        Tasks.push(TaskFunction);

        TaskAvailableCondition.notify_one();

        QueueMutex.unlock();
    }

    void TaskQueue::WaitForCompletion()
    {
        std::unique_lock<std::mutex> lock(QueueMutex);

        while (!Tasks.empty()) {
            TaskAvailableCondition.wait(lock);
        }
    }

    void TaskQueue::Shutdown()
    {
        bIsAcceptingTasks = false;
        bIsShuttingDown = true;

        TaskAvailableCondition.notify_all();

        for (auto thread : Threads) {
            thread->join();

            delete thread;
        }

        Threads.clear();
    }

    void TaskQueue::Join()
    {
        for (auto thread : Threads) {
            thread->join();
        }
    }

    void TaskQueue::WorkerThread()
    {
        while (true)
        {
            // Wait for a task to become available
            std::unique_lock<std::mutex> lock(QueueMutex);

            TaskAvailableCondition.wait(lock, [&] { return !Tasks.empty() || bIsShuttingDown; });

            if (bIsShuttingDown) {
                return;
            }

            // Get the next task
            Task task = Tasks.front();
            Tasks.pop();

            lock.unlock();

            // Execute the task
            task.TaskFunction();
            task.bIsComplete = true;
        }
    }
}