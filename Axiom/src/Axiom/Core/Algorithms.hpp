#pragma once

#include "Common.hpp"
#include "Random.hpp"

namespace ax
{
	namespace Algorithms
	{
		template<typename T>
		inline void Swap(T& a, T& b) {
			T temp = (T&&)a;
			a = (T&&)b;
			b = (T&&)temp;
		}

		template<typename T>
		inline int Partition(T* arr, int low, int high)
		{
			int i = low;
			int j = high;
			T pivot = arr[low];
			while (i < j)
			{
				while (pivot >= arr[i]) i++;
				while (pivot < arr[j])  j--;
				
				if (i < j) Swap(arr[i], arr[j]);
			}
			Swap(arr[low], arr[j]);
			return j;
		}

		template<typename T>
		inline void BubbleSort(T* arr, int len)
		{
			for (int i = 0; i < len; ++i)
			{
				for (int j = 0; j < len - i - 1; ++j)
				{
					if (arr[i + 1] < arr[i])
						Swap(arr[i], arr[i + 1]);
				}
			}
		}

		// QuickSort(arr.begin(), 0, arr.size()-1);
		template<typename T>
		inline void QuickSort(T* arr, int low, int high)
		{
			if (low < high)
			{
				int pivot = Partition(arr, low, high);
				#pragma omp parallel sections
				{
					#pragma omp section
					QuickSort(arr, low, pivot - 1);
					#pragma omp section
					QuickSort(arr, pivot + 1, high);
				}
			}
		}

		template<typename T> 
		inline void Reverse(T* begin, T* end)
		{
			while (begin < end)
			{
				Swap(*begin, *end);
				begin++;
				end--;
			}
		}
	
		template<typename T> 
		inline void Suffle(T* begin, uint64 len)
		{
			Random::PCG rand(Random::RandomSeed64());

			for (uint64 i = len - 1; i > 1; --i)
			{
				Swap(begin[rand.NextBound(i)], begin[i]);
			}
		}

		template<typename T> 
		inline T* BinarySearch(T* begin, int len, T value)
		{
			int low = 0;
			int high = len;

			while (low < high)
			{
				T mid = (low + high) / 2;
				if (begin[mid] == value) return begin + mid;
				else if (begin[mid] < value) low = mid + 1;
				else high = mid-1; // begin[mid] > value
			}
			return nullptr;
		}
	}

}