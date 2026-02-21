#include<vector>
#include<string>
#include<ncurses.h>
#include<iostream>
#include<fstream>
#include<unordered_set>

#define SET_SCROLL 2
#define JK_TO_ESCAPE 1

enum class Mode
{
    NormalMode,
    InsertMode,
    VisualMode,
    CommandMode,
};

class Editor
{
private:
    Mode mode{Mode::NormalMode};
    std::vector<std::string> buffer{};
    std::string bufferCommandLine{};
    std::string linea{};
    std::fstream file;
    std::string fileName;
    std::ofstream outFile;
    int letra{};
    char cLetra = (char)letra;

    int y_actual{};
    int x_actual{};
    int y_max{};
    int y_min{};
    int x_max{};
    int x_min{};

    int x_dinamicMax{};
    bool rareKey{false};
    bool salir{false};

    std::unordered_set<int> setXNavigation{
        'h','l','w','b','0'
    };
    std::unordered_set<int> setYNavigation{
        'j','k' 
    };

public:
    Editor(std::string nombreFile)
    :fileName{nombreFile}
    {
        file.open(nombreFile,std::ios::in | std::ios::out | std::ios::binary);;
        if (!file.is_open())
        {
            std::cerr<<"Error de file";
            return;
        }
        while (std::getline(file,linea))
        {
            buffer.push_back(linea);
        }
        if (buffer.empty())
        {
            buffer.push_back("");
        }
    }

    std::string getModeString()
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

    void renderLine()
    {
        clrtoeol(); 
        mvprintw(y_actual,0,"%s",buffer[y_actual].c_str());
        move(y_actual,x_actual);
        refresh();
    }

    void renderBar()
    {
        start_color();
        init_pair(1,COLOR_RED,COLOR_WHITE);
        int filaEstado = y_max-2;
        std::string modeString = getModeString();
        std::string estado = "||"+modeString+"||      Locacion->"+fileName+"      ||      Historial->"+bufferCommandLine;
        attron(COLOR_PAIR(1));
        mvhline(filaEstado,x_min,' ', x_max);
        mvprintw(filaEstado,x_min,"%s",estado.c_str());
        attroff(COLOR_PAIR(1));
        refresh();
    }

    void renderScreen()
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

    void renderFirstScreen()
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

    void handleInsertRareKeys()
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

    void handleInsertMode()
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

    void handleYScroll()
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

    void handleXScroll()
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
            size_t indiceEspacio = buffer[y_actual].find_last_of(' ',x_actual-2);
            if (indiceEspacio != std::string::npos && x_actual!=0)
            {
                size_t indice = buffer[y_actual].find_last_not_of(' ',indiceEspacio+1);
                if(indice != std::string::npos)
                {
                    x_actual = indice;
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
        x_dinamicMax = x_actual;
    }

    void handleNavegation()
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

    void handleNormalMode()
    {
        if (setXNavigation.find(letra)!=setXNavigation.end()
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

    void handleCommandsTypes()
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

    void handleCommandMode()
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

    void inputsFunction()
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

    void handleResize()
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

    void general()
    {
        renderFirstScreen();
        while (true)
        {
            letra = getch();
            if (letra == KEY_RESIZE)
            {
                handleResize();
                continue;
            }
            else if (letra == 'i' && mode == Mode::NormalMode)
            {
                mode=Mode::InsertMode;
            }
            else if (letra == 'a' && mode == Mode::NormalMode)
            {
                x_actual+=1;
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




};
