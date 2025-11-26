#include "Application.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Application app;
    
    if (!app.init()) {
        std::cerr << "Error al inicializar la consola" << std::endl;
        return 1;
    }
    
    app.run();
    app.cleanup();
    
    return 0;
}
