#pragma once

#include "grid/Grid.hh"
#include "line/Line.hh"

class ManagerGodot;

struct Connector
{
	/// @brief line connected (can be null)
	Line * line = nullptr;
	/// @brief type of the handler connected
	CaseType handler_type;
	/// @brief idx of the handler
	size_t handler_idx;

	/// @brief if the connector is an input of the handler
	bool input = false;

	godot::Vector2i pos;
};

/// @brief disconnect the handler and the line
void disconnect(ManagerGodot &manager_p, Connector &connector_p);
/// @brief connect the handler and the line
/// @return false if no connection was created
bool connect(ManagerGodot &manager_p, Connector &connector_p);

/// @brief disconnect all connectors of the line
void disconnect_all(ManagerGodot &manager_p, Line &line_p);
