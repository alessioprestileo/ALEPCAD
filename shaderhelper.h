#ifndef SHADERHELPER_H
#define SHADERHELPER_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>

class ShaderHelper {
public:
    static const char* readShaderSrc(const char *pathToShader,
                                     std::vector<char> &buffer);
private:
    ShaderHelper();
};
#endif // SHADERHELPER_H
