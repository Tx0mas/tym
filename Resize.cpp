#include"Editor.hpp"

void Editor::handleResize()
{
    endwin();
    refresh();
    erase();
    getmaxyx(stdscr, y_max, x_max);

    if (y_actual>y_max || y_actual<y_min)
    {
        if (y_actual>y_max)
        {
            y_max=y_actual;
        }
        else if(y_actual<y_min)
        {
            y_min=y_actual;
        }
    }
    renderScreen();
}


