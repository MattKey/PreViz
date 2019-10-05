//
//  ShaderManager.hpp
//  FlipSide
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

#ifndef ShaderManager_h
#define ShaderManager_h

#include <stdio.h>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include <vector>
#include <map>
#include <string>

#include "Program.h"

#define SIMPLEPROG 0
#define SIMPLEPROG2 1

#include <memory>

using namespace std;
using namespace glm;

class ShaderManager {
    
public:
    map<int, shared_ptr<Program>> shaderMap;
    string resourceDirectory;
    shared_ptr<Program> currentShader;
    
    ShaderManager(const std::string& resourceDirectory) : resourceDirectory(resourceDirectory) {
        initShaders();
    }
    ~ShaderManager();
    
    void initShaders();
    shared_ptr<Program> initSimpleProgShader();
    
    shared_ptr<Program> getCurrentShader() { return currentShader; }
    void setCurrentShader(int shader) { currentShader = shaderMap[shader]; }
    
    
};

#endif /* ShaderManager_hpp */
