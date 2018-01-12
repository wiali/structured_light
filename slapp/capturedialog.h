#ifndef CAPTUREDIALOG_H
#define CAPTUREDIALOG_H

#include <QDialog>
#include <QImage>
#include <QPixmap>
#include <QList>
#include <QTimer>
#include <QLabel>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <math.h>
#include <iostream>

using namespace std;


#define CAM_MAX_WIDTH 640

namespace Ui {
    class CaptureDialog;
}

class CaptureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CaptureDialog(QWidget *parent = 0);
    ~CaptureDialog();

    QList<IplImage*> *getImages() {
        int acc = exec();
        if(acc==Accepted) {
            QList<IplImage*> *images = new QList<IplImage*>();
            IplImage imgTmp = imgPhase1;
            *images << cvCloneImage(&imgTmp);
            imgTmp = imgPhase2;
            *images << cvCloneImage(&imgTmp);
            imgTmp = imgPhase3;
            *images << cvCloneImage(&imgTmp);

            return images;
        }

        return NULL;
    }

public slots:

    void grabImage();

    void reset();
    void refreshFrame();

signals:

    void stateChanged();

protected:

    enum State {Empty, Await1, Await2, Complete};

    QImage cvMatToQImage(const cv::Mat& inMat);

    void setPixmap(QLabel *label, const cv::Mat& image, int w, int h) {

        QImage qimage = cvMatToQImage(image);
        QPixmap pixmap;
        pixmap.convertFromImage(qimage);
       
        // set label
        label->setPixmap(pixmap.scaled(w,h));
        cout << "pixmap size " << w << "x" << h << endl;
        cout << "label size " << label->width() << "x" << label->height() << endl;
    }


    void updateState();
    void updateLayout();

private:
    Ui::CaptureDialog *ui;

    cv::Mat imgPhase1;
    cv::Mat imgPhase2;
    cv::Mat imgPhase3;

    State m_state;

    int m_width;
    int m_height;
    float m_aspect;

    QTimer *m_timer;

    cv::VideoCapture m_downCam;
    cv::Mat m_frame_downCam;
};

#endif // CAPTUREDIALOG_H
