#pragma once

template<typename T>
struct smart_ptr_list
{
public:
	size_t add(T *val_p)
	{
		size_t idx_l = vector.size();
		if(free_idx.size() > 0)
		{
			idx_l = free_idx.front();
			free_idx.pop_front();
			vector[idx_l] = val_p;
		}
		else
		{
			vector.push_back(val_p);
		}
		return idx_l;
	}

	void free(size_t idx_l)
	{
		vector[idx_l] = nullptr;
		free_idx.push_back(idx_l);
	}

	std::vector<T *> vector;
private:
	std::list<size_t> free_idx;
};
