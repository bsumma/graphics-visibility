//
//  BSPTree.cpp
//  bsp_tree
//
//  Created by Brian Summa on 11/17/23.
//

#include "common.h"


bool nearlyEqual(float a, float b, float epsilon){
  const float absA = fabs(a);
  const float absB = fabs(b);
  const float diff = fabs(a - b);
  
  if (a == b) { // shortcut
    return true;
  } else if (a * b == 0) { // a or b or both are zero
    // relative error is not meaningful here
    return diff < (epsilon * epsilon);
  } else { // use relative error
    return diff / (absA + absB) < epsilon;
  }
}

bool nearlyEqual(vec2 a, vec2 b, float epsilon){
  return nearlyEqual(a.x, b.x, epsilon) && nearlyEqual(a.y, b.y, epsilon);
}

bool BSPTree::inFront(Wall w, vec2 pos){
  //TODO
  return false;
}

vec2 BSPTree::splitPoint(Wall a, Wall b){
  vec2 p;
  //TODO
  return p;
}


void BSPTree::treeBuild(){
  std::vector < Node * > nodes;
  for(unsigned int i=0; i < orig_walls.size(); i++){
    nodes.push_back(new Node(orig_walls[i]));
  }
  
  root = treeBuildRecurse(nodes);
}

BSPTree::Node * BSPTree::treeBuildRecurse(std::vector < Node * > nodes){
  if(nodes.size() == 0){ return NULL; }
  
  Node *root = nodes[0];
  //TODO
  
  return root;
  
}


void BSPTree::treeWalk(vec2 pos,
                           std::vector < Wall > &in_order_walls){
  
  
  if(root != NULL){ treeWalkRecurse(pos, 
                                    root,
                                    in_order_walls); }

}

void BSPTree::treeWalkRecurse(vec2 pos, 
                              Node * node,
                              std::vector < Wall > &in_order_walls){
  
  if(node == NULL){ return; }

  //TODO
  
  
}



void BSPTree::printTree(const std::string& prefix, const Node* node, bool isLeft){
  
  if( node != NULL )
  {
      std::cout << prefix;

      std::cout << (isLeft ? "├──" : "└──" );

      std::cout << *node << std::endl;

    printTree( prefix + (isLeft ? "│   " : "    "), node->left, true);
    printTree( prefix + (isLeft ? "│   " : "    "), node->right, false);
  }else{
    std::cout << prefix;

    std::cout << (isLeft ? "├──" : "└──" );
    
    std::cout << "NULL" << std::endl;
    
  }
  
  
}



void BSPTree::getOpenGLvectors(vec2 pos,
                               std::vector < vec4 > &vertices,
                               std::vector < vec3 > &normals,
                               std::vector < vec4 > &colors)
{
  
  std::vector < Wall > in_order_walls;
  
  if(true){
    treeWalk(pos, in_order_walls);
  }else{
    in_order_walls = orig_walls;
  }
    
  for(unsigned int i=0; i < in_order_walls.size(); i++){
    vertices.push_back( vec4(in_order_walls[i].p1.x, -1, in_order_walls[i].p1.y, 1) );
    normals.push_back( vec3(in_order_walls[i].normal.x, 0, in_order_walls[i].normal.y) );
    colors.push_back( in_order_walls[i].color);
    
    vertices.push_back( vec4(in_order_walls[i].p2.x, -1, in_order_walls[i].p2.y, 1) );
    normals.push_back( vec3(in_order_walls[i].normal.x, 0, in_order_walls[i].normal.y) );
    colors.push_back( in_order_walls[i].color);

    vertices.push_back( vec4(in_order_walls[i].p1.x, 1, in_order_walls[i].p1.y, 1) );
    normals.push_back( vec3(in_order_walls[i].normal.x, 0, in_order_walls[i].normal.y) );
    colors.push_back( in_order_walls[i].color);

    vertices.push_back( vec4(in_order_walls[i].p1.x, 1, in_order_walls[i].p1.y, 1) );
    normals.push_back( vec3(in_order_walls[i].normal.x, 0, in_order_walls[i].normal.y) );
    colors.push_back( in_order_walls[i].color);

    vertices.push_back( vec4(in_order_walls[i].p2.x, 1, in_order_walls[i].p2.y, 1) );
    normals.push_back( vec3(in_order_walls[i].normal.x, 0, in_order_walls[i].normal.y) );
    colors.push_back( in_order_walls[i].color);

    vertices.push_back( vec4(in_order_walls[i].p2.x, -1, in_order_walls[i].p2.y, 1) );
    normals.push_back( vec3(in_order_walls[i].normal.x, 0, in_order_walls[i].normal.y) );
    colors.push_back( in_order_walls[i].color);

  }
}
