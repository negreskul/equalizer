#define _CRT_SECURE_NO_DEPRECATE
#include "Wave.h"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <QDebug>

Wave::Wave(const std::string& filename):
    filepath(filename)
{
    load(filename);
}

Wave::~Wave()
{
}

//Метод преобразования 16-битного (2байтового) числа в пару 8-битных (1-байтовых)
void Wave::splitBytes(short twoBytes, unsigned char & lb, unsigned char & hb)
{
    lb = twoBytes & 0x00FF; //умножаем исходное число на 255
    hb = (twoBytes >> 8) & 0x00FF; // смещаем исходное число на 8 бит ВПРАВО и умножаем на 255
}


void Wave::decode16bit(std::vector<double>& channel, short* data, std::size_t channelSize)
{
    for (std::size_t i = 0; i < channelSize; ++i)
    {
        channel[i] = data[i];
    }
}

//Метод разбиения 2байтовых ячеек массива в 1байтовые
void Wave::decode8bit(std::vector<double>& channel, short* data, std::size_t channelSize)
{
    // low byte and high byte of a 16b word
    unsigned char lb, hb; //lb - левый канал, hb - правый канал
    qDebug() << "channelSize " << channelSize << endl;

    for (std::size_t i = 0; i < channelSize; ++i)
    {
        splitBytes(data[i / 2], lb, hb);
        channel[i] = lb - 128;
    }
    qDebug() << channel[0] << " --- " << channel[1] << endl;
}

void Wave::encode8bit(const std::vector<double>& source, short* data, std::size_t dataSize)
{
    for (std::size_t i = 0; i < dataSize; ++i)
    {
        unsigned char sample1 = static_cast<unsigned char>(sampleAt(source, 2 * i) + 128);
        unsigned char sample2 = static_cast<unsigned char>(sampleAt(source, 2 * i + 1) + 128);
        short hb = sample1, lb = sample2;
        data[i] = ((hb << 8) & 0xFF00) | (lb & 0x00FF);
    }
}

void Wave::encode16bit(const std::vector<double>& source, short* data, std::size_t dataSize)
{
    for (std::size_t i = 0; i < dataSize; ++i)
    {
        short sample = static_cast<short>(sampleAt(source, i));
        data[i] = sample;
    }
}

//Загрузка файла в оперативную память
void Wave::load(const std::string& filename)
{
    this->filepath = filename; // сохранение пути к файлу
    dataSource.clear(); //Затирание массива с сигналом

    std::fstream fs; //Инициализируем файловый поток
    fs.open(this->filepath.c_str(), std::ios::in | std::ios::binary); //Открываем на считывание в бинарном режиме

    // char - тип данных для хранения 1 байта.
    fs.read((char*)(&header), sizeof(WaveHeader)); // Считывание по адресу в header 44-х байтов
    if (header.SubBlockLength == 18) // Если хедер нестандартный, то вычислим WaveSize вручную
    {
        header.WaveSize = header.DataLength - 46 + 8; // 46 => size of non-standart WAVE, 8 => sizeof("RIFF") + sizeof(DataLength)
    }
    short* data = new short[header.WaveSize / 2]; //Создаем временный массив (буфер) для хранения сигнала с размером "ячеек" 2 байта (16 бит).
    // Т.к. WaveSize измеряется в байтах, а тип "short"  занимает 2байта, то и размер массива data = WaveSize / 2
    fs.read((char*)data, header.WaveSize); //считывание побайтово в массив data. В каждую ячейку data[] записывается 2 байта.
    fs.close(); //Закрытие файлового потока

    if (header.Channels == 2 && header.BitsPerSamp == 16) { //Если файл содержит 2 канала (стерео), то преобразуем сигнал в МОНО
        header.WaveSize /= 2; // totalsize = leftchannelsize + rightchannelsize; ...
        header.Channels = 1; // Явно указываем в хедере МОНО режим
        header.BytesPerSamp /= 2;
        header.DataLength = header.WaveSize + sizeof (WaveHeader) - 8; //Пересчет DataLength
        for(int i = 0, j = 0; i < header.WaveSize / 2; ++i, j+=2) //Объединение дорожек (т.е. преобразуем Стерео в Моно)
        {
            // байты в WAV идут в такой последовательности(L => Left Channel Sample, R=> Right Channel Sample): [L R L R L R L ...], поэтому здесь такой цикл.
            //Результат: среднее арифм. по 2м каналам
            data[i] = (data[j] + data[j+1]) / 2; // 0.5 * (left_channel + right_channel)
        }
    }

    unsigned int channelSize = header.WaveSize / header.BytesPerSamp; // вычисление размера канала
    dataSource.resize(channelSize); // Ресайз массива со звуковыым сигналом


    if (16 == header.BitsPerSamp)
    {
        decode16bit(dataSource, data, channelSize); // Если глубина звука - 16 бит, то копируем как есть.
    }
    else
    {
        decode8bit(dataSource, data, channelSize); //Необходимо преобразовать 1-байтовые данные в 2х-байтовые
    }

    // Удаляем буфер
    delete [] data;

}

// Кеширование
void Wave::savefft(const std::string& newfilename, float* dataf)
{
    WaveHeader newheader;

    std::uint32_t frequency = static_cast<std::uint32_t>(header.SampFreq);
    // saving only mono files at the moment
    std::uint16_t channels = 1;
    std::uint16_t bitsPerSample = header.BitsPerSamp;
    // higher dynamic sources will be converted down to 16 bits per sample
    if (bitsPerSample > 16) {
        bitsPerSample = 16;
    }
    std::uint32_t bytesPerSec = frequency * channels * bitsPerSample / 8;
    std::uint32_t waveSize = dataSource.size() * channels * bitsPerSample / 8;

    strncpy(newheader.RIFF, "RIFF", 4);
    // DataLength is the file size excluding first two header fields -
    // - RIFF and DataLength itself, which together take 8 bytes to store
    newheader.DataLength = waveSize + sizeof(WaveHeader) - 8;
    strncpy(newheader.WAVE, "WAVE", 4);
    strncpy(newheader.fmt_, "fmt ", 4);
    newheader.SubBlockLength = 16;
    newheader.formatTag = 1;
    newheader.Channels = channels;
    newheader.SampFreq = frequency;
    newheader.BytesPerSec = bytesPerSec;
    newheader.BytesPerSamp = newheader.Channels * bitsPerSample / 8;
    newheader.BitsPerSamp = bitsPerSample;
    strncpy(newheader.data, "data", 4);
    newheader.WaveSize = waveSize;

    std::ofstream fs;
    fs.open(newfilename.c_str(), std::ios::out | std::ios::binary);
    fs.write((const char*)(&newheader), sizeof(WaveHeader));

    std::size_t _waveSize = newheader.WaveSize;
    short* data = new short[_waveSize / 2];

    if(bitsPerSample == 8) {
        for (std::size_t i = 0; i < _waveSize / 2; ++i)
        {
            unsigned char sample1 = static_cast<unsigned char>(dataf[2 * i] + 128);
            unsigned char sample2 = static_cast<unsigned char>(dataf[2 * i + 1] + 128);
            short hb = sample1, lb = sample2;
            data[i] = ((hb << 8) & 0xFF00) | (lb & 0x00FF);
        }
    }
    else
    {
        for (std::size_t i = 0; i < _waveSize / 2; ++i)
        {
            short sample = static_cast<short>(dataf[i]);
            data[i] = sample;
        }
    }

    fs.write((char*)data, _waveSize);
    header = newheader;
    fs.close();
}

void Wave::save(const std::string& newfilename)
{
    WaveHeader newheader;

    std::uint32_t frequency = static_cast<std::uint32_t>(header.SampFreq);

    std::uint16_t channels = 1;
    header.BitsPerSamp = 16;
    std::uint16_t bitsPerSample = header.BitsPerSamp;

    if (bitsPerSample > 16)
        bitsPerSample = 16;
    std::uint32_t bytesPerSec = frequency * channels * bitsPerSample / 8;
    std::uint32_t waveSize = dataSource.size() * channels * bitsPerSample / 8;

    strncpy(newheader.RIFF, "RIFF", 4);
    newheader.DataLength = waveSize + sizeof(WaveHeader) - 8;
    strncpy(newheader.WAVE, "WAVE", 4); // копирование в массив char
    strncpy(newheader.fmt_, "fmt ", 4);
    newheader.SubBlockLength = 16;
    newheader.formatTag = 1;
    newheader.Channels = channels;
    newheader.SampFreq = frequency;
    newheader.BytesPerSec = bytesPerSec;
    newheader.BytesPerSamp = newheader.Channels * bitsPerSample / 8;
    newheader.BitsPerSamp = bitsPerSample;
    strncpy(newheader.data, "data", 4);
    newheader.WaveSize = waveSize;

    std::ofstream fs;
    fs.open(newfilename.c_str(), std::ios::out | std::ios::binary);
    fs.write((const char*)(&newheader), sizeof(WaveHeader));

    std::size_t _waveSize = newheader.WaveSize;
    short* data = new short[_waveSize / 2];

    for (std::size_t i = 0; i < _waveSize / 2; ++i)
    {
        short sample = static_cast<short>(dataSource[i]);
        data[i] = sample;
    }
    fs.write((char*)data, _waveSize);
    header = newheader;
    fs.close();
}
