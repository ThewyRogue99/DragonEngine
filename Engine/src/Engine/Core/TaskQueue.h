#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace Engine
{
    // A class that represents a task in the queue
    class Task
    {
    public:
        std::function<void()> TaskFunction = nullptr;
        // A flag to indicate whether the task is complete (set by the worker thread)
        bool bIsComplete = false;

        Task(std::function<void()> Function)
            : TaskFunction(Function), bIsComplete(false) { }

        Task(const Task&) = default;
    };

    // A class that manages a queue of tasks and a pool of worker threads
    class TaskQueue
    {
    public:
        TaskQueue(int NumThreads);
        ~TaskQueue();

        // Add a task to the queue
        void PushTask(std::function<void()> TaskFunction);

        // Wait for all tasks in the queue to complete
        void WaitForCompletion();

        // Shut down the queue and wait for the worker threads to complete
        void Shutdown();

        // Wait for all worker threads to complete
        void Join();

    private:
        // The function executed by each worker thread
        void WorkerThread();

    private:
        std::queue<Task> Tasks = { };

        std::vector<std::thread*> Threads = { };

        std::mutex QueueMutex;
        std::condition_variable TaskAvailableCondition;

        bool bIsAcceptingTasks = false;
        bool bIsShuttingDown = false;
    };
}