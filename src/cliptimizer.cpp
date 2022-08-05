#include "cliptimizer.h"
#include "utils.h"
#include "trimdialog.h"

Cliptimizer::Cliptimizer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));

    connect(ui.cfolderBtn, SIGNAL(pressed()), this, SLOT(on_folder_pressed()));
    connect(ui.cclipBtn, SIGNAL(pressed()), this, SLOT(on_clip_pressed()));
    connect(ui.folderBtn, SIGNAL(pressed()), this, SLOT(on_folder_pressed()));
    connect(ui.videoBtn, SIGNAL(pressed()), this, SLOT(on_clip_pressed()));
    connect(ui.trimBtn, SIGNAL(pressed()), this, SLOT(on_trim_pressed()));
    connect(ui.nextBtn, SIGNAL(pressed()), this, SLOT(on_next_pressed()));
    connect(ui.previousBtn, SIGNAL(pressed()), this, SLOT(on_previous_pressed()));
    connect(ui.pauseBtn, SIGNAL(pressed()), this, SLOT(on_pause_pressed()));
    connect(ui.outfolderBtn, SIGNAL(pressed()), this, SLOT(on_outpath_pressed()));
    connect(ui.fullscreenBtn, SIGNAL(pressed()), this, SLOT(on_fullscreen_pressed()));

    player = new QMediaPlayer(ui.gView);
    videoWidget = new QVideoWidget(ui.gView);
    audioOutput = new QAudioOutput(ui.gView);
    ui.gView->SetVideo(videoWidget);
    player->setVideoOutput(videoWidget);
    player->setAudioOutput(audioOutput);

    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(MediaPositionChanged(qint64)));
    connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(MediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(ui.mediaSlider, SIGNAL(lowerValueChanged(int)), this, SLOT(MediaDurationChanged(int)));
    connect(ui.mediaSlider, SIGNAL(currentValueChanged(int)), this, SLOT(MediaDurationChanged(int)));
    connect(ui.mediaSlider, SIGNAL(upperValueChanged(int)), this, SLOT(MediaDurationChanged(int)));

    videoWidget->resize(ui.gView->size());

    ui.stackedWidget->setCurrentIndex(1);
    ui.videostacked->setCurrentIndex(0);
    resize(450, 200);

    ffmpegPath = ".\\ffmpeg.exe";
}

void Cliptimizer::MediaDurationChanged(int duration) {
    qDebug(QString::number(duration).toLocal8Bit());
    player->setPosition(duration);
}

void Cliptimizer::MediaPositionChanged(qint64 pos) {
    ui.video_time->setText(util::MsToFormat(pos, "mm:ss"));
    ui.mediaSlider->setCurrentValue(pos);
}

void Cliptimizer::MediaStatusChanged(QMediaPlayer::MediaStatus status) {
    switch (status) {
        case QMediaPlayer::LoadedMedia:
        {
            clipData = player->metaData();
            ui.video_length->setText(util::MsToFormat(clipData[QMediaMetaData::Duration].toInt(), "mm:ss"));
            ui.mediaSlider->SetRange(0, clipData[QMediaMetaData::Duration].toInt());
            Sleep(1000);
            break;
        }
        case QMediaPlayer::EndOfMedia:
            isPlaying = !isPlaying;
            util::SetButtonIcon(ui.pauseBtn, "play");
        break;
    }
}

void Cliptimizer::ProcessStateChanged(QProcess::ProcessState newState) {
    switch (newState) {
        case QProcess::ProcessState::NotRunning:
        {
            qDebug("not running");
            ffmpeg->deleteLater();

            QLocale locale = this->locale();
            ui.size_after->setText(locale.formattedDataSize(util::GetFileSize(outclipPath),2, QLocale::DataSizeTraditionalFormat));
            ui.optimizeLevel->setVisible(true);

            ui.videostacked->setCurrentIndex(0);

            QString last = currentPath;
            PlayClip(outclipPath);
            player->pause();
            Sleep(500);
            player->play();

            if (overwriteClip)
            {
                bool res = QFile::remove(last);
            }
            if (!clips.isEmpty())
            {
                clips[currentClip] = outclipPath;
                currentPath = outclipPath;
            }
            break;
        }
        case QProcess::ProcessState::Running:
            qDebug("running");
            break;
        case QProcess::ProcessState::Starting:
            qDebug("starting");
            break;
    }
}


void Cliptimizer::on_folder_pressed() {
    QString path = QFileDialog::getExistingDirectory(this, "Choose Folder", QStandardPaths::writableLocation(QStandardPaths::MoviesLocation), QFileDialog::ShowDirsOnly);
    qDebug(path.toLocal8Bit());

    if (path.isEmpty())
        return;

    clips.clear();
    ui.nextBtn->setVisible(true);
    ui.previousBtn->setVisible(true);

    for (const auto& entry : std::filesystem::directory_iterator(path.toStdString())) {
        std::string ext = entry.path().extension().string();
        if (ext == ".mp4" || ext == ".mov" || ext == ".avi")
            clips.append(QString::fromStdString(entry.path().string()));
    }

    currentClip = 0;
    if (outputPath.isEmpty())
    {
        outputPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        ui.outfolder->setText(outputPath);
    }
    else
    {
        outputPath.clear();
    }
    isSingle = false;
    askagain = false;
    if (!isEditing)
    {
        isEditing = true;
        EditMode();
    }
    ui.optimizeLevel->setVisible(false);
    PlayClip(clips[currentClip]);
}

void Cliptimizer::on_clip_pressed()
{
    QString path = QFileDialog::getOpenFileName(this, "Choose Video", QStandardPaths::writableLocation(QStandardPaths::MoviesLocation), tr("Video Files (*.mp4 *.mov *.avi)"), nullptr);
    qDebug(path.toLocal8Bit());

    if (path.isEmpty())
        return;

    ui.nextBtn->setVisible(false);
    ui.previousBtn->setVisible(false);

    outclipPath.clear();
    isSingle = true;
    ui.outfolder->clear();
    if (!isEditing)
    {
        isEditing = true;
        EditMode();
    }
    ui.optimizeLevel->setVisible(false);

    PlayClip(path);

    ui.mediaSlider->SetCurrentValue(0);
}

void Cliptimizer::on_outpath_pressed()
{
    QString path = QFileDialog::getExistingDirectory(this, "Choose Folder", QStandardPaths::writableLocation(QStandardPaths::MoviesLocation), QFileDialog::ShowDirsOnly);
    qDebug(path.toLocal8Bit());

    if (path.isEmpty())
        return;

    outputPath = path;
    ui.outfolder->setText(outputPath);
}

void Cliptimizer::on_fullscreen_pressed()
{
   videoWidget->setFullScreen(true);
    videoWidget->resize(1920,1079);
}

void Cliptimizer::on_trim_pressed() {
    clipExt = util::GetExtention(currentPath);

    if (isSingle)
    {
        outclipPath = QFileDialog::getSaveFileName(this, "Save as", QStandardPaths::writableLocation(QStandardPaths::MoviesLocation), ".mp4") + clipExt;
        if (outclipPath == ".mp4")
        {
            return;
        }
        GenerateDialog();
    }
    else
    {
        CreateOutputPath();

        if (!askagain)
        {
            GenerateDialog();
        }
    }

    QString fps = (ui.fpsCombo->currentText() == "Default") ? clipData[QMediaMetaData::VideoFrameRate].toString() : ui.fpsCombo->currentText();
    QSize res;
    if (ui.resCombo->currentText() == "Default") {
        res = clipData[QMediaMetaData::Resolution].toSize();
    }
    else
    {
        QString resString = ui.resCombo->currentText();
        QStringList l = resString.split(" x ");
        res.scale(l[0].toInt(), l[1].toInt(), Qt::AspectRatioMode::IgnoreAspectRatio);
    }


    QStringList args;
    args << ffmpegPath << "-i" << "\"" + currentPath + "\"" << "-filter:v \"fps=fps=" + fps << ",scale=w=" + STR(res.width()) + ":h = " + STR(res.height()) + "\""
        << ((ui.optimizeSlider->value()>0)?"-crf " + STR(ui.optimizeSlider->value()):"") + " -c:v libx264" << "-ss" << util::MsToFormat(ui.mediaSlider->GetLowerValue(), "hh:mm:ss")
        << "-to" << util::MsToFormat(ui.mediaSlider->GetUpperValue(), "hh:mm:ss")
        << "\"" + outclipPath + "\"";

    QString command = args.join(" ");

    StartFFMPEG(command);


    QMovie* mov = new QMovie(":/Cliptimizer/resources/loading.gif");
    //mov->setScaledSize(QSize(100, 100));
    ui.loadinglabel->setMovie(mov);
    ui.videostacked->setCurrentIndex(1);
    mov->start();

    player->pause();


    QLocale locale = this->locale();
    ui.size_before->setText(locale.formattedDataSize(util::GetFileSize(currentPath),2 ,QLocale::DataSizeTraditionalFormat));

}

void Cliptimizer::on_next_pressed() {
    if (currentClip == clips.size()-1)
        return;
    qDebug("next");
    currentClip++;
    outclipPath.clear();
    ui.optimizeLevel->setVisible(false);
    try {
        PlayClip(clips[currentClip]);
    }
    catch (...) {

    }
}

void Cliptimizer::on_previous_pressed(){
    if (currentClip == 0)
        return;
    currentClip--;
    outclipPath.clear();
    ui.optimizeLevel->setVisible(false);
    try {
        PlayClip(clips[currentClip]);
    }
    catch (...) {

    }
}

void Cliptimizer::on_pause_pressed() {
    isPlaying = !isPlaying;
    if (isPlaying) 
    {
        util::SetButtonIcon(ui.pauseBtn, "pause");
        player->play();
    }
    else
    {
        util::SetButtonIcon(ui.pauseBtn, "play");
        player->pause();
    }

}


bool Cliptimizer::PlayClip(QString path) {
    currentPath = path;
    qDebug(QString::number(currentClip).toLocal8Bit());
    isPlaying = true;

    std::filesystem::path fPath = path.toStdString();
    clipTitle = QString::fromStdString(fPath.stem().string());
    ui.clipTitle->setText(clipTitle);
    QFontMetrics fm = ui.clipTitle->fontMetrics();
    int width = fm.boundingRect(QString::fromStdString(fPath.stem().string())).width();
    ui.clipTitle->setFixedWidth(((width >= 200)?width:200) + 6);
    ui.clipTitle->setVisible(true);

    util::SetButtonIcon(ui.pauseBtn, "pause");
    Sleep(50);
    if (!QFileInfo(currentPath).exists())
    {
        Dialog* diag = new Dialog();
        diag->SetMessage("Couldn't find the clip, do you want to relocate?");
        diag->SetCheckBoxEnabled(false);

        connect(diag, &Dialog::first_btn_pressed, [=](bool state)
            {
                currentPath = QFileDialog::getOpenFileName(this, "Choose Video", QStandardPaths::writableLocation(QStandardPaths::MoviesLocation), tr("Video Files (*.mp4 *.mov *.avi)"), nullptr);

                if (currentPath == ".mp4")
                {
                    return false;
                }
                clips[currentClip] = currentPath;
            });
        connect(diag, &Dialog::second_btn_pressed, [=](bool state)
            {
                return false;
            });

        diag->exec();

    }

    player->setSource(QUrl::fromLocalFile(currentPath));
    videoWidget->setVisible(true);

    player->play();
    return true;
}

void Cliptimizer::EditMode() {
    Sleep(500);

    ui.optimizeLevel->setVisible(false);
    ui.stackedWidget->setCurrentIndex(0);
    setMinimumSize(800, 400);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    resize(800, 500);
}

void Cliptimizer::StartFFMPEG(QString command)
{
    ffmpeg = new QProcess(this);
    connect(ffmpeg, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(ProcessStateChanged(QProcess::ProcessState)));

    ffmpeg->startCommand(command);
}

void Cliptimizer::CreateOutputPath()
{
    if (ui.clipTitle->text() == clipTitle)
    {
        outclipPath = outputPath + "/" + ui.clipTitle->text() + "-cliptimized" + clipExt;
    }
    else
    {
        outclipPath = outputPath + "/" + ui.clipTitle->text() + clipExt;
    }
}

bool Cliptimizer::GenerateDialog()
{
    Dialog* diag = new Dialog();
    diag->SetMessage("Do you want to overwrite the source clip?");

    connect(diag, &Dialog::first_btn_pressed, [=](bool state) {
        askagain = state;
        overwriteClip = true;
        });
    connect(diag, &Dialog::second_btn_pressed, [=](bool state) {
        askagain = state;
        overwriteClip = false;
        });

    diag->exec();
    return overwriteClip;
}

Cliptimizer::~Cliptimizer()
{
}