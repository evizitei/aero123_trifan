#include "FlightController.h"
#include "FlightLog.h"
#include <chrono>
#include <thread>

FlightLog::FlightLog(FlightController* cPtr, std::string fn)
{
    mtx.lock();
    ctrl = cPtr;
    logFileName = fn;
    logStream.open(logFileName);
    shouldClose = false;
    mtx.unlock();
}

void FlightLog::logTime()
{
    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
    std::time_t tsTime = std::chrono::system_clock::to_time_t(timestamp);
    mtx.lock();
    logStream << " ---- " << std::ctime(&tsTime) << "";
    mtx.unlock();
}

void FlightLog::logMessage(std::string message)
{
    logTime();
    mtx.lock();
    logStream << "***************************\n" << message << "\n***************************\n\n";
    logStream.flush();
    mtx.unlock();
}

void FlightLog::close()
{
    mtx.lock();
    logStream.flush();
    logStream.close();
    mtx.unlock();
}

void FlightLog::run()
{
    int logInterval = 1;
    logMessage("Flight Log Begins");
    while (true){
        std::this_thread::sleep_for(std::chrono::milliseconds(logInterval * 1000));
        logMessage(ctrl->getStatus());
        if(shouldClose){
            break;
        }
    }
    close();
}

void FlightLog::signalStop()
{
    mtx.lock();
    shouldClose = true;
    mtx.unlock();
}