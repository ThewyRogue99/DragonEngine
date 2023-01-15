#include "depch.h"
#include "TaskQueue.h"

namespace Engine
{
    TaskQueue::TaskQueue(int NumThreads) : bIsAcceptingTasks(true), bIsShuttingDown(false)
    {
        // Create the worker threads
        for (int i = 0; i < NumThreads; i++) {
            Threads.emplace_back([this] { this->WorkerThread(); });
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

        std::unique_lock<std::mutex> lock(QueueMutex);
        Tasks.push(TaskFunction);
        TaskAvailable.notify_one();
    }

    void TaskQueue::WaitForCompletion()
    {
        std::unique_lock<std::mutex> lock(QueueMutex);

        while (!Tasks.empty()) {
            TaskAvailable.wait(lock);
        }
    }

    void TaskQueue::Shutdown()
    {
        bIsAcceptingTasks = false;
        bIsShuttingDown = true;

        TaskAvailable.notify_all();

        for (auto& thread : Threads) {
            thread.join();
        }
    }

    void TaskQueue::Join()
    {
        for (auto& thread : Threads) {
            thread.join();
        }
    }

    void TaskQueue::WorkerThread()
    {
        while (true)
        {
            // Wait for a task to become available
            std::unique_lock<std::mutex> lock(QueueMutex);
            TaskAvailable.wait(lock, [this] { return !Tasks.empty() || bIsShuttingDown; });

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