#pragma once

#include "line/Line.hh"

line_handler::line_handler(size_t in_p, size_t out_p)
	: in(in_p, nullptr), out(out_p, nullptr)
{}

bool connect_input(line_handler & handler_p, Line & line_p)
{
	for(size_t i = 0 ; i < handler_p.in.size() ; ++ i )
	{
		if(handler_p.in[i] == nullptr)
		{
			handler_p.in[i] = &line_p;
			line_p.handler_end = &handler_p;
			return true;
		}
	}
	return false;
}
bool connect_output(line_handler & handler_p, Line & line_p)
{
	for(size_t i = 0 ; i < handler_p.out.size() ; ++ i )
	{
		if(handler_p.out[i] == nullptr)
		{
			handler_p.out[i] = &line_p;
			line_p.handler_start = &handler_p;
			return true;
		}
	}
	return false;
}

void disconnect(line_handler & handler_p, Line & line_p)
{
	for(size_t i = 0 ; i < handler_p.in.size() ; ++ i )
	{
		if(handler_p.in[i] == &line_p)
		{
			handler_p.in[i] = nullptr;
		}
	}
	for(size_t i = 0 ; i < handler_p.out.size() ; ++ i )
	{
		if(handler_p.out[i] == &line_p)
		{
			handler_p.out[i] = nullptr;
		}
	}
}

void disconnect_all(Line & line_p)
{
	if(line_p.handler_start)
	{
		disconnect(*line_p.handler_start, line_p);
	}
	if(line_p.handler_end)
	{
		disconnect(*line_p.handler_end, line_p);
	}
}