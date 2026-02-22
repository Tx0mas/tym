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

    fs::path tempPath(fileName);
    if (tempPath.has_parent_path())
    {
        actualFilePath = tempPath.parent_path();
    }
    else
    {
        actualFilePath = fs::current_path();
    }

    x_actual = 0;
    y_actual = 0;
}

