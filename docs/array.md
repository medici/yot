
数组
============

数组的声明格式：

```pascal
	array 数组长度 of 类型
```

例子：

```pascal
module arr
	import Text;

	func main()
		|
			var int : array 100 of integer;
				i : integer;
		|
		i := 10;
		int[i] := 99;
		Text.WriteInt(int[i]);
		Text.WriteLn
	end;
end.
```


多维数组
-----

多维数组简洁明了的声明方式：

```pascal
	array N0, N1, ..., Nk of T
```

是以下方式的缩写：

```pascal
	array N0 of
		array N1 of
			...
				array Nk of T
```

数组的索引也有两种试：

```pascal
	arr[N0][N1]...[Nk]
```
和
```pascal
	arr[N0, N1 ... Nk]
```

