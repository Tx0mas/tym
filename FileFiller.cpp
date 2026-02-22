#include"Editor.hpp"

void Editor::fillBufferWithFile()
{
    buffer.clear();
    file.open(fileName,std::ios::in | std::ios::out | std::ios::binary);
    if (!file)
    {
        buffer.push_back("File mal cargada");
    }
    else
    {
        while (std::getline(file,linea))
        {
            buffer.push_back(linea);
        }
        if (buffer.empty())
        {
            buffer.push_back("");
        }
    }
    x_actual = 0;
    y_actual = 0;
}

