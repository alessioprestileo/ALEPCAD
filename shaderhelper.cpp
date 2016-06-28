#include "shaderhelper.h"

ShaderHelper::ShaderHelper() {}

// Read a shader source from a file,
// store the shader source in the input buffer
// and return reference to the buffer
const char* ShaderHelper::readShaderSrc(const char *pathToShader,
                                 std::vector<char> &buffer) {
    QFile file(pathToShader);
    if(!file.open(QIODevice::ReadOnly)) {
        QString message = "Error opening file ";
        message.append(QString(pathToShader));
        QMessageBox::information(0, "Error", message);
    }
    QTextStream inStr(&file);
    while(!inStr.atEnd()) {
        char c;
        inStr >> c;
        buffer.push_back(c);
    }
    file.close();
    buffer.push_back('\0');
    const char *content = &buffer[0];
    return content;
}
