#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>

namespace Engine
{
    struct ProfileResult
    {
        std::string Name;
        long long Start, End;
        uint32_t ThreadID;
    };

    struct InstrumentationSession
    {
        std::string Name;
    };

    class Instrumentor
    {
    private:
        InstrumentationSession* m_CurrentSession;
        std::ofstream m_OutputStream;
        int m_ProfileCount;

    public:
        Instrumentor();

        void BeginSession(const std::string& name, const std::string& filepath = "results.json");

        void EndSession();

        void WriteProfile(const ProfileResult& result);

        void WriteHeader();

        void WriteFooter();

        static Instrumentor& Get();
    };

    class InstrumentationTimer
    {
    public:
        InstrumentationTimer(const char* name);

        ~InstrumentationTimer();

        void Stop();

    private:
        const char* m_Name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
        bool m_Stopped;
    };
}

//#define DE_PROFILE

#ifdef DE_PROFILE
    #define PROFILE_CONCAT1(x, y) x##y
    #define PROFILE_CONCAT2(x, y) PROFILE_CONCAT1(x, y)

    #define DE_PROFILE_BEGIN_SESSION(name, filepath) Engine::Instrumentor::Get().BeginSession(name, filepath)
    #define DE_PROFILE_END_SESSION() Engine::Instrumentor::Get().EndSession()
    #define DE_PROFILE_SCOPE(name) PROFILE_CONCAT2(Engine::InstrumentationTimer timer, __LINE__)(name);
    #define DE_PROFILE_FUNCTION() DE_PROFILE_SCOPE(__FUNCSIG__)
#else
    #define DE_PROFILE_BEGIN_SESSION(name, filepath)
    #define DE_PROFILE_END_SESSION()
    #define DE_PROFILE_FUNCTION()
    #define DE_PROFILE_SCOPE(name)
#endif