#define _CRT_SECURE_NO_DEPRECATE
#ifndef WAVE_H
#define WAVE_H
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>


/* .wav file header structure. */
struct WaveHeader
{
    char   RIFF[4];
    std::uint32_t DataLength;
    char   WAVE[4];
    char   fmt_[4];
    std::uint32_t SubBlockLength;
    std::uint16_t formatTag;
    std::uint16_t Channels;
    std::uint32_t SampFreq;
    std::uint32_t BytesPerSec;
    std::uint16_t BytesPerSamp;
    std::uint16_t BitsPerSamp;
    char   data[4];
    std::uint32_t WaveSize;
};

class Wave
{
public:
    Wave(const std::string& filename); //Конструктор класса Wave
    ~Wave();

    void load(const std::string& filename); //Загрузка файла

    void save(const std::string& newfilename); //Сохранение копии без изменений
    void savefft(const std::string& newfilename, float* dataf); //Сохранение измененной версии

    static void splitBytes(short twoBytes, unsigned char & lb, unsigned char & hb); // Разделение 2-х байтов
    static void decode16bit(std::vector<double>& channel, short* data, std::size_t channelSize); //Декодирование 1битный сигнал
    static void decode8bit(std::vector<double>& channel, short* data, std::size_t channelSize); //Декодирование 8-битный сигнал
    static void encode16bit(const std::vector<double>& source, short* data, std::size_t dataSize); //Кодирование 16-битный сигнал
    static void encode8bit(const std::vector<double>& source, short* data, std::size_t dataSize); //Кодирование 8-битный сигнал

    unsigned int getWaveSize() const // Длина аудиосигнала в байтах
    {
        return header.WaveSize;
    }

    virtual unsigned short getBitsPerSample() const //кол-во бит в секунду
    {
        return header.BitsPerSamp;
    }

    unsigned int getBytesPerSample() const //кол-во байтов на один сэмпл
    {
        return header.BytesPerSamp;
    }

    unsigned int getBytesPerSec() const //кол-во байтов в секунду
    {
        return header.BytesPerSec;
    }

    bool isStereo() const //проверка на Стерео
    {
        return 2 == getChannelsNum();
    }

    bool isMono() const //проверка на моно
    {
        return 1 == getChannelsNum();
    }

    unsigned short getChannelsNum() const //кол-во каналов
    {
        return header.Channels;
    }

    std::string getFilename() const //имя файла
    {
        return filepath;
    }

    static double sampleAt(std::vector<double> dataSource, std::size_t pos) // Просто возвращает сэмпл в нужном месте
    {
        return dataSource[pos];
    }

//protected:
    std::vector<double> dataSource; // массив с звуковым сигналом (с семплами)
    WaveHeader header; //экземпляр хедера
private:
    std::string filepath; //путь к файлу
};
#endif
