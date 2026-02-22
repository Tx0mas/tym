#include"Editor.hpp"

void Editor::general()
{
    fillBufferWithFile();
    if (alreadyRenderFirst == false)
    {
        renderFirstScreen();
    }
    alreadyRenderFirst = true;
    while (true)
    {
        letra = getch();
        if (letra == KEY_RESIZE)
        {
            handleResize();
            continue;
        }
        else if (mode== Mode::NormalMode 
            && setInsertKeys.find(letra)!=setInsertKeys.end() 
            && inExplorer == false)
        {
            if (letra == 'i')
            {
            }
            else if (letra == 'a')
            {
                x_actual+=1;
            }
            else if (letra == 'A')
            {
                x_actual=buffer[y_actual].size(); 
            }
            mode=Mode::InsertMode;
        }
        else if (letra == 27 && mode == Mode::InsertMode)
        {
            mode=Mode::NormalMode;
        }
        else if (letra == ':' && mode == Mode::NormalMode)
        {
            mode=Mode::CommandMode;
            renderScreen();
            inputsFunction();
            if (salir == true)
            {
                break;
            }
        }
        else
        {
            inputsFunction();
        }
        renderScreen();
    }
    endwin();
}


