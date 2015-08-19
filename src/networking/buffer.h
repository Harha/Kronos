#ifndef BUFFER_H
#define BUFFER_H

// std includes
#include <cstdint>

namespace kronos
{

class Buffer
{
public:
    Buffer(uint32_t size = 4096)
    {
        m_data = new unsigned char[size];
        m_data_start = m_data;
        m_data_end = m_data + size;
    }

    ~Buffer()
    {
        m_data = m_data_start;

        if (m_data)
            delete[] m_data;
    }

    unsigned char read_byte()
    {
        return *m_data++;
    }

    unsigned short read_short()
    {
        m_data += 2;
        return *(m_data - 2) << 8 | *(m_data - 1);
    }

    unsigned short read_le_short()
    {
        m_data += 2;
        return *(m_data - 2) | *(m_data - 1) << 8;
    }

    unsigned int read_int()
    {
        m_data += 4;
        return *(m_data - 4) << 24 | *(m_data - 3) << 16 | *(m_data - 2) << 8 | *(m_data - 1);
    }

    int64_t read_long()
    {
        m_data += 8;
        return (int64_t)*(m_data - 8) << 56 | (int64_t)*(m_data - 7) << 48 | (int64_t)*(m_data - 6) << 40 | (int64_t)*(m_data - 5) << 32 | *(m_data - 4) << 24 | *(m_data - 3) << 16 | *(m_data - 2) << 8 | *(m_data - 1);
    }

    unsigned char read_byte_a()
    {
        return *m_data++ - 128;
    }

    unsigned char read_byte_s()
    {
        return *m_data++ + 128;
    }

    unsigned int read_smart_word()
    {
        if(*m_data < 128)
            return *m_data++;

        m_data += 2;
        return (*(m_data - 2) << 8) + *(m_data - 1) - 32768;
    }

    signed char read_signed_byte()
    {
        return (signed char)*m_data++;
    }

    signed int read_signed_short()
    {
        signed int u = *m_data++ << 8 | *m_data++;
        if(u >= 32768)
            u -= 0x10000;
        return u;
    }

    char* read_string()
    {
        int count = 0;
        while(*(m_data + count) != 10 && count++ < 256);
        if(count == 0)
            return "";
        char* str = new char[count + 1];
        for(int i = 0; i <= count; i++)
            str[i] = *m_data++;
        str[count] = 0;
        return str;
    }

    void write_byte(unsigned char c)
    {
        m_data++;
        *(m_data - 1) = c;
    }

    void write_word(unsigned short s)
    {
        *m_data++ = (s >> 8) & 0xff;
        *m_data++ = s & 0xff;
    }

    void write_int(unsigned int i)
    {
        *m_data++ = (i >> 24) & 0xff;
        *m_data++ = (i >> 16) & 0xff;
        *m_data++ = (i >> 8) & 0xff;
        *m_data++ = i & 0xff;
    }

    void write_long(int64_t l)
    {
        *m_data++ = (l >> 56) & 0xff;
        *m_data++ = (l >> 48) & 0xff;
        *m_data++ = (l >> 40) & 0xff;
        *m_data++ = (l >> 32) & 0xff;
        *m_data++ = (l >> 24) & 0xff;
        *m_data++ = (l >> 16) & 0xff;
        *m_data++ = (l >> 8) & 0xff;
        *m_data++ = l & 0xff;
    }

    unsigned char * const getData() const
    {
        return m_data;
    }

    unsigned char * const getDataStart() const
    {
        return m_data_start;
    }

    unsigned char * const getDataEnd() const
    {
        return m_data_end;
    }

private:
    unsigned char *m_data;
    unsigned char *m_data_start;
    unsigned char *m_data_end;
};

}

#endif // BUFFER_H
