#pragma once

class InputBuffer
{
    public:
        size_t bufsiz;
        size_t buflen;
        char* buffer;
        int cursor;

    virtual void clear_buffer()
    {
        buffer[0] = '\0';
        buflen = 0;
        cursor = 0;
    }

    void add(char x)
    {
        int n_chars = 1;
        if (x == '\t')
        {
            n_chars = 4;
            x = ' ';
        }

        for (int i=0; i<n_chars; i++)
        {
            if (cursor >= int(bufsiz)) break;
            if (buflen >= bufsiz) break;

           for (int j=int(buflen); j>=cursor; j--)
                buffer[j+1] = buffer[j];

            buffer[cursor++] = x;
            buffer[++buflen] = '\0';
        }
        buffer[buflen] = '\0';
    }

    void remove()
    {
        if (!buflen || !cursor) return;
        int i = cursor;
        char c = '\0';
        while ((c = buffer[i++]) != '\0')
            buffer[i-2] = c;
        buffer[--buflen] = '\0';
        cursor--;
    }

    void cursor_left()
    {
        cursor--;
        cursor = GS_MAX(cursor, 0);
    }

    void cursor_right()
    {
        cursor++;
        cursor = GS_MIN(cursor, int(buflen));
    }

    explicit InputBuffer(size_t bufsiz) :
        bufsiz(bufsiz), buflen(0), cursor(0)
    {
        buffer = (char*)calloc(bufsiz+1, sizeof(char));
        buffer[bufsiz] = '\0';
        buffer[0] = '\0';
    }

    virtual ~InputBuffer()
    {
        free(buffer);
    }
};
