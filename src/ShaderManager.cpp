//
//  ShaderManager.cpp
//
//  Created by Cam Stocker on 4/23/19.
//

/***********************
 HOW TO ADD A SHADER:
 1) Create a #define in ShaderManager.h that will be used to identify your shader
 2) Add an init function in ShaderManager.cpp and put your initialization code there
    - be sure to add a prototype of this function in ShaderManager.h
 3) Call your init function from initShaders in ShaderManager.cpp and save it to the
    respective location in shaderMap. See example
 
 HOW TO USE A SHADER IN THE RENDER LOOP
 1) first, call shaderManager.setCurrentShader(int name) to set the current shader
 2) To retrieve the current shader, call shaderManager.getCurrentShader()
 3) Use the return value of getCurrentShader() to render
 ***********************/

#include <stdio.h>
#include <iostream>

#include "ShaderManager.h"

void ShaderManager::initShaders() {
    shaderMap[SIMPLEPROG] = initSimpleProgShader();
    shaderMap[SPIDERPROG] = initSpiderProgShader();
    shaderMap[HANDPROG] = initHandProgShader();
    shaderMap[EYEPROG] = initEyeProgShader();
    shaderMap[PUPILPROG] = initPupilProgShader();
}

shared_ptr<Program> ShaderManager::initSimpleProgShader() {
//    // Initialize the GLSL program.
    std::shared_ptr<Program> prog = make_shared<Program>();
    
    prog->setVerbose(true);
    prog->setShaderNames(resourceDirectory + "/shaders/hand_vert.glsl", resourceDirectory + "/shaders/hand_frag.glsl");
    
    if (!prog->init())
    {
        cerr << "One or more shaders failed to compile... exiting!" << endl;
        exit(1);
    }
    
    prog->addUniform("P");
    prog->addUniform("V");
    prog->addUniform("M");
    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");
    prog->addAttribute("vertTex");
    
    return prog;
}

shared_ptr<Program> ShaderManager::initSpiderProgShader() {
//    // Initialize the GLSL program.
    std::shared_ptr<Program> prog = make_shared<Program>();
    
    prog->setVerbose(true);
    prog->setShaderNames(resourceDirectory + "/shaders/spider_vert.glsl", resourceDirectory + "/shaders/spider_frag.glsl");
    
    if (!prog->init())
    {
        cerr << "One or more shaders failed to compile... exiting!" << endl;
        exit(1);
    }
    
    prog->addUniform("P");
    prog->addUniform("V");
    prog->addUniform("M");
    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");
    prog->addAttribute("vertTex");
    
    return prog;
}

shared_ptr<Program> ShaderManager::initPupilProgShader() {
//    // Initialize the GLSL program.
    std::shared_ptr<Program> prog = make_shared<Program>();
    
    prog->setVerbose(true);
    prog->setShaderNames(resourceDirectory + "/shaders/pupil_vert.glsl", resourceDirectory + "/shaders/pupil_frag.glsl");
    
    if (!prog->init())
    {
        cerr << "One or more shaders failed to compile... exiting!" << endl;
        exit(1);
    }
    
    prog->addUniform("P");
    prog->addUniform("V");
    prog->addUniform("M");
    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");
    prog->addAttribute("vertTex");
    
    return prog;
}

shared_ptr<Program> ShaderManager::initEyeProgShader() {
//    // Initialize the GLSL program.
    std::shared_ptr<Program> prog = make_shared<Program>();
    
    prog->setVerbose(true);
    prog->setShaderNames(resourceDirectory + "/shaders/eye_vert.glsl", resourceDirectory + "/shaders/eye_frag.glsl");
    
    if (!prog->init())
    {
        cerr << "One or more shaders failed to compile... exiting!" << endl;
        exit(1);
    }
    
    prog->addUniform("P");
    prog->addUniform("V");
    prog->addUniform("M");
    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");
    prog->addAttribute("vertTex");
    
    return prog;
}

shared_ptr<Program> ShaderManager::initHandProgShader() {
//    // Initialize the GLSL program.
    std::shared_ptr<Program> prog = make_shared<Program>();
    
    prog->setVerbose(true);
    prog->setShaderNames(resourceDirectory + "/shaders/hand_vert.glsl", resourceDirectory + "/shaders/hand_frag.glsl");
    
    if (!prog->init())
    {
        cerr << "One or more shaders failed to compile... exiting!" << endl;
        exit(1);
    }
    
    prog->addUniform("P");
    prog->addUniform("V");
    prog->addUniform("M");
    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");
    prog->addAttribute("vertTex");
    
    return prog;
}
