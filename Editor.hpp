#include<vector>
#include<string>
#include<ncurses.h>
#include<iostream>
#include<fstream>

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
    bool salir{false};
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

    void renderLine()
    {
        clrtoeol(); 
        mvprintw(y_actual,0,"%s",buffer[y_actual].c_str());
        move(y_actual,x_actual);
        refresh();
    }

    void renderScreen()
    {
        for (int fila{};fila<buffer.size();fila++)
        {
            clrtoeol(); 
            mvprintw(fila,0,"%s",buffer[fila].c_str());
        }
        move(y_actual,x_actual);
        refresh();
    }

    void renderFirstScreen()
    {
        initscr();            
        noecho();             
        keypad(stdscr, TRUE);
        cbreak();             
        scrollok(stdscr, FALSE); 
        idlok(stdscr, FALSE);   
        renderScreen();
    }

    void handleInsertMode()
    {
        getyx(stdscr, y_actual, x_actual);
        if (buffer.size()<y_actual)
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
        move(y_actual,x_actual);
        renderLine();
    }

    void handleNormalMode()
    {
        if (letra == 'j')
        {
            y_actual+=1;
        }
        else if (letra == 'k' && y_actual>0)
        {
            y_actual-=1;
        }
        else if (letra == 'l')
        {
            x_actual+=1;
        }
        else if (letra == 'h' && x_actual>0)
        {
            x_actual-=1;
        }
        move(y_actual,x_actual);
        refresh();
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
            clrtoeol();
            move(y_actual,x_actual);
            mode = Mode::NormalMode;
            refresh();
            return;
        }
    }

    void handleCommandMode()
    {
        char cLetra = (char)letra;
        getyx(stdscr, y_actual, x_actual);
        bufferCommandLine.clear();
        bufferCommandLine.push_back(cLetra);

        int command_x{0};
        int command_y{20};

        //esto lo hago para clerear el historial
        move(command_y,command_x); 
        clrtoeol();

        mvprintw(command_y,command_x,"%s",bufferCommandLine.c_str());
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
                clrtoeol();
                move(y_actual,x_actual);
                mode = Mode::NormalMode;
                refresh();
                break;
            }
            else
            {
                bufferCommandLine.push_back(cLetra);
                clrtoeol(); 
                mvprintw(command_y,command_x,"%s",bufferCommandLine.c_str());
                refresh();
            }
        }
        return;
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

    void general()
    {
        renderFirstScreen();

        while (true)
        {
            letra = getch();
            if (letra == 'i' && mode == Mode::NormalMode)
            {
                mode=Mode::InsertMode;
            }
            else if (letra == 27 && mode == Mode::InsertMode)
            {
                mode=Mode::NormalMode;
            }
            else if (letra == ':' && mode == Mode::NormalMode)
            {
                mode=Mode::CommandMode;
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
        }

        endwin();
    }




};
