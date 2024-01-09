#include "Connector.hh"

#include "godot/ManagerGodot.hh"
#include "utils/line_handler.hh"

/// @brief disconnect the handler and the line
void disconnect(ManagerGodot &manager_p, Connector &connector_p)
{
	line_handler &handler_l = manager_p.get_handler(connector_p.handler_type, connector_p.handler_idx);
	if(connector_p.line)
	{
		// disconnect line
		if(connector_p.input)
		{
			connector_p.line->handler_end = nullptr;
		}
		else
		{
			connector_p.line->handler_start = nullptr;
		}
		// disconnect handler
		disconnect(handler_l, *connector_p.line);
	}
}

/// @brief connect the handler and the line
/// @return false if no connection existed
bool connect(ManagerGodot &manager_p, Connector &connector_p)
{
	line_handler &handler_l = manager_p.get_handler(connector_p.handler_type, connector_p.handler_idx);
	if(connector_p.line)
	{
		if(connector_p.input)
		{
			connector_p.line->handler_end = &connector_p;
			return connect_input(handler_l, *connector_p.line);
		}
		else
		{
			connector_p.line->handler_start = &connector_p;
			return connect_output(handler_l, *connector_p.line);
		}
	}
	return false;
}

void disconnect_all(ManagerGodot &manager_p, Line &line_p)
{
	if(line_p.handler_start)
	{
		disconnect(manager_p, *line_p.handler_start);
	}
	if(line_p.handler_end)
	{
		disconnect(manager_p, *line_p.handler_end);
	}
	line_p.handler_start = nullptr;
	line_p.handler_end = nullptr;
}
