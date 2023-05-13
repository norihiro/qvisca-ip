#include "camerawindow.h"
#include "ui_camerawindow.h"

#define CAMERA_THREAD(proc) InsertItem(0, [this]() { proc })

const uint8_t VISCA_POWER_ON = 2;
const uint8_t VISCA_POWER_OFF = 3;
const int VISCA_DIGITAL_EFFECT_LEVEL_FLASH_TRAIL_MAX = 0x18;
const int VISCA_DIGITAL_EFFECT_LEVEL_LUMI_STILL_MAX = 0x20;
const int VISCA_AE_MODES[] = {
    VISCA_AUTO_EXP_FULL_AUTO,     VISCA_AUTO_EXP_MANUAL,        VISCA_AUTO_EXP_SHUTTER_PRIORITY,
    VISCA_AUTO_EXP_IRIS_PRIORITY, VISCA_AUTO_EXP_GAIN_PRIORITY, VISCA_AUTO_EXP_SHUTTER_AUTO,
    VISCA_AUTO_EXP_IRIS_AUTO,     VISCA_AUTO_EXP_GAIN_AUTO,     VISCA_AUTO_EXP_BRIGHT};

// Enable states of the sliders for the different AE modes.
//                            Auto,  Man,  S Pri, I Pri, G Pri, S Aut, I Aut, G Aut, Bright
const bool SHUTTER_ENABLED[] = {false, true, true, false, false, false, true, true, false};
const bool IRIS_ENABLED[] = {false, true, false, true, false, true, false, true, false};
const bool GAIN_ENABLED[] = {false, true, false, false, true, true, true, false, false};
const bool BRIGHT_ENABLED[] = {false, false, false, false, false, false, false, false, true};

CameraWindow::CameraWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::CameraWindow), panSpeed(1), tiltSpeed(1)
{
    ui->setupUi(this);
    StartThread();
}

CameraWindow::~CameraWindow()
{
    StopThread();
    if (camera_valid) {
        VISCA_close(&iface);
    }
    delete ui;
}

void CameraWindow::OpenInterface(const char *host, int port)
{
    std::string host_str = host;
    InsertItem(0, [this, host_str, port]() {
        if (camera_valid) {
            VISCA_close(&iface);
            camera_valid = false;
        }
        if (VISCA_open_udp(&iface, host_str.c_str(), 52381) != VISCA_SUCCESS) {
            fprintf(stderr, "camera_ui: unable to the host %s port %d\n", host_str.c_str(), port);
            return VISCA_FAILURE;
        }
        camera_valid = true;

        camera.address = 1;
        if (VISCA_clear(&iface, &camera) != VISCA_SUCCESS)
            return VISCA_FAILURE;
        if (VISCA_get_camera_info(&iface, &camera) != VISCA_SUCCESS)
            return VISCA_FAILURE;

        QMetaObject::invokeMethod(this, "on_visca_connected");

        return VISCA_SUCCESS;
    });
}

void CameraWindow::UpdateAESliders(int index)
{
    ui->shutterSlider->setEnabled(SHUTTER_ENABLED[index]);
    ui->irisSlider->setEnabled(IRIS_ENABLED[index]);
    ui->gainSlider->setEnabled(GAIN_ENABLED[index]);
    ui->brightSlider->setEnabled(BRIGHT_ENABLED[index]);
    ui->backLightCompensationCheckBox->setEnabled(index == 0);
}

void CameraWindow::on_connectButton_clicked()
{
    QString host;
    int port = 52381;
    auto host_port = ui->hostLineEdit->text().split(':');
    if (host_port.size() >= 1)
        host = host_port[0];
    if (host_port.size() >= 2)
        port = host_port[1].toInt();

    OpenInterface(host.toStdString().c_str(), port);
}

void CameraWindow::on_visca_connected()
{
    ui->vendorLineEdit->setText(QString("0x%1").arg(QString::number(camera.vendor, 16), 4, '0'));
    ui->rOMVerLineEdit->setText(QString("0x%1").arg(QString::number(camera.rom_version, 16), 4, '0'));
    ui->modelLineEdit->setText(QString("0x%1").arg(QString::number(camera.model, 16), 4, '0'));
    ui->socketLineEdit->setText(QString("0x%1").arg(QString::number(camera.socket_num, 16), 4, '0'));
}

void CameraWindow::on_cameraPowerButton_clicked()
{
    uint8_t power = VISCA_POWER_OFF;
    //    VISCA_get_power(&iface, &camera, &power);
    // TODO: Power off seems to wait forever for a response, only do power on
    if (power == VISCA_POWER_ON) {
        CAMERA_THREAD({ return VISCA_set_power(&iface, &camera, VISCA_POWER_OFF); });
    } else {
        CAMERA_THREAD({ return VISCA_set_power(&iface, &camera, VISCA_POWER_ON); });
    }
}

void CameraWindow::on_cameraDisplayCheckBox_stateChanged(int arg1)
{
    switch (arg1) {
    case Qt::Unchecked:
        CAMERA_THREAD({ return VISCA_set_datascreen_off(&iface, &camera); });
        break;
    case Qt::Checked:
        CAMERA_THREAD({ return VISCA_set_datascreen_on(&iface, &camera); });
        break;
    }
}

void CameraWindow::on_cameraIrReceiveCheckBox_stateChanged(int arg1)
{
    switch (arg1) {
    case Qt::Unchecked:
        CAMERA_THREAD({ return VISCA_set_irreceive_off(&iface, &camera); });
        break;
    case Qt::Checked:
        CAMERA_THREAD({ return VISCA_set_irreceive_on(&iface, &camera); });
        break;
    }
}

void CameraWindow::on_panTiltJoystickUpLeftButton_clicked()
{
    InsertItem(CTRL_FLG_PANTILT, [this]() { return VISCA_set_pantilt_upleft(&iface, &camera, panSpeed, tiltSpeed); });
}

void CameraWindow::on_panTiltJoystickUpButton_clicked()
{
    InsertItem(CTRL_FLG_PANTILT, [this]() { return VISCA_set_pantilt_up(&iface, &camera, panSpeed, tiltSpeed); });
}

void CameraWindow::on_panTiltJoystickUpRightButton_clicked()
{
    InsertItem(CTRL_FLG_PANTILT, [this]() { return VISCA_set_pantilt_upright(&iface, &camera, panSpeed, tiltSpeed); });
}

void CameraWindow::on_panTiltJoystickRightButton_clicked()
{
    InsertItem(CTRL_FLG_PANTILT, [this]() { return VISCA_set_pantilt_right(&iface, &camera, panSpeed, tiltSpeed); });
}

void CameraWindow::on_panTiltJoystickDownRightButton_clicked()
{
    InsertItem(CTRL_FLG_PANTILT,
               [this]() { return VISCA_set_pantilt_downright(&iface, &camera, panSpeed, tiltSpeed); });
}

void CameraWindow::on_panTiltJoystickDownButton_clicked()
{
    InsertItem(CTRL_FLG_PANTILT, [this]() { return VISCA_set_pantilt_down(&iface, &camera, panSpeed, tiltSpeed); });
}

void CameraWindow::on_panTiltJoystickDownLeftButton_clicked()
{
    InsertItem(CTRL_FLG_PANTILT, [this]() { return VISCA_set_pantilt_downleft(&iface, &camera, panSpeed, tiltSpeed); });
}

void CameraWindow::on_panTiltJoystickLeftButton_clicked()
{
    InsertItem(CTRL_FLG_PANTILT, [this]() { return VISCA_set_pantilt_left(&iface, &camera, panSpeed, tiltSpeed); });
}

void CameraWindow::on_panTiltJoystickStopButton_clicked()
{
    InsertItem(CTRL_FLG_PANTILT, [this]() { return VISCA_set_pantilt_stop(&iface, &camera, panSpeed, tiltSpeed); });
}

void CameraWindow::on_panTiltHomeButton_clicked()
{
    InsertItem(CTRL_FLG_PANTILT, [this]() { return VISCA_set_pantilt_home(&iface, &camera); });
}

void CameraWindow::on_panTiltResetButton_clicked()
{
    InsertItem(CTRL_FLG_PANTILT, [this]() { return VISCA_set_pantilt_reset(&iface, &camera); });
}

void CameraWindow::on_zoomSlider_valueChanged(int value)
{
    InsertItem(CTRL_FLG_ZOOM, [this, value]() { return VISCA_set_zoom_value(&iface, &camera, value); });
}

void CameraWindow::on_zoomTeleButton_clicked()
{
    InsertItem(CTRL_FLG_ZOOM, [this]() { return VISCA_set_zoom_tele(&iface, &camera); });
}

void CameraWindow::on_zoomStopButton_clicked()
{
    InsertItem(CTRL_FLG_ZOOM, [this]() { return VISCA_set_zoom_stop(&iface, &camera); });
}

void CameraWindow::on_zoomWideButton_clicked()
{
    InsertItem(CTRL_FLG_ZOOM, [this]() { return VISCA_set_zoom_wide(&iface, &camera); });
}

void CameraWindow::on_wBComboBox_currentIndexChanged(int index)
{
    ui->bGainSlider->setEnabled(index == VISCA_WB_MANUAL);
    ui->rGainSlider->setEnabled(index == VISCA_WB_MANUAL);

    InsertItem(0, [this, index]() {
        auto ret = VISCA_set_whitebal_mode(&iface, &camera, index);
        if (ret != VISCA_SUCCESS)
            return ret;
        uint16_t rGain, bGain;
        ret = VISCA_get_rgain_value(&iface, &camera, &rGain);
        if (ret != VISCA_SUCCESS)
            return ret;
        ret = VISCA_get_bgain_value(&iface, &camera, &bGain);
        if (ret != VISCA_SUCCESS)
            return ret;
        QMetaObject::invokeMethod(ui->bGainSlider, "setValue", Q_ARG(int, bGain));
        QMetaObject::invokeMethod(ui->rGainSlider, "setValue", Q_ARG(int, rGain));
        return VISCA_SUCCESS;
    });
}

void CameraWindow::on_rGainSlider_valueChanged(int value)
{
    InsertItem(0, [this, value]() { return VISCA_set_rgain_value(&iface, &camera, value); });
}

void CameraWindow::on_bGainSlider_valueChanged(int value)
{
    InsertItem(0, [this, value]() { return VISCA_set_bgain_value(&iface, &camera, value); });
}

void CameraWindow::on_aEComboBox_currentIndexChanged(int index)
{
    InsertItem(0, [this, index]() {
        auto ret = VISCA_set_auto_exp_mode(&iface, &camera, VISCA_AE_MODES[index]);
        QMetaObject::invokeMethod(this, "UpdateAESliders", Q_ARG(int, index));
        return ret;
    });
}

void CameraWindow::on_shutterSlider_valueChanged(int value)
{
    InsertItem(0, [this, value]() { return VISCA_set_shutter_value(&iface, &camera, value); });
}

void CameraWindow::on_irisSlider_valueChanged(int value)
{
    InsertItem(0, [this, value]() { return VISCA_set_iris_value(&iface, &camera, value); });
}

void CameraWindow::on_gainSlider_valueChanged(int value)
{
    InsertItem(0, [this, value]() { return VISCA_set_gain_value(&iface, &camera, value); });
}

void CameraWindow::on_brightSlider_valueChanged(int value)
{
    InsertItem(0, [this, value]() { return VISCA_set_bright_value(&iface, &camera, value); });
}

void CameraWindow::on_apertureSlider_valueChanged(int value)
{
    InsertItem(0, [this, value]() { return VISCA_set_aperture_value(&iface, &camera, value); });
}

void CameraWindow::on_exposureCompensationSlider_valueChanged(int value)
{
    InsertItem(0, [this, value]() { return VISCA_set_exp_comp_value(&iface, &camera, value); });
}

void CameraWindow::on_exposureCompensationCheckBox_stateChanged(int arg1)
{
    switch (arg1) {
    case Qt::Unchecked:
        ui->exposureCompensationSlider->setEnabled(false);
        CAMERA_THREAD({ return VISCA_set_exp_comp_power(&iface, &camera, VISCA_POWER_OFF); });
        break;
    case Qt::Checked:
        ui->exposureCompensationSlider->setEnabled(true);
        CAMERA_THREAD({ return VISCA_set_exp_comp_power(&iface, &camera, VISCA_POWER_ON); });
        break;
    }
}

void CameraWindow::on_backLightCompensationCheckBox_stateChanged(int arg1)
{
    switch (arg1) {
    case Qt::Unchecked:
        CAMERA_THREAD({ return VISCA_set_backlight_comp(&iface, &camera, VISCA_POWER_OFF); });
        break;
    case Qt::Checked:
        CAMERA_THREAD({ return VISCA_set_backlight_comp(&iface, &camera, VISCA_POWER_ON); });
        break;
    }
}

void CameraWindow::on_slowShutterAutoCheckBox_stateChanged(int arg1)
{
    switch (arg1) {
    case Qt::Unchecked:
        CAMERA_THREAD({ return VISCA_set_slow_shutter_auto(&iface, &camera, VISCA_POWER_OFF); });
        break;
    case Qt::Checked:
        CAMERA_THREAD({ return VISCA_set_slow_shutter_auto(&iface, &camera, VISCA_POWER_ON); });
        break;
    }
}

void CameraWindow::on_wideModeComboBox_currentIndexChanged(int index)
{
    InsertItem(0, [this, index]() { return VISCA_set_wide_mode(&iface, &camera, index); });
}

void CameraWindow::on_pictureEffectComboBox_currentIndexChanged(int index)
{
    InsertItem(0, [this, index]() { return VISCA_set_picture_effect(&iface, &camera, index); });
}

void CameraWindow::on_digitalEffectComboBox_currentIndexChanged(int index)
{
    switch (index) {
    case VISCA_DIGITAL_EFFECT_FLASH:
    case VISCA_DIGITAL_EFFECT_TRAIL:
        ui->digitalEffectLevelSpinBox->setMaximum(VISCA_DIGITAL_EFFECT_LEVEL_FLASH_TRAIL_MAX);
        ui->digitalEffectLevelSpinBox->setEnabled(true);
        break;
    case VISCA_DIGITAL_EFFECT_STILL:
    case VISCA_DIGITAL_EFFECT_LUMI:
        ui->digitalEffectLevelSpinBox->setMaximum(VISCA_DIGITAL_EFFECT_LEVEL_LUMI_STILL_MAX);
        ui->digitalEffectLevelSpinBox->setEnabled(true);
        break;
    default:
        ui->digitalEffectLevelSpinBox->setEnabled(false);
        break;
    }

    InsertItem(0, [this, index]() { return VISCA_set_digital_effect(&iface, &camera, index); });
}

void CameraWindow::on_digitalEffectLevelSpinBox_valueChanged(int arg1)
{
    InsertItem(0, [this, arg1]() { return VISCA_set_digital_effect_level(&iface, &camera, arg1); });
}

void CameraWindow::on_pictureFreezeCheckBox_stateChanged(int arg1)
{
    switch (arg1) {
    case Qt::Unchecked:
        CAMERA_THREAD({ return VISCA_set_freeze(&iface, &camera, VISCA_POWER_OFF); });
        break;
    case Qt::Checked:
        CAMERA_THREAD({ return VISCA_set_freeze(&iface, &camera, VISCA_POWER_ON); });
        break;
    }
}

void CameraWindow::on_pictureMirrorCheckBox_stateChanged(int arg1)
{
    switch (arg1) {
    case Qt::Unchecked:
        CAMERA_THREAD({ return VISCA_set_mirror(&iface, &camera, VISCA_POWER_OFF); });
        break;
    case Qt::Checked:
        CAMERA_THREAD({ return VISCA_set_mirror(&iface, &camera, VISCA_POWER_ON); });
        break;
    }
}

void CameraWindow::on_memoryListWidget_itemSelectionChanged()
{
    ui->memoryButtonsContainerWidget->setEnabled(true);
}

void CameraWindow::on_memoryResetButton_clicked()
{
    CAMERA_THREAD({ return VISCA_memory_reset(&iface, &camera, ui->memoryListWidget->currentRow()); });
}

void CameraWindow::on_memorySetButton_clicked()
{
    CAMERA_THREAD({ return VISCA_memory_set(&iface, &camera, ui->memoryListWidget->currentRow()); });
}

void CameraWindow::on_memoryRecallButton_clicked()
{
    CAMERA_THREAD({ return VISCA_memory_recall(&iface, &camera, ui->memoryListWidget->currentRow()); });
}
