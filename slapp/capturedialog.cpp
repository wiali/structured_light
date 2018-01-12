#include "capturedialog.h"
#include "ui_capturedialog.h"
#include <QCoreApplication>
#include <QDebug>

CaptureDialog::CaptureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CaptureDialog),
    m_state(Empty),
    m_width(640),
    m_height(480),
    m_aspect(0.75f),
    m_timer(0)

{
    ui->setupUi(this);


    // disable Ok button
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);

    // connect slots
    connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(reset()));
    connect(ui->grabButton, SIGNAL(clicked()), this, SLOT(grabImage()));

    // initialize capture
    m_downCam.open(CV_CAP_DSHOW + 0);
    if (!m_downCam.isOpened())
    {
        return;
    }

    auto resolution = QSize(1920, 1280);
    m_downCam.set(CV_CAP_PROP_FRAME_WIDTH, resolution.width());
    m_downCam.set(CV_CAP_PROP_FRAME_HEIGHT, resolution.height());

    m_downCam >> m_frame_downCam;
    m_aspect = resolution.height() / (float)resolution.width();
    m_width = resolution.width();
    m_height = resolution.height();

    updateLayout();
    cout << "Update Layout" << endl;

    //m_capture = cvCreateCameraCapture(0);
    //if(m_capture) {
    //    // grab first frame to 
    //    m_frame = cvQueryFrame(m_capture);
    //}

    //if(m_frame) {
    //    int w = m_frame->width;
    //    int h = m_frame->height;

    //    // aspect ratio heigh/width
    //    m_aspect = h/(float)w;
    //    m_width  = w <= m_width? w : m_width;
    //    m_height = (int) m_width*m_aspect; 

    //    updateLayout();
    //    cout << "Update Layout" << endl;
    //}

    // periodically update frame
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(refreshFrame()));
    m_timer->start(50);

}

void CaptureDialog::updateLayout() {

    // set camImage size
    ui->camImage->resize(m_width,m_height);

    // compute size for thumbs
    int thumbWidth  = m_width/3 -20;
    int thumbHeight = (int) thumbWidth*m_aspect;

    cout << "resize thumbs to " << thumbWidth << "x" << thumbHeight << endl;

    ui->phase1Thumb->resize(thumbWidth,thumbHeight);
    ui->phase2Thumb->resize(thumbWidth,thumbHeight);
    ui->phase3Thumb->resize(thumbWidth,thumbHeight);
    cout << "real size " << ui->phase3Thumb->width() << "x" << ui->phase3Thumb->height() << endl;

}


void CaptureDialog::refreshFrame() {
        
    //if(m_capture) {
    //    // grab first frame to 
    //    m_frame = cvQueryFrame(m_capture);
    //}

    //if(m_frame) {
    //   QCoreApplication::processEvents();
    //   setPixmap(ui->camImage,m_frame,m_width,m_height);
    //   QCoreApplication::processEvents();
    //}

    if (m_downCam.isOpened()) {
        // grab first frame to 
        m_downCam >> m_frame_downCam;
    }

    if (!m_frame_downCam.empty()) 
    {
        QCoreApplication::processEvents();
        setPixmap(ui->camImage, m_frame_downCam, m_width, m_height);
        QCoreApplication::processEvents();
    }
}


void CaptureDialog::reset() {

    ui->phase1Thumb->clear(); ui->phase1Thumb->setText("Phase 1");
    ui->phase2Thumb->clear(); ui->phase2Thumb->setText("Phase 2");
    ui->phase3Thumb->clear(); ui->phase3Thumb->setText("Phase 3");

    m_state = Empty;
    
    // disable
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
}

    
void CaptureDialog::grabImage() {

    int thumbWidth  = m_width/3 -20;
    int thumbHeight = (int) thumbWidth*m_aspect;

    switch(m_state) {
        case Empty:
            imgPhase1 = m_frame_downCam.clone();
            setPixmap(ui->phase1Thumb, m_frame_downCam,thumbWidth,thumbHeight);
            break;

        case Await1:
            imgPhase2 = m_frame_downCam.clone();
            setPixmap(ui->phase2Thumb, m_frame_downCam,thumbWidth,thumbHeight);
            break;

        case Await2:
            imgPhase3 = m_frame_downCam.clone();
            setPixmap(ui->phase3Thumb, m_frame_downCam,thumbWidth,thumbHeight);
            break;
    }

    updateState();
}

void CaptureDialog::updateState() {
    switch(m_state) {
        case Empty:
            m_state = Await1;
            break;
        case Await1:
            m_state = Await2;
            break;
        case Await2:
            m_state = Complete;

            // enable Ok button
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
            break;
    }
}

CaptureDialog::~CaptureDialog()
{
    delete ui;
    m_downCam.release();
}

QImage CaptureDialog::cvMatToQImage(const cv::Mat& inMat)
{
    switch (inMat.type())
    {
        // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image(inMat.data,
            inMat.cols, inMat.rows,
            static_cast<int>(inMat.step),
            QImage::Format_ARGB32);

        return image;
    }

    // 8-bit, 3 channel
    case CV_8UC3:
    {
        QImage image(inMat.data, inMat.cols, inMat.rows,
            static_cast<int>(inMat.step), QImage::Format_RGB888);

        return image.rgbSwapped();
    }

    // 8-bit, 1 channel
    case CV_8UC1:
    {
        QImage image(inMat.data,
            inMat.cols, inMat.rows,
            static_cast<int>(inMat.step),
            QImage::Format_Grayscale8);

        return image;
    }

    default:
        qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
        break;
    }

    return QImage();
}