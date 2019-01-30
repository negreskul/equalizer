#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QTime>
#include <QFileInfo>
#include "Wave.h"
#include "pitcher.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->SpeedSlider_2->setDisabled(true);
    ui->ToneSlider->setDisabled(true);
    ui->trackSlider->setDisabled(true);
    ui->PauseBtn->setDisabled(true);
    ui->PlayBtn->setDisabled(true);
    ui->StopBtn->setDisabled(true);
    ui->ResetBtn->setDisabled(true);
    ui->WaitCalcLabel->hide();

    m_player = new QMediaPlayer(this);          // Инициализируем плеер
    m_playlist = new QMediaPlaylist(m_player);  // Инициализируем плейлист
    m_player->setPlaylist(m_playlist);          // Устанавливаем плейлист в плеер
    m_player->setVolume(this->soundVolume);     // Устанавливаем громкость воспроизведения треков
    connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    connect(ui->PauseBtn, &QToolButton::clicked, m_player, &QMediaPlayer::pause);
    connect(ui->trackSlider, &QSlider::sliderMoved, this, &MainWindow::seek);

    ui->ToneShifterQualityBox->addItem("Низкое", QVariant(1));
    ui->ToneShifterQualityBox->addItem("Среднее", QVariant(2));
    ui->ToneShifterQualityBox->addItem("Высокое", QVariant(3));
    this->restoreSlidersValues(true);
    this->setUpWorkspace();
}

void MainWindow::setUpWorkspace()
{
    QDir dir("./tmp_cache");
    qDebug() << dir.absolutePath() << " || " << endl;
    if (dir.exists())
    {
        dir.removeRecursively();
    }
    dir.mkpath(".");
}

void MainWindow::durationChanged(qint64 duration)
{
    m_duration = duration / 1000;
    ui->trackSlider->setMaximum(m_duration);
}

void MainWindow::positionChanged(qint64 progress)
{
//    if (!ui->trackSlider->isSliderDown())
        ui->trackSlider->setValue(progress / 1000);

    updateDurationInfo(progress / 1000);
    qDebug() << "tick: " << QString::number(progress) << " | " << QString::number(m_duration) << endl;
}

void MainWindow::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || m_duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
            currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60,
            m_duration % 60, (m_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    ui->labelDuration->setText(tStr);
}

void MainWindow::seek(int seconds)
{
    m_player->setPosition(seconds * 1000);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_playListModel;
    delete m_playlist;
    delete m_player;
}

void MainWindow::disableBtns()
{
    ui->PauseBtn->setDisabled(true);
    ui->PlayBtn->setDisabled(true);
    ui->StopBtn->setDisabled(true);
    ui->ResetBtn->setDisabled(true);
}

void MainWindow::enableBtns()
{
    ui->PauseBtn->setEnabled(true);
    ui->PlayBtn->setEnabled(true);
    ui->ResetBtn->setEnabled(true);
}

void MainWindow::restoreSlidersValues(bool defaultMode = false)
{
    if(defaultMode)
    {
        this->speedQtPlayerValue = 10; this->toneValue = 10; this->soundVolume = 100; this->toneShifterQualityPreset = 2; // this->speedValue = 10;
    }
    ui->ToneSlider->setValue(this->toneValue); ui->SpeedSlider_2->setValue(this->speedQtPlayerValue); ui->dial->setValue(this->soundVolume);
    /*----*/
    ui->ToneValueLabel->setText(QString::number(this->toneValue)); ui->SpeedValueLabel_2->setText(QString::number(this->speedQtPlayerValue));
    ui->lcdNumber->display(this->soundVolume);
    ui->ToneShifterQualityBox->setCurrentIndex(this->toneShifterQualityPreset - 1);
    /*----*/
    m_player->setPlaybackRate((double)this->speedQtPlayerValue / (double)10);
    m_player->setVolume(this->soundVolume);
}

void MainWindow::playerPositionChanged(qint64 pos) {
    qDebug() << "2tick: " << QString::number(pos) << endl;
    ui->trackSlider->setValue(pos);
}

void MainWindow::on_OpenFileButton_clicked()
{
    this->fileName = QFileDialog::getOpenFileName(nullptr, "Выберите файл WAV", "", "*.wav");
    if(this->fileName.length() == 0) {
        return;
    }
    ui->FilePathEdit->setText(this->fileName);
    this->playerPos = 0;
    ui->ToneValueLabel->setText("0");

    m_playlist->removeMedia(0);
    m_playlist->addMedia(QUrl(fileName));

    this->enableBtns();
    ui->SpeedSlider_2->setDisabled(true);
    ui->ToneSlider->setDisabled(true);
    ui->trackSlider->setEnabled(true);

    ui->StopBtn->setDisabled(true);


    this->restoreSlidersValues(true);
}

void MainWindow::on_PlayButton_clicked()
{
    if (!this->isModified) {
       this->sound->play();
       durationChanged(m_duration);
    }
    else {}
}

void MainWindow::on_SpeedSlider_sliderMoved(int position)
{
    this->isModified=true;
    this->speedValue=position;
}

void MainWindow::on_dial_sliderMoved(int position)
{
    this->soundVolume = position;
    ui->lcdNumber->display(position);
    m_player->setVolume(position);
}

void MainWindow::on_dial_valueChanged(int position)
{
    this->soundVolume = position;
    ui->lcdNumber->display(position);
    m_player->setVolume(position);
}

void MainWindow::on_oldPauseButton_clicked()
{

}

void MainWindow::on_trackSlider_valueChanged(int value)
{

}

void MainWindow::on_SpeedSlider_2_sliderMoved(int position)
{
    this->isModified=true;
    this->speedQtPlayerValue=position;
    ui->SpeedValueLabel_2->setText(QString::number(position));
    qDebug() << "rate: " << QString::number(position) << " | " << QString::number((double)position/(double)10) << endl;
    m_player->setPlaybackRate((double)position/(double)10);
}


void MainWindow::on_SpeedSlider_2_valueChanged(int value)
{
    this->isModified=true;
    this->speedQtPlayerValue=value;
    ui->SpeedValueLabel_2->setText(QString::number(value));
    qDebug() << "rate: " << QString::number(value) << " | " << QString::number((double)value/(double)10) << endl;
    m_player->setPlaybackRate((double)value/(double)10);
}

void MainWindow::on_PlayBtn_clicked()
{
    ui->SpeedSlider_2->setDisabled(true);
    ui->ToneSlider->setDisabled(true);
    ui->ResetBtn->setDisabled(true);
    this->m_player->play();
    this->m_player->setPosition(this->playerPos);
    ui->trackSlider->setValue(this->playerPos / 1000);
    qint64 songLength = this->m_player->duration();
//    ui->trackSlider->setRange(0, songLength);
    qDebug() << QString::number(songLength) << endl;
    ui->PlayBtn->setDisabled(true);
    ui->PauseBtn->setEnabled(true);
    ui->StopBtn->setEnabled(true);
}

void MainWindow::on_PauseBtn_clicked()
{
    ui->SpeedSlider_2->setEnabled(true);
    ui->ToneSlider->setEnabled(true);
    ui->ResetBtn->setEnabled(true);
//    this->m_player->pause();
    ui->PauseBtn->setDisabled(true);
    ui->PlayBtn->setEnabled(true);
    this->playerPos = this->m_player->position();
}

void MainWindow::on_ResetBtn_clicked()
{
    this->restoreSlidersValues(true);
}

void MainWindow::on_ToneSlider_valueChanged(int position)
{
    ui->WaitCalcLabel->show();
    this->isModified=true;
    this->toneValue=position;
    ui->ToneValueLabel->setText(QString::number(position));
}


void MainWindow::on_ToneSlider_sliderMoved(int position)
{
    this->isModified=true;
    this->toneValue=position;
    ui->ToneValueLabel->setText(QString::number(position));
    ui->WaitCalcLabel->show();
}

void MainWindow::on_ToneSlider_sliderPressed()
{
    ui->WaitCalcLabel->show();
}

void MainWindow::on_ToneSlider_sliderReleased()
{
    ui->WaitCalcLabel->show();
    m_player->stop();
    m_playlist->removeMedia(0);
    if(this->toneValue == 10)
    {
        m_playlist->addMedia(QUrl(this->fileName));
        ui->WaitCalcLabel->hide();
        return;
    }
    QDir absPath ("."); //адрес текущей папки (где находится программа)
    QFileInfo fileLink(this->fileName); // адрес исходного файла

    // номер выбранного качества преобраз. тона (от 1 по 3)
    int presetSelected = ui->ToneShifterQualityBox->itemData(ui->ToneShifterQualityBox->currentIndex()).toInt();
    // полный адрес измененного файла
    QString modifiedFilePath = absPath.absolutePath() + "/tmp_cache/" + QString::number(this->toneValue) + "tone_" + QString::number(presetSelected) + "preset_"  + fileLink.fileName();

    if(!QFileInfo::exists(modifiedFilePath) && !QFileInfo(modifiedFilePath).isFile()) //проверка на наличие закешированного файла
    {
        float toneValueConverted = this->toneValue / 10.0;

        // Загрузка исходного файла для будущего изменения тональности
        wav = new Wave(this->fileName.toLocal8Bit().constData());
        qDebug() << QString::number(wav->dataSource.size()) << "Loaded file: " << QString::fromStdString(wav->getFilename()) << endl;

        //Создание копии wav->dataSource для pitcher-а.
        float* dataf = new float[wav->dataSource.size()];

        //Преобразование из double в float
        for (std::size_t i = 0; i < wav->dataSource.size(); ++i)
        {
            float sample = static_cast<float>(wav->dataSource[i]);
            dataf[i] = sample;
        }

        unsigned int channelSize = wav->getWaveSize() / wav->getBytesPerSample();
        int fftFrameSize = this->pitchShiftPresets[presetSelected-1][0],
            osamp = this->pitchShiftPresets[presetSelected-1][1];

        qDebug() << "fftsamp & osamp" << QString::number(fftFrameSize) << " " << QString::number(osamp) << endl;

//        int presetSelected = ui->ToneShifterQualityBox->itemData(ui->ToneShifterQualityBox->currentIndex()).toInt() - 1; //this->pitchShiftPresets
        smbPitchShift(toneValueConverted, channelSize, fftFrameSize, osamp, wav->header.SampFreq, dataf, dataf, wav->header.Channels);
        qDebug() << modifiedFilePath.toLocal8Bit().constData() << " |-| " << modifiedFilePath  << endl;

        wav->savefft(modifiedFilePath.toLocal8Bit().constData(), dataf);
    }

    m_playlist->addMedia(QUrl(modifiedFilePath.toLocal8Bit()));
    m_player->setPlaybackRate((double)this->speedQtPlayerValue / (double)10);

    ui->WaitCalcLabel->hide();
}

void MainWindow::on_StopBtn_clicked()
{
    this->m_player->stop();
    ui->SpeedSlider_2->setEnabled(true);
    ui->ToneSlider->setEnabled(true);
    ui->ResetBtn->setEnabled(true);
//    this->m_player->pause();
    ui->PauseBtn->setDisabled(true);
    ui->StopBtn->setDisabled(true);
    ui->PlayBtn->setEnabled(true);
    this->playerPos = 0;
}

void MainWindow::on_pushButton_clicked()
{
    QDir absPath ("."); //адрес текущей папки (где находится программа)
    QFileInfo fileLink(this->fileName); // адрес исходного файла

    // номер выбранного качества преобраз. тона (от 1 по 3)
    int presetSelected = ui->ToneShifterQualityBox->itemData(ui->ToneShifterQualityBox->currentIndex()).toInt();
    // полный адрес измененного файла
    QString modifiedFilePath = absPath.absolutePath() + "/tmp_cache/NOCHANGE" + QString::number(this->toneValue) + "tone_" + QString::number(presetSelected) + "preset_"  + fileLink.fileName();

    wav = new Wave(this->fileName.toLocal8Bit().constData());
    qDebug() << QString::number(wav->dataSource.size()) << "Loaded file: " << QString::fromStdString(wav->getFilename()) << endl;

    //Создание копии wav->dataSource для pitcher-а.
    float* dataf = new float[wav->dataSource.size()];

    //Преобразование из double в float
    for (std::size_t i = 0; i < wav->dataSource.size(); ++i)
    {
        float sample = static_cast<float>(wav->dataSource[i] + 80);
        dataf[i] = sample;
    }

    unsigned int channelSize = wav->getWaveSize() / wav->getBytesPerSample();
    int fftFrameSize = this->pitchShiftPresets[presetSelected-1][0],
        osamp = this->pitchShiftPresets[presetSelected-1][1];

    qDebug() << "fftsamp & osamp" << QString::number(fftFrameSize) << " " << QString::number(osamp) << endl;
    qDebug() << modifiedFilePath.toLocal8Bit().constData() << " |-| " << modifiedFilePath  << endl;

    wav->savefft(modifiedFilePath.toLocal8Bit().constData(), dataf);
}
