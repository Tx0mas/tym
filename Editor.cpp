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
    for (auto &file : fs::directory_iterator(actualFilePath))
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




void Editor::inputsFunction()
{
    if (mode == Mode::NormalMode)
    {
        handleNormalMode();
    }
    else if (mode == Mode::InsertMode)
    {
        handleInsertMode();
    }
    else if (mode == Mode::CommandMode)
    {
        handleCommandMode();
    }
}

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

    std::string tempFileName{};
    if (fileName.find_last_of('/')!=std::string::npos)
    {
        tempFileName = fileName.substr(fileName.find_last_of('/'),fileName.length()-1);
    }
    else
    {
        tempFileName = fileName;
    }

    std::string estado = "|"+modeString+"| "
        +tempFileName+" | H: "+bufferCommandLine;
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


