#include"Editor.hpp"

void Editor::handleCommandMode()
{
    char cLetra = (char)letra;
    bufferCommandLine.clear();
    bufferCommandLine.push_back(cLetra);

    move(y_max-1,x_min); 
    refresh();
    mvprintw(y_max-1,x_min,"%s",bufferCommandLine.c_str());
    refresh();

    while(true)
    {
        letra = getch();
        cLetra = (char)letra;
        if(letra == '\n')
        {
            handleCommandsTypes();
            break;
        }
        if (letra == 27)
        {
            move(y_max-1,x_min); //muevo para borrar desde el 0
            clrtoeol();
            refresh();

            move(y_actual,x_actual);
            mode = Mode::NormalMode;
            refresh();
            break;
        }
        else
        {
            bufferCommandLine.push_back(cLetra);
            clrtoeol(); 
            mvprintw(y_max-1,x_min,"%s",bufferCommandLine.c_str());
            refresh();
        }
    }
}


void Editor::handleCommandsTypes()
{
    if( bufferCommandLine == ":q")
    {
        file.close();
        endwin();
        salir = true;
    }
    else if( bufferCommandLine == ":wq")
    {
        std::ofstream outFile (fileName, std::ios::trunc);
        for (auto &lineas : buffer)
        {
            outFile<<lineas<<'\n';
        }
        outFile.close();
        file.close();
        endwin();
        salir = true;
    }
    else if( bufferCommandLine == ":Ex")
    {
        renderFileManager();
        mode=Mode::NormalMode;
        inExplorer = true;
    }
    else
    {
        move(y_max-1,x_min);
        clrtoeol();
        refresh();

        move(y_actual,x_actual);
        mode = Mode::NormalMode;
        refresh();
    }
}

