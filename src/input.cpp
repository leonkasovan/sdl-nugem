/*
 * Copyright (c) 2016 Victor Nivet
 *
 * This file is part of Nugem.
 *
 * Nugem is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 * Nugem is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with Nugem.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "input.hpp"

#include <stdio.h>
#include <iostream>

#include "game.hpp"
#include "player.hpp"
#include "scene.hpp"
#include "scenemenu.hpp"

namespace Nugem {

const char * InputManager::controllerDBfilename = "gamecontrollerdb.txt";

InputManager::InputManager()
{
    SDL_JoystickEventState(SDL_ENABLE);
    loadGameControllerDB();
    mGame = nullptr;
}

InputManager::~InputManager()
{
}

unsigned int InputManager::initialize(Game * game)
{
    mGame = game;
    mDevices.clear();
    mDevices.push_back(std::unique_ptr<InputDevice>(new KeyboardInput(*this)));
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i))
            mDevices.push_back(std::unique_ptr<InputDevice>(new GameController(*this, i)));
        else
            mDevices.push_back(std::unique_ptr<InputDevice>(new Joystick(*this, i)));
    }
    for (auto i = mDevices.begin(); i != mDevices.end(); i++)
        (*i)->initialize();
    return mDevices.size();
}

bool InputManager::loadGameControllerDB()
{
    // check if file exists
    FILE * test = fopen(controllerDBfilename, "r");
    if (test)
        fclose(test);
    else
        return false;
    // if so, load it
    SDL_GameControllerAddMappingsFromFile(controllerDBfilename);
    return true;
}

void InputManager::processSDLEvent(const SDL_Event & e)
{
    // TODO
    switch (e.type) {
    case SDL_CONTROLLERDEVICEADDED:
        return;
    case SDL_CONTROLLERDEVICEREMOVED:
        return;
    case SDL_CONTROLLERDEVICEREMAPPED:
        return;
    case SDL_JOYDEVICEADDED:
        return;
    case SDL_JOYDEVICEREMOVED:
        return;
    };
    for (unsigned int i = 0; i < mDevices.size(); i++)
        mDevices[i]->receiveEvent(e);
}

void InputDevice::receiveEvent(const SDL_Event & e)
{
    InputState eventstate = processEvent(e);
    if ((eventstate.d != INPUT_D_UNDEFINED ||
            eventstate.a != INPUT_B_UNDEFINED ||
            eventstate.b != INPUT_B_UNDEFINED ||
            eventstate.c != INPUT_B_UNDEFINED ||
            eventstate.x != INPUT_B_UNDEFINED ||
            eventstate.y != INPUT_B_UNDEFINED ||
            eventstate.z != INPUT_B_UNDEFINED ||
            eventstate.start != INPUT_B_UNDEFINED ||
            eventstate.back != INPUT_B_UNDEFINED) &&
            eventstate != m_previousChange) {
        m_previousChange = eventstate;
        mManager.registerInput(this, eventstate);

        if (eventstate.d != INPUT_D_UNDEFINED)
            mCurrentState.d = eventstate.d;

        if (eventstate.a != INPUT_B_UNDEFINED)
            mCurrentState.a = eventstate.a;

        if (eventstate.b != INPUT_B_UNDEFINED)
            mCurrentState.b = eventstate.b;

        if (eventstate.c != INPUT_B_UNDEFINED)
            mCurrentState.c = eventstate.c;

        if (eventstate.x != INPUT_B_UNDEFINED)
            mCurrentState.x = eventstate.x;

        if (eventstate.y != INPUT_B_UNDEFINED)
            mCurrentState.y = eventstate.y;

        if (eventstate.z != INPUT_B_UNDEFINED)
            mCurrentState.z = eventstate.z;

        if (eventstate.start != INPUT_B_UNDEFINED)
            mCurrentState.start = eventstate.start;

        if (eventstate.back != INPUT_B_UNDEFINED)
            mCurrentState.back = eventstate.back;
    }
}

InputDevice & InputManager::device(size_t n)
{
    return *mDevices[n];
}

void InputManager::assignDeviceToPlayer(InputDevice * device, Player * player)
{
    if (device)
        device->assignToPlayer(player);
}

void InputManager::registerInput(InputDevice * device, InputState state)
{
    if (device && mGame) {
        Scene& scene = mGame->currentScene();
        if (SceneMenu *smenu = dynamic_cast<SceneMenu *>(&scene)) {
            smenu->receiveInput(device, state);
        }
    }
}

const size_t InputManager::deviceNumber() const
{
    return mDevices.size();
}

const InputState InputDevice::getState() const
{
    return mCurrentState;
}

InputDevice::InputDevice(InputManager & manager): mPlayer(nullptr), mManager(manager)
{
}

bool InputDevice::hasPlayerAssigned() const
{
    return (mPlayer != nullptr);
}

Player * InputDevice::getAssignedPlayer()
{
    return mPlayer;
}

InputState InputDevice::getState()
{
    return mCurrentState;
}

void InputDevice::initialize()
{
    updateGlobalState();
}

void InputDevice::assignToPlayer(Player * assignedPlayer)
{
    mPlayer = assignedPlayer;
}

KeyboardInput::KeyboardInput(InputManager & manager): InputDevice(manager)
{
}

InputState KeyboardInput::processEvent(const SDL_Event & e)
{
    InputState state;
    switch (e.type) {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        updateGlobalState();
        break;
    };
    return state;
}

const InputButtonState KeyboardInput::evaluateKey(SDL_Scancode key)
{
    const uint8_t * keystate = SDL_GetKeyboardState(NULL);
    if (keystate[key])
        return INPUT_B_PRESSED;
    else
        return INPUT_B_RELEASED;
}

void KeyboardInput::updateGlobalState()
{
    mCurrentState.a = evaluateKey(scancodeA);
    mCurrentState.b = evaluateKey(scancodeB);
    mCurrentState.c = evaluateKey(scancodeC);
    mCurrentState.x = evaluateKey(scancodeX);
    mCurrentState.y = evaluateKey(scancodeY);
    mCurrentState.z = evaluateKey(scancodeZ);
    mCurrentState.start = evaluateKey(scancodeStart);
    mCurrentState.back = evaluateKey(scancodeBack);
    const uint8_t * keystate = SDL_GetKeyboardState(NULL);
    if (keystate[scancodeUp]) {
        if (keystate[scancodeRight]) {
            mCurrentState.d = INPUT_D_NE;
        }
        else if (keystate[scancodeLeft]) {
            mCurrentState.d = INPUT_D_NW;
        }
        else {
            mCurrentState.d = INPUT_D_N;
        }
    }
    else if (keystate[scancodeDown]) {
        if (keystate[scancodeRight]) {
            mCurrentState.d = INPUT_D_SE;
        }
        else if (keystate[scancodeLeft]) {
            mCurrentState.d = INPUT_D_SW;
        }
        else {
            mCurrentState.d = INPUT_D_S;
        }
    }
    else {
        if (keystate[scancodeRight]) {
            mCurrentState.d = INPUT_D_E;
        }
        else if (keystate[scancodeLeft]) {
            mCurrentState.d = INPUT_D_W;
        }
        else {
            mCurrentState.d = INPUT_D_NEUTRAL;
        }
    }
}

GameController::GameController(InputManager & manager, const uint32_t jid): InputDevice(manager), m_jid(jid)
{
    m_gcsdl = SDL_GameControllerOpen(jid);
}

GameController::GameController(GameController && gameController): InputDevice(gameController.mManager), m_jid(gameController.m_jid)
{
    m_gcsdl = nullptr;
    std::swap(m_gcsdl, gameController.m_gcsdl);
}

GameController::~GameController()
{
    if (m_gcsdl)
        SDL_GameControllerClose(m_gcsdl);
}

InputState GameController::processEvent(const SDL_Event & e)
{
    InputState state;
    switch (e.type) {
    case SDL_CONTROLLERAXISMOTION: {
        const SDL_ControllerAxisEvent & aevent = e.caxis;
        if (aevent.axis == SDL_CONTROLLER_AXIS_LEFTX || aevent.axis == SDL_CONTROLLER_AXIS_LEFTY) {
            InputDirection dir = getDirection();
            state.d = dir;
        }
        else if (aevent.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
            state.z = getButtonValueForAxis(SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
        }
    }
    break;
    case SDL_CONTROLLERBUTTONDOWN: {
        const SDL_ControllerButtonEvent & bdevent = e.cbutton;
        if ((uint32_t) bdevent.which == m_jid) {
            switch (bdevent.button) {
            case SDL_CONTROLLER_BUTTON_A:
                state.a = INPUT_B_PRESSED;
                break;
            case SDL_CONTROLLER_BUTTON_B:
                state.b = INPUT_B_PRESSED;
                break;
            case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                state.c = INPUT_B_PRESSED;
                break;
            case SDL_CONTROLLER_BUTTON_X:
                state.x = INPUT_B_PRESSED;
                break;
            case SDL_CONTROLLER_BUTTON_Y:
                state.y = INPUT_B_PRESSED;
                break;
            case SDL_CONTROLLER_BUTTON_START:
                state.start = INPUT_B_PRESSED;
                break;
            case SDL_CONTROLLER_BUTTON_BACK:
                state.back = INPUT_B_PRESSED;
                break;
            }
        }
    }
    break;
    case SDL_CONTROLLERBUTTONUP: {
        const SDL_ControllerButtonEvent & buevent = e.cbutton;
        if ((uint32_t) buevent.which == m_jid) {
            switch (buevent.button) {
            case SDL_CONTROLLER_BUTTON_A:
                state.a = INPUT_B_RELEASED;
                break;
            case SDL_CONTROLLER_BUTTON_B:
                state.b = INPUT_B_RELEASED;
                break;
            case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                state.c = INPUT_B_RELEASED;
                break;
            case SDL_CONTROLLER_BUTTON_X:
                state.x = INPUT_B_RELEASED;
                break;
            case SDL_CONTROLLER_BUTTON_Y:
                state.y = INPUT_B_RELEASED;
                break;
            case SDL_CONTROLLER_BUTTON_START:
                state.start = INPUT_B_RELEASED;
                break;
            case SDL_CONTROLLER_BUTTON_BACK:
                state.back = INPUT_B_RELEASED;
                break;
            }
        }
    }
    break;
    };
    return state;
}

void GameController::updateGlobalState()
{
    SDL_GameControllerUpdate();
    mCurrentState.a = getButtonValue(SDL_CONTROLLER_BUTTON_A);
    mCurrentState.b = getButtonValue(SDL_CONTROLLER_BUTTON_B);
    mCurrentState.c = getButtonValueForAxis(SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
    mCurrentState.x = getButtonValue(SDL_CONTROLLER_BUTTON_X);
    mCurrentState.y = getButtonValue(SDL_CONTROLLER_BUTTON_Y);
    mCurrentState.z = getButtonValue(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
    mCurrentState.start = getButtonValue(SDL_CONTROLLER_BUTTON_START);
    mCurrentState.back = getButtonValue(SDL_CONTROLLER_BUTTON_BACK);
    mCurrentState.d = getDirection();
}

InputButtonState GameController::getButtonValue(SDL_GameControllerButton button)
{
    switch (SDL_GameControllerGetButton(m_gcsdl, button)) {
    case 0:
        return INPUT_B_RELEASED;
    case 1:
        return INPUT_B_PRESSED;
    };
    return INPUT_B_UNDEFINED;
}

InputButtonState GameController::getButtonValueForAxis(SDL_GameControllerAxis axis)
{
    if (SDL_GameControllerGetAxis(m_gcsdl, axis) >= threshold)
        return INPUT_B_PRESSED;
    return INPUT_B_RELEASED;
}

InputDirection GameController::getDirection(Sint16 hor, Sint16 vert)
{
    if (hor <= - threshold) {
        // Going to the left
        if (vert <= - threshold) {
            // Going up
            return INPUT_D_NW;
        }
        else if (vert >= threshold) {
            // Going down
            return INPUT_D_SW;
        }
        return INPUT_D_W;
    }
    else if (hor >= threshold) {
        if (vert <= - threshold) {
            // Going up
            return INPUT_D_NE;
        }
        else if (vert >= threshold) {
            // Going down
            return INPUT_D_SE;
        }
        // Going to the right
        return INPUT_D_E;
    }
    if (vert <= - threshold) {
        // Going up
        return INPUT_D_N;
    }
    else if (vert >= threshold) {
        // Going down
        return INPUT_D_S;
    }
    return INPUT_D_NEUTRAL;
}

InputDirection GameController::getDirection()
{
    Sint16 vert = SDL_GameControllerGetAxis(m_gcsdl, SDL_CONTROLLER_AXIS_LEFTY);
    Sint16 hor = SDL_GameControllerGetAxis(m_gcsdl, SDL_CONTROLLER_AXIS_LEFTX);
    return getDirection(hor, vert);
}

Joystick::Joystick(InputManager & manager, const uint32_t jid): InputDevice(manager), m_jid(jid)
{
    m_joysdl = SDL_JoystickOpen(jid);
}

Joystick::Joystick(Joystick && joystick): InputDevice(joystick.mManager), m_jid(joystick.m_jid)
{
    m_joysdl = nullptr;
    std::swap(m_joysdl, joystick.m_joysdl);
}

Joystick::~Joystick()
{
    if (m_joysdl)
        SDL_JoystickClose(m_joysdl);
}

InputState Joystick::processEvent(const SDL_Event & e)
{
    InputState state;
    switch (e.type) {
    case SDL_JOYAXISMOTION:
        break;
    case SDL_JOYBALLMOTION:
        break;
    case SDL_JOYHATMOTION:
        break;
    case SDL_JOYBUTTONDOWN:
        break;
    case SDL_JOYBUTTONUP:
        break;
    };
    return state;
}

void Joystick::updateGlobalState()
{

}

}


