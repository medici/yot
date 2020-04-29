快速排序
============

分治
-----

一般情况下，在所有排序算法中，快速排序是最佳选择；在最坏的情况下，快速排序速度与插入排序相近。与合并排序一样，快速排序把问题分成小问题，然后单独解决每个小问题，最后把结果合并。这种把大问题分成若干小问题，再各个击破的方法，我们称之为**`分治(divide-and-conquer)`**。每个小问题都跟原问题性质相同，所以我们可以用递归函数进行操作。

*   **拆分(divide)：**把数组 `A[p .. r]` 分成两个小数组 `A[p ..  q - 1]` 和 `A[q + 1 .. r]`，以使 `A[p .. q - 1]` 中的每个元素都小于或等于`A[q]`，而`A[q + 1 .. r]`中的每个元素都大于或小于`A[q]`。
*   **攻克(conquer)：**调用递归函数对数组 `A[p ..  q - 1]` 和 `A[q + 1 .. r]` 进行排序。
*   **合并(combine)：**此时每个子数组都已完成排序，并不需要对合并做多余的操作。

```pascal
func QuickSort(var A : array of integer; var p, r: integer)
	| var q : integer; |

	if p < r then
		q = Partition(A, p, r);
		QuickSort(A, p, q - 1);
		QuickSort(A, q + 1, r);
	end
end
```

在拆分函数中，先取数组最后一个元素做为支点(pivot)，筛选出小于或等于支点的所有元素，

```pascal
func Partition(var A : array of integer; var p, r: integer)
	| var len, pivot, i, temp, j : integer; |

	pivot := A[r];
	i := p - 1;

	for j := p to r - 1 do
		if A[j] <= pivot then
			i := i + 1;
			if i #j then
				temp := A[i];
				A[i] := A[j];
				A[j] := temp
			end
		end
	end;
	i := i + 1;
	temp =: A[i];
	A[i] := A[r];
	A[r] := temp;
end
```
