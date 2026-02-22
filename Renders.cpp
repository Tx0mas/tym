#include"Editor.hpp"

std::string Editor::getModeString()
{
    if (mode == Mode::NormalMode)
    {
        return "--NORMAL--";
    }
    else if (mode == Mode::InsertMode)
    {
        return "--INSERT--";
    }
    else if (mode == Mode::CommandMode)
    {
        return "--COMMAND--";
    }
    else if (mode == Mode::VisualMode)
    {
        return "--VISUAL--";
    }
    else
    {
        return "--NORMAL--";
    }
}

void Editor::renderLine()
{
    clrtoeol(); 
    mvprintw(y_actual,0,"%s",buffer[y_actual].c_str());
    move(y_actual,x_actual);
    refresh();
}

void Editor::renderBar()
{
    start_color();
    init_pair(1,COLOR_RED,COLOR_WHITE);
    int filaEstado = y_max-2;
    std::string modeString = getModeString();
    std::string estado = "||"+modeString+"||      Locacion->"
        +fileName+"      ||      Historial->"+bufferCommandLine;
    attron(COLOR_PAIR(1));
    mvhline(filaEstado,x_min,' ', x_max);
    mvprintw(filaEstado,x_min,"%s",estado.c_str());
    attroff(COLOR_PAIR(1));
    refresh();
}

void Editor::renderScreen()
{
    erase();
    renderBar();

    for (int fila{};fila<y_max-2;fila++)
    {
        if (fila+y_min<buffer.size())//relativo (fila+y_min)
        {
            mvprintw(fila,0,"%s",buffer[fila+y_min].c_str());
        }
    }
    move(y_actual-y_min,x_actual);
    refresh();
}

void Editor::renderFirstScreen()
{
    initscr();            
    getmaxyx(stdscr, y_max, x_max);
    noecho();             
    keypad(stdscr, TRUE);
    cbreak();
    scrollok(stdscr, FALSE); 
    idlok(stdscr, FALSE);   
    renderScreen();
}
