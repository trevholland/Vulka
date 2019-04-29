#ifndef _INPUT_H_
#define _INPUT_H_

#include <GLFW\glfw3.h>
#include <map>
#include <vector>

typedef struct InputInfo {
    GLFWwindow* pWindow;
    std::vector<int> closeKeys;
} InputInfo;

class Input
{
public:
    void Initialize(InputInfo* inputInfo);
    void Update();
    void Shutdown();

    void AddKeybinding(uint32_t action, int key);
    bool IsActionPressed(uint32_t action);

private:
    void ProcessCloseKeys();

    GLFWwindow* pWindow;
    int* pCloseKeys;
    size_t mCloseKeyCount;

    std::map<uint32_t, int> mKeybindings; // mapping of action -> GLFW_KEY value
};

#endif _INPUT_H_