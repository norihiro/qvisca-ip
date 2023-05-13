#ifndef CAMERAWINDOW_H
#define CAMERAWINDOW_H

#include <thread>
#include <mutex>
#include <list>
#include <queue>
#include <condition_variable>
#include <stdint.h>
#include <QMainWindow>
#include <visca/libvisca.h>

namespace Ui {
class CameraWindow;
}

#define CTRL_FLG_PANTILT 1
#define CTRL_FLG_ZOOM 2
#define CTRL_FLG_REPLACE (CTRL_FLG_PANTILT | CTRL_FLG_ZOOM)

class CameraWindow : public QMainWindow
{
    Q_OBJECT

    class QueueItem;
    typedef std::function<uint32_t()> callback_t;

public:
    CameraWindow(QWidget *parent = 0);
    ~CameraWindow();

private slots:
    void on_connectButton_clicked();
    void on_cameraPowerButton_clicked();
    void on_cameraDisplayCheckBox_stateChanged(int arg1);
    void on_cameraIrReceiveCheckBox_stateChanged(int arg1);
    void on_panTiltJoystickUpLeftButton_clicked();
    void on_panTiltJoystickUpButton_clicked();
    void on_panTiltJoystickUpRightButton_clicked();
    void on_panTiltJoystickRightButton_clicked();
    void on_panTiltJoystickDownRightButton_clicked();
    void on_panTiltJoystickDownButton_clicked();
    void on_panTiltJoystickDownLeftButton_clicked();
    void on_panTiltJoystickLeftButton_clicked();
    void on_panTiltJoystickStopButton_clicked();
    void on_panTiltHomeButton_clicked();
    void on_panTiltResetButton_clicked();
    void on_zoomSlider_valueChanged(int value);
    void on_zoomTeleButton_clicked();
    void on_zoomStopButton_clicked();
    void on_zoomWideButton_clicked();
    void on_wBComboBox_currentIndexChanged(int index);
    void on_rGainSlider_valueChanged(int value);
    void on_bGainSlider_valueChanged(int value);
    void on_aEComboBox_currentIndexChanged(int index);
    void on_shutterSlider_valueChanged(int value);
    void on_irisSlider_valueChanged(int value);
    void on_gainSlider_valueChanged(int value);
    void on_brightSlider_valueChanged(int value);
    void on_apertureSlider_valueChanged(int value);
    void on_exposureCompensationSlider_valueChanged(int value);
    void on_exposureCompensationCheckBox_stateChanged(int arg1);
    void on_backLightCompensationCheckBox_stateChanged(int arg1);
    void on_slowShutterAutoCheckBox_stateChanged(int arg1);
    void on_wideModeComboBox_currentIndexChanged(int index);
    void on_pictureEffectComboBox_currentIndexChanged(int index);
    void on_digitalEffectComboBox_currentIndexChanged(int index);
    void on_digitalEffectLevelSpinBox_valueChanged(int arg1);
    void on_pictureFreezeCheckBox_stateChanged(int arg1);
    void on_pictureMirrorCheckBox_stateChanged(int arg1);
    void on_memoryListWidget_itemSelectionChanged();
    void on_memoryResetButton_clicked();
    void on_memorySetButton_clicked();
    void on_memoryRecallButton_clicked();

    void on_visca_connected();
    void UpdateAESliders(int index);

private:
    void OpenInterface(const char *host, int port);
    void CloseInterface();

    void StartThread();
    void StopThread();
    void ThreadLoop();
    void InsertItem(uint32_t flags, callback_t callback);

private:
    Ui::CameraWindow *ui;
    VISCAInterface_t interface;
    VISCACamera_t camera;
    bool camera_valid = false;
    std::list<QueueItem> camera_queue;

    std::thread camera_thread;
    std::mutex camera_mutex;
    std::condition_variable camera_cond;
    volatile bool terminate = false;

    unsigned int panSpeed;
    unsigned int tiltSpeed;
};

struct CameraWindow::QueueItem {
    uint32_t flags;
    callback_t callback;
};

#endif // CAMERAWINDOW_H
