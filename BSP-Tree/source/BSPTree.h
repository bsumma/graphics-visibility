//
//  BSPTree.h
//  bsp_tree
//
//  Created by Brian Summa on 11/17/23.
//

#ifndef BSPTree_h
#define BSPTree_h

#include "common.h"

using namespace Angel;

class Wall{
public:
  vec2 p1, p2;
  Wall(vec2 p1, vec2 p2, vec2 normal, 
       vec4 color, std::string label): p1(p1), p2(p2), normal(normal),
                                       color(color), label(label)  {};
  Wall() {};
  
  vec2 normal;
  vec4 color;
  std::string label;
  
  friend std::ostream& operator << ( std::ostream& os, const Wall& e ) {
            return os << e.p1 << "--" << e.p2;
  }
};



class BSPTree{
public:
  BSPTree(std::vector < Wall > orig_walls):orig_walls(orig_walls)
  {
    treeBuild();
  };
  
  void getOpenGLvectors(vec2 pos,
                        std::vector < vec4 > &vertices,
                        std::vector < vec3 > &normals,
                        std::vector < vec4 > &colors);
  
  
private:
  
  class Node{
  public:
    Node(Wall w): w(w){ };
    
    Wall w;
    Node * left;
    Node * right;
    
    friend std::ostream& operator << ( std::ostream& os, const Node& n ) {
      return os << n.w << " " << n.w.label;
    }
    
  };
  
  Node *root;
  
  std::vector < Wall > orig_walls;
  
  void treeBuild();
  
  Node * treeBuildRecurse(std::vector < Node * > nodes);
  
  void treeWalk(vec2 pos,
                    std::vector < Wall > &in_order_walls);
  
  void treeWalkRecurse(vec2 pos,
                       Node * node,
                       std::vector < Wall > &in_order_walls);
  
  void printTree(const std::string& prefix, const Node* node, bool isLeft);
  
  bool inFront(Wall w, vec2 pos);
  vec2 splitPoint(Wall a, Wall b);
  
  
};

#endif /* BSPTree_h */
