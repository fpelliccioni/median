from bisect import bisect_left

# class MovingMedian:
#     def __init__(self, window_size):
#         self.window_size = window_size
#         self.numbers = []

#     def add_number(self, number):
#         ip = bisect_left(self.numbers, number)
#         self.numbers.insert(ip, number)
#         if len(self.numbers) > self.window_size:
#             self.numbers.pop(0)

#     def get_median(self):
#         return self.numbers[len(self.numbers)//2]


# -------------------------------------------------------------------------------------------

# import bisect

# class MovingMedian:
#     def __init__(self, window_size):
#         if window_size % 2 == 0:
#             raise ValueError("Window size must be an odd number")
#         self.window_size = window_size
#         self.numbers = []

#     def add_number(self, num):
#         if len(self.numbers) < self.window_size:
#             bisect.insort_left(self.numbers, num)
#             print(f'numbers: {self.numbers}')
#         else:
#             bisect.insort_left(self.numbers, num)
#             print(f'before pop - numbers: {self.numbers}')
#             self.numbers.pop(0)
#             print(f'after pop  - numbers: {self.numbers}')

#     def get_median(self):
#         length = len(self.numbers)
#         if length == 0:
#             return None
#         else:
#             return self.numbers[length // 2]


# -------------------------------------------------------------------------------------------

# from collections import deque

# class MovingMedian:
#     def __init__(self, window_size):
#         if window_size % 2 == 0:
#             raise ValueError("Window size must be an odd number")
#         self.window_size = window_size
#         self.numbers = deque()

#     def add_number(self, num):
#         self.numbers.appendleft(num)
#         print(f'after append - numbers: {self.numbers}')
#         if len(self.numbers) > self.window_size:
#             self.numbers.pop()
#             print(f'after pop    - numbers: {self.numbers}')

#     def get_median(self):
#         length = len(self.numbers)
#         if length == 0:
#             return None
#         else:
#             numbers_sorted = sorted(self.numbers)
#             return numbers_sorted[length // 2]

# -------------------------------------------------------------------------------------------

# import heapq

# class MovingMedian:
#     def __init__(self, window_size):
#         self.window_size = window_size
#         self.max_heap = []
#         self.min_heap = []

#     def add_number(self, number):
#         if len(self.max_heap) == len(self.min_heap):
#             heapq.heappush(self.min_heap, number)
#             largest_in_min_heap = self.min_heap[0]
#             heapq.heappush(self.max_heap, -heapq.heappop(self.min_heap))
#         else:
#             heapq.heappush(self.max_heap, -number)
#             smallest_in_max_heap = -self.max_heap[0]
#             heapq.heappush(self.min_heap, -heapq.heappop(self.max_heap))

#         if len(self.max_heap) > self.window_size // 2:
#             heapq.heappop(self.max_heap)
#         if len(self.min_heap) > self.window_size // 2 + 1:
#             heapq.heappop(self.min_heap)

#     def get_median(self):
#         if len(self.max_heap) == len(self.min_heap):
#             return (self.min_heap[0] - self.max_heap[0]) / 2
#         else:
#             return -self.max_heap[0]

# -------------------------------------------------------------------------------------------

from collections import deque
import bisect

class MovingMedian:
    def __init__(self, window_size):
        self.window_size = window_size
        self.window = deque()
        self.sorted_window = []

    def add_number(self, number):
        self.window.append(number)
        bisect.insort(self.sorted_window, number)
        if len(self.window) > self.window_size:
            removed_number = self.window.popleft()
            self.sorted_window.remove(removed_number)

    def get_median(self):
        middle = len(self.sorted_window) // 2
        return self.sorted_window[middle]

# -------------------------------------------------------------------------------------------

median = MovingMedian(3)
median.add_number(10)
median.add_number(5)
median.add_number(100)
median.add_number(4)
print(median.get_median())


# -------------------------------------------------------------------------------------------

# # Example usage:
# median = MovingMedian(3)
# median.add_number(1)
# median.add_number(2)
# median.add_number(3)
# print(median.get_median()) # Output: 2
# median.add_number(4)
# print(median.get_median()) # Output: 3
