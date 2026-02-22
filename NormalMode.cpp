#include"Editor.hpp"
void Editor::handleNavegation()
{
    if (setYNavigation.find(letra)!=setYNavigation.end())
    {
        handleYScroll();
    }
    if (setXNavigation.find(letra)!=setXNavigation.end())
    {
        handleXScroll();
    }

    //Para poder mantener la posicion maxima
    if (x_dinamicMax > buffer[y_actual].size())
    {
        x_actual = buffer[y_actual].size();
    }
    else
    {
        x_actual = x_dinamicMax;
    }
    move(y_actual,x_actual);
    refresh();
}

void Editor::handleNormalMode()
{
    if ((letra == '\n' || letra =='-') && inExplorer == true)
    {
        if (letra == '\n')
        {
            if (buffer[y_actual].back()=='/')
            {
                actualFilePath/=buffer[y_actual].substr(0,buffer[y_actual].length()-1);
                actualStringPath = actualFilePath.c_str();
                renderFileManager();
            }
            else
            {
                fs::path tempFilePath = actualFilePath/buffer[y_actual];
                fileName = tempFilePath.string();
                inExplorer = false;
                if (file.is_open()) file.close();
                file.clear();
                fillBufferWithFile();
                renderScreen();
            }
        }
        else if (letra == '-')
        {
            actualFilePath = actualFilePath.parent_path();
            actualStringPath = actualFilePath.c_str();
            renderFileManager();
        }
    }
    else if (setXNavigation.find(letra)!=setXNavigation.end()
        || setYNavigation.find(letra)!=setYNavigation.end())
    {
        handleNavegation();
    }
    else
    {
        //esto para si agranda o achica la screen
        renderScreen();
        refresh();
    }
}


void Editor::handleYScroll()
{
    if (letra == 'j')
    {
        if (y_actual<buffer.size()-1)
        {
            y_actual+=1;
            if (y_actual>=y_max+y_min-SET_SCROLL*2) //si quiero agregar scroll +4 ponerlo aca
            {
                y_min+=1;
            }
        }
    }
    if (letra == 'k')
    {
        if (y_actual>0)
        {
            y_actual-=1;
            if (y_actual < y_min+SET_SCROLL) //idem
            {
                if (y_actual>1) //para que no se nos corra arriba
                {
                    y_min-=1;
                }
            }
        }
    }
}

void Editor::handleXScroll()
{
    if (letra == 'l' && buffer[y_actual].size()>x_actual)
    {
        x_actual+=1;
    }
    else if (letra == 'h' && x_actual>x_min)
    {
        x_actual-=1;
    }
    else if (letra == 'w') 
    {
        size_t indiceEspacio = buffer[y_actual].find_first_of(' ',x_actual+1);
        if (indiceEspacio != std::string::npos) 
        {
            size_t indice = buffer[y_actual].find_first_not_of(' ',indiceEspacio+1);
            if(indice != std::string::npos)
            {
                x_actual = indice;
            }
            else //TODO que x_actual == buffer[y_actual].size() me mande al de abajo al start
            {
                x_actual = buffer[y_actual].size();
            }
        }
        else
        {
            x_actual = buffer[y_actual].size();
        }
    }
    else if (letra == 'b')  //TODO que x_actual == 0 me mande al de arriba
    {
        size_t indiceEspacio = buffer[y_actual].find_last_not_of(' ',x_actual-1); 
        if (indiceEspacio != std::string::npos && x_actual>0)
        {
            size_t indice = buffer[y_actual].find_last_of(' ',indiceEspacio-1);
            if(indice != std::string::npos)
            {
                x_actual = indice+1;
            }
            else
            {
                x_actual = 0;
            }
        }
        else
        {
            x_actual = 0;
        }
    }
    else if (letra == '0')  //TODO que x_actual == 0 me mande al de arriba
    {
        x_actual=0;
    }
    else if (letra == 'f')
    {
        saveLetra = getch();
        if (saveLetra == 27)
        {
            return;
        }
        if (buffer[y_actual].find(saveLetra,x_actual+1)!=std::string::npos)
        {
            size_t indice = buffer[y_actual].find(saveLetra,x_actual+1);
            x_actual = indice;
        }
    }
    else if (letra == ';') //bug me manda al final en el ultimo
    {
        if (buffer[y_actual].find(saveLetra,x_actual+1)!=std::string::npos)
        {
            size_t indice = buffer[y_actual].find(saveLetra,x_actual+1);
            x_actual = indice;
        }
    }
    else if (letra == ',')
    {
        if (buffer[y_actual].find_last_of(saveLetra,x_actual-1)!=std::string::npos)
        {
            size_t indice = buffer[y_actual].find_last_of(saveLetra,x_actual-1);
            x_actual = indice;
        }
    }
    x_dinamicMax = x_actual;
}

