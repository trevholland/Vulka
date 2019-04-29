#include "input.h"
#include "logger.h"

void Input::Initialize(InputInfo* inputInfo)
{
    pWindow = inputInfo->pWindow;
    mCloseKeyCount = inputInfo->closeKeys.size();
    if (mCloseKeyCount > 0)
    {
        pCloseKeys = new int[mCloseKeyCount];
        memcpy(pCloseKeys, inputInfo->closeKeys.data(), mCloseKeyCount * sizeof(int));
    }
}

void Input::Update()
{
    ProcessCloseKeys();
}

void Input::Shutdown()
{
    pWindow = nullptr;
    delete[] pCloseKeys;
    pCloseKeys = nullptr;
    mCloseKeyCount = 0;
}

void Input::ProcessCloseKeys()
{
    if (!pCloseKeys)
    {
        return;
    }
    for (size_t i = 0; i < mCloseKeyCount; ++i)
    {
        if (glfwGetKey(pWindow, pCloseKeys[i]) != GLFW_PRESS)
        {
            return;
        }
    }
    glfwSetWindowShouldClose(pWindow, GLFW_TRUE);
}

void Input::AddKeybinding(uint32_t action, int key)
{
    // this needs to be more smart, allowing a vector of keys for a single action.
    // it also needs to ensure we don't have multiple actions on the same key.
    // for now, we're a little busted...
    #ifndef NDEBUG
    if (mKeybindings.find(action) != mKeybindings.end())
    {
        char actionstr[5];
        // this will be backwards(JUMP will be PMUJ), so we do some swap xor magic.
        memcpy(actionstr, &action, 4);
        // swap the first and last character
        actionstr[0] ^= actionstr[3];
        actionstr[3] ^= actionstr[0];
        actionstr[0] ^= actionstr[3];
        // swap the two middle characters
        actionstr[1] ^= actionstr[2];
        actionstr[2] ^= actionstr[1];
        actionstr[1] ^= actionstr[2];
        // add the null
        actionstr[4] = '\0';
        logger.warn("Overriding keybinding.%s", actionstr);
    }
    #endif

    mKeybindings.insert_or_assign(action, key);
}

bool Input::IsActionPressed(uint32_t action)
{
    auto iter = mKeybindings.find(action);
    if (iter == mKeybindings.end())
    {
        return false;
    }
    return glfwGetKey(pWindow, iter->second) == GLFW_PRESS;
}