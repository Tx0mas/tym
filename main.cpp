#include"Editor.hpp"

int main(int argc,char **argv)
{
    if (argc!=2)
    {
        std::cerr<<"Ingrese la file a editar.";
        return 1;
    }
    Editor editThis{argv[1]};
    editThis.general();

    return 0;
}
