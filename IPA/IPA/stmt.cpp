#include "stmt.h"


Node::NodeType ControlFlowStmt::s_node_type = ControlFlowStmtType;
Node::NodeType IfStmt::s_node_type =		  IfStmtType;
Node::NodeType ForStmt::s_node_type =		  ForStmtType;
Node::NodeType WhileStmt::s_node_type =		  WhileStmtType;
Node::NodeType OperandStmt::s_node_type =	  OperandStmtType;
Node::NodeType CallStmt::s_node_type =		  CallStmtType;

