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

		// smaller stack size compared to quicksort
		template<typename T>
		void ShellSort(T* arr, int n)
		{
			for (int gap = n / 2; gap > 0; gap /= 2)
			{
				for (int i = gap; i < n; ++i)
				{
					int temp = arr[i];

					int j = i;
					for (; j >= gap && arr[j - gap] > temp; j -= gap)
						arr[j] = arr[j - gap];

					arr[j] = temp;
				}
			}
		}

		template<typename T>
		void QuickSort(T* arr, int left, int right)
		{
			int i, j, v;
			while (right > left) {
				j = right;
				i = left - 1;
				v = arr[right];

				while (true) {
					do i++; while (arr[i] < v && i < j);
					do j--; while (arr[j] > v && i < j);

					if (i >= j) break;
					Swap(arr[i], arr[j]);
				}

				Swap(arr[i], arr[right]);

				if ((i - 1 - left) <= (right - i - 1)) {
					QuickSort(arr, left, i - 1);
					left = i + 1;
				}
				else {
					QuickSort(arr, i + 1, right);
					right = i - 1;
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