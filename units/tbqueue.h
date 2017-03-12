#include "tbarr.h"

namespace tblib
{
	template <typename T, typename Comp = DefaultComparator<T> >
	class Queue
	{
		tblib::Array<tblib::HeapArray<T>, 16> data;
		Comp comp;

	public : 
		
		bool Correct ()
		{
			/*
      for (int i=1; i<data.Size(); ++i)
				for (int j=0; j<data[i].Size(); ++j)
					if (data[i][j]<data[i-1][j>>2])
						return false;
						*/
			return true;
		}

		Queue () : comp () {}

		template <typename A1>
		Queue (const A1& a1) : comp(a1) {}

		bool Empty ()
		{
			return data.Size()==0;
		}
		
		void PushBack (const T& t)
		{
			if (data.Size()==0)
			{
				data.EmplaceBack(1);
				data.back().EmplaceBack(t);
			} else
			{
				if (data.back().Size()==(1<<(data.Size()*2-2)))
					data.EmplaceBack(1<<(data.Size()*2));			
				data.back().EmplaceBack(t);

				int b = data.Size()-1, nb = data.back().Size()-1;

				if (b>0)
				{
					T* cur  = &data[b][nb];   
					--b;
					nb >>= 2;
					T* next = &data[b][nb];

					if (comp(*cur,*next))
					{
						T tmp = *cur;
						for (;;)
						{
							*cur = *next;
							
							if (b==0)
							{
								*next = tmp;
								break;
							}

							cur = next;
							--b;
							nb >>= 2;
							next = &data[b][nb];		

							if (comp(tmp,*next))
							{
								;// продолжаем
							} else
							{
								*cur = tmp;
								break;
							}
						}
					}
				}
			}
			assert(Correct());
		}

		void PopBack (T& t)
		{
			assert (data.Size()>0);
			t = data.front().front();

			T tmp = data.back().back();
			data.back().Shrink(data.back().Size()-1);
			if (data.back().Size()==0)
				data.Shrink(data.Size()-1);
			if (data.Size()==0)
				return;

			int b = 0, nb = 0;
			T* cur = &data[b][nb];

			for (;;)
			{
				++b;
				int nnb = nb<<2;
				int nne;

				if (b==data.Size())
					break;
				else
					nne = Min(nnb+4, data[b].Size());
				if (nne<=nnb)
					break;

				T* min = &data[b][nnb];
				nb = nnb;

				++nnb;
				for (; nnb<nne; ++nnb)
				{
					T* ch = &data[b][nnb];
					if (comp(*ch,*min))
					{
						min=ch;
						nb = nnb;
					}
				}

				if (!comp(*min,tmp))
					break;

				*cur = *min;
				cur = min;   
			}
			*cur = tmp;
			assert(Correct());
		}
	};
};