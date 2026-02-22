#include"Editor.hpp"

void Editor::renderFileManager()
{
    bufferFilesInCurrentDir();
    y_actual = 0;
    x_actual = 0;
    getmaxyx(stdscr, y_max, x_max);
    move(x_actual,y_actual);
    renderScreen();
}

void Editor::bufferFilesInCurrentDir()
{
    buffer.clear();
    y_actual=0;
    x_actual=0;
    for (auto &file : fs::directory_iterator(actualStringPath))
    {
        std::string sFile = file.path().filename().string();
        if (sFile[0]=='.') continue;
        else if (file.is_directory())
        {
            sFile.push_back('/');
        }
        buffer.push_back(sFile);
        y_actual+=1;
    }
}



