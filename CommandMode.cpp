#include"Editor.hpp"

void Editor::handleCommandMode()
{
    int x_saved{x_actual};
    int y_saved{y_actual};

    char cLetra = (char)letra;
    bufferCommandLine.clear();
    bufferCommandLine.push_back(cLetra);

    x_actual = x_min;
    y_actual = y_max-1;
    move(y_actual,x_actual); 
    refresh();
    mvprintw(y_actual,x_actual,"%s",bufferCommandLine.c_str());
    x_actual+=1;
    refresh();

    while(true)
    {
        letra = getch();
        cLetra = (char)letra;
        if(letra == '\n')
        {
            x_actual = x_saved;
            y_actual = y_saved;
            handleCommandsTypes();
            break;
        }
        if (letra == 27)
        {
            move(y_actual,x_min); //muevo para borrar desde el 0
            clrtoeol();
            refresh();

            move(y_saved,x_saved);
            x_actual = x_saved;
            y_actual = y_saved;
            mode = Mode::NormalMode;
            refresh();
            break;
        }
        else if (letra == KEY_BACKSPACE)
        {
            if (x_actual>0 && x_actual<=bufferCommandLine.size())
            {
                move(y_actual,x_min); //muevo para borrar desde el 0
                clrtoeol();
                refresh();

                x_actual-=1;
                bufferCommandLine.erase(x_actual,1);
                mvprintw(y_actual,x_min,"%s",bufferCommandLine.c_str());
                refresh();
            }
        }
        else
        {
            bufferCommandLine.push_back(cLetra);
            clrtoeol(); 
            x_actual+=1;
            mvprintw(y_actual,x_min,"%s",bufferCommandLine.c_str());
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
    else if( bufferCommandLine == ":w")
    {
        std::ofstream outFile (fileName, std::ios::trunc);
        for (auto &lineas : buffer)
        {
            outFile<<lineas<<'\n';
        }
        mode=Mode::NormalMode;
        move(y_actual,x_actual);
        refresh();
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

