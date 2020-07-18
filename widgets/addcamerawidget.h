#ifndef ADDCAMERAWIDGET_H
#define ADDCAMERAWIDGET_H

#include "ocv/camerastream.h"
#include "ocv/semaphore.h"


#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QComboBox>

class AddCameraWidget : public QWidget
{
    Q_OBJECT
private:
    QFormLayout* fLayout;
    QGridLayout* gLayout;
    QGroupBox* gBox;

    QPushButton *m_pBtnOpenLocalVideo;
    QPushButton *m_pBtnOpenRTSPStream;
    QPushButton *m_pBtnSave;
    QPushButton *m_pBtnSetUpRule;
    QLineEdit *m_leCameraLocation;
    QLineEdit *m_leCameraName;
    QComboBox *m_cbRuleType;

    QSqlDatabase* m_dataBase;

    CameraStream* m_cameraStream;
    Camera* m_camera;

    void m_pBtnOpenLocalVideo_clicked();
    void m_pBtnSave_clicked();
    void m_pBtnSetUpRule_clicked();

    QMutex m_mutex;
public:
    explicit AddCameraWidget( QSqlDatabase* dataBase,QWidget *parent = nullptr);
    virtual ~AddCameraWidget();

signals:
    void cameraSavedSignal(CameraStream* cameraStream);
};

#endif // ADDCAMERAWIDGET_H
