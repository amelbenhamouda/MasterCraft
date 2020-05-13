#pragma once

#ifndef M_PI
   #define M_PI 3.14159265358979323846
#endif

class FreeflyCamera {
    public:

        virtual  ~FreeflyCamera(){};
        FreeflyCamera(): m_Position(0, 0, -2), m_fPhi(M_PI - 0.02 * M_PI), m_fTheta(0.0f) {
            computeDirectionVectors();

        };

        // 0== M_PI comme sa on a F = 0,0,-1 et on a bien la convention z négative pour la caméra openGL
        void moveLeft(float t) {
            m_Position += t * m_LeftVector;
        }
        
        void moveFront(float t) {
            m_Position += t * m_FrontVector;
        }

        void rotateLeft(float degrees) {
            m_fPhi += (degrees * M_PI) / 180;
            computeDirectionVectors();
        }
        void rotateUp(float degrees) {
            m_fTheta += (degrees * M_PI) / 180;
            computeDirectionVectors();
        }

        glm::mat4 getViewMatrix() const {
            return glm::lookAt(m_Position, m_Position + m_FrontVector, m_UpVector);
            // eye // Le point de vue V (argument point) // Le dernier argument est l'axe vertical de la caméra
        }

        glm::vec3 getFrontVector() const { return m_FrontVector;};
        glm::vec3 getPosition() const {return m_Position;}
        void setPosition(glm::vec3 pos) { m_Position = pos;}

    private:
        glm::vec3 m_Position;
        float m_fPhi;
        float m_fTheta;
        glm::vec3 m_FrontVector;
        glm::vec3 m_LeftVector;
        glm::vec3 m_UpVector;
        void computeDirectionVectors() {
            m_FrontVector[0] = cos(m_fTheta) * sin(m_fPhi); //F⃗ =(cos(θ)sin(ϕ), sin(θ), cos(θ)cos(ϕ))
            m_FrontVector[1] = sin(m_fTheta);
            m_FrontVector[2] = cos(m_fTheta) * cos(m_fPhi);

            m_LeftVector[0] = sin(m_fPhi + M_PI / 2); //L⃗ =(sin(ϕ+π/2), 0, cos(ϕ+π/2))
            m_LeftVector[1] = 0;
            m_LeftVector[2] = cos(m_fPhi + M_PI / 2);

            m_UpVector = glm::cross(m_FrontVector, m_LeftVector);
        }
};
