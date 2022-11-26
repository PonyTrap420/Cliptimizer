#pragma once

#include "stdafx.h"
#include "ui_cliptimizer.h"

class Cliptimizer : public QMainWindow
{
    Q_OBJECT

public:
    Cliptimizer(QWidget *parent = nullptr);
    ~Cliptimizer();

public slots:
    void MediaStatusChanged(QMediaPlayer::MediaStatus status);
    void MediaPositionChanged(qint64 pos);
    void MediaDurationChanged(int duration);

    void ProcessStateChanged(QProcess::ProcessState newState);

private slots:
    void on_folder_pressed();
    void on_clip_pressed();
    void on_delete_pressed();

    void on_trim_pressed();

    void on_next_pressed();
    void on_previous_pressed();

    void on_outpath_pressed();
    void on_pause_pressed();
    void on_fullscreen_pressed();

private:
    Ui::CliptimizerClass ui;

    QProcess* ffmpeg;
    QString ffmpegPath;

    QMediaPlayer* player;
    QVideoWidget* videoWidget;
    QAudioOutput* audioOutput;

    QMediaMetaData clipData;

    bool isEditing = false;
    bool isPlaying = false;
    bool isSingle = false;
    QStringList clips;
    int currentClip;

    bool askagain;
    bool overwriteClip;
    QString currentPath;
    QString outputPath;
    QString outclipPath;
    QString clipTitle;
    QString clipExt;

    bool PlayClip(QString path);
    void StartFFMPEG(QString command);
    void EditMode();
    void CreateOutputPath();
    bool GenerateDialog();
};
