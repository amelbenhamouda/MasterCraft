#ifndef TEXURE_BLOCK_H_INCLUDED
#define TEXURE_BLOCK_H_INCLUDED
#include <GL/glew.h>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <iostream>

namespace mastercraft{
    struct BlockText {
        glimac::Program m_Program;
        GLint uMVPmloc;
        GLint uMVloc;
        GLint uNormloc;
        GLint uTranslate;
        GLint uVMatrix;
        GLint uPMatrix;
        GLint uKd;
        GLint uKs;
        GLint uShininess;
        GLint uLightDir_vs;
        GLint uLightIntensity;
        GLint uTexture;
        GLint uColor;
        /// torch light
        GLint uLightPos_vs;
        GLint uShininess_lamp;
        GLint uLightIntensity_lamp;
        GLint uKd2;
        GLint uKs2;
        GLint uTorchEnable;
        ///water
        GLint uAnimated;
        GLint uPoswater;
        GLint uTime;
        GLint uFrontvec;
        BlockText(const glimac::FilePath& applicationPath): m_Program(loadProgram(applicationPath.dirPath() + "../shaders/shad3Dblock.vs.glsl",
                                                                                  applicationPath.dirPath() + "../shaders/directionallight.fs.glsl")) {
            uMVPmloc = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
            uMVloc = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
            uNormloc = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
            uTexture = glGetUniformLocation(m_Program.getGLId(), "uTexture");
            uTranslate = glGetUniformLocation(m_Program.getGLId(), "uTranslate");
            uVMatrix = glGetUniformLocation(m_Program.getGLId(), "uVMatrix");
            uPMatrix = glGetUniformLocation(m_Program.getGLId(), "uPMatrix");

            uKd = glGetUniformLocation(m_Program.getGLId(), "uKd");
            uKs = glGetUniformLocation(m_Program.getGLId(), "uKs");
            uShininess = glGetUniformLocation(m_Program.getGLId(), "uShininess");
            uLightDir_vs = glGetUniformLocation(m_Program.getGLId(), "uLightDir_vs");
            uLightIntensity = glGetUniformLocation(m_Program.getGLId(), "uLightIntensity");
            uColor = glGetUniformLocation(m_Program.getGLId(), "uColor");
            
            uLightPos_vs = glGetUniformLocation(m_Program.getGLId(), "uLightPos_vs");
            uShininess_lamp = glGetUniformLocation(m_Program.getGLId(), "uShininess_lamp");
            uLightIntensity_lamp = glGetUniformLocation(m_Program.getGLId(), "uLightIntensity_lamp");
            uKd2 = glGetUniformLocation(m_Program.getGLId(), "uKd2");
            uKs2 = glGetUniformLocation(m_Program.getGLId(), "uKs2");
            
            uAnimated = glGetUniformLocation(m_Program.getGLId(), "uAnimated");
            uPoswater = glGetUniformLocation(m_Program.getGLId(), "uPoswater");
            uTime = glGetUniformLocation(m_Program.getGLId(), "uTime");
            
            uFrontvec = glGetUniformLocation(m_Program.getGLId(), "uFrontvec");
            uTorchEnable = glGetUniformLocation(m_Program.getGLId(), "uTorchEnable");
        }
    };

    struct NpcText {
        glimac::Program m_Program;
        GLint uMVPmloc;
        GLint uMVloc;
        GLfloat uSize_cube;
        GLint uNormloc;
        GLint uTranslate;
        GLint uRotate;
        GLint uVMatrix;
        GLint uPMatrix;
        GLint uKd;
        GLint uKs;
        GLint uShininess;
        GLint uLightDir_vs;
        GLint uLightIntensity;
        GLint uTexture;
        GLint uColor;
        //lamp
        GLint uLightPos_vs;
        GLint uLightIntensity_lamp;
        GLint uShininess_lamp;
        GLint uKd2;
        GLint uKs2;
        GLint uTorchEnable;

        NpcText(const glimac::FilePath& applicationPath): m_Program(loadProgram(applicationPath.dirPath() + "../shaders/shad3Dnpc.vs.glsl",
                                                                                applicationPath.dirPath() + "../shaders/shad3Dnpc.fs.glsl")) {
            uMVPmloc = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
            uMVloc = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
            uNormloc = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
            uTexture = glGetUniformLocation(m_Program.getGLId(), "uTexture");
            uTranslate = glGetUniformLocation(m_Program.getGLId(), "uTranslate");
            uRotate = glGetUniformLocation(m_Program.getGLId(), "uRotate");
            uVMatrix = glGetUniformLocation(m_Program.getGLId(), "uVMatrix");
            uPMatrix = glGetUniformLocation(m_Program.getGLId(), "uPMatrix");
            uSize_cube = glGetUniformLocation(m_Program.getGLId(), "uSize_cube");

            uKd = glGetUniformLocation(m_Program.getGLId(), "uKd");
            uKs = glGetUniformLocation(m_Program.getGLId(), "uKs");
            uShininess = glGetUniformLocation(m_Program.getGLId(), "uShininess");
            uLightDir_vs = glGetUniformLocation(m_Program.getGLId(), "uLightDir_vs");
            uLightIntensity = glGetUniformLocation(m_Program.getGLId(), "uLightIntensity");
            uColor = glGetUniformLocation(m_Program.getGLId(), "uColor");

            uLightPos_vs = glGetUniformLocation(m_Program.getGLId(), "uLightPos_vs");
            uShininess_lamp = glGetUniformLocation(m_Program.getGLId(), "uShininess_lamp");
            uLightIntensity_lamp = glGetUniformLocation(m_Program.getGLId(), "uLightIntensity_lamp");
            uKd2 = glGetUniformLocation(m_Program.getGLId(), "uKd2");
            uKs2 = glGetUniformLocation(m_Program.getGLId(), "uKs2");

            uTorchEnable = glGetUniformLocation(m_Program.getGLId(), "uTorchEnable");
        }
    };

    struct Skytext {
        glimac::Program m_Program;
        GLint uCubemap;
        GLint uMVP;
        Skytext(const glimac::FilePath& applicationPath): m_Program(loadProgram(applicationPath.dirPath() + "../shaders/skybox.vs.glsl",
                                                                                applicationPath.dirPath() + "../shaders/skybox.fs.glsl")) {
            uCubemap = glGetUniformLocation(m_Program.getGLId(), "uCubemap");
            uMVP = glGetUniformLocation(m_Program.getGLId(), "uMVP");
        }
    };

    struct SunText {
        glimac::Program m_Program;
        GLint uMVPmloc;
        GLint uMVloc;
        GLfloat uSize_cube;
        GLint uTranslate;
        GLint uRotate;
        GLint uVMatrix;
        GLint uPMatrix;
        GLint uTexture;
        GLint uColor;
        GLint uPosSun;

        SunText(const glimac::FilePath& applicationPath): m_Program(loadProgram(applicationPath.dirPath() + "../shaders/shad3Dsun.vs.glsl",
                                                                                applicationPath.dirPath() + "../shaders/shad3Dsun.fs.glsl")) {
            uMVPmloc = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
            uMVloc = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
            uTexture = glGetUniformLocation(m_Program.getGLId(), "uTexture");
            uTranslate = glGetUniformLocation(m_Program.getGLId(), "uTranslate");
            uRotate = glGetUniformLocation(m_Program.getGLId(), "uRotate");
            uVMatrix = glGetUniformLocation(m_Program.getGLId(), "uVMatrix");
            uPMatrix = glGetUniformLocation(m_Program.getGLId(), "uPMatrix");
            uSize_cube = glGetUniformLocation(m_Program.getGLId(), "uSize_cube");

            uColor = glGetUniformLocation(m_Program.getGLId(), "uColor");
            uPosSun = glGetUniformLocation(m_Program.getGLId(), "uPosSun");
        }
    };
}

#endif // TEXURE_BLOCK_H_INCLUDED
