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

bool has_input_free(line_handler const & handler_p)
{
	for(size_t i = 0 ; i < handler_p.in.size() ; ++ i )
	{
		if(handler_p.in[i] == nullptr)
		{
			return true;
		}
	}
	return false;
}

bool has_output_free(line_handler const & handler_p)
{
	for(size_t i = 0 ; i < handler_p.out.size() ; ++ i )
	{
		if(handler_p.out[i] == nullptr)
		{
			return true;
		}
	}
	return false;
}
