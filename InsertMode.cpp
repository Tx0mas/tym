#include"Editor.hpp"

void Editor::handleInsertRareKeys()
{
    if (letra == '\n')
    {
        if (y_actual>=y_max+y_min-5) //si quiero agregar scroll +4 ponerlo aca
        {
            y_min+=1;
        }

        if (x_actual == buffer[y_actual].size())
        {
            y_actual+=1;
            x_actual =0;
            buffer.insert(buffer.begin() + y_actual,"");
        }
        else
        {
            std::string temp = buffer[y_actual].substr(x_actual);
            buffer[y_actual].erase(x_actual);
            y_actual+=1;
            x_actual =0;
            buffer.insert(buffer.begin() + y_actual,temp);
        }
        renderScreen();
        x_dinamicMax = x_actual;
        rareKey = true;
    }
    else if (letra == KEY_BACKSPACE)
    {
        if (x_actual>0 && x_actual<=buffer[y_actual].size())
        {
            x_actual-=1;
            buffer[y_actual].erase(x_actual,1);
            renderLine();
        }
        x_dinamicMax = x_actual;
        rareKey = true;
    }
}

void Editor::handleInsertMode()
{
    handleInsertRareKeys();

    if (rareKey == true)
    {
        rareKey = false;
        return;
    }

    if (y_actual>=buffer.size())
    {
        buffer.resize(y_actual+1);
    }
    if (buffer[y_actual].size()<x_actual)
    {
        for (int i{};i<x_actual;i++)
        {
            buffer[y_actual].push_back(' ');
        }
    }
    buffer[y_actual].insert(x_actual,1,letra);
    x_actual+=1;
    x_dinamicMax = x_actual;
    move(y_actual,x_actual);
    renderLine();

    if (JK_TO_ESCAPE == 1 && letra == 'j')
    {
        letra = getch();
        if (letra == 'k')
        {
            mode = Mode::NormalMode;
            x_actual-=1;
            buffer[y_actual].erase(x_actual,1);
            renderLine();
        }
    }
}

