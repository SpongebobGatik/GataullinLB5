#include <iostream>
#include <fstream>
#include <string>
#include <regex>

class CodeComponent {
public:
    virtual std::string toHtml() = 0;
};

class Code : public CodeComponent {
private:
    std::string code;
public:
    Code(const std::string& code) : code(code) {}
    std::string toHtml() override {
        std::string htmlCode = "";
        for (char c : code) {
            if (c == '<') {
                htmlCode += "&lt;";
            }
            else if (c == '>') {
                htmlCode += "&gt;";
            }
            else {
                htmlCode += c;
            }
        }
        return htmlCode;
    }
};

class CodeDecorator : public CodeComponent {
protected:
    CodeComponent* component;
public:
    CodeDecorator(CodeComponent* component) : component(component) {}
    std::string toHtml() override {
        return component->toHtml();
    }
};

class KeywordDecorator : public CodeDecorator {
public:
    KeywordDecorator(CodeComponent* component) : CodeDecorator(component) {}
    std::string toHtml() override {
        std::string html = CodeDecorator::toHtml();
        // Однострочные комментарии
        html = std::regex_replace(html, std::regex("//.*"), "<span style='color:#57A64A;'>$&</span>");
        // Многострочные комментарии
        html = std::regex_replace(html, std::regex("/\\*(.|[\r\n])*?\\*/"), "<span style='color:#57A64A;'>$&</span>");
        // Ключевые слова
        html = std::regex_replace(html, std::regex("\\b(int|float|double|char|void|bool|short|long|signed|unsigned|const|static|volatile|if|else|for|while|do|break|continue|return|switch|case|default|class|struct|typedef|enum|namespace|template|this|public|private|protected|virtual|operator|sizeof|dynamic_cast|static_cast|reinterpret_cast|const_cast|new|delete|throw|try|catch|explicit|friend|inline|register|extern|mutable|using|namespace|std)\\b"), "<span style='color:#569CD6;'>$&</span>");
        // Строки
        html = std::regex_replace(html, std::regex("\"(.*?)\""), "<span style='color:#D69D85;'>$&</span>");
        // Числа
        html = std::regex_replace(html, std::regex("\\b(\\d+\\.?\\d*|\\.\\d+)\\b"), "<span style='color:#B5CEA8;'>$&</span>");
        // Препроцессорные директивы
        html = std::regex_replace(html, std::regex("^#\\w+"), "<span style='color:#9CDCFE;'>$&</span>");
        // Типы данных
        html = std::regex_replace(html, std::regex("\\b(std::\\w+|std::string|std::regex)\\b"), "<span style='color:#4EC9B0;'>$&</span>");
        // Функции
        html = std::regex_replace(html, std::regex("\\b(main|printf|cin|cout|cerr|ifstream|ofstream|istream|ostream|getline|strncpy|strcmp|strlen|strcat|memcpy|malloc|free|new|delete|throw|catch|try|std::\\w+|std::ifstream|std::ofstream|std::cerr|std::cout)\\b"), "<span style='color:#DCDCAA;'>$&</span>");
        return html;
    }
};

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Rus");
    if (argc != 2) {
        std::cerr << "Использование: " << argv[0] << " <имя_файла>" << std::endl;
        return 1;
    }
    std::string inputFileName = argv[1];
    std::ifstream file(inputFileName);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << inputFileName << std::endl;
        return 1;
    }
    std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    CodeComponent* rawCode = new Code(code);
    CodeComponent* decoratedCode = new KeywordDecorator(rawCode);
    std::string htmlCode = decoratedCode->toHtml();
    size_t lastdot = inputFileName.find_last_of(".");
    std::string baseName = (lastdot == std::string::npos) ? inputFileName : inputFileName.substr(0, lastdot);
    std::string outputFileName = baseName + ".html";
    std::ofstream htmlFile(outputFileName);
    if (!htmlFile.is_open()) {
        std::cerr << "Не удалось создать HTML файл: " << outputFileName << std::endl;
        return 1;
    }
    htmlFile << "<!DOCTYPE html>\n<html>\n<head>\n<style>\n"
        "body { background-color: #1E1E1E; color: #D4D4D4; font-family: 'Consolas', monospace; white-space: pre; }\n"
        "span { font-weight: normal; }\n"
        ".keyword { color: #569CD6; }\n"
        ".string { color: #CE9178; }\n"
        ".comment { color: #6A9955; }\n"
        ".number { color: #B5CEA8; }\n"
        ".preprocessor { color: #9B9B9B; }\n"
        ".datatype { color: #4EC9B0; }\n"
        ".function { color: #DCDCAA; }\n"
        "</style>\n</head>\n<body>\n"
        << htmlCode
        << "\n</body>\n</html>";
    htmlFile.close();
    delete decoratedCode;
    delete rawCode;
    std::cout << "HTML файл успешно создан: " << outputFileName << std::endl;
    return 0;
}