#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSound>
#include <QStandardItemModel>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include "Wave.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void disableBtns(); void enableBtns();
    void restoreSlidersValues(bool defaultMode);
    void setUpWorkspace();

private slots:


    void on_OpenFileButton_clicked();

    void on_PlayButton_clicked();

    void on_ToneSlider_sliderMoved(int position);

    void on_SpeedSlider_sliderMoved(int position);

    void on_dial_valueChanged(int value);

    void on_dial_sliderMoved(int position);

    void on_oldPauseButton_clicked();

    void on_trackSlider_valueChanged(int value);

    void on_SpeedSlider_2_sliderMoved(int position);

    void on_PlayBtn_clicked();

    void on_PauseBtn_clicked();

    void playerPositionChanged (qint64 pos);

    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);

    void updateDurationInfo(qint64 currentInfo);
    void seek(int seconds);

    void on_ResetBtn_clicked();

    void on_SpeedSlider_2_valueChanged(int value);

    void on_ToneSlider_valueChanged(int value);

    void on_ToneSlider_sliderPressed();

    void on_ToneSlider_sliderReleased();

    void on_StopBtn_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QString fileName;
    int toneValue = 10, speedValue = 0, speedQtPlayerValue = 10, soundVolume=100, toneShifterQualityPreset = 2, /*medium quality*/
        playerPos = 0;
    bool isModified = false, isLocked = true, isPlaying = false, isFileLoaded = false ;
    QSound* sound;
    Wave* wav;
    int pitchShiftPresets[3][2] = {{2048, 4}, {512, 8}, {256, 16}}; // {размер окна, коэф.наложения }

    QStandardItemModel  *m_playListModel;   // Модель данных плейлиста для отображения
    QMediaPlayer        *m_player;          // Проигрыватель треков
    QMediaPlaylist      *m_playlist;        // Плейлиста проигрывателя

    qint64 m_duration, m_position;
};

#endif // MAINWINDOW_H
