#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Fallo al compilar el sombreado de la figura" <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "Shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void) {
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(800, 600, "Soccer ball by Cortez Obed", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    float angle = 0.0f;
    float pentadecagonRadius = 0.4f;
    float pentagonRadius = 0.2f;

    float positions[60]; // The array will be for both pentagon and pentadecagon

    // Calculate the positon of pentadecagon
    for (int i = 0; i < 15; ++i) {
        positions[i * 2] = pentadecagonRadius * cos(angle);
        positions[i * 2 + 1] = pentadecagonRadius * sin(angle);
        angle += 2.0f * 3.14159265f / 15.0f;
    }

    angle = 0.0f; // Reset for pentagon

    // Positions of pentagon
    for (int i = 0; i < 5; ++i) {
        positions[(i + 15) * 2] = pentagonRadius * cos(angle);
        positions[(i + 15) * 2 + 1] = pentagonRadius * sin(angle);
        angle += 2.0f * 3.14159265f / 5.0f;
    }

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 60 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        " gl_Position = position;\n"
        "}\n";

    std::string fragmentShader =
        "#version 330 core\n"
        "\n"
        "uniform int drawMode;\n"
        "layout(location = 0) out vec4 color;\n"
        "\n"
        "void main()\n"
        "{\n"
        " if (drawMode == 0) {\n"
        "  color = vec4(1.0, 1.0, 1.0, 1.0);\n" //White color
        " } else {\n"
        "  color = vec4(0.2, 0.2, 0.2, 1.0);\n" //Dark gray color
        " }\n"
        "}\n";

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    int drawModeLocation = glGetUniformLocation(shader, "drawMode");
    glUniform1i(drawModeLocation, 0); // 0 for fill, 1 for lines

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform1i(drawModeLocation, 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 15);  // Drawing pentadecagon a with a color

        glUniform1i(drawModeLocation, 1);
        glDrawArrays(GL_TRIANGLE_FAN, 15, 5);  // Drawing a Pentagon with a color in (0,0)
        glDrawArrays(GL_TRIANGLES, 0, 15); //simulating the edges of a soccer ball


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
