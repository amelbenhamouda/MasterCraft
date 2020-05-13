#include <cmath>
#include <vector>
#include <iostream>
#include "glimac/common.hpp"
#include "mastercraft/Tree.hpp"
#include "mastercraft/Cube.hpp"

namespace glimac {

	void Tree::build(const int sizeCube) {
		Cube cube(sizeCube);
	    GLsizei count_vertex = cube.getVertexCount();
	    std::vector<ShapeVertex> data;
	    const ShapeVertex* Datapointeur = cube.getDataPointer();
	    for (auto i = 0; i < count_vertex; i++) {
	    	data.push_back(*Datapointeur);
	        Datapointeur++;
	    }
	    float tol = 0.01;
	    tronc(data, count_vertex, tol, sizeCube);
	    leavesFace(data, count_vertex, tol, sizeCube);
	    leavesCote(data, count_vertex, tol, sizeCube);
	}

	void Tree::tronc(std::vector<ShapeVertex> data, GLsizei count_vertex, float tol, const int sizeCube) {
		std::vector<ShapeVertex> tmp;
		// tronc 1
		m_Position.push_back(glm::vec3(0,0,0));
		for (auto elem : data) {
			elem.texCoords[0] *= 0.25 - tol;
	        elem.texCoords[1] *= 0.5 - tol;
	        elem.texCoords[1] += 0.5 + tol * 0.5;
	        elem.texCoords[0] += 0.5 + 0.5 * tol;
	        tmp.push_back(elem);
		}
	    m_nVertexCount = count_vertex;
	    m_Vertices = tmp;

	    // tronc 2
	    m_Position.push_back(glm::vec3(0,1,0));
	    m_nVertexCount += count_vertex;
	    for (auto elem : tmp){
	    	elem.position.y += (1 * sizeCube);
	    	m_Vertices.push_back(elem);
	    }
	}

	void Tree::leavesFace(std::vector<ShapeVertex> data, GLsizei count_vertex, float tol, const int sizeCube) {
		std::vector<ShapeVertex> tmp = data;

		// Feuille du sommet
		m_Position.push_back(glm::vec3(0,8,0));
		m_nVertexCount += count_vertex;
	    for (auto elem : tmp){
	    	elem.position.y += (8 * sizeCube);
	    	elem.texCoords[0] *= 0.25 - tol;
	        elem.texCoords[1] *= 0.5 - tol;
	        elem.texCoords[1] += 0.5 + tol * 0.5;
	        elem.texCoords[0] += 0.25 + 0.5 * tol;
	    	m_Vertices.push_back(elem);
	    }
	    tmp = data;

		//feuille du bas
		for (int j = 2; j < 4; j++) {
			for (int i = -3; i < 4; i++) {
                m_Position.push_back(glm::vec3(i,j,0));
			    m_nVertexCount += count_vertex;
			    for (auto elem : tmp){
			    	elem.position.x += (i * sizeCube);
			    	elem.position.y += (j * sizeCube);
			    	elem.texCoords[0] *= 0.25 - tol;
			        elem.texCoords[1] *= 0.5 - tol;
			        elem.texCoords[1] += 0.5 + tol * 0.5;
			        elem.texCoords[0] += 0.25 + 0.5 * tol;
			    	m_Vertices.push_back(elem);
			    }
			    tmp = data;
			}
		}

		// feuille milieu
		for (int j = 4; j < 6; j++) {
			for (int i = -2; i < 3; i++) {
                m_Position.push_back(glm::vec3(i,j,0));
			    m_nVertexCount += count_vertex;
			    for (auto elem : tmp){
			    	elem.position.x += (i * sizeCube);
			    	elem.position.y += (j * sizeCube);
			    	elem.texCoords[0] *= 0.25 - tol;
			        elem.texCoords[1] *= 0.5 - tol;
			        elem.texCoords[1] += 0.5 + tol * 0.5;
			        elem.texCoords[0] += 0.25 + 0.5 * tol;
			    	m_Vertices.push_back(elem);
			    }
			    tmp = data;
			}
		}

		// feuille milieu 2
		for (int j = 6; j < 8; j++) {
			for (int i = -1; i < 2; i++) {
                m_Position.push_back(glm::vec3(i,j,0));
			    m_nVertexCount += count_vertex;
			    for (auto elem : tmp){
			    	elem.position.x += (i * sizeCube);
			    	elem.position.y += (j * sizeCube);
			    	elem.texCoords[0] *= 0.25 - tol;
			        elem.texCoords[1] *= 0.5 - tol;
			        elem.texCoords[1] += 0.5 + tol * 0.5;
			        elem.texCoords[0] += 0.25 + 0.5 * tol;
			    	m_Vertices.push_back(elem);
			    }
			    tmp = data;
			}
		}
	}

	void Tree::leavesCote(std::vector<ShapeVertex> data, GLsizei count_vertex, float tol, const int sizeCube) {
		std::vector<ShapeVertex> tmp = data;

		//feuille du bas
		for (int j = 2; j < 4; j++) {
			for (int i = -3; i < 4; i++) {
                m_Position.push_back(glm::vec3(0,j,i));
			    m_nVertexCount += count_vertex;
			    for (auto elem : tmp){
			    	elem.position.y += (j * sizeCube);
			    	elem.position.z += (i * sizeCube);
			    	elem.texCoords[0] *= 0.25 - tol;
			        elem.texCoords[1] *= 0.5 - tol;
			        elem.texCoords[1] += 0.5 + tol * 0.5;
			        elem.texCoords[0] += 0.25 + 0.5 * tol;
			    	m_Vertices.push_back(elem);
			    }
			    tmp = data;
			}
		}

		// feuille milieu
		for (int j = 4; j < 6; j++) {
			for (int i = -2; i < 3; i++) {
                m_Position.push_back(glm::vec3(0,j,i));
			    m_nVertexCount += count_vertex;
			    for (auto elem : tmp){
			    	elem.position.y += (j * sizeCube);
			    	elem.position.z += (i * sizeCube);
			    	elem.texCoords[0] *= 0.25 - tol;
			        elem.texCoords[1] *= 0.5 - tol;
			        elem.texCoords[1] += 0.5 + tol * 0.5;
			        elem.texCoords[0] += 0.25 + 0.5 * tol;
			    	m_Vertices.push_back(elem);
			    }
			    tmp = data;
			}
		}

		// feuille milieu 2
		for (int j = 6; j < 8; j++) {
			for (int i = -1; i < 2; i++) {
                m_Position.push_back(glm::vec3(0,j,i));
			    m_nVertexCount += count_vertex;
			    for (auto elem : tmp){
			    	elem.position.y += (j * sizeCube);
			    	elem.position.z += (i * sizeCube);
			    	elem.texCoords[0] *= 0.25 - tol;
			        elem.texCoords[1] *= 0.5 - tol;
			        elem.texCoords[1] += 0.5 + tol * 0.5;
			        elem.texCoords[0] += 0.25 + 0.5 * tol;
			    	m_Vertices.push_back(elem);
			    }
			    tmp = data;
			}
		}
	}
}
