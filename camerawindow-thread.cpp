#include <thread>
#include "camerawindow.h"

void CameraWindow::StartThread()
{
    camera_thread = std::thread([this] { ThreadLoop(); });
}

void CameraWindow::StopThread()
{
    {
        std::unique_lock<std::mutex> lock(camera_mutex);
        terminate = true;
        camera_cond.notify_all();
    }
    camera_thread.join();
}

void CameraWindow::InsertItem(uint32_t flags, callback_t callback)
{
    QueueItem item = {flags, callback};
    std::unique_lock<std::mutex> lock(camera_mutex);

    if (flags & CTRL_FLG_REPLACE) {
        camera_queue.remove_if(
            [flags](const QueueItem &it) { return (it.flags & flags & CTRL_FLG_REPLACE) ? true : false; });
    }

    camera_queue.push_back(item);
    camera_cond.notify_all();
}

void CameraWindow::ThreadLoop()
{
    while (!terminate) {
        std::unique_lock<std::mutex> lock(camera_mutex);
        camera_cond.wait(lock, [&] { return !camera_queue.empty() || terminate; });
        while (!camera_queue.empty()) {
            QueueItem it = *camera_queue.begin();
            camera_queue.erase(camera_queue.begin());
            lock.unlock();
            it.callback();
            lock.lock();
        }
    }
}
