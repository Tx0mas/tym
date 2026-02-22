#include<vector>
#include<string>
#include<ncurses.h>
#include<iostream>
#include<fstream>
#include<unordered_set>
#include<filesystem>

namespace fs = std::filesystem;

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
    int saveLetra{};
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
    bool inExplorer{false};
    bool alreadyRenderFirst{false};

    const char* actualStringPath = std::getenv("HOME"); 
    fs::path actualFilePath = actualStringPath;
    //fs::current_path()) //capaz puedo poner solo current_path

    std::unordered_set<int> setXNavigation{
        'h','l','w','b','0','f',';',','
    };
    std::unordered_set<int> setYNavigation{
        'j','k' 
    };
    std::unordered_set<int> setInsertKeys{
        'i','a','A'
    };

public:
    Editor(std::string nombreFile)
    :fileName{nombreFile}
    {
    }
    void fileFiller();
    void fillBufferWithFile();

    std::string getModeString();
    void renderLine();
    void renderBar();
    void renderScreen();
    void renderFirstScreen();
    void handleInsertRareKeys();
    void handleInsertMode();

    void handleYScroll();
    void handleXScroll();

    void handleNormalMode();
    void handleNavegation();
    void handleCommandsTypes();
    void handleCommandMode();

    void inputsFunction();

    void handleResize();

    void general();

    //FILEMANAGERCRAP
    void renderFileManager();
    void bufferFilesInCurrentDir();
    //FILEMANAGERCRAP



};
